#include <gtest/gtest.h>
#include <scroom/bitmap-helpers.hh>

#include "../colorconfig/CustomColorOperations.hh"

TEST(ColorOperations_Tests, colorOperations_create) { // NOLINT
  PipetteCommonOperationsCustomColor::Ptr colorOperations =
      OperationsCustomColors::create(4);
  EXPECT_NE(colorOperations, nullptr);
}

TEST(ColorOperations_Tests, colorOperations_setspp) { // NOLINT
  OperationsCustomColors operations(8);
  EXPECT_EQ(operations.getBpp(), 64);
}