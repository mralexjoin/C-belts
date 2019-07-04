#pragma once

#include "json.h"

#include <memory>
#include <set>
#include <string_view>
#include <vector>

namespace BusesRouting {
  template <typename It>
  class PairRange {
  public:
    using ValueType = typename std::iterator_traits<It>::value_type;

    PairRange(It begin, It end) {
      for (auto lhs = begin, rhs = next(lhs);
           rhs != end;
           lhs = rhs, rhs = next(rhs)) {
        pairs.push_back({ *lhs, *rhs });
      }
    }
    auto begin() const { return pairs.begin(); }
    auto end() const { return pairs.end(); }

  private:
    std::vector<std::pair<const ValueType&, const ValueType&>> pairs;
  };

  struct Stop;
  using StopHolder = std::shared_ptr<Stop>;

  class Bus;
  using BusHolder = std::shared_ptr<Bus>;

  class BusStats;
  using BusStatsHolder = std::shared_ptr<const BusStats>;

  using StopBuses = std::set<std::string_view>;
  using StopBusesHolder = std::shared_ptr<StopBuses>;

  using Time = double;
  using StringHolder = std::shared_ptr<const std::string>;

  class JsonSerializable {
  public:
    virtual Json::Node ToJson() const = 0;
    virtual ~JsonSerializable() = default;
  protected:
    virtual void FillJson(Json::Node& node) const = 0;
  };
}
