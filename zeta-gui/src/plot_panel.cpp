#include "plot_panel.hpp"

#include <GL/gl.h>
#include <cfloat>
#include <flint/mag.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/gdicmn.h>
#include <wx/settings.h>

struct rgb24_t {
    uint8_t r, g, b;
};

static constexpr rgb24_t EVEN_POINT_COLOR = { 255, 0, 0 };
static constexpr rgb24_t ODD_POINT_COLOR  = { 0, 0, 255 };

BEGIN_EVENT_TABLE(PlotPanel, wxGLCanvas)
    EVT_MOTION(PlotPanel::MouseMoved)
    EVT_LEFT_DOWN(PlotPanel::MouseDown)
    EVT_LEFT_UP(PlotPanel::MouseReleased)
    EVT_SIZE(PlotPanel::Resized)
    EVT_MOUSEWHEEL(PlotPanel::MouseWheelMoved)
    EVT_PAINT(PlotPanel::Render)
END_EVENT_TABLE()

void PlotPanel::MouseMoved(wxMouseEvent& event)
{
    if (!is_dragging) {
        return;
    }

    wxPoint new_pos = event.GetPosition();
    wxPoint delta = new_pos - drag_position;
    
    pos.x += delta.x * 1.f;
    pos.y += delta.y * 1.f;

    drag_position = new_pos;
    Refresh();
}

void PlotPanel::MouseDown(wxMouseEvent& event)
{
    drag_position = event.GetPosition();
    is_dragging = true;
}

void PlotPanel::MouseReleased(wxMouseEvent& event)
{
    is_dragging = false;
}

void PlotPanel::MouseWheelMoved(wxMouseEvent& event)
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

PlotPanel::PlotPanel(wxWindow *parent, const int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, 
    wxSize(WINDOW_WIDTH, WINDOW_HEIGHT), wxFULL_REPAINT_ON_RESIZE),
    context(this)
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void PlotPanel::Resized(wxSizeEvent& evt)
{
    Refresh();
}
 
Vec2d PlotPanel::SpaceToScreenCoord(Vec2d vec)
{
    return {
        WINDOW_WIDTH / 2.0 + pos.x + vec.x * zoom,
        WINDOW_HEIGHT / 2.0 + pos.y + vec.y * zoom * y_zoom
    };
}

Vec2d PlotPanel::ScreenToSpaceCoord(Vec2d vec)
{
    return {
        (vec.x - pos.x - WINDOW_WIDTH / 2.0) / zoom,
        (vec.y - pos.y - WINDOW_HEIGHT / 2.0) / (zoom * y_zoom)
    };
}

void PlotPanel::RenderAxes()
{
    Vec2d zero = SpaceToScreenCoord({ 0.0, 0.0 });
    
    double x_end = WINDOW_WIDTH - 10.0;
    double y_end = WINDOW_HEIGHT - 10.0;
    double y_begin = 10.0;

    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2d(zero.x, zero.y);
    glVertex2d(x_end, zero.y);

    glVertex2d(x_end, zero.y);
    glVertex2d(x_end - 10.0, zero.y - 10.0);
    
    glVertex2d(x_end, zero.y);
    glVertex2d(x_end - 10.0, zero.y + 10.0);

    glVertex2d(zero.x, y_begin);
    glVertex2d(zero.x, y_end);

    glVertex2d(zero.x, y_end);
    glVertex2d(zero.x - 10.0, y_end - 10.0);

    glVertex2d(zero.x, y_end);
    glVertex2d(zero.x + 10.0, y_end - 10.0);
    
    double scale = 1 << ilogb(zoom);
    for (int i = 1; i < 100; i++) {
        Vec2d coord = SpaceToScreenCoord({ i * 50 / scale, 0 });
        if (coord.x > WINDOW_WIDTH - 30.0)
            break;

        glVertex2d(coord.x, coord.y - 5.0);
        glVertex2d(coord.x, coord.y + 5.0);
    }

    scale = 1 << ilogb(zoom * y_zoom);
    for (int i = 1; i < 100; i++) {
        Vec2d coord = SpaceToScreenCoord({ 0, i * 32 / scale });
        if (coord.y > WINDOW_HEIGHT - 30.0)
            break;

        glVertex2d(coord.x - 5.0, coord.y);
        glVertex2d(coord.x + 5.0, coord.y);
    }

    for (int i = 1; i < 100; i++) {
        Vec2d coord = SpaceToScreenCoord({ 0, -i * 32 / scale });
        if (coord.y < 30.0)
            break;

        glVertex2d(coord.x - 5.0, coord.y);
        glVertex2d(coord.x + 5.0, coord.y);
    }

    glEnd();
}

void PlotPanel::Render(wxPaintEvent &evt)
{
    if(!IsShown()) return;
    
    wxPaintDC dc(this);
    wxGLCanvas::SetCurrent(context);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    wxColour bg_col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    glClearColor(bg_col.Red() / 255.f, bg_col.Green() / 255.f, bg_col.Blue() / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // midpoints
    glBegin(GL_QUADS);
    for (int i = 0; i < coeffs.size(); i++) {
        rgb24_t color = (i % 2) ? ODD_POINT_COLOR : EVEN_POINT_COLOR;
        glColor3ub(color.r, color.g, color.b); 
        
        double mid = arf_get_d(&coeffs[i]->real.mid, ARF_RND_NEAR);
        Vec2d coord = SpaceToScreenCoord({ (double)i, mid });
        
        glVertex2d(coord.x - 3, coord.y);
        glVertex2d(coord.x, coord.y + 3);
        glVertex2d(coord.x + 3, coord.y);
        glVertex2d(coord.x, coord.y - 3);
    }
    glEnd();
    
    RenderAxes();

    glFlush();
    SwapBuffers();
}
