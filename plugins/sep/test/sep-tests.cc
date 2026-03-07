#include <gtk/gtk.h>

#include <gtest/gtest.h>

#include "../sep.hh"

TEST(Sep_Tests, sep_create) { // NOLINT
  Sep::Ptr sep = Sep::create();
  EXPECT_NE(sep, nullptr);
}

TEST(Sep_Tests, sep_getPluginName) { // NOLINT
  Sep::Ptr sep = Sep::create();
  EXPECT_EQ(sep->getPluginName(), "SEP and SLI");
}

TEST(Sep_Tests, sep_getVersion) { // NOLINT
  Sep::Ptr sep = Sep::create();
  EXPECT_FALSE(sep->getPluginVersion().empty());
}

TEST(Sep_Tests, sep_filters_name) { // NOLINT
  Sep::Ptr sep = Sep::create();
  auto filters = sep->getFilters();
  EXPECT_NE(filters.front(), nullptr);

  // should be the same since we only put one element
  EXPECT_EQ(filters.front(), filters.back());

  // check that the correct name is given
  EXPECT_STREQ(gtk_file_filter_get_name(filters.front()), "SEP/SLI files");
}