#include "zeta_app.hpp"
#include "zeta_frames.h"
#include "shared.hpp"

bool ZetaApp::OnInit()
{
    frames.main = new ZetaFrame(nullptr);
    frames.new_file = new NewFileFrame(nullptr);
    frames.rendering_settings = new RenderingSettingsFrame(nullptr);
    frames.main->Show(true);
    frames.main->SetStatusText("Welcome to Zeta GUI");
    return true;
}
