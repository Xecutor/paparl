#pragma once

#include <string>

enum class GeneratorType{
  downtown,
  suburbs,
  warehouse,
  seaport,
};


std::string getGeneratorNameByType(GeneratorType gt);
