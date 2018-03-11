#include "CombatScreen.hpp"
#include "MapGenerators.hpp"
#include "MissionDetails.hpp"
#include "Enemy.hpp"
#include "Menu.hpp"
#include "TextMessage.hpp"

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
      switch(evt.keySym)
      {
        case GK_ESCAPE:
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
          break;
        }
        case GK_RETURN:
          case GK_RETURN2:
          case GK_KP_ENTER:
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
          break;
        }
        case GK_1:
          selSlot=AgentActor::NrgySlot::nsAttack;
          break;
        case GK_2:
          selSlot=AgentActor::NrgySlot::nsDefense;
          break;
        case GK_3:
          selSlot=AgentActor::NrgySlot::nsSpeed;
          break;
        case GK_4:
          selSlot=AgentActor::NrgySlot::nsPerception;
          break;
        case GK_HOME:
        {
          player->changeNrgySlot(selSlot, 5.0f);
          break;
        }
        case GK_END:
        {
          if(evt.keyMod&GK_MOD_CTRL)
          {
            for(int i=0;i<4;++i)
            {
              player->changeNrgySlot((AgentActor::NrgySlot)i, -5.0f);
            }
          }
          else
          {
            player->changeNrgySlot(selSlot, -5.0f);
          }
          break;
        }
        default:break;
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

struct CombatScreen::ScanAnimation:CombatScreen::Animation{
  ScanAnimation(CombatScreen* argCs):cs(argCs){}
  CombatScreen* cs;
  void step()override
  {
    cs->progressScan();
  }
  bool hasFinished()const override
  {
    return !cs->scanAnimation;
  }

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
  auto ent=generateLevel(md.gt, md.seed, md, enemies, map);
  map.setMemColors(Color(0.0f,0.0f,0.15f), Color(0.05f,0.0f,0.05f));
  addActor(ent, 0.0f, player);
  for(auto& e:enemies)
  {
    addActor(e.pos, 1.0f, std::make_shared<EnemyBase>(e.et));
  }
  for(auto et:equipped)
  {
    player->addEquipment(createAgentEquipment(et));
  }
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
  for(auto& a:animations)
  {
    a->step();
  }
  animations.erase(std::remove_if(animations.begin(), animations.end(), [](auto& a){return a->hasFinished();}), animations.end());

  if(targetMode)
  {
    for(auto p:ILine(player->getPos(), targetPos))
    {
      p=mapToScreen(p);
      con.fillRect({{p},{1,1}}, 0xba220e);
    }
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
  for(auto& e:player->getEquipment())
  {
    if(e->canBeFired || e->canBeThrown)
    {
      if(e->count)
      {
        printPanelText(Color::black, FORMAT("%{} x%{}", e->getName(), e->count));
      }
    }
    else
    {
      printPanelText(Color::black,e->getName().c_str());
    }
  }
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
      if(actor->isHidden(0.0f))
      {
        if(!actor->isHidden(player->getPerception()))
        {
          addActorToMap(actor);
        }
        else
        {
          continue;
        }
      }
      if(actor->getPos()==lookPos)
      {
        con.fillRect({panelPos, {mapOffsetX,1}}, 0x22bbc9);
      }
      printPanelText(Color::black, FORMAT("%{}", actor->getName()));
      printBar(Color::red, Color::black, actor->getHp());
      runMode=false;
    }
  }
}

void CombatScreen::onKeyboardEvent(const KeyboardEvent &evt)
{
  if(evt.eventType!=ketPress)
  {
    return;
  }
  if(runMode)
  {
    runMode=false;
  }
  for(auto& a:animations)
  {
    if(a->isBlocking())
    {
      return;
    }
  }
  using namespace keyboard;
  if(targetMode)
  {
    Dir d;
    IPos dpos;
    if(!getDirForKey(evt.keySym, dpos, d))
    {
      switch(evt.keySym)
      {
        case GK_ESCAPE:targetMode=false;break;
        case GK_RETURN:
        case GK_RETURN2:
        case GK_KP_ENTER:
          targetMode=false;
          if(targetFor==tfFire)
          {
            for(auto& e:player->getEquipment())
            {
              if(e->canBeFired)
              {
                e->fireThis(this);
              }
            }
          }
          else if(targetFor==tfThrow)
          {
            for(auto& e:player->getEquipment())
            {
              if(e->canBeThrown)
              {
                e->throwThis(this);
              }
            }
          }
          timeLine.add(1.0, player);
          turn();
          break;
        default:break;
      }
    }
    else
    {
      targetPos+=dpos;
    }
    return;
  }
  float turnCost;
  bool turnSpent = false;
  Dir d;
  IPos dpos;
  if(!getDirForKey(evt.keySym, dpos, d))
  {
    switch(evt.keySym)
    {
      case GK_ESCAPE:
      {
        scon->pushScreen<Menu>().
            setDimBackground(true).
            add("Continue", [](){}).
            add("Request evacuation", [this](){
              escapeMode=true;
              scon->pushScreen<TextMessage>("You must reach the barrier for the evacuation.");
            }).
            add("Request URGENT evacuation",[this](){
              abortMission(true);
            });

      }
      case GK_CLEAR: //??? sdl?
      case GK_KP_5:
      case GK_SPACE:
      case GK_PERIOD:
        turnCost=player->getSpeed();
        turnSpent=true;
        break;
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
      case GK_H:
      {
        scon->pushScreen<TextMessage>(
R"(
Use cursor or keypad to move around
space, . or keypad5 to skip turn
n - show Nrgy distribution screen
l - toggle flashlight
f - fire the gun
t - throw a grenade
s - use scanner
)");
        break;
      }
      case GK_F:
      {
        bool haveGun=false;
        bool haveAmmo=false;
        for(auto& e:player->getEquipment())
        {
          if(e->canBeFired)
          {
            haveGun=true;
            haveAmmo=e->count;
            break;
          }
        }
        if(!haveGun)
        {
          scon->pushScreen<TextMessage>("You don't have a gun").setBg(0xc6c11f);
          return;
        }
        if(!haveAmmo)
        {
          scon->pushScreen<TextMessage>("You are out of ammo").setBg(0xc6c11f);
          return;
        }
        targetFor=TargetFor::tfFire;
        enterTargetMode();
        break;
      }
      case GK_T:
      {
        bool haveGrenade=false;
        for(auto& e:player->getEquipment())
        {
          if(e->canBeThrown)
          {
            haveGrenade=e->count>0;
            break;
          }
        }
        if(!haveGrenade)
        {
          scon->pushScreen<TextMessage>("You don't have a grenade");
          return;
        }
        targetFor=TargetFor::tfThrow;
        enterTargetMode();
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
        if(evt.keyMod&GK_MOD_SHIFT)
        {
          runMode=true;
          runDir=dpos;
        }
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

void CombatScreen::restoreMapAt(IPos p)
{
  auto& to=map.getUserInfo(p);
  fillTileInfo(map, p, *to.tile);
  if(to.actor)
  {
    addActorToMap(to.actor);
  }
}

void CombatScreen::playerDied()
{
  MissionResultInfo mri;
  mri.result=MissionResultInfo::kia;
  onMissionEnd(mri);
  close();
}

void CombatScreen::abortMission(bool evac)
{
  MissionResultInfo mri;
  mri.result=evac?MissionResultInfo::evacuated:MissionResultInfo::escaped;
  onMissionEnd(mri);
  close();
}

void CombatScreen::afterTurn()
{
  if(escapeMode)
  {
    for(auto p:IRect(-1,-1,3,3))
    {
      auto tile=map.getUserInfo(player->getPos()+p).tile;
      if(tile && tile->gtt==GameTileType::barrier)
      {
        abortMission(false);
        return;
      }
    }
  }
  if(!timeLine.getEnemies().empty())
  {
    return;
  }
  MissionResultInfo mri;
  mri.result=MissionResultInfo::success;
  onMissionEnd(mri);
  close();
}

void CombatScreen::startScan()
{
  animations.push_back(std::make_shared<ScanAnimation>(this));
  auto enemies=timeLine.getEnemies();
  scanTargets.clear();
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
  for(int i=0;i<2;++i)
  {
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

void CombatScreen::enterTargetMode()
{
  targetMode=true;
  auto& visCells=map.getVisibleCells();
  std::vector<GameActorPtr> enemies;
  for(auto p:visCells)
  {
    auto& to=map.getUserInfo(p);
    if(to.actor && to.actor->isEnemy())
    {
      enemies.push_back(to.actor);
    }
  }
  if(enemies.empty())
  {
    targetPos=player->getPos();
    return;
  }
  IPos pp=player->getPos();
  std::sort(enemies.begin(), enemies.end(), [pp](auto& a, auto& b){
    IPos p1=a->getPos()-pp;
    IPos p2=b->getPos()-pp;
    return (p1.x*p1.x+p1.y*p1.y)<(p2.x*p2.x+p2.y*p2.y);
  });
  targetPos=enemies.front()->getPos();
}
