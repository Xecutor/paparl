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
  screen->init();
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
  auto oldSize=screens.size();
  screens.erase(std::remove_if(screens.begin(), screens.end(),removePred), screens.end());
  if(screens.empty())
  {
    engine.exitLoop();
    return;
  }
  if(oldSize!=screens.size())
  {
    screens.back()->onTopScreen();
  }
  size_t idx = screens.size() - 1;
  while(idx>0 && !screens[idx]->isFullScreen())
  {
    --idx;
  }
  for(;idx<screens.size();++idx)
  {
    auto screen=screens[idx];
    if(!screen->isFullScreen() && screen->dimBackground())
    {
      con.transformColors(con.getConRect(),[](Color& fg, Color& bg)
      {
        fg.changeValue(0.6f);
        fg.changeSaturation(0.6f);
        bg.changeValue(0.6f);
        bg.changeSaturation(0.6f);
      });
    }
    screen->draw();
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
