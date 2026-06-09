from datetime import datetime

from odoo import api, models, fields

ENGINES = [
    ('qweb-pdf-wkhtmltopdf', "WkHtmlToPdf"),
    ('qweb-pdf-paper-muncher', "Paper Muncher"),
]


class Reftest(models.Model):
    _name = "reftest.reftest"
    _description = "Paper Muncher Reftest"

    state = fields.Selection([
        ('draft', 'Draft'),
        ('run', 'Run'),
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

    def run_test(self):
        for test in self:
            self.env.company.external_report_layout_id = test.layout_id.view_id

            test.report_id.report_type = test.reference_engine
            reference_pdf_content, _ = self.env['ir.actions.report']._render_qweb_pdf(test.report_id, res_ids=[test.res_id])
            test.report_id.report_type = test.engine
            pdf_content, _ = self.env['ir.actions.report']._render_qweb_pdf(test.report_id, res_ids=[test.res_id])
            test.report_id.report_type = 'qweb-pdf'

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
