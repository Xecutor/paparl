#include "TextMessage.hpp"


void TextMessage::init()
{
  if(lines.empty())
  {
    return;
  }
  int maxWidth=(int)std::max_element(
        lines.begin(),
        lines.end(),
        [](const std::string& a, const std::string& b){return a.length()<b.length();})->length();
  IPos sz(maxWidth+2,(int)lines.size()+2);
  IRect r{con.getConRect().size/2-sz/2, sz};
  textPanel.setRect(r);
  textPanel.setPrintFlags(Console::pfAlignCenter|Console::pfKeepBackground);
}

void TextMessage::draw()
{
  con.clearRect(textPanel.getRect());
  con.fillRect(textPanel.getRect(), bg);
  textPanel.reset();
  textPanel.advance();
  con.curBg=bg;
  con.curFg=fg;
  for(auto& l:lines)
  {
    textPanel.addLine(l);
  }
}

void TextMessage::onKeyboardEvent(const KeyboardEvent& evt)
{
  if(evt.eventType!=ketPress)
  {
    return;
  }
  using namespace keyboard;
  switch(evt.keySym)
  {
    case GK_RETURN:
    case GK_RETURN2:
    case GK_KP_ENTER:
    case GK_ESCAPE:
    case GK_SPACE:
      close();
    default:break;
  }
}
