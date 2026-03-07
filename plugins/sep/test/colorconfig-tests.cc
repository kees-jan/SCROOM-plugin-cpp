#include <gtest/gtest.h>

#include "../colorconfig/CustomColorConfig.hh"
#include "testglobals.hh"

TEST(ColorConfig_Tests, colorConfig_create) { // NOLINT
  ColorConfig();
  ColorConfig colorConfig;
  EXPECT_EQ(colorConfig.getDefinedColors().size(), 0u);
}

TEST(ColorConfig_Tests, colorConfig_load_without_file) { // NOLINT
  ColorConfig colorConfig;
  colorConfig.loadFile();
  EXPECT_EQ(colorConfig.getDefinedColors().size(), 4u);
}

TEST(ColorConfig_Tests, colorConfig_load_with_file) { // NOLINT
  ColorConfig colorConfig;
  colorConfig.loadFile(TestFiles::getPathToFile("colours.json"));
  EXPECT_EQ(colorConfig.getDefinedColors().size(), 5u);
}

TEST(ColorConfig_Tests, colorConfig_non_existent) { // NOLINT
  ColorConfig colorConfig;
  colorConfig.addNonExistentDefaultColors();
  EXPECT_EQ(colorConfig.getDefinedColors().size(), 4u);
}

TEST(ColorConfig_Tests, colorConfig_get_default_name) { // NOLINT
  ColorConfig colorConfig;
  colorConfig.addNonExistentDefaultColors();
  EXPECT_NE(colorConfig.getColorByNameOrAlias("c"), nullptr);
}

TEST(ColorConfig_Tests, colorConfig_get_name) { // NOLINT
  ColorConfig colorConfig;
  colorConfig.loadFile(TestFiles::getPathToFile("colours.json"));
  EXPECT_NE(colorConfig.getColorByNameOrAlias("b"), nullptr);
}