#ifndef ZETA_SHARED_HPP
#define ZETA_SHARED_HPP

#include "zeta_frames.h"
#include <flint_wrappers/vector.hpp>
#include <map>
#include <wx/glcanvas.h>

struct RenderingSettings {
    bool show_legend = true;
    int line_length = 5;
    int gl_attribs[3] = { WX_GL_SAMPLES, 4, 0 };
};

enum class ComputeMethod {
    FAST,
    SLOW
};

struct Frames {
    ZetaFrame *main;
    NewFileFrame *new_file;
    RenderingSettingsFrame *rendering_settings;
};

extern std::map<int, flint::Vector> coefficients;
extern int selected_solution;
extern RenderingSettings rendering_settings;
extern Frames frames;

#endif
