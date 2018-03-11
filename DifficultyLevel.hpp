#pragma once

#include <string>

enum class DifficultyLevel{
  easy,
  average,
  hard,
  epic
};


inline std::string getDifficultyName(DifficultyLevel dl)
{
  using DL=DifficultyLevel;
  switch(dl)
  {
    case DL::easy:return "Easy";
    case DL::average:return "Average";
    case DL::hard:return "Hard";
    case DL::epic:return "Epic";
  }
  return {};
}
