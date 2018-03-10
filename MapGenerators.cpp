#include "MapGenerators.hpp"
#include "GameLoop.hpp"
#include "MissionDetails.hpp"
#include "Rand.hpp"

void fillTileInfo(GameMap& map, IPos p, const GameTile& tile)
{
  map.getUserInfo(p).tile=&tile;
  map.setInfo(p.x, p.y, tile.fg, tile.bg, tile.sym);
  map.setMoveCost(p.x, p.y, tile.walkCost > 0 ? tile.walkCost : GameMap::impassableTile);
  map.setBlock(p.x, p.y, tile.blocksLight);
  if(tile.lightSource)
  {
    map.setLightSource(p, tile.lightStrength, tile.lightTint);
  }
  else
  {
    map.setLightSource(p, 0, {});
  }
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
  void genEnemies(const std::vector<IRect>& placement)
  {
    std::set<IPos> used;
    for(auto& e:md.enemies)
    {
      IRect br=vectorRand(rnd, placement);
      for(int i=0;i<e.count;++i)
      {
        IPos rp;
        do{
          rp=rectRand(rnd, br);
        }while(used.find(rp)!=used.end());
        enemies.push_back({e.et, rp});
        if(e.scattered)
        {
          br=vectorRand(rnd, placement);
        }
      }
    }
  }

  const std::vector<MissionDetails::EnemyPosition>& getEnemiesPositions()
  {
    return enemies;
  }

protected:
  std::mt19937 rnd;
  MissionDetails md;
  IPos entrance;
  std::vector<MissionDetails::EnemyPosition> enemies;
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
    auto& tile=getTile(gtt);
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

void makeBarrier(GameMap& map, const IRect& mapRect)
{
  auto& btile=getTile(GameTileType::barrier);
  int x0=mapRect.pos.x-1;
  int y0=mapRect.pos.y-1;
  int x1=x0+mapRect.size.x+1;
  int y1=x0+mapRect.size.y+1;
  for(int x=x0;x<=x1;++x)
  {
    fillTileInfo(map, {x,y0}, btile);
    fillTileInfo(map, {x,y1}, btile);
  }
  for(int y=y0;y<=y1;++y)
  {
    fillTileInfo(map, {x0,y}, btile);
    fillTileInfo(map, {x1,y}, btile);
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
    IRect mapRect(0,0,200,200);
    BSPTree bsp(rnd);
    bsp.init(18,23,5,11);
    bsp.setBorderWidthCalculator(borderWidthCalculator);
    bsp.create(mapRect);
    auto borders=bsp.getBorders();
    entrance=borders.front()->border.pos+borders.front()->border.size/2;
    for(auto n:borders)
    {
      auto& tile = n->depth<5?getTile(GameTileType::aslphalt) : getTile(GameTileType::tilepath);
      auto& htile = getTile(GameTileType::aslphaltHLine);
      auto& vtile = getTile(GameTileType::aslphaltVLine);
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
    int carsCount=intRand(rnd, 20,30);

    for(int i=0;i<carsCount;++i)
    {
      IPos carPos;
      Dir carDir;
      for(;;)
      {
        auto n=vectorRand(rnd, borders);
        if(n->depth>4)
        {
          continue;
        }
        int lanes;
        IPos off;
        IPos sz;
        if(n->nt==BSPTree::NodeType::intermediateH)
        {
          lanes=n->border.size.x>7?2:1;
          carDir=boolRand(rnd)?Dir::up:Dir::down;
          if(carDir==Dir::up)
          {
            carPos.x=(n->border.tre().x-intRand(rnd,0,lanes)*3)-1;
          }
          else
          {
            carPos.x=(n->border.tle().x+intRand(rnd,0,lanes)*3)+1;
          }
          carPos.y=n->border.pos.y+3+intRand(rnd,3,n->border.size.y-6);
          off=IPos(1,3);
          sz=IPos(3,7);
        }
        else
        {
          lanes = n->border.size.y>7?2:1;
          carDir=boolRand(rnd)?Dir::left:Dir::right;
          if(carDir==Dir::left)
          {
            carPos.y=(n->border.tle().y+intRand(rnd, 0, lanes)*3)+1;
          }
          else
          {
            carPos.y=(n->border.ble().y-intRand(rnd,0, lanes)*3)-1;
          }
          carPos.x=n->border.pos.x+3+intRand(rnd, 3, n->border.size.x-6);
          off=IPos(3,1);
          sz=IPos(7,3);
        }
        bool good=true;
        for(auto p:IRect(carPos-off,sz))
        {
          if(map.getUserInfo(p).tile->gtt!=GameTileType::aslphalt)
          {
            good=false;
            break;
          }
        }
        if(good)
        {
          break;
        }
      }
      float r=0.5f+intRand(rnd, 0, 128)/127.0f;
      float g=0.5f+intRand(rnd, 0, 128)/127.0f;
      float b=0.5f+intRand(rnd, 0, 128)/127.0f;
      Color carColor(r,g,b);
      makeCar(map, carPos, carDir, carColor, md.mt!=MissionTime::day);
    }


    auto buildings = bsp.getRooms();
    for(auto n:buildings)
    {
      auto& wtile = getTile(GameTileType::concreteWall);
      auto& ptile = getTile(GameTileType::tilepath);
      auto& gtile = getTile(GameTileType::glassWall);
      auto& ftile1 = getTile(GameTileType::interiorFloor1);
      auto& ftile2 = getTile(GameTileType::interiorFloor2);
      IRect r = n->rect;
      //reserve some space around building
      r.pos+=IPos(5,5);
      r.size-=IPos(10,10);

      //pathtile around building
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
          //building walls
          fillTileInfo(map, p, wtile);
        }
        else
        {
          //building floor
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
      //bushes and benches around building
      {
        int decorSides=intRand(rnd, 0, 15);
        auto& bush=getTile(GameTileType::bush);
        auto& bench=getTile(GameTileType::bench);
        for(int i=0;i<4;++i)
        {
          if(!(decorSides&(1<<i)))
          {
            continue;
          }
          bool haveBench=boolRand(rnd);
          Dir d=getDir4FromInt(i);
          ILine side=getRectSide(n->rect, d);
          int count=0;
          for(auto p:side)
          {
            if(((count++)%3)==0)
            {
              if(count>6 && haveBench)
              {
                fillTileInfo(map, p, bench);
                haveBench=false;
              }
              else
              {
                fillTileInfo(map, p, bush);
              }
            }
          }
        }
      }

      {
        int windowWall=intRand(rnd,0, 4);
        Dir wd=getDir4FromInt(windowWall);
        ILine side=getRectSide(r, wd);
        IPos norm=getDir(wd);
        norm.rotCW();
        side.startPos+=norm*3;
        norm=getDir(wd);
        norm.rotCCW();
        side.endPos+=norm*3;
        IPos wdir=IPos(0,0)-getDir(wd);

        for(auto p:side)
        {
          fillTileInfo(map, p, gtile);
        }
        IPos wstart=side.startPos;
        IPos wend=side.endPos;
        IPos ddstart=wdir;
        ddstart.rotCW();
        IPos ddend=wdir;
        ddend.rotCCW();
        for(auto p:Linei<int>(wstart+wdir*2+ddstart, wend+wdir*2+ddend))
        {
          fillTileInfo(map, p, wtile);
        }
        if(chanceRand(rnd, 0.7))
        {
          auto& clight = getTile(GameTileType::ceilingLight);
          fillTileInfo(map, (wstart+wdir*2+wend)/2, clight);
        }
      }

      auto& ltile = md.mt==MissionTime::day?getTile(GameTileType::streetLightOff):getTile(GameTileType::streetLight);
      auto& bltile = getTile(GameTileType::brokenStreetLight);
      fillTileInfo(map, r.tle()-IPos{2,2}, chanceRand(rnd, 0.02)?bltile:ltile);
      fillTileInfo(map, r.tre()+IPos{2,-2}, chanceRand(rnd, 0.02)?bltile:ltile);
      fillTileInfo(map, r.ble()-IPos{2,-2}, chanceRand(rnd, 0.02)?bltile:ltile);
      fillTileInfo(map, r.bre()+IPos{2,2}, chanceRand(rnd, 0.02)?bltile:ltile);
    }
    //makeCar(map, entrance+IPos(3,3), Dir::down, Color::red, true);
    makeBarrier(map, mapRect);
    std::vector<IRect> enemiesPlacement;
    for(auto& b:borders)
    {
      enemiesPlacement.push_back(b->border);
    }
    genEnemies(enemiesPlacement);
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

IPos generateLevel(GeneratorType gt, uint32_t seed, const MissionDetails& md, std::vector<MissionDetails::EnemyPosition>& enemies, GameMap& map)
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
  enemies=gen->getEnemiesPositions();
  return gen->getEntrance();
}
