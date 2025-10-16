#include <gtest/gtest.h>
#include "../Simulator.hpp"

TEST(ManTest, CreationAndBasicCalculation) {
  auto man = Man::CreateMe("John Doe", 75);

  ASSERT_NE(man, nullptr);
  EXPECT_EQ(man->GetPeopleCount(), 1);
  EXPECT_EQ(man->GetHappiness(), 9);
  EXPECT_EQ(man->CalculateHappiness(), 9);
}

TEST(CityTest, AggregationAndAverageHappiness) {
  auto city = City::CreateMe("Atlantis");

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
  auto city = City::CreateMe("GhostTown");

  EXPECT_EQ(city->GetPeopleCount(), 0);
  EXPECT_EQ(city->GetHappiness(), 0);
  EXPECT_EQ(city->CalculateHappiness(), 0);
}

TEST(ObserverTest, HappinessChangePropagation) {
  auto country = Country::CreateMe("Nation");
  auto city = country->Create("Capital");
  auto man = city->Create("Dave", 5);

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
  auto country = Country::CreateMe("Nation");
  auto city = country->Create("Capital");
  auto man = city->Create("Dave", 9);  // Начинаем с максимального счастья

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
  auto country = Country::CreateMe("Nation");
  auto city = country->Create("Capital");
  auto man = city->Create("Dave", 5);

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
  auto man_high = Man::CreateMe("High", 10);
  EXPECT_EQ(man_high->GetHappiness(), 9);

  auto man_setter = Man::CreateMe("Setter", 7);
  EXPECT_EQ(man_setter->GetHappiness(), 7);

  man_setter->SetHappiness(15);
  EXPECT_EQ(man_setter->GetHappiness(), 9);

  man_setter->SetHappiness(5);
  EXPECT_EQ(man_setter->GetHappiness(), 5);
}

TEST(SettleTest, MoveBetweenSettlements) {
  auto old_city = City::CreateMe("OldCity");
  auto new_city = City::CreateMe("NewCity");
  auto man = Man::CreateMe("Eve", 100);

  man->Settle(old_city.get());
  EXPECT_EQ(old_city->GetPeopleCount(), 1);
  EXPECT_EQ(old_city->GetHappiness(), 9);  // Ожидаем 9

  man->Settle(new_city.get());

  EXPECT_EQ(old_city->GetPeopleCount(), 0);
  EXPECT_EQ(old_city->GetHappiness(), 0);

  EXPECT_EQ(new_city->GetPeopleCount(), 1);
  EXPECT_EQ(new_city->GetHappiness(), 9);  // Ожидаем 9
}

TEST(CreateTest, CustomDeleterCallsSettle) {
  auto city = City::CreateMe("TestCity");

  std::weak_ptr<Man> weak_man;

  {
    auto man = city->Create("Temporary", 50);
    weak_man = man;

    EXPECT_EQ(city->GetPeopleCount(), 1);
    EXPECT_EQ(city->GetHappiness(), 9);  // Ожидаем 9
  }

  EXPECT_TRUE(weak_man.expired());
  EXPECT_EQ(city->GetPeopleCount(), 0);
  EXPECT_EQ(city->GetHappiness(), 0);
}
