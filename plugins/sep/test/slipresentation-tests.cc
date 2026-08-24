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

SliPresentation::Ptr createPresentation() {
  SliPresentation::Ptr presentation =
      SliPresentation::create(nullptr, makeLogger());
  EXPECT_NE(presentation, nullptr);
  if (!presentation)
    throw std::runtime_error("SliPresentation::create returned null");
  // Assign the callbacks to dummy functions to avoid exceptions
  presentation->source->enableInteractions = [] {};
  presentation->source->disableInteractions = [] {};
  return presentation;
}

void dummyRedraw(SliPresentation::Ptr presentation) {
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
    for (int retries = 100;
         retries > 0 &&
         (!presentation->source ||
          !presentation->source->rgbCache.count(std::min(0, zoom)));
         retries--) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    EXPECT_TRUE(presentation->source->rgbCache.at(std::min(0, zoom)));
  }
}

///////////////////////////////////////////////////////////////////////////////
// Tests

TEST(Sli_Tests, slipresentation_load_sli_tiffonly) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_tiffonly.sli"));
  ASSERT_EQ(presentation->getLayers().size(), SLI_NOF_LAYERS);
  dummyRedraw(presentation);
}

TEST(Sli_Tests, slipresentation_load_sli_seponly) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_seponly.sli"));
  ASSERT_EQ(presentation->getLayers().size(), SLI_NOF_LAYERS);
  dummyRedraw(presentation);
}

TEST(Sli_Tests, slipresentation_load_sli_septiffmixed) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_septiffmixed.sli"));
  ASSERT_EQ(presentation->getLayers().size(), SLI_NOF_LAYERS);
  dummyRedraw(presentation);
}

TEST(Sli_Tests, slipresentation_load_sli_scale) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_scale.sli"));
  ASSERT_EQ(presentation->getLayers().size(), SLI_NOF_LAYERS);
  dummyRedraw(presentation);
}

TEST(Sli_Tests, slipresentation_load_sli_xoffset) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_xoffset.sli"));
  ASSERT_EQ(presentation->getLayers().size(), SLI_NOF_LAYERS);
  dummyRedraw(presentation);
}

TEST(Sli_Tests, slipresentation_load_sli_varnish) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_varnish.sli"));
  std::cout << presentation->getLayers().size() << '\n';
  ASSERT_EQ(presentation->getLayers().size(), SLI_NOF_LAYERS);
}

TEST(Sli_Tests, slipresentation_load_sli_varnish_wrongpath) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_varnish_wrongpath.sli"));
  ASSERT_EQ(presentation->getLayers().size(), 0u);
}

TEST(Sli_Tests, slipresentation_presentationinterface_inherited) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPath());

  std::string nameStr = "testname";
  std::string valueStr;

  ASSERT_FALSE(presentation->isPropertyDefined(nameStr));
  ASSERT_FALSE(presentation->getProperty(nameStr, valueStr));
  ASSERT_EQ(valueStr, "");

  presentation->properties["testname"] = "testvalue";

  ASSERT_TRUE(presentation->isPropertyDefined(nameStr));
  ASSERT_TRUE(presentation->getProperty(nameStr, valueStr));
  ASSERT_EQ(valueStr, "testvalue");

  ASSERT_EQ(presentation->getTitle(), TestFiles::getPath());

  presentation.reset();
}

TEST(Sli_Tests, slipresentation_pipette_tool_multiple_colors) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_pipette.sli"));
  ASSERT_EQ(presentation->getLayers().size(), 1u);
  dummyRedraw(presentation);
  // Testing 4 CMYK pixels + rectangle larger than the canvas
  Scroom::Utils::Rectangle<double> rect1{0, 0, 3, 4};
  auto result = presentation->getPixelAverages(rect1);
  for (auto r : result)
    EXPECT_NEAR(r.second, 63.75, 0.0001);
}

TEST(Sli_Tests, slipresentation_pipette_tool_one_color) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_pipette.sli"));
  ASSERT_EQ(presentation->getLayers().size(), 1u);
  dummyRedraw(presentation);
  // C
  Scroom::Utils::Rectangle<double> rect1{0, 0, 1, 1};
  auto result = presentation->getPixelAverages(rect1);
  EXPECT_NEAR(result[0].second, 255, 0.0001);
  // M
  Scroom::Utils::Rectangle<double> rect2{1, 0, 1, 1};
  result = presentation->getPixelAverages(rect2);
  EXPECT_NEAR(result[1].second, 255, 0.0001);
  // Y
  Scroom::Utils::Rectangle<double> rect3{0, 1, 1, 1};
  result = presentation->getPixelAverages(rect3);
  EXPECT_NEAR(result[2].second, 255, 0.0001);
  // K
  Scroom::Utils::Rectangle<double> rect4{1, 1, 1, 1};
  result = presentation->getPixelAverages(rect4);
  EXPECT_NEAR(result[3].second, 255, 0.0001);
}

TEST(Sli_Tests, slipresentation_pipette_tool_zero_area) { // NOLINT
  SliPresentation::Ptr presentation = createPresentation();
  presentation->load(TestFiles::getPathToFile("sli_pipette.sli"));
  ASSERT_EQ(presentation->getLayers().size(), 1u);
  dummyRedraw(presentation);

  Scroom::Utils::Rectangle<double> rect1{0, 0, 0, 0};
  auto result = presentation->getPixelAverages(rect1);
  EXPECT_TRUE(result.empty());
}