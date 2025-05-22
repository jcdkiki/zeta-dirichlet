#ifndef ZETA_FRAME_HPP
#define ZETA_FRAME_HPP

#include "plot_panel.hpp"

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/spinctrl.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>
#include <wx/choice.h>

#include <unordered_map>

class ZetaFrame : public wxFrame {
private:
    wxSpinCtrl *n_zeros_ctrl;
    wxSpinCtrl *byte_precision_ctrl;
    PlotPanel  *plot_panel;
    wxTextCtrl *fix_text_ctrl;
    wxChoice   *coeffs_choice;

    std::unordered_map<int, acb::Vector> coefficients;

    wxDECLARE_EVENT_TABLE();

    void OnLoadFile(wxFileDirPickerEvent &event);
    void OnCoeffsChoice(wxCommandEvent &event);

public:
    ZetaFrame();
};

#endif
