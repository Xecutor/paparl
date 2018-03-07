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

IRect GameScreen::calcRect(IPos pos, IPos size, GameScreen::RectOrigin ro)
{
  switch(ro)
  {
    case roTopLeft:
      return {pos, size};
    case roTopRight:
      return {pos-size.xOnly(), size};
    case roBottonLeft:
      return {pos-size.yOnly(), size};
    case roBottomRight:
      return {pos-size, size};
    case roTopCenter:
      return {pos-size.xOnly()/2, size};
    case roLeftCenter:
      return {pos-size.yOnly()/2, size};
    case roRightCenter:
      return {pos-IPos{size.x, size.y/2}, size};
    case roBottomCenter:
      return {pos-IPos{size.x/2, size.y}, size};
    case roCenter:
      return {pos-size/2, size};
    case roScreenCenter:
      return {IPos{con.width()/2-size.x/2, con.height()/2-size.y/2}, size};
  }
  return {};
}
