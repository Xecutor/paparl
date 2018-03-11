#include "EquipmentSelector.hpp"
#include "Menu.hpp"


void EquipmentSelector::init()
{
  updateMenu();
}

void EquipmentSelector::onTopScreen()
{
  if(updated)
  {
    updated=false;
    updateMenu();
  }
  else
  {
    handler(equipped);
    close();
  }
}

void EquipmentSelector::draw()
{
  con.curBg=Color::black;
  con.clear();
}

void EquipmentSelector::updateMenu()
{
  auto& menu=scon->pushScreen<Menu>();
  int idx=0;
  for(auto aet:getAllEquipmentTypes())
  {
    menu.add(getEquipmentNameByType(aet)+(equipped.find(aet)==equipped.end()?"":" - equipped"), [this,idx,aet](){
      auto it=equipped.find(aet);
      if(it==equipped.end())
      {
        equipped.insert(aet);
      }
      else
      {
        equipped.erase(aet);
      }
      selectionIdx=idx;
      updated=true;
    });
    ++idx;
  }
  menu.setSelectedIndex(selectionIdx);
}
