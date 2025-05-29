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
    Vec2d pos = {0.0, 0.0};
    double zoom = 5.0;
    double y_zoom = 5.0;

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
    PlotPanel(wxWindow *parent);

    DECLARE_EVENT_TABLE()
};

class CoefficientsPlotPanel : public PlotPanel {
    std::vector<double> coefficients;

    void RenderData();

public:
    CoefficientsPlotPanel(wxWindow *parent) : PlotPanel(parent) {}

    void FitPlot();
    void SetCoefficients(const std::vector<double> &coefficients)
    {
        this->coefficients = coefficients;
    }
    virtual ~CoefficientsPlotPanel() = default;
};

class SeriesPlotPanel : public PlotPanel {
public:
    SeriesPlotPanel(wxWindow *parent)
        : PlotPanel(parent)
    {}

    void SetCoefficients(const flint::Vector &coeffs);
    virtual ~SeriesPlotPanel() = default;

private:
    flint::Vector coeffs;
    flint::Vector bases;
    void RenderData();
};

#endif
