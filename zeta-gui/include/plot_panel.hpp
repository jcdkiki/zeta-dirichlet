#ifndef PLOT_PANEL_HPP
#define PLOT_PANEL_HPP

#include <flint_wrappers/vector.hpp>
#include <wx/glcanvas.h>

struct Vec2d {
    double x;
    double y;
};

class PlotPanel : public wxGLCanvas {
protected:
    wxGLContext context;

    bool is_dragging = false;
    wxPoint drag_position;

    Vec2d SpaceToScreenCoord(Vec2d vec);
    Vec2d ScreenToSpaceCoord(Vec2d vec);

    void RenderAxes();
    void RenderGrid();

    virtual void RenderData() = 0;

    void Resized(wxSizeEvent &evt);
    void Render(wxPaintEvent &evt);
    void MouseMoved(wxMouseEvent &event);
    void MouseWheelMoved(wxMouseEvent &event);
    void MouseDown(wxMouseEvent &event);
    void MouseReleased(wxMouseEvent &event);

public:
    PlotPanel(wxWindow *parent, int id, wxPoint position, wxSize size, int flags);

    Vec2d pos = {0.0, 0.0};
    double zoom = 5.0;
    double y_zoom = 5.0;

    void EnableMultisampling();
    void DisableMultisampling();

    DECLARE_EVENT_TABLE()
};

class PointsPlotPanel : public PlotPanel {
    virtual size_t GetPointsCount() = 0;
    virtual double GetPoint(int i) = 0;

    void RenderData();

public:
    PointsPlotPanel(wxWindow *parent, int id, wxPoint position, wxSize size, int flags) : PlotPanel(parent, id, position, size, flags) {}

    void FitPlot();
    virtual ~PointsPlotPanel() = default;
};

class CoefficientsPlotPanel : public PointsPlotPanel {
    size_t GetPointsCount();
    double GetPoint(int i);

public:
    CoefficientsPlotPanel(wxWindow *parent, int id, wxPoint position, wxSize size, int flags) : PointsPlotPanel(parent, id, position, size, flags) {}
    virtual ~CoefficientsPlotPanel() = default;
};

class ErrorsPlotPanel : public PointsPlotPanel {
    size_t GetPointsCount();
    double GetPoint(int i);

public:
    ErrorsPlotPanel(wxWindow *parent, int id, wxPoint position, wxSize size, int flags) : PointsPlotPanel(parent, id, position, size, flags) {}
    virtual ~ErrorsPlotPanel() = default;
};

class SeriesPlotPanel : public PlotPanel {
public:
    SeriesPlotPanel(wxWindow *parent, int id, wxPoint position, wxSize size, int flags) : PlotPanel(parent, id, position, size, flags) {}
    virtual ~SeriesPlotPanel() = default;
    
private:    
    flint::Vector bases;
    void RenderData();
};

#endif
