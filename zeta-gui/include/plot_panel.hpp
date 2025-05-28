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
    PlotPanel(wxWindow *parent, const int *args);

    DECLARE_EVENT_TABLE()
};

class CoefficientsPlotPanel : public PlotPanel {
    std::vector<double> coefficients;

    void RenderData();

  public:
    CoefficientsPlotPanel(wxWindow *parent, const int *args) : PlotPanel(parent, args) {}

    void FitPlot();
    void SetCoefficients(const std::vector<double> &coefficients)
    {
        this->coefficients = coefficients;
    }
    virtual ~CoefficientsPlotPanel() = default;
};

class DynamicPlotPanel : public PlotPanel {
  public:
    DynamicPlotPanel(wxWindow *parent, const int *args)
        : PlotPanel(parent, args), func(nullptr), user_ptr(nullptr)
    {
    }

    typedef double (*Func)(double x, void *user_ptr);
    Func func;
    void *user_ptr;

    virtual ~DynamicPlotPanel() = default;

  private:
    void RenderData();
};

#endif
