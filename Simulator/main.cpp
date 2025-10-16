#include <iostream>
#include "Simulator.hpp"

int main() {
  auto earth = Planet::CreateMe("Earth");
  auto belarus = earth->Create("Belarus");
  belarus->Print(std::cout);
  auto minsk = belarus->Create("Minsk");
  auto ivan = minsk->Create("Ivan");
  ivan->SetHappiness(9);
  minsk->Print(std::cout);
  belarus->Print(std::cout);

  auto oleg = minsk->Create("Oleg", 6);
  oleg->Print(std::cout);
  auto brest = belarus->Create("Brest");
  ivan->Settle(brest.get());
  brest->Print(std::cout);
  belarus->Print(std::cout);
  brest.reset();
  oleg->Settle(minsk.get());
  // city1->Print(std::cout);
  minsk->Print(std::cout);
  belarus->Print(std::cout);

  return 0;
}