#include "MissionSelector.hpp"
#include "Menu.hpp"

void MissionSelector::init()
{
  auto& menu=scon->pushScreen<Menu>(IPos(0,con.height()/2));
  menu.setOrigin(roLeftCenter);

  for(int idx=0;idx<(int)missions.size();++idx)
  {
    menu.add(FORMAT("Mission %{}", idx + 1),[idx,this](){onSelectMission(idx);close();},[this,idx](){selectedMission=idx;});
  }
  if(selectedMission<0)
  {
    selectedMission=0;
  }
  menu.setSelectedIndex(selectedMission);
  details.setRect({{con.width()/2,0},{con.width()/2, con.height()}});
}

void MissionSelector::draw()
{
  con.curBg=Color::black;
  con.clear();
  MissionDetails& md=missions[selectedMission];
  con.clearRect(details.getRect());
  con.fillRect(details.getRect(), 0x47a0a3);
  con.curFg=Color::black;
  details.reset();
  details.addLine(FORMAT("Mission %{}", selectedMission+1));
  details.addLine(FORMAT("Mission difficulty %{}", getDifficultyName(md.dl)));
  details.addLine(FORMAT("Location:%{}",getGeneratorNameByType(md.gt)));
  details.addLine(FORMAT("Reputation points:%{}",md.rp));
  details.addLine("Expected enemies:");
  for(auto& e:md.enemies)
  {
    details.addLine(FORMAT("%{} x%{}", getEnemyNameForType(e.et), e.count));
  }
}
