#ifndef PLOT_PANEL_HPP
#define PLOT_PANEL_HPP

#include "acb_vector.hpp"
#include <wx/glcanvas.h>

struct Vec2d {
	double x;
	double y;
};

class PlotPanel : public wxGLCanvas {
    wxGLContext context;
	acb::Vector coeffs;

	bool is_dragging = false;
	wxPoint drag_position;

	Vec2d pos     = { 0.0, 0.0 };
	double zoom   = 5.0;
	double y_zoom = 5.0;

	Vec2d SpaceToScreenCoord(Vec2d vec);
	Vec2d ScreenToSpaceCoord(Vec2d vec);
	void RenderAxes();

public:
	PlotPanel(wxWindow *parent, const int *args);
	virtual ~PlotPanel() = default;
    
	void SetCoeffs(const acb::Vector &coeffs) { this->coeffs = coeffs; }

	void Resized(wxSizeEvent& evt);
	void Render(wxPaintEvent& evt);
	
	void MouseMoved(wxMouseEvent& event);
	void MouseWheelMoved(wxMouseEvent& event);
	void MouseDown(wxMouseEvent& event);
	void MouseReleased(wxMouseEvent& event);
	
	DECLARE_EVENT_TABLE()
};


#endif
