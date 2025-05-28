#include "zeta_frame.hpp"
#include "dirichlet_series.hpp"
#include "flint_wrappers/complex.hpp"
#include "nested_systems_solver.hpp"
#include "plot_panel.hpp"
#include "reader.hpp"
#include "utils.hpp"

#include <sstream>
#include <wx/gbsizer.h>
#include <wx/stattext.h>

static constexpr int ZETA_FRAME_STYLE = wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX);

enum { ID_LoadFile = 1, ID_NZeros, ID_BytePrecision, ID_CoeffsChoice, ID_FitButton };

wxBEGIN_EVENT_TABLE(ZetaFrame, wxFrame) EVT_FILEPICKER_CHANGED(ID_LoadFile, ZetaFrame::OnLoadFile)
    EVT_CHOICE(ID_CoeffsChoice, ZetaFrame::OnCoeffsChoice)
        EVT_BUTTON(ID_FitButton, ZetaFrame::OnFitButton) wxEND_EVENT_TABLE();

ZetaFrame::ZetaFrame()
    : wxFrame(nullptr, wxID_ANY, "zeta-dirichlet", wxDefaultPosition, wxDefaultSize,
              ZETA_FRAME_STYLE)
{
    CreateStatusBar();
    SetStatusText("Welcome to zeta-dirichlet!");

    wxGridBagSizer *sizer = new wxGridBagSizer();

    wxFilePickerCtrl *file_picker = new wxFilePickerCtrl(
        this, ID_LoadFile, wxEmptyString, "Open zeros.val file", wxFileSelectorDefaultWildcardStr,
        wxDefaultPosition, wxSize(300, -1), wxFLP_OPEN | wxFLP_FILE_MUST_EXIST);

    n_zeros_ctrl = new wxSpinCtrl(this, ID_NZeros, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                  wxSP_ARROW_KEYS, 1, INT_MAX, 50);

    byte_precision_ctrl = new wxSpinCtrl(this, ID_BytePrecision, wxEmptyString, wxDefaultPosition,
                                         wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, 2048);

    choicebook = new wxChoicebook(this, wxID_ANY);
    coefficients_plot = new CoefficientsPlotPanel(choicebook, nullptr);
    errors_plot = new CoefficientsPlotPanel(choicebook, nullptr);
    series_plot = new DynamicPlotPanel(choicebook, nullptr);
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

double GetSeriesY(double x, DirichletSeries *series)
{
    flint::Complex X(x), Y;
    series->calculate(Y, X, 2048);
    return arf_get_d(&Y.real().mid(), ARF_RND_NEAR);
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
            ss >> std::ws >> a >> idx >> std::ws >> equals >> std::ws >> lparen >> std::ws >>
                real >> std::ws >> comma >> std::ws >> imag >> std::ws >> rparen;

            if (ss.fail() || a != 'a' || equals != '=' || lparen != '(' || comma != ',' ||
                rparen != ')') {
                SetStatusText("Failed to parse: " + line);
                return;
            }

            fixed_coeficients.push_back(coefficient(idx, real, imag));
        }

        flint::Vector zeros(2 * n_zeros);
        read_zeros(zeros, event.GetPath().c_str(), n_zeros, byte_precision);

        NestedSystemsSolver solver(fixed_coeficients, zeros, byte_precision);
        solver.slow_solve_all();

        coeffs_choice->Clear();
        coeffs_choice->Append("None");
        coefficients.clear();

        for (int i = 0; i < n_zeros; i++) {
            flint::Vector acb_coefficients = solver.get_coefs_vector(i);
            std::vector<double> double_coefficients;
            std::vector<double> double_errors;
            for (int j = 0; j < acb_coefficients.size(); j++) {
                double c = arf_get_d(&acb_coefficients[j].real().mid(), ARF_RND_NEAR);
                mag_log(&acb_coefficients[j].real().rad(), &acb_coefficients[j].real().rad());
                double e = mag_get_d(&acb_coefficients[j].real().rad());
                double_coefficients.push_back(c);
                double_errors.push_back(e);
            }

            this->acb_coefficients[i + 1] = acb_coefficients;

            if (!isnan(double_coefficients[0])) {
                coefficients[i + 1] = std::move(double_coefficients);
                errors[i + 1] = std::move(double_errors);
                coeffs_choice->Append(wxString::Format("%d zeros", i + 1));
            }
        }

        coeffs_choice->SetSelection(coeffs_choice->GetCount() - 1);
        coefficients_plot->SetCoefficients(coefficients.rbegin()->second);
        errors_plot->SetCoefficients(errors.rbegin()->second);

        if (series_plot->user_ptr != nullptr) {
            delete ((DirichletSeries *)series_plot->user_ptr);
        }
        series_plot->user_ptr = new DirichletSeries(acb_coefficients.rbegin()->second);
        series_plot->func = (DynamicPlotPanel::Func)GetSeriesY;
    }
    catch (std::exception &e) {
        SetStatusText(e.what());
        return;
    }

    SetStatusText("Everything done!");
}

void ZetaFrame::OnCoeffsChoice(wxCommandEvent &event)
{
    if (series_plot->user_ptr != nullptr) {
        delete (DirichletSeries *)series_plot->user_ptr;
    }

    int selection = event.GetSelection();
    if (selection == 0) {
        coefficients_plot->SetCoefficients(std::vector<double>());
        errors_plot->SetCoefficients(std::vector<double>());

        series_plot->func = nullptr;
        return;
    }

    int n_zeros;
    sscanf(event.GetString().c_str(), "%d", &n_zeros);

    coefficients_plot->SetCoefficients(coefficients[n_zeros]);
    errors_plot->SetCoefficients(errors[n_zeros]);
    series_plot->user_ptr = new DirichletSeries(acb_coefficients[n_zeros]);
    series_plot->func = (DynamicPlotPanel::Func)GetSeriesY;
}

void ZetaFrame::OnFitButton(wxCommandEvent &event)
{
    coefficients_plot->FitPlot();
    errors_plot->FitPlot();
}
