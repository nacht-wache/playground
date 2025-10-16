#include "Simulator.hpp"

std::ostream& operator<<(std::ostream& os, const IHandle& h) {
  return h.Print(os);
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

std::ostream& Man::Print(std::ostream& os) const {
  return os << "Man name: " << m_name << "; Happiness: " << CalculateHappiness()
            << std::endl;
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

std::ostream& City::Print(std::ostream& os) const {
  return os << "City name: " << m_name
            << "; Happiness: " << CalculateHappiness() << std::endl;
}

std::ostream& Country::Print(std::ostream& os) const {
  return os << "Country name: " << m_name
            << "; Happiness: " << CalculateHappiness() << std::endl;
}

std::ostream& Planet::Print(std::ostream& os) const {
  return os << "Planet name: " << m_name
            << "; Happiness: " << CalculateHappiness() << std::endl;
}
