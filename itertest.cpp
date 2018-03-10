#include <stdio.h>
#include "Utility.hpp"

int main()
{
  using namespace glider;
  Recti<int> r(Posi<int>(0,0), Posi<int>(5,5));
  auto end = r.end();
  printf("%d,%d\n", end.pos.x, end.pos.y);
  for(auto p:r)
  {
    printf("%d, %d\n", p.x, p.y);
  }
  printf("===\n");
  for(auto p:Recti<int>(0,0,0,5))
  {
    printf("%d, %d\n", p.x, p.y);
  }
  printf("===\n");
  for(auto p:Linei<int>({5,5},{5,10}))
  {
    printf("%d, %d\n", p.x, p.y);
  }
  printf("===\n");
  for(auto p:Linei<int>({5,5},{10,10}))
  {
    printf("%d, %d\n", p.x, p.y);
  }
  printf("===\n");
  for(auto p:Linei<int>({15,15},{10,5}))
  {
    printf("%d, %d\n", p.x, p.y);
  }
  printf("===\n");
  Posi<int> d(1,0);
  d.rotCW();
  printf("%d, %d\n", d.x, d.y);
  d.rotCCW();
  d.rotCCW();
  printf("%d, %d\n", d.x, d.y);
}
