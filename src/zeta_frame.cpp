#include "zeta_frame.hpp"
#include "reader.hpp"
#include "solver.hpp"
#include "plot_panel.hpp"
#include "utils.hpp"

#include <sstream>
#include <wx/gdicmn.h>
#include <wx/glcanvas.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>

enum {
    ID_LoadFile = 1,
    ID_NZeros,
    ID_BytePrecision
};

wxBEGIN_EVENT_TABLE(ZetaFrame, wxFrame)
    EVT_FILEPICKER_CHANGED(ID_LoadFile, ZetaFrame::OnLoadFile)
wxEND_EVENT_TABLE();

ZetaFrame::ZetaFrame() : wxFrame(nullptr, wxID_ANY, "zeta-dirichlet")
{
    CreateStatusBar();
    SetStatusText("Welcome to zeta-dirichlet!");
    
    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *upper_sizer = new wxBoxSizer(wxHORIZONTAL);
    main_sizer->Add(upper_sizer);

    wxFilePickerCtrl *file_picker = new wxFilePickerCtrl(this,
        ID_LoadFile, wxEmptyString, "Open zeros.val file",
        wxFileSelectorDefaultWildcardStr, wxDefaultPosition,
        wxSize(300, -1), wxFLP_OPEN | wxFLP_FILE_MUST_EXIST 
    );
    upper_sizer->Add(file_picker);
    
    n_zeros_ctrl = new wxSpinCtrl(this,
        ID_NZeros, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 1, INT_MAX, 50
    );
    upper_sizer->Add(n_zeros_ctrl);

    byte_precision_ctrl = new wxSpinCtrl(this,
        ID_BytePrecision, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 1, INT_MAX, 2048
    );
    upper_sizer->Add(byte_precision_ctrl);

    plot_panel = new PlotPanel(this, nullptr);
    main_sizer->Add(plot_panel);

    wxBoxSizer *lower_sizer = new wxBoxSizer(wxHORIZONTAL);
    main_sizer->AddSpacer(10);
    main_sizer->Add(lower_sizer);

    fix_text_ctrl = new wxTextCtrl(this,
        wxID_ANY, "a0 = (1, 0)", wxDefaultPosition, wxSize(-1, 100),
        wxTE_MULTILINE | wxVSCROLL | wxTE_DONTWRAP
    );
    lower_sizer->AddSpacer(10);
    lower_sizer->Add(fix_text_ctrl);
    lower_sizer->AddSpacer(30);
    lower_sizer->Add(new wxStaticText(this, wxID_ANY, "..."));
    
    SetSizerAndFit(main_sizer);
}

void ZetaFrame::OnLoadFile(wxFileDirPickerEvent &event)
{
    int n_zeros = n_zeros_ctrl->GetValue();
    int byte_precision = byte_precision_ctrl->GetValue();

    std::vector<coefficient> fixed_coeficients;
    for (int i = 0; i < fix_text_ctrl->GetNumberOfLines(); i++) {
        wxString line = fix_text_ctrl->GetLineText(i);
        if (line[0] == '#' || line.empty())
            continue;

        char a, equals, lparen, rparen, comma;
        int idx;
        double real, imag;

        std::stringstream ss;
        ss << line.c_str();
        ss >> std::ws >> a >> idx >> std::ws >> equals >> std::ws >> lparen
           >> std::ws >> real >> std::ws >> comma >> std::ws >> imag >> std::ws >> rparen; 

        if (ss.fail() || a != 'a' || equals != '=' || lparen != '(' || comma != ',' || rparen != ')') {
            SetStatusText("Failed to parse: " + line);
            return;
        }

        fixed_coeficients.push_back(coefficient(idx, real, imag));
    }

    acb::Vector zeros(2 * n_zeros);
    read_zeros(zeros, event.GetPath().c_str(), n_zeros, byte_precision);

    slong system_size = 2 * n_zeros + fixed_coeficients.size();

    acb_matrix res(system_size, 1);
    solve(res, zeros, fixed_coeficients, byte_precision);

    plot_panel->SetCoeffs(acb::Vector(res(0), system_size));

    SetStatusText("Everything done!");
}
