#include <iostream>
#include "Simulator.hpp"

int main() {
  auto earth = Planet::CreateMe("Earth");
  auto belarus = earth->Create("Belarus");
  std::cout << *belarus << std::endl;
  auto minsk = belarus->Create("Minsk");
  auto ivan = minsk->Create("Ivan");
  ivan->SetHappiness(9);
  std::cout << *ivan << std::endl;
  std::cout << *belarus << std::endl;

  auto oleg = minsk->Create("Oleg", 6);
  std::cout << *oleg << std::endl;
  auto brest = belarus->Create("Brest");
  ivan->Settle(brest.get());
  std::cout << *brest << std::endl;
  std::cout << *belarus << std::endl;
  brest.reset();
  oleg->Settle(minsk.get());
  if (brest)
    std::cout << *brest << std::endl;
  std::cout << *minsk << std::endl;
  std::cout << *belarus << std::endl;

  return 0;
}