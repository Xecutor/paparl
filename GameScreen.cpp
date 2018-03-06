#include "GameScreen.hpp"
#include "Console.hpp"

ScreensController::ScreensController(Console& argCon):con(argCon)
{
  con.setDrawCb([this](){draw();});
  con.setKeyCb([this](auto ke){onKeyboardEvent(ke);});
  con.setMouseCb([this](auto me){onMouseEvent(me);});
}
void ScreensController::pushScreen(std::shared_ptr<GameScreen> screen)
{
  screens.push_back(screen);
}

void ScreensController::onMouseEvent(const MouseEvent& evt)
{
  if(screens.empty())
  {
    return;
  }
  screens.back()->onMouseEvent(evt);
}

void ScreensController::onKeyboardEvent(const KeyboardEvent& evt)
{
  if(screens.empty())
  {
    return;
  }
  screens.back()->onKeyboardEvent(evt);

}
void ScreensController::draw()
{
  auto removePred = [](const std::shared_ptr<GameScreen>& screen)
  {
    return screen->needToClose();
  };
  screens.erase(std::remove_if(screens.begin(), screens.end(),removePred), screens.end());
  if(screens.empty())
  {
    engine.exitLoop();
    return;
  }
  auto rit = screens.rbegin();
  while(rit!=screens.rend() && !(*rit)->isFullScreen())
  {
    ++rit;
  }
  if(rit==screens.rend())
  {
    --rit;
  }
  for(auto it = --rit.base(), end=screens.end();it!=end;++it)
  {
    (*it)->draw();
  }
}
