#include "Simulator.hpp"

std::ostream& operator<<(std::ostream& os, const IHandle& h) {
  return os << h.GetName() << ": Happiness: " << h.CalculateHappiness();
}

Man::Man(std::string name)
    : BaseT(std::move(name)) {
  m_peopleSize = 1;
}

Man::Man(std::string name, uint8_t happiness)
    : BaseT(std::move(name)) {
  if (happiness > 9) {
    happiness = 9;
  }

  m_happiness = happiness;
  m_peopleSize = 1;
}

void Man::SetHappiness(uint8_t happiness) {
  if (happiness > 9) {
    happiness = 9;
  }

  if (m_observer) {
    m_observer->OnChange(happiness - m_happiness);
  }
  m_happiness = happiness;
}
