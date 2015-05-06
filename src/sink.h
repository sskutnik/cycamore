#ifndef CYCAMORE_SRC_SINK_H_
#define CYCAMORE_SRC_SINK_H_

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "cyclus.h"

namespace cycamore {

class Context;

/// This facility acts as a sink of materials and products with a fixed
/// throughput (per time step) capacity and a lifetime capacity defined by a
/// total inventory size.  The inventory size and throughput capacity both
/// default to infinite. If a recipe is provided, it will request material with
/// that recipe. Requests are made for any number of specified commodities.
class Sink : public cyclus::Facility  {
 public:

  Sink(cyclus::Context* ctx);

  virtual ~Sink();

  #pragma cyclus note {"doc": "A sink facility that accepts specified " \
                              "amounts of commodities from other agents"}

  #pragma cyclus decl

  virtual std::string str();

  virtual void Tick();

  virtual void Tock();

  /// @brief SinkFacilities request Materials of their given commodity. Note
  /// that it is assumed the Sink operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief SinkFacilities request Products of their given
  /// commodity. Note that it is assumed the Sink operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetGenRsrcRequests();

  /// @brief SinkFacilities place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief SinkFacilities place accepted trade Materials in their Inventory
  virtual void AcceptGenRsrcTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  ///  add a commodity to the set of input commodities
  ///  @param name the commodity name
  inline void AddCommodity(std::string name) { in_commods.push_back(name); }

  ///  sets the size of the storage inventory for received material
  ///  @param size the storage size
  inline void SetMaxInventorySize(double size) {
    max_inv_size = size;
    inventory.set_capacity(size);
  }

  /// @return the maximum inventory storage size
  inline double MaxInventorySize() const { return inventory.capacity(); }

  /// @return the current inventory storage size
  inline double InventorySize() const { return inventory.quantity(); }

  /// determines the amount to request
  inline double RequestAmt() const {
    return std::min(capacity, std::max(0.0, inventory.space()));
  }

  /// sets the capacity of a material generated at any given time step
  /// @param capacity the reception capacity
  inline void Capacity(double cap) { capacity = cap; }

  /// @return the reception capacity at any given time step
  inline double Capacity() const { return capacity; }

  /// @return the input commodities
  inline const std::vector<std::string>&
      input_commodities() const { return in_commods; }

 private:
  /// all facilities must have at least one input commodity
  #pragma cyclus var {"tooltip": "input commodities", \
                      "doc": "commodities that the sink facility accepts", \
                      "uitype": ["oneormore", "incommodity"]}
  std::vector<std::string> in_commods;

  /// monthly acceptance capacity
  #pragma cyclus var {"default": 1e299, "tooltip": "sink capacity", \
                      "doc": "capacity the sink facility can " \
                             "accept at each time step"}
  double capacity;

  #pragma cyclus var {"default": "", "tooltip": "requested composition", \
                      "doc": "name of recipe to use for material requests, where " \
                             "the default (empty string) is to accept everything", \
                      "uitype": "recipe"}
  std::string recipe_name;

  /// max inventory size
  #pragma cyclus var {"default": 1e299, \
                      "tooltip": "sink maximum inventory size", \
                      "doc": "total maximum inventory size of sink facility"}
  double max_inv_size;

  /// this facility holds material in storage.
  #pragma cyclus var {'capacity': 'max_inv_size'}
  cyclus::toolkit::ResourceBuff inventory;
};

}  // namespace cycamore

#endif  // CYCAMORE_SRC_SINK_H_
