import io
import pathlib
import shutil
import subprocess
import tempfile
from datetime import datetime

from markupsafe import Markup

from odoo import api, fields, models
from odoo.exceptions import UserError

try:
    from PIL import Image, ImageChops, ImageStat
except ImportError:  # pragma: no cover - depends on the local runtime
    Image = None
    ImageChops = None
    ImageStat = None

ENGINES = [
    ('qweb-pdf-wkhtmltopdf', "WkHtmlToPdf"),
    ('qweb-pdf-paper-muncher', "Paper Muncher"),
]
DEFAULT_REFERENCE_ENGINE = 'qweb-pdf-wkhtmltopdf'
DEFAULT_ENGINE = 'qweb-pdf-paper-muncher'
PDF_REPORT_TYPES = {'qweb-pdf', DEFAULT_REFERENCE_ENGINE, DEFAULT_ENGINE}
PDFTOPPM_BIN = shutil.which('pdftoppm')


class Reftest(models.Model):
    _name = "reftest.reftest"
    _description = "Paper Muncher Reftest"
    _inherit = ['mail.thread']

    state = fields.Selection([
        ('draft', 'Draft'),
        ('run', 'Run'),
        ('dnf', 'DNF'),
    ], required=True, default='draft', readonly=True)

    similarity_score = fields.Float(
        string="Similarity",
        readonly=True,
        store=True,
        digits=(5, 2),
        group_operator='avg',
    )
    report_id = fields.Many2one("ir.actions.report", required=True, domain=[('report_type', 'in', ['qweb-pdf'] + [x[0] for x in ENGINES])])
    res_model = fields.Char(related='report_id.model')
    res_id = fields.Many2oneReference(required=True, model_field='res_model')
    layout_id = fields.Many2one('report.layout', required=True)
    reference_engine = fields.Selection(ENGINES, required=True)
    engine = fields.Selection(ENGINES, required=True)
    output_pdf_ids = fields.Many2many(
        'ir.attachment',
        'reftest_output_pdf_attachment_rel',
        'reftest_id',
        'attachment_id',
        readonly=True,
    )
    preview_image_ids = fields.Many2many(
        'ir.attachment',
        'reftest_preview_image_attachment_rel',
        'reftest_id',
        'attachment_id',
        readonly=True,
    )

    _check_engine_different = models.Constraint(
        "CHECK(reference_engine <> engine)"
    )

    @api.onchange('report_id')
    def _onchange_report_id(self):
        self.res_id = False

    def _get_test_company(self):
        self.ensure_one()
        record = self.env[self.res_model].browse(self.res_id)
        if 'company_id' in record._fields and record.company_id:
            return record.company_id
        if 'company_ids' in record._fields and record.company_ids:
            return record.company_ids[:1]
        return self.env.company

    def _get_report_context(self, company):
        self.ensure_one()
        return {
            'allowed_company_ids': company.ids,
            'company_id': company.id,
            'force_company': company.id,
            'lang': self.env.user.lang,
            'report_layout_id': self.layout_id.id,
            'report_layout_view_id': self.layout_id.view_id.id,
        }

    def _clear_current_artifacts(self):
        self.ensure_one()
        self.similarity_score = False
        self.output_pdf_ids = [(5, 0, 0)]
        self.preview_image_ids = [(5, 0, 0)]

    def _render_first_page_preview(self, pdf_content, label):
        self.ensure_one()
        if not PDFTOPPM_BIN:
            raise UserError("pdftoppm is required to generate PDF previews.")
        if not Image or not ImageChops or not ImageStat:
            raise UserError("Pillow is required to compute preview similarity.")

        with tempfile.TemporaryDirectory(prefix='odoo-reftest-preview-') as tmpdir:
            tmpdir_path = pathlib.Path(tmpdir)
            pdf_path = tmpdir_path / f'{label}.pdf'
            png_prefix = tmpdir_path / 'page'
            pdf_path.write_bytes(pdf_content)
            subprocess.run(
                [PDFTOPPM_BIN, '-png', '-f', '1', '-singlefile', str(pdf_path), str(png_prefix)],
                check=True,
                capture_output=True,
                text=True,
            )
            png_path = png_prefix.with_suffix('.png')
            if not png_path.exists():
                raise UserError("pdftoppm did not generate a preview image.")
            return png_path.read_bytes()

    def _compute_preview_similarity(self, left_content, right_content):
        self.ensure_one()
        if not Image or not ImageChops or not ImageStat:
            raise UserError("Pillow is required to compute preview similarity.")

        with Image.open(io.BytesIO(left_content)) as left_image, Image.open(io.BytesIO(right_content)) as right_image:
            left = left_image.convert('RGB')
            right = right_image.convert('RGB')
            if left.size != right.size:
                right = right.resize(left.size)
            diff = ImageChops.difference(left, right)
            stat = ImageStat.Stat(diff)
            if not stat.rms:
                return 100.0
            rms = sum(stat.rms) / len(stat.rms)
            similarity = max(0.0, 100.0 * (1.0 - (rms / 255.0)))
            return round(similarity, 2)

    def _create_pdf_attachments(self, report_name, record_name, timestamp, reference_pdf_content, test_pdf_content):
        self.ensure_one()
        return self.env['ir.attachment'].create([
            {
                'name': f'reference_{report_name}_{record_name}_{timestamp}.pdf',
                'raw': reference_pdf_content,
                'mimetype': 'application/pdf',
                'res_model': self._name,
                'res_id': self.id,
            },
            {
                'name': f'test_{report_name}_{record_name}_{timestamp}.pdf',
                'raw': test_pdf_content,
                'mimetype': 'application/pdf',
                'res_model': self._name,
                'res_id': self.id,
            },
        ])

    def _create_preview_attachments(self, report_name, record_name, timestamp, reference_preview_content, test_preview_content):
        self.ensure_one()
        return self.env['ir.attachment'].create([
            {
                'name': f'reference_preview_{report_name}_{record_name}_{timestamp}.png',
                'raw': reference_preview_content,
                'mimetype': 'image/png',
                'res_model': self._name,
                'res_id': self.id,
            },
            {
                'name': f'test_preview_{report_name}_{record_name}_{timestamp}.png',
                'raw': test_preview_content,
                'mimetype': 'image/png',
                'res_model': self._name,
                'res_id': self.id,
            },
        ])

    def _post_preview_message(self, reference_preview, test_preview, similarity_score):
        self.ensure_one()
        body = Markup(
            """
            <p>First-page similarity: <strong>{score:.2f}%</strong></p>
            <div style="display:flex; gap:16px; flex-wrap:wrap; align-items:flex-start;">
                <figure style="margin:0;">
                    <figcaption style="margin-bottom:8px;"><strong>Reference</strong></figcaption>
                    <img src="/web/image/{reference_id}" style="max-width:420px; border:1px solid #d0d7de;"/>
                </figure>
                <figure style="margin:0;">
                    <figcaption style="margin-bottom:8px;"><strong>Test</strong></figcaption>
                    <img src="/web/image/{test_id}" style="max-width:420px; border:1px solid #d0d7de;"/>
                </figure>
            </div>
            """
        ).format(
            score=similarity_score,
            reference_id=reference_preview.id,
            test_id=test_preview.id,
        )
        self.message_post(
            body=body,
            attachment_ids=[reference_preview.id, test_preview.id],
            subtype_xmlid='mail.mt_note',
        )

    @api.model
    def _get_run_all_layouts(self):
        return self.env['report.layout'].search([], order='sequence, id')

    @api.model
    def _get_run_all_reports(self):
        reports = self.env['ir.actions.report'].search([
            ('report_name', '!=', False),
            ('model', '!=', False),
        ], order='id')
        return reports.filtered(lambda report: (report.report_type or 'qweb-pdf') in PDF_REPORT_TYPES)

    @api.model
    def _get_run_all_record(self, report):
        try:
            model = self.env[report.model]
        except KeyError:
            return False
        if not getattr(model, '_auto', True):
            return False
        if getattr(model, '_abstract', False):
            return False
        if getattr(model, '_transient', False):
            return False
        return model.with_context(active_test=False).search([], order='id', limit=1)

    @api.model
    def action_run_all(self, *args):
        reftest_model = self.env['reftest.reftest']
        existing_tests = reftest_model.search([])
        existing_attachments = existing_tests.output_pdf_ids | existing_tests.preview_image_ids
        existing_tests.unlink()
        existing_attachments.unlink()

        layouts = self._get_run_all_layouts()
        if not layouts:
            raise UserError("No report layout is available to run the reftests.")

        reports = self._get_run_all_reports()
        values_list = []
        for report in reports:
            record = self._get_run_all_record(report)
            if not record:
                continue
            for layout in layouts:
                values_list.append({
                    'report_id': report.id,
                    'res_id': record.id,
                    'layout_id': layout.id,
                    'reference_engine': DEFAULT_REFERENCE_ENGINE,
                    'engine': DEFAULT_ENGINE,
                })

        if not values_list:
            raise UserError("No runnable reports with demo data were found.")

        tests = reftest_model.create(values_list)
        print(f"[reftest] Starting Run All with {len(tests)} tests across {len(reports)} reports and {len(layouts)} layouts")
        tests.run_test()
        return {
            'type': 'ir.actions.client',
            'tag': 'reload',
        }

    def run_test(self):
        total = len(self)
        for index, test in enumerate(self, start=1):
            print(f"[reftest] Running {index}/{total}: report={test.report_id.display_name} layout={test.layout_id.name}")
            test._clear_current_artifacts()

            target_record = self.env[test.res_model].browse(test.res_id).exists()
            if not target_record:
                test.state = 'dnf'
                print(f"[reftest] DNF {index}/{total}: report={test.report_id.display_name} layout={test.layout_id.name} error=missing target record")
                continue

            company = test._get_test_company()
            report_action = test.report_id.with_context(test._get_report_context(company))
            original_layout = company.external_report_layout_id
            original_report_type = test.report_id.report_type
            reference_pdf_content = False
            pdf_content = False
            render_error = None

            try:
                company.external_report_layout_id = test.layout_id.view_id
                test.report_id.report_type = test.reference_engine
                reference_pdf_content, _ = self.env['ir.actions.report']._render_qweb_pdf(report_action, res_ids=target_record.ids)
                test.report_id.report_type = test.engine
                pdf_content, _ = self.env['ir.actions.report']._render_qweb_pdf(report_action, res_ids=target_record.ids)
            except Exception as err:
                render_error = err
            finally:
                test.report_id.report_type = original_report_type or 'qweb-pdf'
                company.external_report_layout_id = original_layout

            if render_error:
                test.state = 'dnf'
                print(f"[reftest] DNF {index}/{total}: report={test.report_id.display_name} layout={test.layout_id.name} error={render_error!r}")
                continue

            record_name = target_record.display_name
            report_name = test.report_id.display_name
            timestamp = datetime.now().strftime("%y%m%d%H%M%S")

            test.output_pdf_ids = test._create_pdf_attachments(
                report_name,
                record_name,
                timestamp,
                reference_pdf_content,
                pdf_content,
            )

            try:
                reference_preview_content = test._render_first_page_preview(reference_pdf_content, 'reference')
                test_preview_content = test._render_first_page_preview(pdf_content, 'test')
                similarity_score = test._compute_preview_similarity(reference_preview_content, test_preview_content)
                test.preview_image_ids = test._create_preview_attachments(
                    report_name,
                    record_name,
                    timestamp,
                    reference_preview_content,
                    test_preview_content,
                )
                test.similarity_score = similarity_score
                test._post_preview_message(test.preview_image_ids[0], test.preview_image_ids[1], similarity_score)
            except Exception as err:
                test.state = 'dnf'
                test.similarity_score = False
                print(f"[reftest] DNF {index}/{total}: report={test.report_id.display_name} layout={test.layout_id.name} error={err!r}")
                continue

            test.state = 'run'
            print(
                f"[reftest] Completed {index}/{total}: "
                f"report={test.report_id.display_name} layout={test.layout_id.name} similarity={test.similarity_score:.2f}%"
            )
