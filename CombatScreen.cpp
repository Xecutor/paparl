#include "CombatScreen.hpp"
#include "MapGenerators.hpp"
#include "MissionDetails.hpp"
#include "Enemy.hpp"
#include "Menu.hpp"

void customformat(kst::FormatBuffer& buf,double v,int w,int p)
{
  char buf2[64];
  size_t len=sprintf(buf2,"%*.*lf",w,p,v);
  buf.Append(buf2,len);
}

void customformat(kst::FormatBuffer& buf,float v,int w,int p)
{
  char buf2[64];
  size_t len=sprintf(buf2,"%*.*lf",w,p,v);
  buf.Append(buf2,len);
}


class NrgySelector:public GameScreen{
public:
  NrgySelector(Console& argCon, ScreensController* argScon, PlayerPtr argPlayer):GameScreen (argCon, argScon),player(argPlayer)
  {
    oldNrgy=player->getNrgyDistr();
  }

  NrgySelector& setCancelHandler(std::function<void()> cancelHandler)
  {
    onCancel=cancelHandler;
    return *this;
  }

  NrgySelector& setAcceptHandler(std::function<void()> acceptHandler)
  {
    onAccept=acceptHandler;
    return *this;
  }

  void onKeyboardEvent(const KeyboardEvent& evt)override
  {
    if(evt.eventType!=ketPress)
    {
      return;
    }
    IPos dpos;
    Dir d;
    if(getDirForKey(evt.keySym, dpos, d))
    {
      switch(d)
      {
        case Dir::up:
        {
          player->changeNrgySlot(selSlot, 0.5f);
          break;
        }
        case Dir::down:
        {
          player->changeNrgySlot(selSlot, -0.5f);
          break;
        }
        case Dir::right:
        {
          selSlot=(AgentActor::NrgySlot)((selSlot+1)%4);
          break;
        }
        case Dir::left:
        {
          selSlot=(AgentActor::NrgySlot)((selSlot-1+4)%4);
          break;
        }
        default:break;
      }
    }
    else
    {
      using namespace keyboard;
      if(evt.keySym==GK_ESCAPE)
      {
        for(int i=0;i<4;++i)
        {
          AgentActor::NrgySlot ns=(AgentActor::NrgySlot)i;
          player->changeNrgySlot(ns, -player->getNrgyDistr()[ns]);
        }
        for(int i=0;i<4;++i)
        {
          AgentActor::NrgySlot ns=(AgentActor::NrgySlot)i;
          player->changeNrgySlot(ns,oldNrgy[i]);
        }
        if(onCancel)
        {
          onCancel();
        }
        close();
      }
      if(evt.keySym==GK_RETURN || evt.keySym==GK_RETURN2 || evt.keySym==GK_KP_ENTER)
      {
        bool nrgyChanged=false;
        auto curNrgy=player->getNrgyDistr();
        for(int i=0;i<4;++i)
        {
          float diff=fabsf(oldNrgy[i]-curNrgy[i]);
          if(diff>std::numeric_limits<float>::epsilon())
          {
            nrgyChanged=true;
            break;
          }
        }
        if(nrgyChanged)
        {
          if(onAccept)
          {
            onAccept();
          }
        }
        else
        {
          if(onCancel)
          {
            onCancel();
          }
        }
        close();
      }
    }
  }
  void draw()override
  {
    IRect r=calcRect({}, IPos{8, 11}, roScreenCenter);
    con.clearRect(r);
    con.fillRect(r, Color(0x47a0a3));
    auto nrgy=player->getNrgyDistr();
    int idx=0;
    IPos p=r.ble();
    const char* nrgLabels[]={
      "AT", "DE", "SP", "PR"
    };
    for(auto v:nrgy)
    {
      int h=(int)(v*2);
      con.fillRect(IRect{p+IPos(0,-h), IPos(1, h)}, Color::red);
      if(idx!=(int)selSlot)
      {
        con.printAt(p, Color::red, {}, Console::pfKeepBackground, nrgLabels[idx]);
      }
      else
      {
        con.printAt(p, Color::black, Color::red, 0, nrgLabels[idx]);
      }
      p.x+=2;
      ++idx;
    }
  }
  bool isFullScreen()const override
  {
    return false;
  }
  bool dimBackground()const override
  {
    return true;
  }
  std::function<void()> onCancel;
  std::function<void()> onAccept;
  PlayerPtr player;
  std::array<float,4> oldNrgy;
  AgentActor::NrgySlot selSlot = AgentActor::NrgySlot::nsAttack;
};

void CombatScreen::init()
{
  if(md.mt==MissionTime::day)
  {
    map.setGlobalLight(800, 0x2d7596);
  }
  else if(md.mt==MissionTime::night)
  {
    map.setGlobalLight(0, 0);
  }
  else if(md.mt==MissionTime::evening)
  {
    map.setGlobalLight(200, 0x453370);
  }
  std::vector<MissionDetails::EnemyPosition> enemies;
  auto ent=generateLevel(GeneratorType::downtown, 0, md, enemies, map);
  map.setMemColors(Color(0.0f,0.0f,0.15f), Color(0.05f,0.0f,0.05f));
  addActor(ent, 0.0f, player);
  for(auto& e:enemies)
  {
    addActor(e.pos, 1.0f, std::make_shared<EnemyBase>(e.et));
  }
  //addActor(ent+IPos(3,3), 1.0f, std::make_shared<EnemyBase>(EnemyType::angryGhost));
  turn();
}

void CombatScreen::printPanelText(Color clr, const char* txt)
{
  con.printAt(panelPos, clr, {}, Console::pfKeepBackground, txt);
  ++panelPos.y;
}

void CombatScreen::printBar(Color clr1, Color clr2, float val)
{
  int hw=(int)(val*mapOffsetX);
  con.fillRect(IRect{panelPos, {hw, 1}}, clr1);
  con.fillRect(IRect{panelPos+IPos(hw,0), {mapOffsetX-hw, 1}}, clr2);
  ++panelPos.y;
}

void CombatScreen::drawPanel()
{
  if(scanAnimation)
  {
    progressScan();
  }
  con.fillRect(IRect{0,0,mapOffsetX, con.height()}, Color(0x47a0a3));
  panelPos={0,0};
  printPanelText(Color::black, "Health");
  printBar(Color::green, Color::red, player->getHp());
  printPanelText(Color::black, "Stamina");
  printBar(Color::blue, Color::red, player->getStamina());
  auto nrgy = player->getNrgyDistr();
  printPanelText(Color::black,
                 FORMAT("AT:%{:.1} DE:%{:.1} SP:%{:.1} PR:%{:.1}",
                   nrgy[AgentActor::nsAttack],
                   nrgy[AgentActor::nsDefense],
                   nrgy[AgentActor::nsSpeed],
                   nrgy[AgentActor::nsPerception]));
  printPanelText(Color::black,"");

  if(map.getVis(lookPos))
  {
    Color bg=map.getLitBg(lookPos);
    float h,s,v;
    bg.toHSV(h,s,v);
    if(v>0.01)
    {
      auto& info=map.getUserInfo(lookPos);
      auto tile=info.tile;
      auto actor=info.actor;
      if(actor)
      {
        printPanelText(Color::black, FORMAT("%{} on the %{}", actor->getName(), tile->desc));
      }
      else
      {
        printPanelText(Color::black, FORMAT("%{}", tile->desc));
      }
    }
  }

  for(auto p : map.getVisibleCells())
  {
    auto actor=map.getUserInfo(p).actor;
    if(actor && actor->isEnemy())
    {
      printPanelText(Color::black, FORMAT("%{}", actor->getName()));
      printBar(Color::green, Color::red, actor->getHp());
    }
  }
}

void CombatScreen::onKeyboardEvent(const KeyboardEvent &evt)
{
  if(evt.eventType!=ketPress)
  {
    return;
  }
  using namespace keyboard;
  float turnCost;
  bool turnSpent = false;
  Dir d;
  IPos dpos;
  if(!getDirForKey(evt.keySym, dpos, d))
  {
    switch(evt.keySym)
    {
      case GK_N:
      {
        scon->pushScreen<NrgySelector>(player).setAcceptHandler([this](){nrgyChanged();});
        break;
      }
      case GK_L:
      {
        player->toggleFlashLight();
        turnCost=1.0f;
        turnSpent=true;
        break;
      }
      case GK_S:
        startScan();
        break;
      default:
        break;
    }
  }
  else
  {
    int wc=canActorMoveTo(*player,dpos);
    if(wc==-1)
    {
      auto target=map.getUserInfo(player->getPos()+dpos).actor;
      if(target)
      {
        target->takeDamage(this, player.get(), player->getAttack());
        turnCost=1.0f;
        turnSpent=true;
      }
    }
    else
    {
      if(wc!=0)
      {
        turnCost=moveActorBy(player, dpos);
        turnSpent=true;
      }
    }
  }
  if(turnSpent)
  {
    timeLine.add(turnCost, player);
    turn();
  }
}

void CombatScreen::onMouseEvent(const MouseEvent& evt)
{
  if(evt.eventType!=metMove)
  {
    return;
  }
  lookPos=con.screenToCon(evt);
  lookPos-=IPos(mapOffsetX, mapOffsetY);
  int dx = (con.width()-mapOffsetX)/2;
  int dy = (con.height()-mapOffsetY)/2;
  int mx = player->getPos().x - dx;
  int my = player->getPos().y - dy;
  lookPos+=IPos(mx,my);
}

void CombatScreen::startScan()
{
  auto enemies=timeLine.getEnemies();
  for(auto e:enemies)
  {
    scanTargets.push_back(e->getPos());
  }
  scanAnimation=true;
  scanCreateIndex=0;
  auto& ring=map.cfov.getRing(20);
  scanRing.clear();
  for(auto& r:ring)
  {
    scanRing.push_back({r.p, r.r});
  }
  std::sort(scanRing.begin(), scanRing.end(),[](const RingItem& a, const RingItem&b){return a.r.sa<b.r.sa;});
}

void CombatScreen::progressScan()
{
  IPos pp=player->getPos();
  IPos c=getMapArea().pos+getMapArea().size/2;
  if(scanCreateIndex<scanRing.size()+scanRing.size()/4)
  {
    auto pr=scanRing[(scanCreateIndex-1+scanRing.size())%scanRing.size()];
    auto r=scanRing[scanCreateIndex%scanRing.size()];
    auto nr=scanRing[(scanCreateIndex+1)%scanRing.size()];
    ++scanCreateIndex;
    AngleRange ar(pr.r.sa, nr.r.ea);
    bool found=false;
    for(auto t:scanTargets)
    {
      int a=AngleRange::getAngle(t.x-pp.x,t.y-pp.y);
      if(ar.inRange(a))
      {
        found=true;
        break;
      }
    }
    scanItems.push_back({});
    scanItems.back().dst=c+r.pos;
    scanItems.back().clr=Color(0x399b30);
    scanItems.back().needToFade=!found;
  }
  if(scanItems.empty())
  {
    scanAnimation=false;
    return;
  }
  for(auto& si:scanItems)
  {
    for(auto p:ILine(c,si.dst))
    {
      con.printAt(p, {}, si.clr, Console::pfKeepForeground, " ");
    }
    if(si.needToFade)
    {
      si.clr.changeValue(0.9f);
    }
    --si.fadeCount;
  }
  scanItems.erase(std::remove_if(scanItems.begin(), scanItems.end(),[](const ScanItem& si){return si.fadeCount==0;}), scanItems.end());
}

void CombatScreen::nrgyChanged()
{
  timeLine.add(1.0f, player);
  turn();
}
