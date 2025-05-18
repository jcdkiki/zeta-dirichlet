#ifndef ZETA_FRAME_HPP
#define ZETA_FRAME_HPP

#include "plot_panel.hpp"
#include <wx/frame.h>
#include <wx/spinctrl.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>

class ZetaFrame : public wxFrame {
public:
    wxSpinCtrl *n_zeros_ctrl;
    wxSpinCtrl *byte_precision_ctrl;
    PlotPanel  *plot_panel;
    wxTextCtrl *fix_text_ctrl;

    ZetaFrame();

private:
    void OnLoadFile(wxFileDirPickerEvent &event);

    wxDECLARE_EVENT_TABLE();
};

#endif
