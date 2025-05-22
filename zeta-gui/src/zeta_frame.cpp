#include "zeta_frame.hpp"
#include "reader.hpp"
#include "plot_panel.hpp"
#include "utils.hpp"
#include "nested_systems_solver.hpp"

#include <sstream>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>

enum {
    ID_LoadFile = 1,
    ID_NZeros,
    ID_BytePrecision,
    ID_CoeffsChoice,
};

wxBEGIN_EVENT_TABLE(ZetaFrame, wxFrame)
    EVT_FILEPICKER_CHANGED(ID_LoadFile, ZetaFrame::OnLoadFile)
    EVT_CHOICE(ID_CoeffsChoice, ZetaFrame::OnCoeffsChoice)
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

    coeffs_choice = new wxChoice(this, ID_CoeffsChoice);
    coeffs_choice->Append("None");
    coeffs_choice->SetSelection(0);
    upper_sizer->Add(coeffs_choice);

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
    try {
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

        NestedSystemsSolver solver(fixed_coeficients, zeros, byte_precision);
        solver.qr_solve_all();
        
        coeffs_choice->Clear();
        coeffs_choice->Append("None");
        coefficients.clear();

        for (int i = 0; i < n_zeros; i++) {
            coefficients[i + 1] = solver.get_coefs_vector(i);
            coeffs_choice->Append(wxString::Format("%d zeros", i + 1)); 
        }

        coeffs_choice->SetSelection(coeffs_choice->GetCount() - 1);
        plot_panel->SetCoeffs(coefficients[n_zeros]);
    }
    catch (std::exception &e) {
        SetStatusText(e.what());
        return;
    }

    SetStatusText("Everything done!");

    // конечный ряд с заданными коэффициентами
    // DirichletSeries series(ns_solver.get_coefs_vector(0));
    // пример вычисления ряда в точке
    // можно расскоментировать
    // acb_t r;
    // acb_init(r);

    // acb_t X;
    // acb_init(X);
    // acb_set_d_d(X, 1.0, 0.0);
    // series.calculate(r, X, BYTE_PRECISION);
    // acb_printd(r, N_PRECISION);
    // std::cout<<std::endl;

    // acb_clear(r);
    // acb_clear(X);
}

void ZetaFrame::OnCoeffsChoice(wxCommandEvent &event)
{
    int selection = event.GetSelection();
    if (selection == 0) {
        plot_panel->SetCoeffs(acb::Vector());
    }

    plot_panel->SetCoeffs(coefficients[selection]);
}
