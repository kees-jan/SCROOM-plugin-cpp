#include <gtest/gtest.h>

#include <cmath>

#include "../seppresentation.hh"
#include "testglobals.hh"

TEST(SepPresentation_Tests, seppresentation_create) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_NE(presentation, nullptr);
}

TEST(SepPresentation_Tests, seppresentation_load_false) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_FALSE(presentation->load(TestFiles::getPathToFile("sep_test.sep")));
  EXPECT_EQ(presentation->width, 0);
  EXPECT_EQ(presentation->height, 0);
}

TEST(SepPresentation_Tests, seppresentation_load_false_2) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_FALSE(presentation->load(""));
  EXPECT_EQ(presentation->width, 0);
  EXPECT_EQ(presentation->height, 0);
}

TEST(SepPresentation_Tests, seppresentation_load_true) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_TRUE(presentation->load(TestFiles::getPathToFile("sep_cmyk.sep")));
  EXPECT_EQ(presentation->width, 600);
  EXPECT_EQ(presentation->height, 400);
  for (auto c : presentation->sep_source->channels) {
    EXPECT_NE(presentation->sep_source->channel_files[c], nullptr);
  }
}

TEST(SepPresentation_Tests, seppresentation_getTitle) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_TRUE(presentation->load(TestFiles::getPathToFile("sep_cmyk.sep")));
  EXPECT_EQ(presentation->getTitle(), TestFiles::getPathToFile("sep_cmyk.sep"));
}

TEST(SepPresentation_Tests, seppresentation_getTransform) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_TRUE(presentation->load(TestFiles::getPathToFile("sep_cmyk.sep")));
  EXPECT_NE(presentation->getTransform(), nullptr);
}

TEST(SepPresentation_Tests, seppresentation_getRect) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_TRUE(presentation->load(TestFiles::getPathToFile("sep_cmyk.sep")));
  auto rect = presentation->getRect();
  EXPECT_EQ(rect.getTop(), 0);
  EXPECT_EQ(rect.getLeft(), 0);
  EXPECT_EQ(rect.getWidth(), 600);
  EXPECT_EQ(rect.getHeight(), 400);
}

TEST(SepPresentation_Tests, seppresentation_getViews) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  auto views = presentation->getViews();
  EXPECT_TRUE(views.empty());
}

TEST(SepPresentation_Tests, seppresentation_pipette) { // NOLINT
  SepPresentation::Ptr presentation = SepPresentation::create(makeLogger());
  EXPECT_TRUE(presentation->load(TestFiles::getPathToFile("sep_cmyk.sep")));
  auto rect = presentation->getRect();
  EXPECT_EQ(rect.getTop(), 0);
  EXPECT_EQ(rect.getLeft(), 0);
  EXPECT_EQ(rect.getWidth(), 600);
  EXPECT_EQ(rect.getHeight(), 400);

  for (auto c : presentation->sep_source->channels) {
    EXPECT_NE(presentation->sep_source->channel_files[c], nullptr);
  }

  auto averages = presentation->getPixelAverages(rect);

  for (auto &avg : averages) {
    EXPECT_FALSE(avg.first.empty());
    EXPECT_FALSE(std::isnan(avg.second));
  }
}