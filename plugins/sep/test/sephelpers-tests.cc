#include <gtest/gtest.h>

#include <cmath>

#include "../sep-helpers.hh"

TEST(SepHelpers_Tests, sephelpers_sum_empty_pipette_colours) { // NOLINT
  auto res = sumPipetteColors({}, {{"C", 1.0}});

  EXPECT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].first, "C");
  EXPECT_NEAR(res[0].second, 1.0, 1e-4);
}

TEST(SepHelpers_Tests, sephelpers_sum_pipette_colours) { // NOLINT
  auto res = sumPipetteColors({{"C", 1.0}}, {{"C", 1.0}});

  EXPECT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].first, "C");
  EXPECT_NEAR(res[0].second, 2.0, 1e-4);
}

TEST(SepHelpers_Tests, sephelpers_divide_pipette_colours) { // NOLINT
  auto res = dividePipetteColors({{"C", 4.0}}, 4);

  EXPECT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].first, "C");
  EXPECT_NEAR(res[0].second, 1.0, 1e-4);
}
