#ifndef ZETA_FRAME_HPP
#define ZETA_FRAME_HPP

#include <flint_wrappers/vector.hpp>
#include "plot_panel.hpp"

#include <wx/choice.h>
#include <wx/choicebk.h>
#include <wx/event.h>
#include <wx/filepicker.h>
#include <wx/frame.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>

#include <map>

class ZetaFrame : public wxFrame {
  private:
    wxSpinCtrl *n_zeros_ctrl;
    wxSpinCtrl *byte_precision_ctrl;
    wxTextCtrl *fix_text_ctrl;
    wxChoice *coeffs_choice;
    wxChoicebook *choicebook;

    CoefficientsPlotPanel *coefficients_plot;
    CoefficientsPlotPanel *errors_plot;
    DynamicPlotPanel *series_plot;

    std::map<int, flint::Vector> acb_coefficients;
    std::map<int, std::vector<double>> coefficients;
    std::map<int, std::vector<double>> errors;

    wxDECLARE_EVENT_TABLE();

    void OnLoadFile(wxFileDirPickerEvent &event);
    void OnCoeffsChoice(wxCommandEvent &event);
    void OnFitButton(wxCommandEvent &event);

  public:
    ZetaFrame();
};

#endif
