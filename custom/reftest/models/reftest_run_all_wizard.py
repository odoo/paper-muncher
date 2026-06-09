from odoo import api, fields, models
from odoo.exceptions import UserError

from .reftest_reftest import DEFAULT_ENGINE, DEFAULT_REFERENCE_ENGINE, PDF_REPORT_TYPES


class ReftestRunAllWizard(models.TransientModel):
    _name = 'reftest.run.all.wizard'
    _description = 'Reftest Run All Wizard'

    layout_scope = fields.Selection([
        ('all', 'All layouts'),
        ('selected', 'Selected layouts'),
    ], required=True, default='all')
    layout_ids = fields.Many2many('report.layout', string='Layouts')

    report_scope = fields.Selection([
        ('all', 'All runnable reports'),
        ('selected', 'Selected reports'),
    ], required=True, default='all')
    report_ids = fields.Many2many(
        'ir.actions.report',
        string='Reports',
        domain=[('report_type', 'in', list(PDF_REPORT_TYPES))],
    )

    clear_previous = fields.Boolean(string='Clear previous reftests', default=True)
    reference_engine = fields.Selection(selection='_selection_engines', required=True, default=DEFAULT_REFERENCE_ENGINE)
    engine = fields.Selection(selection='_selection_engines', required=True, default=DEFAULT_ENGINE)

    @api.model
    def _selection_engines(self):
        return self.env['reftest.reftest']._fields['engine'].selection

    def action_run(self):
        self.ensure_one()
        reftest_model = self.env['reftest.reftest']
        layout_model = self.env['report.layout']

        if self.reference_engine == self.engine:
            raise UserError('Reference engine and test engine must be different.')

        if self.layout_scope == 'all':
            layouts = reftest_model._get_run_all_layouts()
        else:
            layouts = layout_model.browse(self.layout_ids.ids)

        if not layouts:
            raise UserError('Please select at least one layout.')

        if self.report_scope == 'all':
            reports = reftest_model._get_run_all_reports()
        else:
            selected_report_ids = set(self.report_ids.ids)
            runnable_report_ids = set(reftest_model._get_run_all_reports().ids)
            report_ids = runnable_report_ids.intersection(selected_report_ids)
            reports = self.env['ir.actions.report'].browse(list(report_ids))

        if not reports:
            raise UserError('Please select at least one runnable report.')

        if self.clear_previous:
            existing_tests = reftest_model.search([])
            existing_attachments = existing_tests.output_pdf_ids | existing_tests.preview_image_ids
            existing_tests.unlink()
            existing_attachments.unlink()

        values_list = reftest_model._prepare_run_all_values(
            reports,
            layouts,
            reference_engine=self.reference_engine,
            engine=self.engine,
        )
        if not values_list:
            raise UserError('No runnable reports with demo data were found for this selection.')

        tests = reftest_model.create(values_list)
        print(
            f"[reftest] Starting Run All with {len(tests)} tests across "
            f"{len(reports)} reports and {len(layouts)} layouts"
        )
        tests.run_test()
        return {'type': 'ir.actions.client', 'tag': 'reload'}
