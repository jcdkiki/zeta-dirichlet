#include "plot_panel.hpp"
#include "flint_wrappers/vector.hpp"
#include "shared.hpp"

#include <GL/gl.h>
#include <algorithm>
#include <flint/arf.h>
#include <flint/arf_types.h>
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

PlotPanel::PlotPanel(wxWindow *parent, int id, wxPoint position, wxSize size, int flags)
    : wxGLCanvas(
        parent, id,
        rendering_settings.gl_attribs[0] == 0 ? nullptr : rendering_settings.gl_attribs,
        position, size, wxFULL_REPAINT_ON_RESIZE), context(this)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void PlotPanel::Resized(wxSizeEvent &evt)
{
    Refresh();
}

Vec2d PlotPanel::SpaceToScreenCoord(Vec2d vec)
{
    wxSize size = GetSize();
    return {size.x / 2.0 + pos.x + vec.x * zoom,
            size.y / 2.0 + pos.y - vec.y * zoom * y_zoom};
}

Vec2d PlotPanel::ScreenToSpaceCoord(Vec2d vec)
{
    wxSize size = GetSize();
    return {(vec.x - pos.x - size.x / 2.0) / zoom,
            (vec.y - pos.y - size.y / 2.0) / (-zoom * y_zoom)};
}

void PlotPanel::RenderGrid()
{
    wxSize win_size = GetSize();
    Vec2d zero = SpaceToScreenCoord({0.0, 0.0});

    glColor3ub(0xB0, 0xB0, 0xB0);
    glLineWidth(1.0);

    glBegin(GL_LINES);

    double scale = ldexpl(1, ilogb(zoom));
    double step = 32 / scale * zoom;
    for (double x = SpaceToScreenCoord({32 / scale, 0}).x; x < win_size.x; x += step) {
        glVertex2d(x, 0);
        glVertex2d(x, win_size.y);
    }

    for (double x = SpaceToScreenCoord({-32 / scale, 0}).x; x > 0.0; x -= step) {
        glVertex2d(x, 0);
        glVertex2d(x, win_size.y);
    }

    scale = ldexpl(1, ilogb(zoom * y_zoom));
    step = 32 / scale * zoom * y_zoom;
    for (double y = SpaceToScreenCoord({0, 32 / scale}).y; y > 0.0; y -= step) {
        glVertex2d(0, y);
        glVertex2d(win_size.x, y);
    }

    for (double y = SpaceToScreenCoord({0, -32 / scale}).y; y < win_size.y; y += step) {
        glVertex2d(0, y);
        glVertex2d(win_size.x, y);
    }

    glEnd();
}

void PlotPanel::RenderAxes()
{
    wxSize size = GetSize();
    Vec2d zero = SpaceToScreenCoord({0.0, 0.0});

    double x_end = size.x - 10.0;
    double x_begin = 10.0;
    double y_begin = size.y - 10.0;
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
    for (double x = SpaceToScreenCoord({32 / scale, 0}).x; x < size.x - 30.0; x += step) {
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

    for (double y = SpaceToScreenCoord({0, -32 / scale}).y; y < size.y - 30.0; y += step) {
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
    wxSize win_size = GetSize();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    wxColour bg_col = *wxWHITE;
    glClearColor(bg_col.Red() / 255.f, bg_col.Green() / 255.f, bg_col.Blue() / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, win_size.x, win_size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, win_size.x, win_size.y, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    RenderGrid();
    RenderAxes();
    RenderData();

    glFlush();
    SwapBuffers();
}

// TODO: we are doing ARF_TO_D every frame.... fix this
void PointsPlotPanel::RenderData()
{
    glBegin(GL_QUADS);
    int n = GetPointsCount();
    for (int i = 0; i < n; i++) {
        rgb24_t color = (i % 2) ? ODD_POINT_COLOR : EVEN_POINT_COLOR;
        glColor3ub(color.r, color.g, color.b);
        Vec2d coord = SpaceToScreenCoord({(double)i, GetPoint(i) });

        double size = std::max(3.0, zoom / 4.0);
        glVertex2d(coord.x - size, coord.y);
        glVertex2d(coord.x, coord.y + size);
        glVertex2d(coord.x + size, coord.y);
        glVertex2d(coord.x, coord.y - size);
    }
    glEnd();
}

void PointsPlotPanel::FitPlot()
{
    wxSize win_size = GetSize();
    double requested_width, requested_height;

    size_t n = GetPointsCount();
    if (n == 0) {
        pos.x = 0;
        pos.y = 0;

        requested_width = 1;
        requested_height = 1;
    }
    else {
        double y_min = GetPoint(0);
        double y_max = y_min;

        for (int i = 1; i < n; i++) {
            double y = GetPoint(i);
            y_min = std::min(y, y_min);
            y_max = std::max(y, y_max);
        }

        pos.x = n / -2.0;
        pos.y = (y_min + y_max) / 2.0;

        requested_width = n;
        requested_height = (y_max - y_min);

        if (requested_height == 0)
            requested_height = 1;
    }

    zoom = (double)win_size.x / requested_width / 1.5;
    y_zoom = (double)win_size.y / requested_height / 1.5 / zoom;

    pos.x *= zoom;
    pos.y *= zoom * y_zoom;

    Refresh();
}

static constexpr int BYTE_PRECISION = 256;

void SeriesPlotPanel::RenderData()
{
    flint::Vector &coeffs = coefficients[selected_solution];

    if (bases.size() != coeffs.size()) {
        bases = flint::Vector(coefficients[selected_solution].size());
        for (int i = 0; i < coefficients[selected_solution].size(); i++) {
            bases[i].set((double)(i + 1));
        }
    }

    wxSize win_size = GetSize();
    if (coeffs.size() == 0) {
        return;
    }

    double scale = ldexpl(1, ilogb(zoom));
    double x_step = rendering_settings.line_length / zoom;
    double x_min = ScreenToSpaceCoord({0, 0}).x;
    double x_max = ScreenToSpaceCoord({(double)win_size.x, 0}).x;

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


size_t CoefficientsPlotPanel::GetPointsCount()
{
    if (selected_solution == -1) {
        return 0;
    }
    return coefficients[selected_solution].size();
}

size_t ErrorsPlotPanel::GetPointsCount()
{
    if (selected_solution == -1) {
        return 0;
    }
    return coefficients[selected_solution].size();
}

double CoefficientsPlotPanel::GetPoint(int i)
{
    return arf_get_d(&coefficients[selected_solution][i].real().mid(), ARF_RND_NEAR);
}

double ErrorsPlotPanel::GetPoint(int i)
{
    return mag_get_d(&coefficients[selected_solution][i].real().rad());
}
