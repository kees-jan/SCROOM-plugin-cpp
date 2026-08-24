#include <gtest/gtest.h>

#include "../colorconfig/CustomColorConfig.hh"
#include "testglobals.hh"

TEST(ColorConfig_Tests, colorConfig_load_without_file) { // NOLINT
  ColorConfig colorConfig(makeLogger());
  EXPECT_EQ(colorConfig.getDefinedColors().size(), 4u);
  EXPECT_NE(colorConfig.getColorByNameOrAlias("c"), nullptr);
  EXPECT_NE(colorConfig.getColorByNameOrAlias("m"), nullptr);
  EXPECT_NE(colorConfig.getColorByNameOrAlias("y"), nullptr);
  EXPECT_NE(colorConfig.getColorByNameOrAlias("k"), nullptr);
}

TEST(ColorConfig_Tests, colorConfig_load_with_file) { // NOLINT
  ColorConfig colorConfig(makeLogger(),
                          TestFiles::getPathToFile("colours.json"));
  EXPECT_EQ(colorConfig.getDefinedColors().size(), 5u);
}

TEST(ColorConfig_Tests, colorConfig_get_name) { // NOLINT
  ColorConfig colorConfig(makeLogger(),
                          TestFiles::getPathToFile("colours.json"));
  EXPECT_NE(colorConfig.getColorByNameOrAlias("b"), nullptr);
}