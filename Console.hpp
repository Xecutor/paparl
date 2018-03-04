#pragma once

#include <map>
#include <vector>
#include <functional>

#include "kst/Throw.hpp"
#include "kst/Logger.hpp"

#include "Engine.hpp"
#include "Drawable.hpp"
#include "EventHandler.hpp"
#include "ResourcesManager.hpp"
#include "Types.hpp"
#include "VertexBuffer.hpp"
#include "Text.hpp"
#include "Image.hpp"
#include "GLState.hpp"
#include "Timer.hpp"

using namespace glider;

class Console:public EventHandler,public Drawable{
public:
  void init(const std::string& fontFile, int fontSize, int argTileWidth, int argTileHeight, int argW,int argH)
  {
    w=argW;
    h=argH;
    tileWidth=argTileWidth;
    tileHeight=argTileHeight;
    img=new Image;
    fnt=manager.getFont(fontFile.c_str(),fontSize,false);
    txt.assignFont(fnt);
    txt.setColor(Color::white);
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
  virtual ~Console(){}
  virtual void onActiveChange(bool active){}
  virtual void onResize(){};
  virtual void onQuit()
  {
    engine.exitApp();
  }
  
  void draw()override
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
  void onFrameUpdate(int mcsec)override
  {
    if(drawCb) {
      drawCb();
    }
  }
  
  virtual void onUserEvent(void* data1,void* data2){}
  
  void onKeyboardEvent(KeyboardEvent& argEvent)override
  {
    if(keyCb) {
      keyCb(argEvent);
    }
  }

  using ColorVector=std::vector<uint32_t>;
  using ColorMatrix=std::vector<ColorVector>;
  using SymbolVector=std::vector<ushort>;
  using SymbolMatrix=std::vector<SymbolVector>;

  FontRef fnt;
  Text txt;
  Image* img;
  VertexBuffer vb;
  VertexBuffer bg;
  int w,h;
  int tileWidth, tileHeight;
  typedef std::map<ushort,Pos> CharMap;
  CharMap cm;
  int fillX=0;
  int fillY=0;
  float ch,cw;

  ColorMatrix bgm,bgmOld;
  ColorMatrix fgm,fgmOld;
  SymbolMatrix sym,symOld;

  Color curFg,curBg;
  
  std::function<void()> drawCb;
  std::function<void(const KeyboardEvent&)> keyCb;
  std::function<void(const MouseEvent&)> mouseCb;

  CharMap::iterator initChar(ushort c)
  {
    char buf[5];
    UString::ucs2ToUtf8(c,buf);
    txt.setText(buf,true);
    static kst::Logger* log=kst::Logger::getLogger("con.chr");
    LOGDEBUG(log,"char=%s/%x, w=%d, h=%d",buf,c,txt.getWidth(),txt.getHeight());
    int xfix=0;
    if(c==0x250c || c==0x2514 || c==0x2554 || c==0x255a || c==0x251c)
    {
      //xfix=1;
    }

    txt.setPosition(Pos((float)(fillX+xfix+(tileWidth-txt.getWidth())/2),(float)(fillY+(tileHeight-txt.getHeight())/2)));
    state.enableScissors(glider::Rect((float)fillX,(float)fillY,(float)tileWidth+2,(float)tileHeight+2),true);
    img->renderHere(&txt);
    state.disableScissors();
    CharMap::iterator it=cm.insert(CharMap::value_type(c,Pos(fillX*1.0f/img->getTexWidth(),fillY*1.0f/img->getTexHeight()))).first;
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
  void clear()
  {
    int idx = 0;
    CharMap::iterator it=cm.find(' ');
    if(it==cm.end())
    {
      it=initChar(' ');
    }
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
  void printAt(int x,int y,const char* str)
  {
    int pos=0;
    while(str[pos])
    {
      ushort c=UString::getNextSymbol(str,pos);
      if(Recti<int>(0,0,w,h).isInside(IPos(x,y)))
      {
        sym[y][x]=c;
        fgm[y][x]=curFg.packTo32();
        bgm[y][x]=curBg.packTo32();
        CharMap::iterator it=cm.find(c);
        if(it==cm.end())
        {
          it=initChar(c);
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
  void printEx(int x,int y,const ColorVector& argFg,const ColorVector& argBg,const SymbolVector& argSym)
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
};

struct SnapShot{
  Console::ColorMatrix fg,bg;
  Console::SymbolMatrix sym;
  Console* con;
  SnapShot(Console* argCon, const IPos& pos,const IPos& argSize):con(argCon)
  {
    IPos size=argSize;
    if(pos.x+size.x>con->w)
    {
      size.x=con->w-pos.x;
    }
    if(pos.y+size.y>con->h)
    {
      size.y=con->h-pos.y;
    }
    for(int y=0;y<size.y;++y)
    {
      fg.push_back(Console::ColorVector());
      fg.back().insert(fg.back().begin(),con->fgm[pos.y+y].begin()+pos.x,con->fgm[pos.y+y].begin()+pos.x+size.x);
      bg.push_back(Console::ColorVector());
      bg.back().insert(bg.back().begin(),con->bgm[pos.y+y].begin()+pos.x,con->bgm[pos.y+y].begin()+pos.x+size.x);
      sym.push_back(Console::SymbolVector());
      sym.back().insert(sym.back().begin(),con->sym[pos.y+y].begin()+pos.x,con->sym[pos.y+y].begin()+pos.x+size.x);
    }
  }
  void draw(const IPos& pos)
  {
    if(fg.empty())
    {
      return;
    }
    for(size_t y=0;y<fg.size();++y)
    {
      con->printEx(pos.x,pos.y+y,fg[y],bg[y],sym[y]);
    }
  }
};
