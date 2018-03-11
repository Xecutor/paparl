#include "GeneratorType.hpp"

std::string getGeneratorNameByType(GeneratorType gt)
{
  using GT=GeneratorType;
  switch(gt)
  {
    case GT::downtown:return "Downtown";
    case GT::seaport:return "Seaport";
    case GT::suburbs:return "Suburbs";
    case GT::warehouse:return "Warehouse";
  }
  return "";
}
