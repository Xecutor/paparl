#pragma once

#include "EventHandler.hpp"
#include <memory>

using namespace glider;


class ScreensController;
class Console;

class GameScreen{
public:
  GameScreen(Console& argCon, ScreensController* argScon):con(argCon), scon(argScon){}
  virtual ~GameScreen(){}
  virtual void onMouseEvent(const MouseEvent& evt) {}
  virtual void onKeyboardEvent(const KeyboardEvent& evt) = 0;
  virtual void draw() = 0;
  virtual bool isFullScreen()const
  {
    return true;
  }
  bool needToClose()const
  {
    return closeRequest;
  }
  void close()
  {
    closeRequest = true;
  }
protected:
  Console& con;
  ScreensController* scon;
  bool closeRequest = false;
};


class ScreensController {
public:
  ScreensController(Console& argCon);
  void pushScreen(std::shared_ptr<GameScreen> screen);

  template <class GS, class... Args>
  GS& pushScreen(Args... args)
  {
    auto ptr = std::make_shared<GS>(con, this, std::forward<Args...>(args)...);
    pushScreen(ptr);
    return *ptr;
  }

  void onMouseEvent(const MouseEvent& evt);
  void onKeyboardEvent(const KeyboardEvent& evt);
  void draw();
protected:
  Console& con;
  using ScreensVector = std::vector<std::shared_ptr<GameScreen>>;
  ScreensVector screens;
};
