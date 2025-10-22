#include <gtest/gtest.h>
#include "../Simulator.hpp"

TEST(ManTest, CreationAndBasicCalculation) {
  const auto man = Man::CreateMe("John Doe", 75);

  ASSERT_NE(man, nullptr);
  EXPECT_EQ(man->GetPeopleCount(), 1);
  EXPECT_EQ(man->GetHappiness(), 9);
  EXPECT_EQ(man->CalculateHappiness(), 9);
}

TEST(CityTest, AggregationAndAverageHappiness) {
  const auto city = City::CreateMe("Atlantis");

  auto man1 = city->Create("Alice", 80);
  auto man2 = city->Create("Bob", 60);

  EXPECT_EQ(city->GetPeopleCount(), 2);
  EXPECT_EQ(city->GetHappiness(), 18);
  EXPECT_EQ(city->CalculateHappiness(), 9);

  man1.reset();

  EXPECT_EQ(city->GetPeopleCount(), 1);
  EXPECT_EQ(city->GetHappiness(), 9);
  EXPECT_EQ(city->CalculateHappiness(), 9);
}

TEST(CityTest, EmptyCityAverage) {
  const auto city = City::CreateMe("GhostTown");

  EXPECT_EQ(city->GetPeopleCount(), 0);
  EXPECT_EQ(city->GetHappiness(), 0);
  EXPECT_EQ(city->CalculateHappiness(), 0);
}

TEST(ObserverTest, HappinessChangePropagation) {
  const auto country = Country::CreateMe("Nation");
  const auto city = country->Create("Capital");
  const auto man = city->Create("Dave", 5);

  EXPECT_EQ(man->GetHappiness(), 5);
  EXPECT_EQ(city->GetHappiness(), 5);
  EXPECT_EQ(country->GetHappiness(), 5);

  man->SetHappiness(8);

  EXPECT_EQ(man->GetHappiness(), 8);
  EXPECT_EQ(city->GetHappiness(), 8);
  EXPECT_EQ(country->GetHappiness(), 8);

  man->SetHappiness(50);

  EXPECT_EQ(man->GetHappiness(), 9);
  EXPECT_EQ(city->GetHappiness(), 9);
  EXPECT_EQ(country->GetHappiness(), 9);
}

TEST(ObserverTest, HappinessDecreasePropagation) {
  const auto country = Country::CreateMe("Nation");
  const auto city = country->Create("Capital");
  const auto man = city->Create("Dave", 9);  // Начинаем с максимального счастья

  EXPECT_EQ(man->GetHappiness(), 9);
  EXPECT_EQ(city->GetHappiness(), 9);
  EXPECT_EQ(country->GetHappiness(), 9);

  man->SetHappiness(3);

  EXPECT_EQ(man->GetHappiness(), 3);
  EXPECT_EQ(city->GetHappiness(), 3);
  EXPECT_EQ(country->GetHappiness(), 3);

  man->SetHappiness(0);
  EXPECT_EQ(man->GetHappiness(), 0);
  EXPECT_EQ(city->GetHappiness(), 0);
  EXPECT_EQ(country->GetHappiness(), 0);
}

TEST(ObserverTest, SetHappinessNoChange) {
  const auto country = Country::CreateMe("Nation");
  const auto city = country->Create("Capital");
  const auto man = city->Create("Dave", 5);

  EXPECT_EQ(man->GetHappiness(), 5);
  EXPECT_EQ(city->GetHappiness(), 5);

  man->SetHappiness(5);
  EXPECT_EQ(man->GetHappiness(), 5);
  EXPECT_EQ(city->GetHappiness(), 5);

  man->SetHappiness(9);
  EXPECT_EQ(man->GetHappiness(), 9);

  man->SetHappiness(150);
  EXPECT_EQ(man->GetHappiness(), 9);
  EXPECT_EQ(city->GetHappiness(), 9);
}

TEST(ManTest, HappinessBoundsCheck) {
  const auto manHigh = Man::CreateMe("High", 10);
  EXPECT_EQ(manHigh->GetHappiness(), 9);

  const auto manSetter = Man::CreateMe("Setter", 7);
  EXPECT_EQ(manSetter->GetHappiness(), 7);

  manSetter->SetHappiness(15);
  EXPECT_EQ(manSetter->GetHappiness(), 9);

  manSetter->SetHappiness(5);
  EXPECT_EQ(manSetter->GetHappiness(), 5);
}

TEST(SettleTest, MoveBetweenSettlements) {
  const auto oldCity = City::CreateMe("OldCity");
  const auto newCity = City::CreateMe("NewCity");
  auto man = Man::CreateMe("Eve", 100);

  man->Settle(oldCity.get());
  EXPECT_EQ(oldCity->GetPeopleCount(), 1);
  EXPECT_EQ(oldCity->GetHappiness(), 9);  // Ожидаем 9

  man->Settle(newCity.get());

  EXPECT_EQ(oldCity->GetPeopleCount(), 0);
  EXPECT_EQ(oldCity->GetHappiness(), 0);

  EXPECT_EQ(newCity->GetPeopleCount(), 1);
  EXPECT_EQ(newCity->GetHappiness(), 9);  // Ожидаем 9
}

TEST(CreateTest, CustomDeleterCallsSettle) {
  const auto city = City::CreateMe("TestCity");

  std::weak_ptr<Man> weakMan;

  {
    const auto man = city->Create("Temporary", 50);
    weakMan = man;

    EXPECT_EQ(city->GetPeopleCount(), 1);
    EXPECT_EQ(city->GetHappiness(), 9);  // Ожидаем 9
  }

  EXPECT_TRUE(weakMan.expired());
  EXPECT_EQ(city->GetPeopleCount(), 0);
  EXPECT_EQ(city->GetHappiness(), 0);
}
