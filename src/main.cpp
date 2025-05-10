#include <filesystem>
#include <wx/app.h>
#include <wx/filepicker.h>
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>

#include "reader.hpp"
#include "solver.hpp"

static constexpr int N_PRECISION = 20;
static constexpr int BYTE_PRECISION = 2048;
static constexpr int N_ZEROS = 100;

class MyApp : public wxApp {
  public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame {
  public:
    MyFrame();

  private:
    void OnLoadFile(wxFileDirPickerEvent &event);

    wxDECLARE_EVENT_TABLE();
};

enum { ID_LoadFile = 1 };

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_FILEPICKER_CHANGED(ID_LoadFile, MyFrame::OnLoadFile)
        wxEND_EVENT_TABLE();

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "zeta-dirichlet")
{
    CreateStatusBar();
    SetStatusText("Welcome to zeta-dirichlet!");

    wxPanel *panel = new wxPanel(this, wxID_ANY);
    wxFilePickerCtrl *file_picker = new wxFilePickerCtrl(panel, ID_LoadFile);
    file_picker->SetSize(300, -1);
}

void MyFrame::OnLoadFile(wxFileDirPickerEvent &event)
{
    acb_vector zeros(N_ZEROS);

    read_zeros(zeros, path, N_ZEROS, BYTE_PRECISION);

    // print_zeros(zeros, N_ZEROS, N_PRECISION);

    acb_mat_t matrix;

    slong system_size = 2 * N_ZEROS + 1;

    acb_mat_init(matrix, system_size, system_size);

    make_matrix(matrix, system_size, zeros, BYTE_PRECISION);

    solve(matrix, system_size, BYTE_PRECISION);

    acb_mat_clear(matrix);

    SetStatusText("Everything done!");
}
