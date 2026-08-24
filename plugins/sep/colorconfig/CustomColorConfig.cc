//
// Created by developer on 01-06-21.
//
#include "CustomColorConfig.hh"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <list>
#include <mutex>
#include <unordered_set>

namespace pt = boost::property_tree;

ColorConfig::ColorConfig(Scroom::Logger logger_, std::string file)
    : logger(logger_) {
  colors.clear();
  pt::ptree root;
  boost::filesystem::path full_path(boost::filesystem::current_path());
  if (file == "colours.json") {
    full_path.append(file);
  } else {
    full_path = file;
  }
  if (!boost::filesystem::exists(
          full_path)) { // File does not exist on file system
    logger->warn("Colours file does not exist at path: {}", full_path.string());
    logger->info("Loading default CMYK");
    addNonExistentDefaultColors();
    return;
  }

  try {
    pt::read_json(full_path.string(), root);
  } catch (const std::exception &e) {
    // Loading didnt work
    logger->warn(
        "Loading colours file failed. file at: {} is most likely ill formed.",
        full_path.string());
    logger->info("Loading default CMYK");
    addNonExistentDefaultColors();
    return;
  }

  std::unordered_set<std::string> seenNamesAndAliases = {};
  seenNamesAndAliases.insert("V"); // Insert placeholder for varnish

  logger->info("Loading colour config file. NOTE: v is reserved for varnish, "
               "so should not be defined as name or alias!!");
  for (pt::ptree::value_type &v : root.get_child("colours")) {
    parseColor(v, seenNamesAndAliases);
  }
  addNonExistentDefaultColors();
}

ColorConfig::Ptr ColorConfig::instance(Scroom::Logger logger) {
  static std::mutex mut;
  static std::weak_ptr<ColorConfig> inst;
  std::lock_guard const lock(mut);
  Ptr result = inst.lock();
  if (!result) {
    result = Ptr(new ColorConfig(logger));
    inst = result;
  }
  return result;
}

void ColorConfig::addNonExistentDefaultColors() {

  // If no cyan configuration exists, add the default configuration
  if (!getColorByNameOrAlias("c")) {
    CustomColor::Ptr newColour = std::make_shared<CustomColor>("C", 1, 0, 0, 0);

    colors.push_back(newColour);
  }

  // If no magenta configuration exists, add the default configuration
  if (!getColorByNameOrAlias("m")) {
    CustomColor::Ptr newColour = std::make_shared<CustomColor>("M", 0, 1, 0, 0);

    colors.push_back(newColour);
  }

  // If no yellow configuration exists, add the default configuration
  if (!getColorByNameOrAlias("y")) {
    CustomColor::Ptr newColour = std::make_shared<CustomColor>("Y", 0, 0, 1, 0);

    colors.push_back(newColour);
  }

  // If no key configuration exists, add the default configuration
  if (!getColorByNameOrAlias("k")) {
    CustomColor::Ptr newColour = std::make_shared<CustomColor>("K", 0, 0, 0, 1);

    colors.push_back(newColour);
  }
}

CustomColor::Ptr ColorConfig::getColorByNameOrAlias(std::string name) {
  boost::algorithm::to_upper(name);
  auto definedColors = getDefinedColors();
  std::string lowerName;
  std::string lowerAlias;
  for (auto &color : definedColors) {
    lowerName = boost::algorithm::to_upper_copy(color->name);
    if (lowerName == name) {
      return color;
    }

    for (auto const &alias : color->aliases) {
      lowerAlias = boost::algorithm::to_upper_copy(alias);
      if (lowerAlias == name) {
        return color;
      }
    }
  }
  return nullptr;
}

std::vector<CustomColor::Ptr> ColorConfig::getDefinedColors() { return colors; }

void ColorConfig::parseColor(
    pt::ptree::value_type &v,
    std::unordered_set<std::string> &seenNamesAndAliases) {
  auto name = v.second.get<std::string>("name");
  boost::algorithm::to_upper(name); // Convert the name to uppercase

  // Check if this name has not yet been seen before
  if (seenNamesAndAliases.find(name) != seenNamesAndAliases.end()) {
    // It exists
    logger->error("Duplicate name or alias: {}!", name);
    // Color already exists, so it should not be added to the colors
    return;
  }
  // Color is new, so we can add it to the defined colors
  // First add the name to the seen names and aliases
  seenNamesAndAliases.insert(name);

  auto c = v.second.get<float>("cMultiplier");
  auto m = v.second.get<float>("mMultiplier");
  auto y = v.second.get<float>("yMultiplier");
  auto k = v.second.get<float>("kMultiplier");
  CustomColor::Ptr newColour = std::make_shared<CustomColor>(name, c, m, y, k);

  // Initialise aliases vector
  std::vector<std::string> validAliases = {};
  // Try to load aliases, if the field exists
  try {
    // Get the aliases array
    pt::ptree array = v.second.get_child("aliasses");
    // Initialise an iterator over the aliasses array
    pt::ptree::iterator iterator = array.begin();

    // Store aliases in vector
    for (; iterator != array.end(); iterator++) {
      // Load alias with uppercase included
      auto alias = iterator->second.get_value<std::string>();
      // Convert alias to all upercase
      boost::algorithm::to_upper(alias);

      // Test if an alias already exists in a different colour
      if (seenNamesAndAliases.find(alias) != seenNamesAndAliases.end()) {
        // It exists
        logger->error("Duplicate alias: {}!", alias);
      } else {
        // It is a new alias
        logger->debug("New alias: {}", alias);
        seenNamesAndAliases.insert(alias);
      }

      validAliases.push_back(alias);
    }

    // Set aliassses for newColour
    newColour->aliases = validAliases;
  } catch (const std::exception &e) {
    // When no aliasses exist, ignore exception
    logger->debug("No aliasses found.");
  }

  colors.push_back(newColour);
}