#include "shared.hpp"
#include "zeta_frames.h"
#include "utils.hpp"
#include <reader.hpp>
#include <nested_systems_solver.hpp>
#include <sstream>

#ifndef ZETA_WIN32
#include <thread>
#endif

void ComputeAsync()
{
    // TODO: fast vs slow mode
    frames.main->SetStatusText("Processing...");
    frames.new_file->Enable(false);
    frames.main->Enable(false);
    frames.rendering_settings->Enable(false);

    int matsize = frames.new_file->matsize_ctrl->GetValue();
    int byte_precision = frames.new_file->precision_ctrl->GetValue();

    try {
        std::vector<coefficient> fixed_coeficients;
        for (int i = 0; i < frames.new_file->coeffs_text->GetNumberOfLines(); i++) {
            wxString line = frames.new_file->coeffs_text->GetLineText(i);
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
                frames.main->SetStatusText("Failed to parse: " + line);
                return;
            }

            fixed_coeficients.push_back(coefficient(idx, real, imag));
        }

        slong n_zeros = matsize / 2 + 1;
        flint::Vector zeros(2 * n_zeros);
        read_zeros(zeros, frames.new_file->file_picker->GetPath().c_str(), n_zeros, byte_precision);

        NestedSystemsSolver solver(matsize, fixed_coeficients, zeros, byte_precision);
        if (frames.new_file->method_choice->GetStringSelection() == "Slow")
            solver.slow_solve_all();
        else
            solver.optimized_lu_solve_all();

        frames.main->solution_choice->Clear();
        frames.main->solution_choice->Append("None");
        coefficients.clear();

        for (int i = 0; i < n_zeros; i++) {
            flint::Vector &solution = solver.get_coefs_vector(i);
            coefficients[i + 1] = solution;
            frames.main->solution_choice->Append(wxString::Format("%d zeros", i + 1));
        }


        frames.main->solution_choice->SetSelection(frames.main->solution_choice->GetCount() - 1);
        selected_solution = coefficients.rbegin()->first;
    }
    catch (std::exception &e) {
        frames.main->SetStatusText(e.what());
        frames.new_file->Enable(true);
        frames.main->Enable(true);
        frames.rendering_settings->Enable(true);
        return;
    }

    frames.main->SetStatusText("Done!");
    frames.new_file->Enable(true);
    frames.main->Enable(true);
    frames.rendering_settings->Enable(true);
}

void NewFileFrame::Compute(wxCommandEvent& event)
{
    if (file_picker->GetPath().empty()) {
        frames.main->SetStatusText("Select file.");
        return;
    }

#ifndef ZETA_WIN32
    std::thread t(ComputeAsync);
    t.detach();
#else
    ComputeAsync();
#endif
}

void NewFileFrame::OnClose( wxCloseEvent& event )
{
    if(event.CanVeto())
    {
        Show(false);
        event.Veto();
    }
}
