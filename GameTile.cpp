#include "GameTile.hpp"


const GameTile& getTile(GameTileType tile)
{

#define DECLTILE(name, sym, bg, fg, ls, lss, lst, mc, bl, desc) \
  case GameTileType::name: { \
    static const GameTile name{GameTileType::name, sym, bg, fg, ls, lss, lst, mc, bl, desc}; \
    return name; }

  switch(tile)
  {          //name           //sym      //bg     //fg    //ls  //lss    //lst  //mc   //bl  //desc
    DECLTILE(barrier,           " ", 0x31bc51,       {},  true,  400, 0x31bc51,    0,  true, "barrier");
    DECLTILE(aslphalt,          " ", 0x2f3238,       {}, false,    0,       {},  100, false, "asphalt");
    DECLTILE(aslphaltHLine,     "-", 0x2f3238,       {}, false,    0,       {},  100, false, "asphalt");
    DECLTILE(aslphaltVLine,     "|", 0x2f3238,       {}, false,    0,       {},  100, false, "asphalt");
    DECLTILE(concrete,          " ", 0x4e4f51,       {}, false,    0,       {},  100, false, "concrete");
    DECLTILE(tilepath,          "#", 0x4e4f51, 0x877f7d, false,    0,       {},  100, false, "tile path");
    DECLTILE(dirt,              ".", 0x602719,       {}, false,    0,       {},  120, false, "dirt");
    DECLTILE(bush,              "*", 0x602719, 0x2d7a29, false,    0,       {},  200,  true, "bush");
    DECLTILE(bench,             "h", 0x4e4f51, 0x7a3628, false,    0,       {},    0, false, "bench");
    DECLTILE(concreteWall,      "█", 0x877f7d, 0x877f7d, false,    0,       {},    0,  true, "concrete wall");
    DECLTILE(streetLight,       "*", 0x747c74, 0xdbdb46,  true,  500, 0xdbdb46,    0, false, "street light");
    DECLTILE(streetLightOff,    "*", 0x747c74, 0xdbdb46, false,  500, 0xdbdb46,    0, false, "street light");
    DECLTILE(brokenStreetLight, "*", 0x747c74, 0xdbdb46, false,  500, 0xdbdb46,    0, false, "broken street light");
    DECLTILE(glassWall,         " ", 0xaeb1b2,       {}, false,    0,       {},    0, false, "glass wall");
    DECLTILE(ceilingLight,      " ", 0xaeb1b2, 0xffffff,  true,  600, 0xffffff,    0, false, "ceiling light");
    DECLTILE(interiorFloor1,    " ", 0x8c7952,       {}, false,    0,       {},  100, false, "floor");
    DECLTILE(interiorFloor2,    " ", 0x705c33,       {}, false,    0,       {},  100, false, "floor");
    DECLTILE(carTL,             "/",       {},       {}, false,    0,       {},    0,  true, "car");
    DECLTILE(carH,              "-",       {},       {}, false,    0,       {},    0,  true, "car");
    DECLTILE(carTR,            "\\",       {},       {}, false,    0,       {},    0,  true, "car");
    DECLTILE(carV,              "|",       {},       {}, false,    0,       {},    0,  true, "car");
    DECLTILE(carX,              "+",       {},       {}, false,    0,       {},    0,  true, "car");
    DECLTILE(carSP,             " ",       {},       {}, false,    0,       {},    0,  true, "car");
    DECLTILE(carB,              "_",       {},       {}, false,    0,       {},    0,  true, "car");
  }
  static const GameTile dummy{GameTileType::barrier, " ", {}, {}, false, 0, {}, 0, false, ""};
  return dummy;
}
