#include "plot_panel.hpp"

#include <GL/gl.h>
#include <algorithm>
#include <wx/colour.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/settings.h>

struct rgb24_t {
    uint8_t r, g, b;
};

static constexpr rgb24_t EVEN_POINT_COLOR = {0xFF, 0x23, 0x5E};
static constexpr rgb24_t ODD_POINT_COLOR = {0x02, 0x17, 0xFF};
static constexpr rgb24_t LINE_COLOR = {0x02, 0x17, 0xFF};

BEGIN_EVENT_TABLE(PlotPanel, wxGLCanvas)
EVT_MOTION(PlotPanel::MouseMoved)
EVT_LEFT_DOWN(PlotPanel::MouseDown)
EVT_LEFT_UP(PlotPanel::MouseReleased)
EVT_SIZE(PlotPanel::Resized)
EVT_MOUSEWHEEL(PlotPanel::MouseWheelMoved)
EVT_PAINT(PlotPanel::Render)
END_EVENT_TABLE()

void PlotPanel::MouseMoved(wxMouseEvent &event)
{
    wxPoint new_pos = event.GetPosition();

    if (is_dragging) {
        wxPoint delta = new_pos - drag_position;

        pos.x += delta.x * 1.f;
        pos.y += delta.y * 1.f;
        Refresh();
    }

    drag_position = new_pos;
}

void PlotPanel::MouseDown(wxMouseEvent &event)
{
    drag_position = event.GetPosition();
    is_dragging = true;
}

void PlotPanel::MouseReleased(wxMouseEvent &event)
{
    is_dragging = false;
}

void PlotPanel::MouseWheelMoved(wxMouseEvent &event)
{
    wxSize win_size = GetSize();

    if (event.ControlDown()) {
        double new_y_zoom = y_zoom + event.GetWheelRotation() * 0.25 / 120.f;
        if (new_y_zoom > 0.0001) {
            y_zoom = new_y_zoom;
        }
        Refresh();
        return;
    }

    double new_zoom = zoom + event.GetWheelRotation() * 0.25 / 120.f;
    if (new_zoom > 0.0001) {
        zoom = new_zoom;
    }

    Refresh();
}

static constexpr int WINDOW_WIDTH = 640;
static constexpr int WINDOW_HEIGHT = 480;

static const int GL_ATTRIBS[] = {
    WX_GL_SAMPLES, 4,
    0
};

PlotPanel::PlotPanel(wxWindow *parent)
    : wxGLCanvas(parent, wxID_ANY, GL_ATTRIBS, wxDefaultPosition, wxSize(WINDOW_WIDTH, WINDOW_HEIGHT),
                 wxFULL_REPAINT_ON_RESIZE),
      context(this)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void PlotPanel::Resized(wxSizeEvent &evt)
{
    Refresh();
}

Vec2d PlotPanel::SpaceToScreenCoord(Vec2d vec)
{
    return {WINDOW_WIDTH / 2.0 + pos.x + vec.x * zoom,
            WINDOW_HEIGHT / 2.0 + pos.y - vec.y * zoom * y_zoom};
}

Vec2d PlotPanel::ScreenToSpaceCoord(Vec2d vec)
{
    return {(vec.x - pos.x - WINDOW_WIDTH / 2.0) / zoom,
            (vec.y - pos.y - WINDOW_HEIGHT / 2.0) / (-zoom * y_zoom)};
}

void PlotPanel::RenderGrid()
{
    Vec2d zero = SpaceToScreenCoord({0.0, 0.0});

    glColor3ub(0xB0, 0xB0, 0xB0);
    glLineWidth(1.0);

    glBegin(GL_LINES);

    double scale = ldexpl(1, ilogb(zoom));
    double step = 32 / scale * zoom;
    for (double x = SpaceToScreenCoord({32 / scale, 0}).x; x < WINDOW_WIDTH; x += step) {
        glVertex2d(x, 0);
        glVertex2d(x, WINDOW_HEIGHT);
    }

    for (double x = SpaceToScreenCoord({-32 / scale, 0}).x; x > 0.0; x -= step) {
        glVertex2d(x, 0);
        glVertex2d(x, WINDOW_HEIGHT);
    }

    scale = ldexpl(1, ilogb(zoom * y_zoom));
    step = 32 / scale * zoom * y_zoom;
    for (double y = SpaceToScreenCoord({0, 32 / scale}).y; y > 0.0; y -= step) {
        glVertex2d(0, y);
        glVertex2d(WINDOW_WIDTH, y);
    }

    for (double y = SpaceToScreenCoord({0, -32 / scale}).y; y < WINDOW_HEIGHT; y += step) {
        glVertex2d(0, y);
        glVertex2d(WINDOW_WIDTH, y);
    }

    glEnd();
}

void PlotPanel::RenderAxes()
{
    Vec2d zero = SpaceToScreenCoord({0.0, 0.0});

    double x_end = WINDOW_WIDTH - 10.0;
    double x_begin = 10.0;
    double y_begin = WINDOW_HEIGHT - 10.0;
    double y_end = 10.0;

    glColor3ub(0, 0, 0);
    glLineWidth(2.0);

    glBegin(GL_LINES);
    glColor3ub(0, 0, 0);
    glVertex2d(x_begin, zero.y);
    glVertex2d(x_end, zero.y);

    glVertex2d(x_end, zero.y);
    glVertex2d(x_end - 10.0, zero.y - 5.0);

    glVertex2d(x_end, zero.y);
    glVertex2d(x_end - 10.0, zero.y + 5.0);

    glVertex2d(zero.x, y_begin);
    glVertex2d(zero.x, y_end);

    glVertex2d(zero.x, y_end);
    glVertex2d(zero.x - 5.0, y_end + 10.0);

    glVertex2d(zero.x, y_end);
    glVertex2d(zero.x + 5.0, y_end + 10.0);

    double scale = ldexpl(1, ilogb(zoom));
    double step = 32 / scale * zoom;
    for (double x = SpaceToScreenCoord({32 / scale, 0}).x; x < WINDOW_WIDTH - 30.0; x += step) {
        glVertex2d(x, zero.y - 5.0);
        glVertex2d(x, zero.y + 5.0);
    }

    for (double x = SpaceToScreenCoord({-32 / scale, 0}).x; x > 30.0; x -= step) {
        glVertex2d(x, zero.y - 5.0);
        glVertex2d(x, zero.y + 5.0);
    }

    scale = ldexpl(1, ilogb(zoom * y_zoom));
    step = 32 / scale * zoom * y_zoom;
    for (double y = SpaceToScreenCoord({0, 32 / scale}).y; y > 30.0; y -= step) {
        glVertex2d(zero.x - 5.0, y);
        glVertex2d(zero.x + 5.0, y);
    }

    for (double y = SpaceToScreenCoord({0, -32 / scale}).y; y < WINDOW_HEIGHT - 30.0; y += step) {
        glVertex2d(zero.x - 5.0, y);
        glVertex2d(zero.x + 5.0, y);
    }

    glEnd();
}

void PlotPanel::Render(wxPaintEvent &evt)
{
    if (!IsShown())
        return;

    wxPaintDC dc(this);
    wxGLCanvas::SetCurrent(context);
    
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    wxColour bg_col = *wxWHITE;
    glClearColor(bg_col.Red() / 255.f, bg_col.Green() / 255.f, bg_col.Blue() / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    RenderGrid();
    RenderAxes();
    RenderData();

    glFlush();
    SwapBuffers();
}

void CoefficientsPlotPanel::RenderData()
{
    glBegin(GL_QUADS);
    for (int i = 0; i < coefficients.size(); i++) {
        rgb24_t color = (i % 2) ? ODD_POINT_COLOR : EVEN_POINT_COLOR;
        glColor3ub(color.r, color.g, color.b);
        Vec2d coord = SpaceToScreenCoord({(double)i, coefficients[i]});

        double size = std::max(3.0, zoom / 4.0);
        glVertex2d(coord.x - size, coord.y);
        glVertex2d(coord.x, coord.y + size);
        glVertex2d(coord.x + size, coord.y);
        glVertex2d(coord.x, coord.y - size);
    }
    glEnd();
}

void CoefficientsPlotPanel::FitPlot()
{
    double requested_width, requested_height;

    if (coefficients.size() == 0) {
        pos.x = 0;
        pos.y = 0;

        requested_width = 1;
        requested_height = 1;
    }
    else {
        double y_min = *std::min_element(coefficients.begin(), coefficients.end());
        double y_max = *std::max_element(coefficients.begin(), coefficients.end());

        pos.x = coefficients.size() / -2.0;
        pos.y = (y_min + y_max) / 2.0;

        requested_width = coefficients.size();
        requested_height = (y_max - y_min);

        if (requested_height == 0)
            requested_height = 1;
    }

    zoom = (double)WINDOW_WIDTH / requested_width / 1.5;
    y_zoom = (double)WINDOW_HEIGHT / requested_height / 1.5 / zoom;

    pos.x *= zoom;
    pos.y *= zoom * y_zoom;

    Refresh();
}

void SeriesPlotPanel::SetCoefficients(const flint::Vector &coeffs)
{
    this->coeffs = coeffs;

    bases = flint::Vector(coeffs.size());
    for (int i = 0; i < coeffs.size(); i++) {
        bases[i].set((double)(i + 1));
    }
}

static constexpr int BYTE_PRECISION = 256;

void SeriesPlotPanel::RenderData()
{
    if (coeffs.size() == 0) {
        return;
    }

    double scale = ldexpl(1, ilogb(zoom));
    double x_step = 5.f / zoom;
    double x_min = ScreenToSpaceCoord({0, 0}).x;
    double x_max = ScreenToSpaceCoord({WINDOW_WIDTH, 0}).x;

    x_min = floor(x_min / x_step) * x_step;
    int n = ceil((x_max - x_min) / x_step);

    glColor3ub(LINE_COLOR.r, LINE_COLOR.g, LINE_COLOR.b);
    glLineWidth(2.0);

    flint::Complex result;
    flint::Complex tmp;
    flint::Complex cur_x(x_min);
    flint::Complex step(x_step);
    cur_x.neg();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n; i++) {
        result.zero();
        for (int i = 0; i < coeffs.size(); i++) {
            flint::pow(tmp, bases[i], cur_x, BYTE_PRECISION);
            flint::mul(tmp, tmp, coeffs[i], BYTE_PRECISION);
            flint::add(result, result, tmp, BYTE_PRECISION);
        }

        flint::sub(cur_x, cur_x, step, BYTE_PRECISION);
        Vec2d pos = SpaceToScreenCoord({ x_min + i*x_step, arf_get_d(&result.get()->real.mid, ARF_RND_NEAR) });
        glVertex2d(pos.x, pos.y);
    }
    glEnd();
}
