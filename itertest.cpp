#include <stdio.h>
#include "Utility.hpp"

int main()
{
  using namespace glider;
  Recti<int> r(Posi<int>(0,0), Posi<int>(5,5));
  auto end = r.end();
  printf("%d,%d\n", end.pos.x, end.pos.y);
  for(auto p:r) {
    printf("%d, %d\n", p.x, p.y);
  }
}