from datetime import datetime

from odoo import api, models, fields
from odoo.exceptions import UserError

ENGINES = [
    ('qweb-pdf-wkhtmltopdf', "WkHtmlToPdf"),
    ('qweb-pdf-paper-muncher', "Paper Muncher"),
]
DEFAULT_REFERENCE_ENGINE = 'qweb-pdf-wkhtmltopdf'
DEFAULT_ENGINE = 'qweb-pdf-paper-muncher'
PDF_REPORT_TYPES = {'qweb-pdf', DEFAULT_REFERENCE_ENGINE, DEFAULT_ENGINE}


class Reftest(models.Model):
    _name = "reftest.reftest"
    _description = "Paper Muncher Reftest"

    state = fields.Selection([
        ('draft', 'Draft'),
        ('run', 'Run'),
        ('dnf', 'DNF'),
    ], required=True, default='draft', readonly=True)

    report_id = fields.Many2one("ir.actions.report", required=True, domain=[('report_type', 'in', ['qweb-pdf'] + [x[0] for x in ENGINES])])
    res_model = fields.Char(related='report_id.model')
    res_id = fields.Many2oneReference(required=True,model_field='res_model')
    layout_id = fields.Many2one('report.layout', required=True)
    reference_engine = fields.Selection(ENGINES, required=True)
    engine = fields.Selection(ENGINES, required=True)
    output_pdf_ids = fields.Many2many('ir.attachment', readonly=True)

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

    @api.model
    def _get_run_all_layout(self):
        company_layout = self.env.company.external_report_layout_id
        if company_layout:
            layout = self.env['report.layout'].search([('view_id', '=', company_layout.id)], limit=1)
            if layout:
                return layout
        return self.env['report.layout'].search([], order='sequence, id', limit=1)

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
            return model
        if getattr(model, '_abstract', False):
            return model
        if getattr(model, '_transient', False):
            return model
        return model.with_context(active_test=False).search([], order='id', limit=1)

    @api.model
    def action_run_all(self, *args):
        reftest_model = self.env['reftest.reftest']
        existing_tests = reftest_model.search([])
        existing_attachments = existing_tests.output_pdf_ids
        existing_tests.unlink()
        existing_attachments.unlink()

        layout = self._get_run_all_layout()
        if not layout:
            raise UserError("No report layout is available to run the reftests.")

        values_list = []
        for report in self._get_run_all_reports():
            record = self._get_run_all_record(report)
            if not record:
                continue
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
        tests.run_test()
        return {
            'type': 'ir.actions.client',
            'tag': 'reload',
        }

    def run_test(self):
        for test in self:
            company = test._get_test_company()
            report_action = test.report_id.with_context(test._get_report_context(company))
            original_layout = company.external_report_layout_id
            original_report_type = test.report_id.report_type
            reference_pdf_content = False
            pdf_content = False

            try:
                company.external_report_layout_id = test.layout_id.view_id

                test.report_id.report_type = test.reference_engine
                reference_pdf_content, _ = self.env['ir.actions.report']._render_qweb_pdf(report_action, res_ids=[test.res_id])
                test.report_id.report_type = test.engine
                pdf_content, _ = self.env['ir.actions.report']._render_qweb_pdf(report_action, res_ids=[test.res_id])
            except Exception:
                test.state = 'dnf'
            finally:
                test.report_id.report_type = original_report_type or 'qweb-pdf'
                company.external_report_layout_id = original_layout

            if not reference_pdf_content or not pdf_content:
                continue

            record_name = self.env[test.res_model].browse(test.res_id).display_name
            report_name = test.report_id.display_name
            timestamp = datetime.now().strftime("%y%m%d%H%M%S")

            test.output_pdf_ids = self.env['ir.attachment'].create([
                {
                    'name': f'reference_{report_name}_{record_name}_{timestamp}.pdf',
                    'raw': reference_pdf_content,
                },
                {
                    'name': f'test_{report_name}_{record_name}_{timestamp}.pdf',
                    'raw': pdf_content,
                },
            ])
            test.state = 'run'
