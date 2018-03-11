#include "MissionControl.hpp"
#include "CombatScreen.hpp"
#include "Menu.hpp"
#include "MissionSelector.hpp"
#include "EquipmentSelector.hpp"
#include "TextMessage.hpp"

const std::string MissionControl::saveFileName="paparl.save";

void MissionControl::init()
{
  menu=scon->pushScreen<Menu>().
      setAutoClose(false).
      add("Select mission",[this](){selectMission();}).
      add("Select equipment", [this](){selectEquipment();}).
      add("Start mission", [this](){startMission();}).
      add("Save and quit",[this](){save();close();menu->close();}).
      shared_from_this();

  if(saveExists())
  {
    load();
  }
  else
  {
    RandGen rnd;
    rnd.seed((uint32_t)time(nullptr));
    seed=rnd();
    scon->pushScreen<TextMessage>(
R"(
Welcome to P.A.P.A. regional office.

I do understand that you aren't going to spend your life
in this hole. But headquarters do not need newbies without
any experience. Let's have some mutual benefits, shell we?
You get your reputation points and experience for you promotion,
we get some missions cleared.
)");
  }
  refreshMissions();
}

void MissionControl::draw()
{
  con.curBg=Color::black;
  con.clear();
  con.fillRect({{0,0},{con.width(),1}},0x47a0a3);
  con.printAlignedAt({0, 0}, con.width(), Console::pfKeepBackground|Console::pfAlignCenter, "P.A.P.A. regional office mission control");
  con.fillRect({{0,5},{con.width(),1}}, Color::gray/2.0f);
  con.printAlignedAt({0, 5}, con.width(), Console::pfKeepBackground|Console::pfAlignCenter,
                     FORMAT("You have %{} reputation points, %{} to go", rp, 1000-rp));
}

bool MissionControl::saveExists()
{
  return kst::File::Exists(saveFileName.c_str());
}

void MissionControl::refreshMissions()
{
  rnd.seed(seed);
  int missionsCount=intRand(rnd,4,9);
  availableMissions.clear();
  for(int i=0;i<missionsCount;++i)
  {
    availableMissions.push_back({});
    MissionDetails& md=availableMissions.back();
    if(rp<100)
    {
      md.dl=DifficultyLevel::easy;
      md.rp=20;
    }
    else if(rp<500)
    {
      md.dl=DifficultyLevel::average;
      md.rp=30;
    }
    else if(rp<900)
    {
      md.dl=DifficultyLevel::hard;
      md.rp=50;
    }
    else
    {
      md.dl=DifficultyLevel::epic;
      md.rp=100;
    }
    md.seed=rnd();
    switch(intRand(rnd, 0,5))
    {
      case 0:
      case 1:
      case 2:md.gt=GeneratorType::downtown;break;
      case 3:md.gt=GeneratorType::suburbs;break;
      case 4:md.gt=GeneratorType::warehouse;break;
    }
    md.enemies.push_back({getRandomEnemyForDifficulty(rnd, md.dl), boolRand(rnd), ((int)md.dl)+intRand(rnd,1,3)});
    switch(intRand(rnd,0, 6))
    {
      case 0:
      case 1:
      case 2:
        md.mt=MissionTime::day;
        break;
      case 3:
      case 4:
        md.mt=MissionTime::evening;
        break;
      case 5:
        md.mt=MissionTime::night;
        break;
    }
  }
}

void MissionControl::selectMission()
{
  scon->pushScreen<MissionSelector>(availableMissions, selectedMission, [this](int missionIdx){selectedMission=missionIdx;});
}

void MissionControl::selectEquipment()
{
  auto& es=scon->pushScreen<EquipmentSelector>(equipped);
  es.setCompletionHandler([this](std::set<AgentEquipmentType> argEq){equipped=argEq;});
}

void MissionControl::startMission()
{
  if(selectedMission==-1 || selectedMission>(int)availableMissions.size())
  {
    scon->pushScreen<TextMessage>("Please selection mission first");
    return;
  }
  save(true);
  scon->pushScreen<CombatScreen>(availableMissions[selectedMission],equipped,[this](MissionResultInfo mri){onMissionEnd(mri);});
}

void MissionControl::onMissionEnd(MissionResultInfo mri)
{
  if(mri.result==mri.kia)
  {
    auto& msg=scon->pushScreen<TextMessage>("Game over!");
    msg.setBg(0xa01d09);
    msg.setFg(Color::white);
    try {
      kst::File::Unlink(saveFileName.c_str());
    } catch (std::exception&) {
    }
    menu->close();
    close();
    return;
  }
  if(mri.result==mri.escaped || mri.result==mri.evacuated)
  {
    int rpChange=availableMissions[selectedMission].rp;
    rpChange=mri.result==mri.escaped?rpChange/2:rpChange*2;
    rp-=rpChange;
    scon->pushScreen<TextMessage>(FORMAT("Mission aborted!\n%{} reputation points lost", rpChange));
    if(rp<0)
    {
      rp=0;
    }
  }
  else
  {
    rp+=availableMissions[selectedMission].rp;
    if(rp>=1000)
    {
      scon->pushScreen<TextMessage>("Congratilations!\nYou earned enough reputation points for a promotion to P.A.P.A. headquaters!\nYou won the game.");
      menu->close();
      close();
      try {
        kst::File::Unlink(saveFileName.c_str());
      } catch (std::exception&) {
      }
      return;
    }
    else
    {
      scon->pushScreen<TextMessage>(FORMAT("Mission successful!\n%{} reputation points earned", availableMissions[selectedMission].rp));
    }
  }
  seed=rnd();
  save();
  refreshMissions();
  selectedMission=-1;
}

void MissionControl::save(bool missionStart)
{
  try{
    kst::File f;
    f.RWCreate(saveFileName.c_str());
    f.WriteNetInt32(saveVersion);
    f.WriteNetInt32(rp);
    f.WriteNetInt32(seed);
    f.WriteNetInt32(selectedMission);
    f.WriteByte(missionStart?1:0);
    f.WriteNetInt16((uint16_t)equipped.size());
    for(auto eq:equipped)
    {
      f.WriteNetInt16((uint16_t)eq);
    }
  }catch(std::exception& e)
  {
    LOGERROR("save","Exception:%{}", e.what());
    auto& msg=scon->pushScreen<TextMessage>("Failed to create save file!");
    msg.setBg(0xa01d09);
    msg.setFg(Color::white);
  }
}

void MissionControl::load()
{
  if(!kst::File::Exists(saveFileName.c_str()))
  {
    return;
  }
  try {
    kst::File f;
    f.ROpen(saveFileName.c_str());
    uint32_t saveFileVersion=f.ReadNetInt32();
    if(saveFileVersion!=saveVersion)
    {
      auto& msg=scon->pushScreen<TextMessage>("Save version mismatch!");
      msg.setBg(0xa01d09);
      msg.setFg(Color::white);
      return;
    }
    rp=f.ReadNetInt32();
    seed=f.ReadNetInt32();
    selectedMission=f.ReadNetInt32();
    bool missionStart=f.ReadByte()!=0;
    if(missionStart)
    {
      refreshMissions();
      startMission();
    }
    uint16_t eqCnt=f.ReadNetInt16();
    auto allEq=getAllEquipmentTypes();
    for(uint16_t i=0;i<eqCnt;++i)
    {
      AgentEquipmentType eq=(AgentEquipmentType)f.ReadNetInt16();
      if(std::find(allEq.begin(), allEq.end(), eq)!=allEq.end())
      {
        equipped.insert(eq);
      }
    }
  } catch (std::exception& e) {
    LOGERROR("load","Exception:%{}", e.what());
    auto& msg=scon->pushScreen<TextMessage>("Failed to load save file!");
    msg.setBg(0xa01d09);
    msg.setFg(Color::white);
  }
}
