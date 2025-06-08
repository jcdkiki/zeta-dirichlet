///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "zeta_frames.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ZetaFrame, wxFrame )
	EVT_CLOSE( ZetaFrame::_wxFB_OnClose )
	EVT_MENU( ID_NewCoefficients, ZetaFrame::_wxFB_NewFile )
	EVT_MENU( ID_LoadCoefficients, ZetaFrame::_wxFB_LoadFile )
	EVT_MENU( ID_SaveCoefficients, ZetaFrame::_wxFB_SaveFile )
	EVT_MENU( ID_RenderingSettings, ZetaFrame::_wxFB_RenderingSettings )
	EVT_CHOICE( ID_SolutionChoice, ZetaFrame::_wxFB_SolutionChoice )
	EVT_BUTTON( ID_FitPlot, ZetaFrame::_wxFB_FitPlot )
END_EVENT_TABLE()

ZetaFrame::ZetaFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	status = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
	menubar = new wxMenuBar( 0 );
	file_menu = new wxMenu();
	wxMenuItem* new_file;
	new_file = new wxMenuItem( file_menu, ID_NewCoefficients, wxString( _("New") ) , _("Compute new coefficients"), wxITEM_NORMAL );
	file_menu->Append( new_file );

	wxMenuItem* load_file;
	load_file = new wxMenuItem( file_menu, ID_LoadCoefficients, wxString( _("Load") ) , _("Load coefficients from file"), wxITEM_NORMAL );
	file_menu->Append( load_file );

	wxMenuItem* save_file;
	save_file = new wxMenuItem( file_menu, ID_SaveCoefficients, wxString( _("Save") ) , _("Save coefficients to file"), wxITEM_NORMAL );
	file_menu->Append( save_file );

	menubar->Append( file_menu, _("File") );

	settings_menu = new wxMenu();
	wxMenuItem* rendering_settings;
	rendering_settings = new wxMenuItem( settings_menu, ID_RenderingSettings, wxString( _("Rendering...") ) , _("Change rendering settings for better looks  or better performance"), wxITEM_NORMAL );
	settings_menu->Append( rendering_settings );

	menubar->Append( settings_menu, _("Settings") );

	this->SetMenuBar( menubar );

	wxBoxSizer* main_sizer;
	main_sizer = new wxBoxSizer( wxVERTICAL );

	notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( 640,480 ), wxNB_BOTTOM );
	series_pane = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	series_sizer = new wxBoxSizer( wxVERTICAL );

	series_plot = new SeriesPlotPanel( series_pane, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	series_sizer->Add( series_plot, 1, wxALL|wxEXPAND, 5 );


	series_pane->SetSizer( series_sizer );
	series_pane->Layout();
	series_sizer->Fit( series_pane );
	notebook->AddPage( series_pane, _("Series"), false );
	coefficients_pane = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	coefficients_sizer = new wxBoxSizer( wxVERTICAL );

	coefficients_plot = new CoefficientsPlotPanel( coefficients_pane, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	coefficients_sizer->Add( coefficients_plot, 1, wxALL|wxEXPAND, 5 );


	coefficients_pane->SetSizer( coefficients_sizer );
	coefficients_pane->Layout();
	coefficients_sizer->Fit( coefficients_pane );
	notebook->AddPage( coefficients_pane, _("Coefficients"), false );
	errors_pane = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	errors_sizer = new wxBoxSizer( wxVERTICAL );

	errors_plot = new ErrorsPlotPanel( errors_pane, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	errors_sizer->Add( errors_plot, 1, wxALL|wxEXPAND, 5 );


	errors_pane->SetSizer( errors_sizer );
	errors_pane->Layout();
	errors_sizer->Fit( errors_pane );
	notebook->AddPage( errors_pane, _("Errors"), false );

	main_sizer->Add( notebook, 1, wxEXPAND | wxALL, 5 );

	wxBoxSizer* lower_sizer;
	lower_sizer = new wxBoxSizer( wxHORIZONTAL );

	nzeros_label = new wxStaticText( this, wxID_ANY, _("Solution"), wxDefaultPosition, wxDefaultSize, 0 );
	nzeros_label->Wrap( -1 );
	lower_sizer->Add( nzeros_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString solution_choiceChoices[] = { _("None") };
	int solution_choiceNChoices = sizeof( solution_choiceChoices ) / sizeof( wxString );
	solution_choice = new wxChoice( this, ID_SolutionChoice, wxDefaultPosition, wxDefaultSize, solution_choiceNChoices, solution_choiceChoices, 0 );
	solution_choice->SetSelection( 0 );
	lower_sizer->Add( solution_choice, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	fit_button = new wxButton( this, ID_FitPlot, _("Fit"), wxDefaultPosition, wxDefaultSize, 0 );
	lower_sizer->Add( fit_button, 0, wxALL, 5 );


	main_sizer->Add( lower_sizer, 0, wxEXPAND, 5 );


	this->SetSizer( main_sizer );
	this->Layout();
	main_sizer->Fit( this );


	this->Centre( wxBOTH );
}

ZetaFrame::~ZetaFrame()
{
}

BEGIN_EVENT_TABLE( NewFileFrame, wxFrame )
	EVT_CLOSE( NewFileFrame::_wxFB_OnClose )
	EVT_BUTTON( ID_Compute, NewFileFrame::_wxFB_Compute )
END_EVENT_TABLE()

NewFileFrame::NewFileFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* main_sizer;
	main_sizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* options_sizer;
	options_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	options_sizer->SetFlexibleDirection( wxBOTH );
	options_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	label_file = new wxStaticText( this, wxID_ANY, _("File:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_file->Wrap( -1 );
	options_sizer->Add( label_file, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL|wxEXPAND, 5 );

	file_picker = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), _("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	options_sizer->Add( file_picker, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );

	label_method = new wxStaticText( this, wxID_ANY, _("Method:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_method->Wrap( -1 );
	options_sizer->Add( label_method, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	wxString method_choiceChoices[] = { _("Fast"), _("Slow") };
	int method_choiceNChoices = sizeof( method_choiceChoices ) / sizeof( wxString );
	method_choice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, method_choiceNChoices, method_choiceChoices, 0 );
	method_choice->SetSelection( 0 );
	options_sizer->Add( method_choice, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );

	label_matsize = new wxStaticText( this, wxID_ANY, _("Matrix size:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_matsize->Wrap( -1 );
	options_sizer->Add( label_matsize, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	matsize_ctrl = new wxSpinCtrl( this, wxID_ANY, wxT("50"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2147483647, 50 );
	options_sizer->Add( matsize_ctrl, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );

	label_precision = new wxStaticText( this, wxID_ANY, _("Byte precision:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_precision->Wrap( -1 );
	options_sizer->Add( label_precision, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	precision_ctrl = new wxSpinCtrl( this, wxID_ANY, wxT("2048"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2147483647, 50 );
	options_sizer->Add( precision_ctrl, 1, wxALL|wxEXPAND, 5 );


	main_sizer->Add( options_sizer, 0, wxALL|wxEXPAND, 5 );

	label_fixed = new wxStaticText( this, wxID_ANY, _("Fixed coefficients:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_fixed->Wrap( -1 );
	main_sizer->Add( label_fixed, 0, wxALL, 10 );

	coeffs_text = new wxTextCtrl( this, wxID_ANY, _("a0 = (1, 0)"), wxDefaultPosition, wxSize( -1,200 ), wxTE_MULTILINE );
	coeffs_text->SetMinSize( wxSize( -1,200 ) );

	main_sizer->Add( coeffs_text, 1, wxALL|wxEXPAND, 10 );

	compute_button = new wxButton( this, ID_Compute, _("Compute"), wxDefaultPosition, wxDefaultSize, 0 );
	main_sizer->Add( compute_button, 0, wxALL, 10 );


	this->SetSizer( main_sizer );
	this->Layout();
	main_sizer->Fit( this );

	this->Centre( wxBOTH );
}

NewFileFrame::~NewFileFrame()
{
}

BEGIN_EVENT_TABLE( RenderingSettingsFrame, wxFrame )
	EVT_CLOSE( RenderingSettingsFrame::_wxFB_OnClose )
	EVT_BUTTON( ID_ApplyRenderingSettings, RenderingSettingsFrame::_wxFB_Apply )
END_EVENT_TABLE()

RenderingSettingsFrame::RenderingSettingsFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* main_sizer;
	main_sizer = new wxBoxSizer( wxVERTICAL );

	check_legend = new wxCheckBox( this, wxID_ANY, _("Show legend"), wxDefaultPosition, wxDefaultSize, 0 );
	check_legend->SetValue(true);
	main_sizer->Add( check_legend, 0, wxALL, 10 );

	check_multisample = new wxCheckBox( this, wxID_ANY, _("Enable multisampling"), wxDefaultPosition, wxDefaultSize, 0 );
	check_multisample->SetValue(true);
	main_sizer->Add( check_multisample, 0, wxALL, 10 );

	wxBoxSizer* multisampling_sizer;
	multisampling_sizer = new wxBoxSizer( wxHORIZONTAL );

	label_multisampling = new wxStaticText( this, wxID_ANY, _("Samples per pixel:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_multisampling->Wrap( -1 );
	multisampling_sizer->Add( label_multisampling, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	samples_ctrl = new wxSpinCtrl( this, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 32, 0 );
	multisampling_sizer->Add( samples_ctrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 10 );


	main_sizer->Add( multisampling_sizer, 0, wxALL|wxEXPAND, 0 );

	wxBoxSizer* line_sizer;
	line_sizer = new wxBoxSizer( wxHORIZONTAL );

	label_line = new wxStaticText( this, wxID_ANY, _("Line pixels per step:"), wxDefaultPosition, wxDefaultSize, 0 );
	label_line->Wrap( -1 );
	line_sizer->Add( label_line, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	line_ctrl = new wxSpinCtrl( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1024, 0 );
	line_sizer->Add( line_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10 );


	main_sizer->Add( line_sizer, 0, wxALL|wxEXPAND, 0 );

	apply_button = new wxButton( this, ID_ApplyRenderingSettings, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	main_sizer->Add( apply_button, 0, wxALL, 5 );


	this->SetSizer( main_sizer );
	this->Layout();
	main_sizer->Fit( this );

	this->Centre( wxBOTH );
}

RenderingSettingsFrame::~RenderingSettingsFrame()
{
}
