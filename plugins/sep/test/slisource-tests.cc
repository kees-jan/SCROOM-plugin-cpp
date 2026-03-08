#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>
#include <thread>

#include "../sli/slipresentation.hh"
#include "testglobals.hh"
#include <scroom/scroominterface.hh>

constexpr size_t SLI_NOF_LAYERS = 4;

///////////////////////////////////////////////////////////////////////////////
// Helper functions

SliPresentation::Ptr createPresentation1() {
  SliPresentation::Ptr presentation = SliPresentation::create(nullptr);
  EXPECT_NE(presentation, nullptr);
  if (!presentation)
    throw std::runtime_error("SliPresentation::create returned null");
  // Assign the callbacks to dummy functions to avoid exceptions
  presentation->source->enableInteractions = [] {};
  presentation->source->disableInteractions = [] {};
  return presentation;
}

void dummyRedraw1(SliPresentation::Ptr presentation) {
  // Create dummy objects to call redraw() with
  cairo_surface_t *surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
  cairo_t *cr = cairo_create(surface);
  Scroom::Utils::Rectangle<double> rect(0.0, 0.0, 100.0, 100.0);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // redraw() for all zoom levels from 5 to -2 and check whether cache has been
  // computed
  for (int zoom = 5; zoom > -3; zoom--) {
    presentation->redraw(nullptr, cr, rect, zoom);
    std::this_thread::sleep_for(std::chrono::milliseconds(
        1000)); // Very liberal, shouldn't fail because of time
    ASSERT_TRUE(presentation->source->rgbCache.at(std::min(0, zoom)));
  }
  ASSERT_NE(presentation, nullptr);
}

///////////////////////////////////////////////////////////////////////////////
// Tests

TEST(Sli_Tests, slisource_clearbottomsurface_all_toggled) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation1();

  presentation->load(TestFiles::getPathToFile("sli_tiffonly.sli"));
  dummyRedraw1(presentation);
  presentation->source->toggled = boost::dynamic_bitset<>{SLI_NOF_LAYERS}.set();
  presentation->source->clearBottomSurface();
  int total_height = presentation->source->total_height;
  int total_width = presentation->source->total_width;
  bool allZero = true;
  auto surface = presentation->source->rgbCache.at(0)->getBitmap();
  for (int i = 0; i < total_height * total_width * 4; i++) {
    if (surface[i] != 0) {
      allZero = false;
      break;
    }
  }
  EXPECT_TRUE(allZero);
}

TEST(Sli_Tests, slisource_clearbottomsurface_none_toggled) { // NOLINT
  SliPresentation::Ptr presentation1 = createPresentation1();
  SliPresentation::Ptr presentation2 = createPresentation1();

  presentation1->load(TestFiles::getPathToFile("sli_tiffonly.sli"));
  presentation2->load(TestFiles::getPathToFile("sli_tiffonly.sli"));
  dummyRedraw1(presentation1);
  dummyRedraw1(presentation2);
  presentation1->source->toggled = boost::dynamic_bitset<>{SLI_NOF_LAYERS};
  presentation1->source->clearBottomSurface();
  int total_height = presentation1->source->total_height;
  int total_width = presentation1->source->total_width;
  auto surface1 = presentation1->source->rgbCache.at(0)->getBitmap();
  auto surface2 = presentation2->source->rgbCache.at(0)->getBitmap();
  bool bothEqual = true;
  for (int i = 0; i < total_height * total_width * 4; i++) {
    if (surface1[i] != surface2[i]) {
      bothEqual = false;
      break;
    }
  }
  EXPECT_TRUE(bothEqual);
}

TEST(Sli_Tests, slisource_clearbottomsurface_some_toggled) { // NOLINT
  SliPresentation::Ptr presentation1 = createPresentation1();
  SliPresentation::Ptr presentation2 = createPresentation1();

  presentation1->load(TestFiles::getPathToFile("sli_tiffonly.sli"));
  presentation2->load(TestFiles::getPathToFile("sli_tiffonly.sli"));
  dummyRedraw1(presentation1);
  dummyRedraw1(presentation2);
  presentation1->source->toggled =
      boost::dynamic_bitset<>{SLI_NOF_LAYERS}.set(0);
  presentation1->source->clearBottomSurface();
  int height = presentation1->source->layers[0]->height;
  int width = presentation1->source->layers[0]->width;
  int total_height = presentation1->source->total_height;
  int total_width = presentation1->source->total_width;
  auto surface1 = presentation1->source->rgbCache.at(0)->getBitmap();
  auto surface2 = presentation2->source->rgbCache.at(0)->getBitmap();

  bool someZero = true;
  for (int i = 0; i < height * width * 4; i++) {
    if (surface1[i] != 0) {
      someZero = false;
      break;
    }
  }
  EXPECT_TRUE(someZero);

  bool bothEqual = true;
  for (int i = height * width * 4; i < total_height * total_width * 4; i++) {
    if (surface1[i] != surface2[i]) {
      bothEqual = false;
      break;
    }
  }
  EXPECT_TRUE(bothEqual);
}

// tinycmyk.tif (cmyk) = [(255,0,0,0),(0,255,0,0),(0,0,255,0),(0,0,0,255)]
// tinycmyk.tif (bgra) =
//              [(255,255,0,255),(255,0,255,255),(0,255,255,255),(0,0,0,255)]
TEST(Sli_Tests, slisource_computergb_yoffset) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation1();
  presentation->load(TestFiles::getPathToFile("sli_tinycmyk.sli"));
  dummyRedraw1(presentation);
  auto surface = presentation->source->getSurface(0)->getBitmap();
  presentation->source->computeRgb();
  // bgra conversion of tinycmyk.tif
  uint8_t tinycmyk[] = {255, 255, 0,   255, 255, 0, 255, 255,
                        0,   255, 255, 255, 0,   0, 0,   255};

  for (int i = 0; i < 2 * 2 * 4; i++) {
    EXPECT_EQ(surface[i], tinycmyk[i]);
  }
}

TEST(Sli_Tests, slisource_computergb_xoffset) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation1();
  presentation->load(TestFiles::getPathToFile("sli_tinycmyk_xoffset.sli"));
  dummyRedraw1(presentation);
  auto surface = presentation->source->rgbCache.at(0)->getBitmap();
  presentation->source->computeRgb();
  // bgra conversion of tinycmyk.tif
  uint8_t tinycmyk[] = {0, 0, 0, 0, 255, 255, 0,   255, 255, 0, 255, 255,
                        0, 0, 0, 0, 0,   255, 255, 255, 0,   0, 0,   255};

  for (int i = 0; i < 2 * 3 * 4; i++) {
    EXPECT_EQ(surface[i], tinycmyk[i]);
  }
}