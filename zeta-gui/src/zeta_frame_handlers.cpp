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

    solution_choice->Clear();
    solution_choice->Append("None");
    for (auto &pair : coefficients) {
        solution_choice->Append(wxString::Format("%d zeros", pair.first));
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
