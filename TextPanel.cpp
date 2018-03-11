#include "TextPanel.hpp"


void TextPanel::addLine(const std::string& line)
{
  if(pos.x<rect.pos.x || pos.y>=rect.pos.y+rect.size.y)
  {
    return;
  }
  con.printAlignedAt(pos, rect.size.x, printFlags,
              (int)line.length()>rect.size.x?line.substr(0, rect.size.x).c_str():line.c_str());
  advance();
}

void TextPanel::addText(const std::string& text)
{
  splitTextToLines(text, [this](std::string line){addLine(line);});
}

void TextPanel::splitTextToLines(const std::string& text, std::function<void (std::string)> cb)
{
  std::string::size_type prevPos=0;
  std::string::size_type pos=text.find('\n', prevPos);
  while(pos!=std::string::npos)
  {
    cb(text.substr(prevPos, pos-prevPos));
    prevPos=pos+1;
    pos=text.find('\n', prevPos);
  }
  if(text.length()-prevPos>1)
  {
    cb(text.substr(prevPos));
  }
}
