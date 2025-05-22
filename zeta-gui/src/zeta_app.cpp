#include "zeta_app.hpp"
#include "zeta_frame.hpp"

bool ZetaApp::OnInit()
{
    ZetaFrame *frame = new ZetaFrame();
    frame->Show(true);
    return true;
}
