///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/button.h>
#include "plot_panel.hpp"
#include <wx/frame.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class ZetaFrame
///////////////////////////////////////////////////////////////////////////////
class ZetaFrame : public wxFrame
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
		void _wxFB_NewFile( wxCommandEvent& event ){ NewFile( event ); }
		void _wxFB_SaveFile( wxCommandEvent& event ){ SaveFile( event ); }
		void _wxFB_LoadFile( wxCommandEvent& event ){ LoadFile( event ); }
		void _wxFB_RenderingSettings( wxCommandEvent& event ){ RenderingSettings( event ); }
		void _wxFB_SolutionChoice( wxCommandEvent& event ){ SolutionChoice( event ); }
		void _wxFB_FitPlot( wxCommandEvent& event ){ FitPlot( event ); }


	protected:
		enum
		{
			ID_NewCoefficients = 6000,
			ID_SaveCoefficients,
			ID_LoadCoefficients,
			ID_RenderingSettings,
			ID_SolutionChoice,
			ID_FitPlot,
		};

		wxStatusBar* status;
		wxMenuBar* menubar;
		wxMenu* file_menu;
		wxMenu* settings_menu;
		wxNotebook* notebook;
		wxStaticText* nzeros_label;
		wxButton* fit_button;

		// Virtual event handlers, override them in your derived class
		void OnClose( wxCloseEvent& event );
		void NewFile( wxCommandEvent& event );
		void SaveFile( wxCommandEvent& event );
		void LoadFile( wxCommandEvent& event );
		void RenderingSettings( wxCommandEvent& event );
		void SolutionChoice( wxCommandEvent& event );
		void FitPlot( wxCommandEvent& event );


	public:
		wxPanel* series_pane;
		wxBoxSizer* series_sizer;
		SeriesPlotPanel* series_plot;
		wxPanel* coefficients_pane;
		wxBoxSizer* coefficients_sizer;
		CoefficientsPlotPanel* coefficients_plot;
		wxPanel* errors_pane;
		wxBoxSizer* errors_sizer;
		ErrorsPlotPanel* errors_plot;
		wxChoice* solution_choice;

		ZetaFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Zeta GUI"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxMINIMIZE|wxSYSTEM_MENU|wxTAB_TRAVERSAL );

		~ZetaFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class NewFileFrame
///////////////////////////////////////////////////////////////////////////////
class NewFileFrame : public wxFrame
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
		void _wxFB_Compute( wxCommandEvent& event ){ Compute( event ); }


	protected:
		enum
		{
			ID_Compute = 6000,
		};

		wxStaticText* label_file;
		wxStaticText* label_method;
		wxStaticText* label_matsize;
		wxStaticText* label_precision;
		wxStaticText* label_fixed;
		wxButton* compute_button;

		// Virtual event handlers, override them in your derived class
		void OnClose( wxCloseEvent& event );
		void Compute( wxCommandEvent& event );


	public:
		wxFilePickerCtrl* file_picker;
		wxChoice* method_choice;
		wxSpinCtrl* matsize_ctrl;
		wxSpinCtrl* precision_ctrl;
		wxTextCtrl* coeffs_text;

		NewFileFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New coefficients"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL );

		~NewFileFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class RenderingSettingsFrame
///////////////////////////////////////////////////////////////////////////////
class RenderingSettingsFrame : public wxFrame
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
		void _wxFB_Apply( wxCommandEvent& event ){ Apply( event ); }


	protected:
		enum
		{
			ID_ApplyRenderingSettings = 6000,
		};

		wxCheckBox* check_legend;
		wxCheckBox* check_multisample;
		wxStaticText* label_multisampling;
		wxSpinCtrl* samples_ctrl;
		wxStaticText* label_line;
		wxSpinCtrl* line_ctrl;
		wxButton* apply_button;

		// Virtual event handlers, override them in your derived class
		void OnClose( wxCloseEvent& event );
		void Apply( wxCommandEvent& event );


	public:

		RenderingSettingsFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Rendering Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxMINIMIZE|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL );

		~RenderingSettingsFrame();

};

