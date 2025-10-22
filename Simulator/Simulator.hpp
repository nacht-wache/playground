#pragma once

#include <cstddef>
#include <memory>

/*
 * Базовая идея - использовать дерево наблюдателей на std::weak_ptr-ах с
 * глубиной = 4. Убийство/разрушение - std::shared_ptr::reset c CustomDeleter
 * Переселение и выселение с помощью Settle. nullptr -> выселяемся
 * Переселение за O(1): макс 8 обходов.
 * Выселение за O(1): макс 4 обхода.
 * Расчёт среднего за O(1) за счёт кэширования на уровне объекта (Человек, Город
 * и т.д.)
 */

class Man;
class City;
class Country;
class Planet;

struct IHandle {
  virtual ~IHandle() = default;

  [[nodiscard]] virtual std::size_t CalculateHappiness() const = 0;
  [[nodiscard]] virtual std::size_t GetHappiness() const = 0;
  [[nodiscard]] virtual std::size_t GetPeopleCount() const = 0;

  [[nodiscard]] virtual std::string_view GetName() const = 0;
};

std::ostream& operator<<(std::ostream& os, const IHandle& h);

struct IObserver {
  enum class MoveType { in = 0, out };

  virtual ~IObserver() = default;

  virtual void OnChange(size_t diff) = 0;
  virtual void OnMove(IHandle& handle, MoveType type) = 0;
};

namespace detail {
template <typename Derived, typename TCreate, typename TSettlement>
class TBase : public IHandle {
  template <typename D, typename T, typename S>
  friend class TBase;

 public:
  template <typename... Args>
  static std::shared_ptr<Derived> CreateMe(Args&&... args) {
    return CreateMePrivate(std::default_delete<Derived>{},
                           std::forward<Args>(args)...);
  }

  [[nodiscard]] std::size_t CalculateHappiness() const override {
    if (m_peopleSize != 0) {
      return m_happiness / m_peopleSize;
    }
    return 0;
  }

  [[nodiscard]] std::size_t GetHappiness() const override {
    return m_happiness;
  }

  [[nodiscard]] std::size_t GetPeopleCount() const override {
    return m_peopleSize;
  }

  [[nodiscard]] std::string_view GetName() const override {
    return m_name;
  }

  template <typename... Args>
    requires std::convertible_to<TCreate&, IHandle&>
  std::shared_ptr<TCreate> Create(Args&&... args) {
    auto obj = TCreate::CreateMePrivate(
        [](TCreate* deleteObj) {
          deleteObj->Settle(nullptr);
          delete deleteObj;
        },
        std::forward<Args>(args)...);

    obj->Settle(static_cast<Derived*>(this));
    return obj;
  }

  void Settle(TSettlement* settlement)
    requires std::derived_from<TSettlement, IHandle>
  {
    if (m_observer) {
      m_observer->OnMove(*this, IObserver::MoveType::out);
    }
    m_observer.reset();
    if (settlement) {
      m_observer = settlement->CreateObserver();
      m_observer->OnMove(*this, IObserver::MoveType::in);
    }
  }

 protected:
  explicit TBase(std::string name)
      : m_name(std::move(name)) {
  }

 private:
  std::shared_ptr<IObserver> CreateObserver()
    requires std::derived_from<TCreate, IHandle>
  {
    struct Obs : IObserver {
      void OnChange(size_t diff) override {
        if (auto locked = obj.lock()) {
          locked->m_happiness += diff;
        }

        if (auto locked = m_observer.lock()) {
          locked->OnChange(diff);
        }
      }

      void OnMove(IHandle& handle, MoveType type) override {
        if (auto locked = obj.lock()) {
          auto const happiness = handle.GetHappiness();
          locked->m_happiness +=
              (type == MoveType::out ? -happiness : happiness);

          auto const peopleCount = handle.GetPeopleCount();
          locked->m_peopleSize +=
              (type == MoveType::out ? -peopleCount : +peopleCount);
        }

        if (auto locked = m_observer.lock()) {
          locked->OnMove(handle, type);
        }
      }

      explicit Obs(std::weak_ptr<TBase> baseHandle,
                   std::weak_ptr<IObserver> observer)
          : obj(std::move(baseHandle)),
            m_observer(std::move(observer)) {
      }

      std::weak_ptr<TBase> obj;
      std::weak_ptr<IObserver> m_observer;
    };

    return std::make_shared<Obs>(
        static_cast<Derived*>(this)->shared_from_this(), m_observer);
  }

  template <typename Deleter, typename... Args>
  static std::shared_ptr<Derived> CreateMePrivate(Deleter deleter,
                                                  Args&&... args) {
    return std::shared_ptr<Derived>(new Derived(std::forward<Args>(args)...),
                                    std::move(deleter));
  }

 protected:
  std::string m_name;

  size_t m_happiness = 0;
  size_t m_peopleSize = 0;

  std::shared_ptr<IObserver> m_observer;
};
}  // namespace detail

class Man : public detail::TBase<Man, std::false_type, City> {
  using BaseT = detail::TBase<Man, std::false_type, City>;
  using ThisT = Man;

 public:
  explicit Man(std::string name);
  Man(std::string name, uint8_t happiness);

  void SetHappiness(uint8_t happiness);
};

class City : public detail::TBase<City, Man, Country>,
             public std::enable_shared_from_this<City> {
  using BaseT = detail::TBase<City, Man, Country>;
  using ThisT = City;

  using BaseT::BaseT;
};

class Country : public detail::TBase<Country, City, Planet>,
                public std::enable_shared_from_this<Country> {
  using BaseT = detail::TBase<Country, City, Planet>;
  using ThisT = Country;

  using BaseT::BaseT;
};

class Planet : public detail::TBase<Planet, Country, std::false_type>,
               public std::enable_shared_from_this<Planet> {
  using BaseT = detail::TBase<Planet, Country, std::false_type>;
  using ThisT = Planet;

  using BaseT::BaseT;
};
