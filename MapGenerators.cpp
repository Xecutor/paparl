#include "MapGenerators.hpp"
#include "GameLoop.hpp"
#include "MissonDetails.hpp"
#include <random>

void fillTileInfo(GameMap& map, IPos p, const GameTile& tile)
{
  map.getUserInfo(p).tile=&tile;
  map.setInfo(p.x, p.y, tile.fg, tile.bg, tile.sym);
  map.setMoveCost(p.x, p.y, tile.walkCost);
  map.setBlock(p.x, p.y, tile.blocksLight);
  if(tile.lightSource)
  {
    map.setLightSource(p.x, p.y, tile.lightStrength, tile.lightTint);
  }
}

int intRand(std::mt19937& rnd, int minVal, int maxValEx)
{
  double v = rnd();
  v-=rnd.min();
  v/=((double)rnd.max())+1.0;
  v*=(maxValEx-minVal);
  v+=minVal;
  return (int)v;
}

bool boolRand(std::mt19937& rnd)
{
  auto v = rnd();
  v-=rnd.min();
  return v>=(rnd.max()-rnd.min())/2;
}

double dblRand(std::mt19937& rnd)
{
  double v = rnd();
  v-=rnd.min();
  v/=((double)rnd.max())+1.0;
  return v;
}

bool chanceRand(std::mt19937& rnd, double chance)
{
  return dblRand(rnd)<chance;
}

class BSPTree{
public:
  BSPTree(std::mt19937& argRnd):rnd(argRnd){}
  void init(int argMinSize, int argMaxSize, int argBorderMinSize, int argBorderMaxSize)
  {
    minSize=argMinSize;
    maxSize=argMaxSize;
    borderMinSize=argBorderMinSize;
    borderMaxSize=argBorderMaxSize;
  }
  void create(IRect space)
  {
    root=alloc();
    root->rect=space;
    createRec(root, 0);
  }

  enum class NodeType{
    none,
    intermediateH,
    intermediateV,
    finalFull,
    finalExtraSpace
  };

  struct Node{
    NodeType nt=NodeType::none;
    IRect rect;
    IRect border;
    int depth = 0;
    std::unique_ptr<Node> left, right;
  };

  Node& get()
  {
    return *root;
  }

  std::vector<Node*> getBorders()
  {
    std::vector<Node*> rv;
    getBordersRec(root.get(), rv);
    return rv;
  }

  std::vector<Node*> getRooms()
  {
    std::vector<Node*> rv;
    getRoomsRec(root.get(), rv);
    return rv;
  }

  void setBorderWidthCalculator(std::function<int(std::mt19937&,int)> bwc)
  {
    borderWidthCalculator=bwc;
  }

protected:
  std::mt19937& rnd;
  int minSize;
  int maxSize;
  int borderMinSize;
  int borderMaxSize;

  std::function<int(std::mt19937&,int)> borderWidthCalculator;

  std::unique_ptr<Node> root;
  std::unique_ptr<Node> alloc()
  {
    return std::make_unique<Node>();
  }
  void createRec(std::unique_ptr<Node>& node, int depth)
  {
    IRect r = node->rect;
    int bwidth;
    if(borderWidthCalculator)
    {
      bwidth=borderWidthCalculator(rnd, depth);
    }
    else
    {
      bwidth=intRand(rnd, borderMinSize, borderMaxSize + 1);
    }
    //horizontal division. parts are to the left and to the right of the border
    bool hdiv = boolRand(rnd);
    int freeSz = (hdiv?r.size.x:r.size.y)-bwidth;
    if(freeSz/2<minSize)
    {
      hdiv=!hdiv;
      freeSz = (hdiv?r.size.x:r.size.y)-bwidth;
      if(freeSz/2<minSize)
      {
        node->nt=NodeType::finalExtraSpace;
        return;
      }
    }
    int bPosRange = freeSz-minSize*2;
    int bPos = intRand(rnd, minSize, minSize+bPosRange+1);
    if(hdiv)
    {
      node->nt=NodeType::intermediateH;
      node->left=alloc();
      node->left->depth=depth+1;
      node->left->rect.pos=r.pos;
      node->left->rect.size=IPos{bPos, r.size.y};
      node->border.pos=r.pos+IPos(bPos, 0);
      node->border.size={bwidth, r.size.y};
      node->right=alloc();
      node->right->depth=depth+1;
      node->right->rect.pos=node->border.tr();
      node->right->rect.size={r.size.x-node->left->rect.size.x-bwidth, r.size.y};
    }
    else
    {
      node->nt=NodeType::intermediateV;
      node->left=alloc();
      node->left->depth=depth+1;
      node->left->rect.pos=r.pos;
      node->left->rect.size=IPos{r.size.x, bPos};
      node->border.pos=r.pos+IPos(0, bPos);
      node->border.size={r.size.x, bwidth};
      node->right=alloc();
      node->right->depth=depth+1;
      node->right->rect.pos=node->border.bl();
      node->right->rect.size={r.size.x, r.size.y-node->left->rect.size.y-bwidth};
    }
    if(node->left->rect.size.x>maxSize || node->left->rect.size.y>maxSize)
    {
      createRec(node->left, depth+1);
    }
    else
    {
      if(node->left->rect.size.x>minSize && node->left->rect.size.y>minSize && node->left->rect.size.x<maxSize && node->left->rect.size.y<maxSize )
      {
        node->left->nt=NodeType::finalFull;
      }
      else
      {
        node->left->nt=NodeType::finalExtraSpace;
      }
    }
    if(node->right->rect.size.x>maxSize || node->right->rect.size.y>maxSize)
    {
      createRec(node->right, depth+1);
    }
    else
    {
      if(node->right->rect.size.x>minSize && node->right->rect.size.y>minSize && node->right->rect.size.x<maxSize && node->right->rect.size.y<maxSize )
      {
        node->right->nt=NodeType::finalFull;
      }
      else
      {
        node->right->nt=NodeType::finalExtraSpace;
      }
    }
  }
  void getBordersRec(Node* ptr, std::vector<Node*>& v)
  {
    if(ptr->nt==NodeType::intermediateH || ptr->nt==NodeType::intermediateV)
    {
      v.push_back(ptr);
      getBordersRec(ptr->left.get(), v);
      getBordersRec(ptr->right.get(), v);
    }
  }
  void getRoomsRec(Node* ptr, std::vector<Node*>& v)
  {
    if(ptr->nt!=NodeType::intermediateH && ptr->nt!=NodeType::intermediateV)
    {
      v.push_back(ptr);
    }
    else
    {
      getRoomsRec(ptr->left.get(), v);
      getRoomsRec(ptr->right.get(), v);
    }
  }
};

class Generator{
public:
  virtual ~Generator(){}
  void init(uint32_t seed, const MissionDetails& argMd)
  {
    rnd.seed(seed);
    md=argMd;
  }
  virtual void generate(GameMap& map)=0;
  IPos getEntrance()
  {
    return entrance;
  }
protected:
  std::mt19937 rnd;
  MissionDetails md;
  IPos entrance;
};

/*
/-\
| |
+-+
| |
| |
+-+
\_/
*/

void makeCar(GameMap& map, IPos pos, Dir d, Color clr, bool lights)
{
  using GTT=GameTileType;
  GameTileType tmpl[] = {
    GTT::carTL, GTT::carH,  GTT::carTR,
    GTT::carV,  GTT::carSP, GTT::carV,
    GTT::carX,  GTT::carH,  GTT::carX,
    GTT::carV,  GTT::carSP, GTT::carV,
    GTT::carV,  GTT::carSP, GTT::carV,
    GTT::carX,  GTT::carH,  GTT::carX,
    GTT::carTR, GTT::carB, GTT::carTL
  };
  IPos cc(1,3);
  int idx=0;

  for(auto p:IRect(0,0,3,7))
  {
    p-=cc;
    int sa = AngleRange::SCALED_PI+AngleRange::SCALED_PI_2+AngleRange::SCALED_PI_2/2;
    int ea = AngleRange::SCALED_PI+AngleRange::SCALED_PI_2/2;
    switch(d)
    {
      case Dir::right:
        p.rotCW();
        sa = AngleRange::addNorm(sa, AngleRange::SCALED_PI_2);
        ea = AngleRange::addNorm(ea, AngleRange::SCALED_PI_2);
        break;
      case Dir::down:
        p.rotCW();p.rotCW();
        sa = AngleRange::addNorm(sa, AngleRange::SCALED_PI);
        ea = AngleRange::addNorm(ea, AngleRange::SCALED_PI);
        break;
      case Dir::left:
        p.rotCCW();
        sa = AngleRange::subNorm(sa, AngleRange::SCALED_PI_2);
        ea = AngleRange::subNorm(ea, AngleRange::SCALED_PI_2);
        break;
      default:break;
    }
    auto gtt=tmpl[idx];
    if(d==Dir::left || d==Dir::right)
    {
      switch(gtt)
      {
        case GTT::carTL:gtt=GTT::carTR;break;
        case GTT::carTR:gtt=GTT::carTL;break;
        case GTT::carH:gtt=GTT::carV;break;
        case GTT::carV:gtt=GTT::carH;break;
        case GTT::carB:gtt=GTT::carV;break;
        default:break;
      }
    }
    else if(d==Dir::down)
    {
      if(gtt==GTT::carB)
      {
        gtt=GTT::carH;
      }
    }
    auto tile=getTile(gtt);
    p+=pos;
    Color bg=map.getUserInfo(p.x, p.y).tile->bg;
    fillTileInfo(map, p, tile);
    map.setBg(p.x, p.y, bg);
    map.setFg(p.x, p.y, clr);
    if((idx==0 || idx==2) && lights)
    {
      map.setLightCone(p, AngleRange(sa, ea));
      map.setLightSource(p.x, p.y, 1000, Color::white);
    }

    ++idx;
  }
}

class DowntownGenerator : public Generator{
public:
  static int borderWidthCalculator(std::mt19937& rnd, int depth)
  {
    if(depth<5)
    {
      switch(depth)
      {
        case 0:return 13;
        case 1:return boolRand(rnd)?13:7;
        case 2:return boolRand(rnd)?13:7;
      }
      return 7;
    }
    else
    {
      return 4;
    }
  }
  void generate(GameMap& map)override
  {
    BSPTree bsp(rnd);
    bsp.init(15,20,5,11);
    bsp.setBorderWidthCalculator(borderWidthCalculator);
    bsp.create({{0,0},{200,200}});
    auto borders=bsp.getBorders();
    entrance=borders.front()->border.pos+borders.front()->border.size/2;
    for(auto n:borders)
    {
      auto tile = n->depth<5?getTile(GameTileType::aslphalt) : getTile(GameTileType::tilepath);
      auto htile = getTile(GameTileType::aslphaltHLine);
      auto vtile = getTile(GameTileType::aslphaltVLine);
      for(auto p:n->border)
      {
        if(n->depth<=4 && n->nt==BSPTree::NodeType::intermediateH && p.x==n->border.pos.x+n->border.size.x/2)
        {
            fillTileInfo(map, p, vtile);
        }
        else if(n->depth<=4 && n->nt==BSPTree::NodeType::intermediateV && p.y==n->border.pos.y+n->border.size.y/2)
        {
          fillTileInfo(map, p, htile);
        }
        else
        {
          fillTileInfo(map, p, tile);
        }
      }
    }
    auto buildings = bsp.getRooms();
    for(auto n:buildings)
    {
      auto wtile = getTile(GameTileType::concreteWall);
      auto ptile = getTile(GameTileType::tilepath);
      auto gtile = getTile(GameTileType::glassWall);
      auto ftile1 = getTile(GameTileType::interiorFloor1);
      auto ftile2 = getTile(GameTileType::interiorFloor2);
      IRect r = n->rect;
      r.pos+=IPos(3,3);
      r.size-=IPos(6,6);
      for(auto p:n->rect)
      {
        if(!r.isInside(p))
        {
          fillTileInfo(map, p, ptile);
        }
      }
      for(auto p:r)
      {
        if(r.onBorder(p))
        {
          fillTileInfo(map, p, wtile);
        }
        else
        {
          if((p.x+p.y)%2)
          {
            fillTileInfo(map, p, ftile1);
          }
          else
          {
            fillTileInfo(map, p, ftile2);
          }
        }
      }
      int windowWall=intRand(rnd,0, 4);
      IPos wstart,wend;
      IPos wdir;
      switch(windowWall)
      {
        case 0:
        {
          wstart=r.tle()+IPos(3,0);
          wend=r.tre()-IPos(3,0);
          wdir=IPos(0,1);
          break;
        }
        case 1:
        {
          wstart=r.tre()+IPos(0,3);
          wend=r.bre()-IPos(0,3);
          wdir=IPos(-1,0);
          break;
        }
        case 2:
        {
          wstart=r.bre()-IPos(3,0);
          wend=r.ble()+IPos(3,0);
          wdir=IPos(0,-1);
          break;
        }
        case 3:
        {
          wstart=r.ble()-IPos(0,3);
          wend=r.tle()+IPos(0,3);
          wdir=IPos(1,0);
          break;
        }
      }
      for(auto p:Linei<int>(wstart, wend))
      {
        fillTileInfo(map, p, gtile);
      }
      {
        IPos ddstart=wdir;
        ddstart.rotCW();
        IPos ddend=wdir;
        ddend.rotCCW();
        for(auto p:Linei<int>(wstart+wdir*2+ddstart, wend+wdir*2+ddend))
        {
          fillTileInfo(map, p, wtile);
        }
      }
      if(chanceRand(rnd, 0.7))
      {
        auto clight = getTile(GameTileType::ceilingLight);
        fillTileInfo(map, (wstart+wdir*2+wend)/2, clight);
      }


      auto ltile = md.mt==MissionTime::day?getTile(GameTileType::streetLightOff):getTile(GameTileType::streetLight);
      auto bltile = getTile(GameTileType::brokenStreetLight);
      fillTileInfo(map, r.tle()-IPos{2,2}, chanceRand(rnd, 0.02)?bltile:ltile);
      fillTileInfo(map, r.tre()+IPos{2,-2}, chanceRand(rnd, 0.02)?bltile:ltile);
      fillTileInfo(map, r.ble()-IPos{2,-2}, chanceRand(rnd, 0.02)?bltile:ltile);
      fillTileInfo(map, r.bre()+IPos{2,2}, chanceRand(rnd, 0.02)?bltile:ltile);
    }
    makeCar(map, entrance+IPos(3,3), Dir::down, Color::red, true);
  }
protected:
};

class SuburbsGenerator : public Generator{
public:
  void generate(GameMap& map)override
  {

  }
protected:
};

class WarehousGenerator : public Generator{
public:
  void generate(GameMap& map)
  {

  }
protected:
};

class SeaportGenerator : public Generator{
public:
  void generate(GameMap& map)
  {

  }
protected:
};

IPos generateLevel(GeneratorType gt, uint32_t seed, const MissionDetails& md, GameMap& map)
{
  std::unique_ptr<Generator> gen;
  switch(gt)
  {
    case GeneratorType::downtown:
      gen=std::make_unique<DowntownGenerator>();
      break;
    case GeneratorType::suburbs:
      gen=std::make_unique<SuburbsGenerator>();
      break;
    case GeneratorType::warehouse:
      gen=std::make_unique<WarehousGenerator>();
    case GeneratorType::seaport:
      gen=std::make_unique<SeaportGenerator>();
      break;
  }
  gen->init(seed, md);
  gen->generate(map);
  return gen->getEntrance();
}
