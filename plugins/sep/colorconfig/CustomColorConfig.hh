//
// Created by developer on 01-06-21.
//

#pragma once
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

#include <memory>

#include <scroom/logger.hh>

#include "CustomColor.hh"
#include <scroom/plugininformationinterface.hh>
#include <scroom/utilities.hh>
#include <unordered_set>

namespace pt = boost::property_tree;
class ColorConfig {
public:
  using Ptr = std::shared_ptr<ColorConfig>;

public: // For testing
  explicit ColorConfig(Scroom::Logger logger,
                       std::string file = "colours.json");

private:
  Scroom::Logger logger;
  std::vector<CustomColor::Ptr> colors;

public:
  static Ptr instance(Scroom::Logger logger);

  std::vector<CustomColor::Ptr> getDefinedColors();
  CustomColor::Ptr getColorByNameOrAlias(std::string name);

  void addNonExistentDefaultColors();

private:
  void parseColor(pt::ptree::value_type &v,
                  std::unordered_set<std::string> &seenNamesAndAliases);
};
