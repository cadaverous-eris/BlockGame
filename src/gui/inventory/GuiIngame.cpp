#include "GuiIngame.h"


#include "render/ui/UIRenderer.h"

namespace eng {





  void GuiIngame::drawBackground() const {
    uiRenderer->drawColoredQuad({ 0, 0, 99 }, { width, height }, 0x0000007F_c);
    //uiRenderer->flushColored(); // TODO: remove?
  }

}