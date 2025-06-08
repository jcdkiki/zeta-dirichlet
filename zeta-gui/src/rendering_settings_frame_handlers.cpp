#include "plot_panel.hpp"
#include "zeta_frames.h"
#include "shared.hpp"
#include <wx/gdicmn.h>
#include <wx/glcanvas.h>

void RenderingSettingsFrame::Apply(wxCommandEvent& event)
{
    if (check_multisample->IsChecked()) {
        rendering_settings.gl_attribs[0] = WX_GL_SAMPLES;
        rendering_settings.gl_attribs[1] = samples_ctrl->GetValue();
        rendering_settings.gl_attribs[2] = 0;
    }
    else {
        rendering_settings.gl_attribs[0] = 0;
    }
    
    rendering_settings.show_legend = check_legend->IsChecked();
    rendering_settings.line_length = line_ctrl->GetValue();

    Vec2d series_pos = frames.main->series_plot->pos;
    double series_zoom = frames.main->series_plot->zoom;
    double series_y_zoom = frames.main->series_plot->y_zoom;
    
    Vec2d coeffs_pos = frames.main->coefficients_plot->pos;
    double coeffs_zoom = frames.main->coefficients_plot->zoom;
    double coeffs_y_zoom = frames.main->coefficients_plot->y_zoom;
    
    Vec2d errors_pos = frames.main->errors_plot->pos;
    double errors_zoom = frames.main->errors_plot->zoom;
    double errors_y_zoom = frames.main->errors_plot->y_zoom;
    
    frames.main->series_plot->Destroy();
    frames.main->series_plot = new SeriesPlotPanel(frames.main->series_pane, wxID_ANY, wxDefaultPosition, wxSize(640, 480), 0);
    frames.main->series_sizer->Add(frames.main->series_plot);
    frames.main->series_plot->pos = series_pos;
    frames.main->series_plot->zoom = series_zoom;
    frames.main->series_plot->y_zoom = series_y_zoom;
    
    frames.main->coefficients_plot->Destroy();
    frames.main->coefficients_plot = new CoefficientsPlotPanel(frames.main->coefficients_pane, wxID_ANY, wxDefaultPosition, wxSize(640, 480), 0);
    frames.main->coefficients_sizer->Add(frames.main->coefficients_plot);
    frames.main->coefficients_plot->pos = coeffs_pos;
    frames.main->coefficients_plot->zoom = coeffs_zoom;
    frames.main->coefficients_plot->y_zoom = coeffs_y_zoom;

    frames.main->errors_plot->Destroy();
    frames.main->errors_plot = new ErrorsPlotPanel(frames.main->errors_pane, wxID_ANY, wxDefaultPosition, wxSize(640, 480), 0);
    frames.main->errors_sizer->Add(frames.main->errors_plot);
    frames.main->errors_plot->pos = errors_pos;
    frames.main->errors_plot->zoom = errors_zoom;
    frames.main->errors_plot->y_zoom = errors_y_zoom;
}

void RenderingSettingsFrame::OnClose(wxCloseEvent& event)
{
    if(event.CanVeto())
    {
        Show(false);
        event.Veto();
    }
}
