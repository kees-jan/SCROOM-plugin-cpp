#include <gtest/gtest.h>

#include <boost/dynamic_bitset.hpp>

#include "../sli/sli-helpers.hh"
#include "../sli/slilayer.hh"
#include "testglobals.hh"
#include <scroom/scroominterface.hh>

TEST(Sli_Tests, slihelpers_clearsurface) { // NOLINT
  unsigned int width = 10;
  unsigned int height = 10;
  auto surfaceWrapper =
      SurfaceWrapper::create(width, height, CAIRO_FORMAT_ARGB32); // 40x10
  cairo_t *cr = cairo_create(surfaceWrapper->surface);
  cairo_set_source_rgba(cr, 1, 1, 1, 1);
  cairo_paint(cr); // paint everything whites
  int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
  uint8_t *data = cairo_image_surface_get_data(surfaceWrapper->surface);
  bool allWhite = true;
  bool allClear = true;

  // Check all pixels are white
  for (unsigned int i = 0; i < height * stride; i += (stride / width)) {
    if (data[i] != 255) {
      allWhite = false;
      break;
    }
  }
  EXPECT_TRUE(allWhite);

  surfaceWrapper->clearSurface();

  // Checks all pixels have been cleared (ie = 0)
  for (unsigned int i = 0; i < height * stride; i++) {
    if (data[i] != 0) {
      allClear = false;
      break;
    }
  }
  EXPECT_TRUE(allClear);
  cairo_destroy(cr);
}

TEST(Sli_Tests, slihelpers_clearsurface_rect) { // NOLINT
  unsigned int width = 10;
  unsigned int height = 10;
  auto surfaceWrapper =
      SurfaceWrapper::create(width, height, CAIRO_FORMAT_ARGB32); // 40x10
  cairo_t *cr = cairo_create(surfaceWrapper->surface);
  cairo_set_source_rgba(cr, 1, 1, 1, 1);
  cairo_paint(cr); // paint everything whites
  int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
  uint8_t *data = cairo_image_surface_get_data(surfaceWrapper->surface);
  Scroom::Utils::Rectangle<int> rect = {0, 0, 5, 10}; // in bytes
  bool allWhite = true;
  bool rectClear = true;

  // Check all pixels are white
  for (unsigned int i = 0; i < height * stride; i += (stride / width)) {
    if (data[i] != 255) {
      allWhite = false;
      break;
    }
  }
  EXPECT_TRUE(allWhite);

  surfaceWrapper->clearSurface(rect);

  // Checks that the pixels inside the rectangles have been cleared
  // and that those outside are still white
  for (unsigned int i = 0; i < height * stride; i++) {
    if ((i % stride) < 5 * (stride / width)) // clear part
    {
      if (data[i] != 0) {
        rectClear = false;
        break;
      }
    } else {
      if (data[i] != 255) {
        rectClear = false;
        break;
      }
    }
  }
  EXPECT_TRUE(rectClear);
  cairo_destroy(cr);
}

TEST(Sli_Tests, slihelpers_get_area) { // NOLINT
  Scroom::Utils::Rectangle<int> rect1{0, 0, 6969, 420};
  Scroom::Utils::Rectangle<int> rect2{0, 0, 0, 0};
  Scroom::Utils::Rectangle<int> rect3{0, 1, 2, 2};
  Scroom::Utils::Rectangle<int> rect4{1, 0, 1, 1};

  EXPECT_EQ(getArea(rect1), 2926980);
  EXPECT_EQ(getArea(rect2), 0);
  EXPECT_EQ(getArea(rect3), 4);
  EXPECT_EQ(getArea(rect4), 1);
}

TEST(Sli_Tests, slihelpers_to_bytes_rectangle) { // NOLINT
  Scroom::Utils::Rectangle<int> rect1{10, 10, 100, 100};
  auto bRect = toBytesRectangle(rect1);

  EXPECT_EQ(bRect.getWidth(), 400);
  EXPECT_EQ(bRect.getHeight(), 100);
  EXPECT_EQ(bRect.getLeft(), 40);
  EXPECT_EQ(bRect.getTop(), 10);
}

TEST(Sli_Tests, slihelpers_point_to_offset) { // NOLINT
  Scroom::Utils::Point<int> p1{2, 2};
  int stride = 5;
  Scroom::Utils::Rectangle<int> rect{2, 2, 5, 5};
  Scroom::Utils::Point<int> p2{3, 3};

  EXPECT_EQ(pointToOffset(p1, stride), 12);
  EXPECT_EQ(pointToOffset(rect, p2), 6);
}

TEST(Sli_Tests, slihelpers_spanned_rectangle) { // NOLINT
  unsigned int n_layers = 2;
  boost::dynamic_bitset<> bitmap{n_layers};
  std::vector<SliLayer::Ptr> layers;
  auto const logger = makeLogger();

  layers.push_back(SliLayer::create("", "Layer", 0, 0, logger));
  layers[0]->xoffset = 0;
  layers[0]->yoffset = 0;
  layers[0]->width = 100;
  layers[0]->height = 100;
  bitmap.set(0);

  auto spanRect1 = spannedRectangle(bitmap, layers, false);
  EXPECT_EQ(getArea(spanRect1), 100 * 100);

  layers.push_back(SliLayer::create("", "Layer", 0, 0, logger));
  layers[1]->xoffset = 10;
  layers[1]->yoffset = 10;
  layers[1]->width = 100;
  layers[1]->height = 100;
  bitmap.set(1);

  auto spanRect2 = spannedRectangle(bitmap, layers, false);
  EXPECT_EQ(getArea(spanRect2), 110 * 110);
}