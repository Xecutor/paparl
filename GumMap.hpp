#pragma once

#include <vector>
#include <unordered_map>

#include "Types.hpp"
#include "AStarSearch.hpp"
#include "cfov.hpp"
#include "Console.hpp"

template <class UserTileInfo>
class GumMap{
public:
  static CircularFov<IPos> cfov;
  typedef AStarSearch<GumMap,IPos> MyAStar;
  static MyAStar astar;
  GumMap()
  {
    globalLightStrength=100;
    globalLightTint=Color::white;
    lightMode=false;
    lightTint=0;
    lightStrength=0;
    memBg=Color::black;
    memFg=Color::gray/2;
    cacheChunk=nullptr;
    LOGDEBUG("map","ctor");
  }
  ~GumMap()
  {
    LOGDEBUG("map","dtor");
  }
  bool isInside(int,int)
  {
    return true;
  }
  TileInfo& getTile(int x,int y,int)
  {
    return getCell(x,y).ti;
  }
  UserTileInfo& getUserInfo(int x, int y)
  {
    return getCell(x,y).user;
  }
  UserTileInfo& getUserInfo(IPos pos)
  {
    return getCell(pos.x,pos.y).user;
  }
  void setVisible(int x,int y,int r)
  {
    if(lightMode)
    {
      MapCell& mc=getCell(x,y);
      int ls=1000*(maxLightRadius*lightStrength/1000-r)/maxLightRadius;
      Color tmp=Color(mc.lightTint)*(((float)mc.lightStrength)/1000.0f)+Color(lightTint)*((float)(maxLightRadius-r)/maxLightRadius);
      tmp.clamp();
      mc.lightTint=tmp.packTo32();
      mc.lightStrength+=ls;
      if(mc.lightStrength>1000)mc.lightStrength=1000;
    }else
    {
      getCell(x,y).visible=true;
      visCells.push_back(IPos(x,y));
    }
  }
  void clearVisibility()
  {
    for(std::vector<IPos>::iterator it=visCells.begin(),end=visCells.end();it!=end;++it)
    {
      getCell(*it).visible=false;
    }
    visCells.clear();
  }
  const std::vector<IPos>& getVisibleCells()const
  {
    return visCells;
  }
  void setBlock(int x,int y,bool value)
  {
    getCell(x,y).ti.block=value;
  }
  void setOutdoors(int x,int y,bool value)
  {
    getCell(x,y).outdoors=value;
  }
  void setMoveCost(int x,int y,uint16_t value)
  {
    getCell(x,y).moveCost=value;
  }
  void setLightSource(IPos p,uint16_t strength,const Color& tint)
  {
    setLightSource(p.x, p.y, strength, tint);
  }
  void setLightSource(int x,int y,uint16_t strength,const Color& tint)
  {
    MapCell& mc=getCell(x,y);
    mc.lightSource=strength!=0;
    mc.lsStrength=strength;
    mc.lsTint=tint.packTo32();
    lightChanged=true;
  }

  void setLightCone(IPos p, AngleRange cone)
  {
    getCell(p.x,p.y).lightCone=cone;
  }

  void setGlobalLight(uint16_t strength,const Color& tint)
  {
    globalLightStrength=strength;
    globalLightTint=tint;
    lightChanged=true;
  }

  void setInfo(IPos p,const Color& fg,const Color& bg,const std::string& sym)
  {
    setInfo(p.x, p.y, fg, bg, sym);
  }
  void setInfo(int x,int y,const Color& fg,const Color& bg,const std::string& sym)
  {
    MapCell& mc=getCell(x,y);
    mc.fg=fg.packTo32();
    mc.bg=bg.packTo32();
    mc.setSym(sym);
  }
  void setFg(int x,int y,const Color& clr)
  {
    MapCell& mc=getCell(x,y);
    mc.fg=clr.packTo32();
  }
  void setBg(int x,int y,const Color& clr)
  {
    MapCell& mc=getCell(x,y);
    mc.bg=clr.packTo32();
  }
  void setSym(int x,int y,const std::string& sym)
  {
    MapCell& mc=getCell(x,y);
    mc.setSym(sym);
  }
  void calcFov(int x,int y,int r)
  {
    cfov.generateFov(*this,IPos(x,y),r);
  }
  bool getVis(int x,int y)
  {
    return getCell(x,y).visible;
  }
  bool getVis(IPos p)
  {
    return getCell(p.x,p.y).visible;
  }
  Color getBg(IPos p)
  {
    MapCell& mc=getCell(p.x,p.y);
    return mc.bg;
  }
  Color getLitBg(IPos p)
  {
    return getLitBg(p.x, p.y);
  }
  Color getLitBg(int x,int y)
  {
    MapCell& mc=getCell(x,y);
    return calcLitColor(mc.bg,mc.lightTint,mc.lightStrength,mc.outdoors);
  }
  void setMemColors(const Color& argMemFg,const Color& argMemBg)
  {
    memFg=argMemFg;
    memBg=argMemBg;
  }
  void draw(Console* con, int mapX,int mapY,int x0,int y0,int w0,int h0)
  {
    updateLight(mapX,mapY,w0,h0);
    for(int y=0;y<h0;++y)
    {
      for(int x=0;x<w0;++x)
      {
        MapCell& mc=getCell(mapX+x,mapY+y);
        if(mc.visible && (mc.lightStrength>0 || mc.outdoors))
        {
          mc.saveMem();
          con->curBg=calcLitColor(mc.bg,mc.lightTint,mc.lightStrength,mc.outdoors);
          con->curFg=calcLitColor(mc.fg,mc.lightTint,mc.lightStrength,mc.outdoors);
          con->printAt({x0+x,y0+y},0, mc.sym);
        }else
        {
          con->curBg=memBg;
          con->curFg=memFg;
          con->printAt({x0+x,y0+y},0, mc.memSym);
        }
      }
    }
  }
  bool findPath(IPos from, IPos to)
  {
    return findPath(from.x, from.y, to.x, to.y);
  }
  bool findPath(int x0,int y0,int x1,int y1);
  uint32_t getHash(const IPos& pos)
  {
    return pos.x*pos.y*8882693;
  }
  int getSuccessors(StateCostTmpl<IPos,int>* succ,IPos* pos,IPos* par)
  {
    static int x8[]={-1,0,1,-1,1,-1,0,1};
    static int y8[]={-1,-1,-1,0,0,1,1,1};
    static int cs[]={141,100,141,100,100,141,100,141};
    int rv=0;
    for(int i=0;i<8;++i)
    {
      int x=pos->x+x8[i];
      int y=pos->y+y8[i];
      MapCell& mc=getCell(x,y);
      if(mc.moveCost==impassableTile)continue;
      if(par && par->x==x && par->y==y)continue;
      succ[rv].state=IPos(x,y);
      succ[rv].cost=cs[i]*mc.moveCost/100;
      ++rv;
    }
    return rv;
  }
  int goalDistanceEstimate(const IPos& a,const IPos& b)
  {
    int dx=abs(b.x-a.x);
    int dy=abs(b.y-a.y);
    if(dx>dy)
    {
      return dy*141+(dx-dy)*100;
    }else
    {
      return dx*141+(dy-dx)*100;
    }
  }

  void setMaxLightRadius(int newValue)
  {
    if(maxLightRadius!=newValue)
    {
      maxLightRadius=newValue;
      lightChanged=true;
    }
  }

  static const uint16_t impassableTile=65535;

protected:

  int maxLightRadius=15;

  bool lightChanged = true;
  IPos lastLightPos;

  struct MapCell{
    MapCell():bg(Color::black.packTo32()),fg(Color::white.packTo32()),lightTint(0),lsTint(0),lightStrength(0),lsStrength(0),
        moveCost(100),visible(false),outdoors(true),lightSource(false)
    {
      sym[0]=' ';sym[1]=0;memSym[0]=' ';memSym[1]=0;
      lightCone.closed=true;
    }
    UserTileInfo user;
    TileInfo ti;
    uint32_t bg,fg,lightTint,lsTint;
    AngleRange lightCone{0,0};
    uint16_t lightStrength,lsStrength;
    uint16_t moveCost;
    char sym[5];
    char memSym[5];
    bool visible;
    bool outdoors;
    bool lightSource;
    void setSym(const std::string& argSym)
    {
      size_t idx = 0;
      while(idx<argSym.length() && idx<sizeof(sym)-1)
      {
        sym[idx]=argSym[idx];
        ++idx;
      }
      sym[idx]=0;
    }
    void saveMem()
    {
      memcpy(memSym,sym,sizeof(sym));
    }
  };

  void updateLight(int x0,int y0,int w,int h)
  {
    if(!lightChanged && lastLightPos==IPos(x0,y0))
    {
      return;
    }
    lightChanged=false;
    lastLightPos=IPos(x0,y0);
    x0-=maxLightRadius;
    y0-=maxLightRadius;
    w+=maxLightRadius*2;
    h+=maxLightRadius*2;
    std::vector<std::pair<MapCell*,IPos>> lightSources;
    for(int y=y0;y<y0+h;++y)
    {
      for(int x=x0;x<x0+w;++x)
      {
        MapCell& mc=getCell(x,y);
        if(mc.lightSource)
        {
          lightSources.push_back(std::make_pair(&mc,IPos(x,y)));
        }
        mc.lightStrength=0;
        mc.lightTint=0;
      }
    }
    lightMode=true;
    for(auto p:lightSources)
    {
      MapCell& mc=*p.first;
      lightTint=mc.lsTint;
      lightStrength=mc.lsStrength;
      if(!mc.lightCone.closed)
      {
        cfov.setPermBlock(mc.lightCone);
      }
      calcFov(p.second.x,p.second.y,mc.lsStrength*maxLightRadius/1000);
      if(!mc.lightCone.closed)
      {
        cfov.clearPermBlock();
      }
    }
    lightMode=false;
  }

  Color calcLitColor(Color clr,Color tint,uint16_t strength,bool global)
  {
    Color rv=clr;
    if(global)
    {
      tint=tint*(strength/1000.0f)+globalLightTint*(globalLightStrength/1000.0f);
      tint.clamp();
      uint16_t tstr=strength+globalLightStrength;
      if(tstr>1000)tstr=1000;
      rv*=tstr/1000.0f;
    }else
    {
      tint=tint*(strength/1000.0f);
      rv*=strength/1000.0f;
    }
    Color tclr=clr;
    tclr.tint(tint);
    rv=(rv+tclr)/2.0f;
    rv.clamp();
    return rv;
  }

  struct ChunkHasher{
    uint32_t operator()(const IPos& pos)const
    {
      return pos.x*pos.y*8882693;
    }
  };

  typedef std::vector<MapCell> MapVector;
  typedef std::vector<MapVector> MapMatrix;
  typedef std::unordered_map<Posi<int>,MapMatrix,ChunkHasher> ChunkMap;
  ChunkMap cm;
  std::vector<IPos> visCells;

  MapCell& getCell(const IPos& pos)
  {
    return getCell(pos.x,pos.y);
  }

  IPos cachePos;
  MapMatrix* cacheChunk;
  MapCell& getCell(int x,int y)
  {
    IPos cpos(x>>4,y>>4);
    if(cpos==cachePos && cacheChunk)
    {
      return (*cacheChunk)[y&0xf][x&0xf];
    }
    typename ChunkMap::iterator it=cm.find(cpos);
    if(it==cm.end())
    {
      it=cm.insert(ChunkMap::value_type(cpos,MapMatrix(16,MapVector(16)))).first;
    }
    cachePos=cpos;
    cacheChunk=&it->second;
    return it->second[y&0xf][x&0xf];
  }

  bool lightMode;
  uint32_t lightTint;
  uint16_t lightStrength;
  uint16_t globalLightStrength;
  Color globalLightTint;
  Color memFg,memBg;
};

template <class UserTileInfo>
CircularFov<IPos> GumMap<UserTileInfo>::cfov;
template <class UserTileInfo>
typename GumMap<UserTileInfo>::MyAStar GumMap<UserTileInfo>::astar;

template <class UserTileInfo>
bool GumMap<UserTileInfo>::findPath(int x0,int y0,int x1,int y1)
{
  astar.assignMap(this);
  astar.setStartAndGoalStates(IPos(x0,y0),IPos(x1,y1));
 typename MyAStar::AStarSearchState st;
  while((st=astar.searchStep())==MyAStar::SEARCH_STATE_SEARCHING)
  {
  }
  return st==MyAStar::SEARCH_STATE_SUCCEEDED;
}
