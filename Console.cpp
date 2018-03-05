#include "Console.hpp"

void Console::init(const std::string& fontFile, int fontSize, int argTileWidth, int argTileHeight, int argW, int argH)
{
  w=argW;
  h=argH;
  tileWidth=argTileWidth;
  tileHeight=argTileHeight;
  img=new Image;
  fnt=manager.getFont(fontFile.c_str(),fontSize,0);
  fntGlow=manager.getFont(fontFile.c_str(),fontSize,ffGlow);
  txt.assignFont(fnt);
  txtGlow.assignFont(fntGlow);
  txt.setColor(Color::white);
  txtGlow.setColor(Color::white);
  img->create(1024,1024);
  img->clear();
  //img->renderHere(&txt);
  cw=tileWidth*1.0f/img->getTexWidth();
  ch=tileHeight*1.0f/img->getTexHeight();
  initChar(' ');
  curFg=Color::white;
  curBg=Color::black;
  bgm.resize(h,ColorVector(w,curBg.packTo32()));
  bgmOld=bgm;
  fgm.resize(h,ColorVector(w,curFg.packTo32()));
  fgmOld=fgm;

  sym.resize(h,SymbolVector(w,' '));
  symOld=sym;

  //    int sl=strlen(str);
  //    int x=1;
  //    int y=0;
  //    for(;*str;++str,++x)
  //    {
  //      char t[2]={*str,0};
  //      txt.setText(t);
  //      txt.setPosition(Pos(x*(tileWidth+2)+(tileWidth-txt.getWidth())/2,y*(tileHeight+2)+(tileHeight-txt.getHeight())/2));
  //      if(x+tileWidth+2>=txt.getWidth())
  //      {
  //        x=0;y+=tileHeight+2;
  //      }
  //      img->renderHere(&txt);
  //    }
  vb.setSize(w*h*4);
  bg.setSize(w*h*4);
  Grid g(Pos((float)w*tileWidth,(float)h*tileHeight),w,h);
  VxVector& v=vb.getVBuf();
  ClrVector& c=vb.getCBuf();
  VxVector& bv=bg.getVBuf();
  ClrVector& bc=bg.getCBuf();
  VxVector& t=vb.getTBuf();
  for(int y=0;y<h;++y)
  {
    for(int x=0;x<w;++x)
    {
      g.getCell(x,y).pushQuad(v);
      g.getCell(x,y).pushQuad(bv);
      c.push4(Color::white);
      bc.push4(Color::black);
      glider::Rect(0,0,cw,ch).pushQuad(t);
    }
  }
  bg.update();
  vb.update();
  /*const char* str1="AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtVvUuWwXxYyZz";
    const char* str2="0123456789!@#$%^&*()";
    printAt(0,0,"hello world, lalala. привет!");
    printAt(0,1,str1);
    printAt(0,2,str2);*/
}

void Console::draw()
{
  //Rectangle(Rect(0,0,128,128),Color::blue).draw();
  int tf=-1,cf=-1,bf=-1,tt=-1,ct=-1,bt=-1;
  int idx=0;
  for(int y=0;y<h;++y)
  {
    for(int x=0;x<w;++x,idx+=4)
    {
      if(sym[y][x]!=symOld[y][x])
      {
        LOGDEBUG("con", "sym changed");
        tt=idx;
        if(tf==-1)
        {
          tf=idx;
        }
      }
      if(bgm[y][x]!=bgmOld[y][x])
      {
        LOGDEBUG("con", "bg changed");
        bt=idx;
        if(bf==-1)
        {
          bf=idx;
        }
      }
      if(fgm[y][x]!=fgmOld[y][x])
      {
        LOGDEBUG("con", "fg changed");
        ct=idx;
        if(cf==-1)
        {
          cf=idx;
        }
      }
    }
  }
  if(tf!=-1)
  {
    vb.update(ufTexture,tf,tt+4);
    symOld=sym;
  }
  if(cf!=-1)
  {
    vb.update(ufColors,cf,ct+4);
    fgmOld=fgm;
  }
  if(bf!=-1)
  {
    bg.update(ufColors,bf,bt+4);
    bgmOld=bgm;
  }
  state.loadIdentity();
  bg.draw();
  img->bind();
  vb.draw();
  img->unbind();
  /*
    Image imgx;
    imgx.create(128,128);
    imgx.setPosition(Pos(50,100));
    imgx.setCenter(Pos(0,0));
    Rectangle r2(Rect(0,0,128,128),Color::blue);
    imgx.renderHere(&r2);
    Rectangle r(Rect(0,0,128,128),Color::red);
    state.enableScissors(Rect(10,10,20,20),true);
    imgx.renderHere(&r);
    state.disableScissors();
    imgx.draw();*/
  /*Text temp;
    temp.assignFont(fnt);
    temp.setText("╔════╗\n║    ║\n╚════╝");
    temp.setPosition(Pos(50,100));
    temp.draw();*/
  //txt.draw();
}

Console::CharMap::iterator Console::initChar(ushort c, bool glow)
{
  char buf[5];
  UString::ucs2ToUtf8(c,buf);
  if(glow)
  {
    txtGlow.setText(buf, true);
  }
  txt.setText(buf,true);
  static kst::Logger* log=kst::Logger::getLogger("con.chr");
  //LOGDEBUG(log,"char=%s/%x, w=%d, h=%d",buf,c,txt.getWidth(),txt.getHeight());
  int xfix=0;
  //    if(c==0x250c || c==0x2514 || c==0x2554 || c==0x255a || c==0x251c)
  //    {
  //      //xfix=1;
  //    }
  if(glow)
  {
    txtGlow.setPosition(Pos((float)(fillX+xfix+(tileWidth-txtGlow.getWidth())/2),(float)(fillY+(tileHeight-txtGlow.getHeight())/2)));
  }
  txt.setPosition(Pos((float)(fillX+xfix+(tileWidth-txt.getWidth())/2),(float)(fillY+(tileHeight-txt.getHeight())/2)));
  state.enableScissors(glider::Rect((float)fillX,(float)fillY,(float)tileWidth+2,(float)tileHeight+2),true);
  if(glow)
  {
    img->renderHere(&txtGlow);
  }
  img->renderHere(&txt);
  state.disableScissors();
  uint32_t charCode = c;
  if(glow)
  {
    charCode |= glowBit;
  }
  CharMap::iterator it=cm.insert(CharMap::value_type(charCode,Pos(fillX*1.0f/img->getTexWidth(),fillY*1.0f/img->getTexHeight()))).first;
  fillX+=tileWidth+2;
  if(fillX+tileWidth+2>img->getTexWidth())
  {
    fillX=0;
    fillY+=tileHeight+2;
    if(fillY+tileHeight+2>img->getTexHeight())
    {
      KSTHROW("Too many tiles used");
    }
  }
  return it;
}

void Console::clear()
{
  int idx = 0;
  CharMap::iterator it=cm.find(' ');
  uint32_t fg32 = curFg.packTo32();
  uint32_t bg32 = curBg.packTo32();
  for(int y=0;y<h;++y)
  {
    for(int x=0;x<w;++x,idx+=4)
    {
      sym[y][x]=' ';
      fgm[y][x]=fg32;
      bgm[y][x]=bg32;
      glider::Rect(it->second.x,it->second.y,cw,ch).setQuad(vb.getTBuf(),idx);
      vb.getCBuf().set4(curFg,idx);
      bg.getCBuf().set4(curBg,idx);
    }
  }
}

void Console::printAt(IPos pos, const Color& fg, const Color& bg, uint32_t flags, const char* str)
{
  curFg=fg;
  curBg=bg;
  printAt(pos, flags, str);
}

void Console::printAt(IPos argPos, uint32_t flags, const char* str)
{
  int x = argPos.x;
  int y = argPos.y;
  int pos=0;
  while(str[pos])
  {
    ushort c=UString::getNextSymbol(str,pos);
    if(Recti<int>(0,0,w,h).isInside(IPos(x,y)))
    {
      sym[y][x]=c;
      fgm[y][x]=curFg.packTo32();
      bgm[y][x]=curBg.packTo32();
      uint32_t charCode = getCharCode(c);
      CharMap::iterator it=cm.find(charCode);
      if(it==cm.end())
      {
        it=initChar(c, curGlow);
      }
      int idx=y*w*4+x*4;
      glider::Rect(it->second.x,it->second.y,cw,ch).setQuad(vb.getTBuf(),idx);
      vb.getCBuf().set4(curFg,idx);
      bg.getCBuf().set4(curBg,idx);
    }
    ++x;
    if(x>=w)
    {
      x=0;
      ++y;
      if(y>=h)
      {
        break;
      }
    }
  }
}

void Console::printEx(int x, int y, const Console::ColorVector& argFg, const Console::ColorVector& argBg, const Console::SymbolVector& argSym)
{
  for(size_t i=0;i<argFg.size();++i,++x)
  {
    if(x>=w)continue;
    ushort c=argSym[i];
    sym[y][x]=c;
    fgm[y][x]=argFg[i];
    bgm[y][x]=argBg[i];
    CharMap::iterator it=cm.find(c);
    if(cm.find(c)==cm.end())
    {
      it=initChar(c);
    }
    int idx=y*w*4+x*4;
    glider::Rect(it->second.x,it->second.y,cw,ch).setQuad(vb.getTBuf(),idx);
    vb.getCBuf().set4(argFg[i],idx);
    bg.getCBuf().set4(argBg[i],idx);
  }
}
