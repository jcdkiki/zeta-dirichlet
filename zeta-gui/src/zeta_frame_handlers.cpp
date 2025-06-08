/*
#include <GL/gl.h>
#include "zeta_frame.hpp"
#include "flint_wrappers/vector.hpp"
#include "nested_systems_solver.hpp"
#include "plot_panel.hpp"
#include "reader.hpp"
#include "utils.hpp"

#include <sstream>
#include <wx/gbsizer.h>
#include <wx/generic/numdlgg.h>
#include <wx/glcanvas.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/numdlg.h>

static constexpr int ZETA_FRAME_STYLE = wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX);

wxBEGIN_EVENT_TABLE(ZetaFrame, wxFrame) EVT_FILEPICKER_CHANGED(ID_LoadFile, ZetaFrame::OnLoadFile)
    EVT_CHOICE(ID_CoeffsChoice, ZetaFrame::OnCoeffsChoice)
    EVT_BUTTON(ID_FitButton, ZetaFrame::OnFitButton)
    EVT_MENU(ID_LineQuality_Low, ZetaFrame::LineQuality_Low)
    EVT_MENU(ID_LineQuality_Mid, ZetaFrame::LineQuality_Mid)
    EVT_MENU(ID_LineQuality_Best, ZetaFrame::LineQuality_Best)
    EVT_MENU(ID_LineQuality_Custom, ZetaFrame::LineQuality_Custom)
    EVT_MENU(ID_EnableMultisampling, ZetaFrame::EnableMultisampling)
wxEND_EVENT_TABLE();

ZetaFrame::ZetaFrame()
    : wxFrame(nullptr, wxID_ANY, "zeta-dirichlet", wxDefaultPosition, wxDefaultSize,
              ZETA_FRAME_STYLE)
{
    CreateStatusBar();
    SetStatusText("Welcome to zeta-dirichlet!");
#ifdef ZETA_WIN32
    SetIcon(wxICON(APPICON));
#endif

    menu = new wxMenuBar();
    coeff_menu = new wxMenu();
    coeff_menu->Append(ID_NewCoefficients, "New", "Compute new coefficients from .val file");
    coeff_menu->Append(ID_SaveCoefficients, "Save", "Save coefficient to .bin file");
    coeff_menu->Append(ID_LoadCoefficients, "Load", "Load coefficents from .bin file");
    settings_menu = new wxMenu();
    settings_menu->AppendCheckItem(ID_ShowLegend, "Legend", "Show legend");
    settings_menu->AppendCheckItem(ID_EnableMultisampling, "Multisampling", "Enable multisampling for line and point rendering");
    line_submenu = new wxMenu();
    line_submenu->AppendRadioItem(ID_LineQuality_Low, "Low (15px)", "Low quality line rendering");
    line_submenu->AppendRadioItem(ID_LineQuality_Mid, "Mid (5px)", "Mid quality line rendering");
    line_submenu->AppendRadioItem(ID_LineQuality_Best, "Best (1px)", "Best quality line rendering");
    line_submenu->AppendRadioItem(ID_LineQuality_Custom, "Custom...", "Custom quality line rendering");
    line_submenu->Check(ID_LineQuality_Mid, true);
    settings_menu->AppendSubMenu(line_submenu, "Line rendering quality");
    settings_menu->Append(wxID_ANY, "More...", "Bit precision, colors, etc...");
    menu->Append(coeff_menu, "Coefficients");
    menu->Append(settings_menu, "Settings");
    SetMenuBar(menu);


    wxGridBagSizer *sizer = new wxGridBagSizer();

    wxFilePickerCtrl *file_picker = new wxFilePickerCtrl(
        this, ID_LoadFile, wxEmptyString, "Open zeros.val file", wxFileSelectorDefaultWildcardStr,
        wxDefaultPosition, wxSize(300, -1), wxFLP_OPEN | wxFLP_FILE_MUST_EXIST);

    n_zeros_ctrl = new wxSpinCtrl(this, ID_NZeros, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                  wxSP_ARROW_KEYS, 1, INT_MAX, 50);

    byte_precision_ctrl = new wxSpinCtrl(this, ID_BytePrecision, wxEmptyString, wxDefaultPosition,
                                         wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, 2048);

    choicebook = new wxChoicebook(this, wxID_ANY);
    coefficients_plot = new CoefficientsPlotPanel(choicebook);
    errors_plot = new CoefficientsPlotPanel(choicebook);
    series_plot = new SeriesPlotPanel(choicebook);
    choicebook->AddPage(coefficients_plot, "Coefficients");
    choicebook->AddPage(errors_plot, "Errors");
    choicebook->AddPage(series_plot, "Series");

    fix_text_ctrl = new wxTextCtrl(this, wxID_ANY, "a0 = (1, 0)", wxDefaultPosition,
                                   wxSize(-1, 100), wxTE_MULTILINE | wxVSCROLL | wxTE_DONTWRAP);

    coeffs_choice = new wxChoice(this, ID_CoeffsChoice);
    coeffs_choice->Append("None");
    coeffs_choice->SetSelection(0);

    wxButton *fit_button = new wxButton(this, ID_FitButton, "Fit");

    // SIZER
    sizer->Add(file_picker, wxGBPosition(0, 0), wxDefaultSpan, wxEXPAND);
    sizer->Add(n_zeros_ctrl, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND);
    sizer->Add(byte_precision_ctrl, wxGBPosition(0, 2), wxDefaultSpan, wxEXPAND);

    sizer->Add(fix_text_ctrl, wxGBPosition(1, 0), wxDefaultSpan, wxEXPAND);
    sizer->Add(choicebook, wxGBPosition(1, 1), wxGBSpan(1, 2), wxEXPAND);

    sizer->Add(coeffs_choice, wxGBPosition(2, 0), wxDefaultSpan, wxEXPAND);
    sizer->Add(fit_button, wxGBPosition(2, 1), wxDefaultSpan);

    SetSizerAndFit(sizer);
}

void ZetaFrame::OnFitButton(wxCommandEvent &event)
{
    coefficients_plot->FitPlot();
    errors_plot->FitPlot();
}
*/

#include "flint_wrappers/vector.hpp"
#include "shared.hpp"
#include "zeta_frames.h"
#include <flint/arf.h>
#include <flint/mag.h>
#include <wx/filedlg.h>

void ZetaFrame::NewFile( wxCommandEvent& event )
{
    frames.new_file->Show(true);
    frames.new_file->SetFocus();
}

void ZetaFrame::SaveFile( wxCommandEvent& event )
{
    wxFileDialog dialog(this, _("Save BIN file"), "", "", "BIN files (*.bin)|*.bin", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() == wxID_CANCEL)
        return;
    
    FILE *fp = fopen(dialog.GetPath(), "wb");
    if (fp == nullptr) {
        return;
    }

    int n = coefficients.size();
    fwrite(&n, 1, sizeof(n), fp);
    for (auto &pair : coefficients) {
        int nzeros = pair.first;
        flint::Vector &vector = pair.second;
        int vector_size = vector.size();

        fwrite(&nzeros, 1, sizeof(nzeros), fp);
        fwrite(&vector_size, 1, sizeof(vector_size), fp);
        for (int i = 0; i < vector_size; i++) {
            arf_dump_file(fp, &vector[i].real().mid());
            fputc(' ', fp);
            mag_dump_file(fp, &vector[i].real().rad());
            fputc(' ', fp);
            arf_dump_file(fp, &vector[i].imag().mid());
            fputc(' ', fp);
            mag_dump_file(fp, &vector[i].imag().rad());
            fputc(' ', fp);
        }
    }

    fclose(fp);
}

void ZetaFrame::LoadFile( wxCommandEvent& event )
{
    wxFileDialog dialog(this, _("Load BIN file"), "", "", "BIN files (*.bin)|*.bin", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_CANCEL)
        return;

    FILE *fp = fopen(dialog.GetPath(), "rb");
    if (fp == nullptr) {
        return;
    }

    selected_solution = -1;
    coefficients.clear();

    int n;
    fread(&n, 1, sizeof(n), fp);
    for (int i = 0; i < n; i++) {
        int nzeros, vector_size;
        fread(&nzeros, 1, sizeof(nzeros), fp);
        fread(&vector_size, 1, sizeof(vector_size), fp);

        coefficients[nzeros] = flint::Vector(vector_size);
        for (int j = 0; j < vector_size; j++) {
            arf_load_file(&coefficients[nzeros][j].real().mid(), fp);
            fgetc(fp); // whitespace
            mag_load_file(&coefficients[nzeros][j].real().rad(), fp);
            fgetc(fp); // whitespace
            arf_load_file(&coefficients[nzeros][j].imag().mid(), fp);
            fgetc(fp); // whitespace
            mag_load_file(&coefficients[nzeros][j].imag().rad(), fp);
            fgetc(fp); // whitespace
        }
    }

    selected_solution = coefficients.rbegin()->first;

    fclose(fp);
}

void ZetaFrame::RenderingSettings( wxCommandEvent& event )
{
    frames.rendering_settings->Show(true);
    frames.rendering_settings->SetFocus();
}

void ZetaFrame::SolutionChoice( wxCommandEvent& event )
{
    int selection = event.GetSelection();
    if (selection == 0) {
        selected_solution = -1;
        return;
    }

    int n_zeros;
    sscanf(event.GetString().c_str(), "%d", &n_zeros);
    selected_solution = n_zeros;
}

void ZetaFrame::FitPlot( wxCommandEvent& event )
{
    coefficients_plot->FitPlot();
    errors_plot->FitPlot();
}

void ZetaFrame::OnClose( wxCloseEvent& event )
{
    frames.new_file->Close();
    frames.rendering_settings->Close();
    wxWindow::Close();
}
