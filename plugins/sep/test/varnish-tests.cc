#include <gtest/gtest.h>

#include "../varnish/varnish.hh"
#include "testglobals.hh"
#include <scroom/scroominterface.hh>
#include <scroom/viewinterface.hh>

///////////////////////////////////////////////////////////////////////////////
// Helper object

class DummyViewInterface : public ViewInterface {

public:
  DummyViewInterface(){};
  ~DummyViewInterface(){};
  void invalidate(){};
  ProgressInterface::Ptr getProgressInterface() { return nullptr; }

  static ViewInterface::Ptr create() {
    return ViewInterface::Ptr(new DummyViewInterface());
  }

  void addSideWidget(std::string, GtkWidget *w) {
    require(Scroom::GtkHelpers::on_ui_thread());

    // Add the widget to a fresh GTK box instead.
    // This won't verify UI integrity, but it should
    // at least allow the code to run once in headless CI
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(box), w, true, true, 0);
  }
  void removeSideWidget(GtkWidget *) {
    require(Scroom::GtkHelpers::on_ui_thread());
  };
  void addToToolbar(GtkToolItem *) {
    require(Scroom::GtkHelpers::on_ui_thread());
  };
  void removeFromToolbar(GtkToolItem *) {
    require(Scroom::GtkHelpers::on_ui_thread());
  };
  void registerSelectionListener(SelectionListener::Ptr){};
  void registerPostRenderer(PostRenderer::Ptr){};
  void setStatusMessage(const std::string &) {
    require(Scroom::GtkHelpers::on_ui_thread());
  };
  std::shared_ptr<PresentationInterface> getCurrentPresentation() {
    return nullptr;
  };
  void addToolButton(GtkToggleButton *, ToolStateListener::Ptr) {
    require(Scroom::GtkHelpers::on_ui_thread());
  };
};

///////////////////////////////////////////////////////////////////////////////
// Tests for varnish loading

TEST(varnish_tests, varnish_load_valid_tiff) { // NOLINT
  SliLayer::Ptr test_varnishLayer = SliLayer::create(
      TestFiles::getPathToFile("v_valid.tif"), "SomeCoolTitle", 0, 0);
  test_varnishLayer->fillMetaFromTiff(8, 1);
  test_varnishLayer->fillBitmapFromTiff();
  Varnish::Ptr test_varnish = Varnish::create(test_varnishLayer);
  // Properties set correctly?
  EXPECT_EQ(test_varnish->layer->name, "SomeCoolTitle");
  EXPECT_EQ(test_varnish->layer->filepath,
            TestFiles::getPathToFile("v_valid.tif"));
  EXPECT_EQ(test_varnish->layer->width, 20);
  EXPECT_EQ(test_varnish->layer->height, 20);
  EXPECT_EQ(test_varnish->layer->xoffset, 0);
  EXPECT_EQ(test_varnish->layer->yoffset, 0);
  EXPECT_EQ(test_varnish->layer->xAspect, 1);
  EXPECT_EQ(test_varnish->layer->yAspect, 1);
  EXPECT_FALSE(test_varnish->inverted);
  // Valid cairo surface?
  EXPECT_TRUE(test_varnish->surface);
}

TEST(varnish_tests, varnish_load_valid_tiff_centimeter) { // NOLINT
  SliLayer::Ptr test_varnishLayer =
      SliLayer::create(TestFiles::getPathToFile("v_valid_centimeter.tif"),
                       "SomeCoolTitle", 0, 0);
  test_varnishLayer->fillMetaFromTiff(8, 1);
  test_varnishLayer->fillBitmapFromTiff();
  Varnish::Ptr test_varnish = Varnish::create(test_varnishLayer);
  // Properties set correctly?
  EXPECT_EQ(test_varnish->layer->name, "SomeCoolTitle");
  EXPECT_EQ(test_varnish->layer->filepath,
            TestFiles::getPathToFile("v_valid_centimeter.tif"));
  EXPECT_EQ(test_varnish->layer->width, 20);
  EXPECT_EQ(test_varnish->layer->height, 20);
  EXPECT_EQ(test_varnish->layer->xoffset, 0);
  EXPECT_EQ(test_varnish->layer->yoffset, 0);
  EXPECT_EQ(test_varnish->layer->xAspect, 1);
  EXPECT_EQ(test_varnish->layer->yAspect, 1);
  EXPECT_FALSE(test_varnish->inverted);
  // Valid cairo surface?
  EXPECT_TRUE(test_varnish->surface);
}

TEST(varnish_tests, varnish_load_valid_tiff_no_spp_tag) { // NOLINT
  // This test file does not have a SamplePerPixel tag set, but should default
  // to 1 spp
  SliLayer::Ptr test_varnishLayer =
      SliLayer::create(TestFiles::getPathToFile("v_valid_no_spp_tag.tif"),
                       "SomeCoolTitle", 0, 0);
  test_varnishLayer->fillMetaFromTiff(8, 1);
  test_varnishLayer->fillBitmapFromTiff();
  Varnish::Ptr test_varnish = Varnish::create(test_varnishLayer);
  // Properties set correctly?
  EXPECT_EQ(test_varnish->layer->name, "SomeCoolTitle");
  EXPECT_EQ(test_varnish->layer->filepath,
            TestFiles::getPathToFile("v_valid_no_spp_tag.tif"));
  EXPECT_EQ(test_varnish->layer->width, 20);
  EXPECT_EQ(test_varnish->layer->height, 20);
  EXPECT_EQ(test_varnish->layer->xoffset, 0);
  EXPECT_EQ(test_varnish->layer->yoffset, 0);
  EXPECT_EQ(test_varnish->layer->xAspect, 1);
  EXPECT_EQ(test_varnish->layer->yAspect, 1);
  EXPECT_FALSE(test_varnish->inverted);
  // Valid cairo surface?
  EXPECT_TRUE(test_varnish->surface);
}

TEST(varnish_tests, varnish_load_invalid_tiff) { // NOLINT
  SliLayer::Ptr test_varnishLayer = SliLayer::create(
      TestFiles::getPathToFile("v_invalidrgb.tif"), "Another Title", 0, 0);
  // Tif handling should fail here
  EXPECT_FALSE(test_varnishLayer->fillMetaFromTiff(8, 1));
  // Properties set correctly?
  EXPECT_EQ(test_varnishLayer->name, "Another Title");
  EXPECT_EQ(test_varnishLayer->filepath,
            TestFiles::getPathToFile("v_invalidrgb.tif"));
  EXPECT_EQ(test_varnishLayer->width, 0);
  EXPECT_EQ(test_varnishLayer->height, 0);
  EXPECT_EQ(test_varnishLayer->xoffset, 0);
  EXPECT_EQ(test_varnishLayer->yoffset, 0);
  EXPECT_EQ(test_varnishLayer->xAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->yAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->bitmap, nullptr);
}

TEST(varnish_tests, varnish_load_zero_res_tiff) { // NOLINT
  // This test file is missing a width tag
  SliLayer::Ptr test_varnishLayer =
      SliLayer::create(TestFiles::getPathToFile("v_invalid_no_width.tif"),
                       "Another Title", 0, 0);
  // Tif handling should fail here
  EXPECT_FALSE(test_varnishLayer->fillMetaFromTiff(8, 1));
  // Properties set correctly?
  EXPECT_EQ(test_varnishLayer->name, "Another Title");
  EXPECT_EQ(test_varnishLayer->filepath,
            TestFiles::getPathToFile("v_invalid_no_width.tif"));
  EXPECT_EQ(test_varnishLayer->width, 0);
  EXPECT_EQ(test_varnishLayer->height, 0);
  EXPECT_EQ(test_varnishLayer->xoffset, 0);
  EXPECT_EQ(test_varnishLayer->yoffset, 0);
  EXPECT_EQ(test_varnishLayer->xAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->yAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->bitmap, nullptr);
}

TEST(varnish_tests, varnish_load_corrupted_tiff) { // NOLINT
  // This test files magic number and file data was randomly edited.
  SliLayer::Ptr test_varnishLayer = SliLayer::create(
      TestFiles::getPathToFile("v_corrupted.tif"), "Another Title", 0, 0);
  // Tif handling should fail here
  EXPECT_FALSE(test_varnishLayer->fillMetaFromTiff(8, 1));
  // Properties set correctly?
  EXPECT_EQ(test_varnishLayer->name, "Another Title");
  EXPECT_EQ(test_varnishLayer->filepath,
            TestFiles::getPathToFile("v_corrupted.tif"));
  EXPECT_EQ(test_varnishLayer->width, 0);
  EXPECT_EQ(test_varnishLayer->height, 0);
  EXPECT_EQ(test_varnishLayer->xoffset, 0);
  EXPECT_EQ(test_varnishLayer->yoffset, 0);
  EXPECT_EQ(test_varnishLayer->xAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->yAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->bitmap, nullptr);
}

TEST(varnish_tests, varnish_load_nonexistent_tiff) { // NOLINT
  SliLayer::Ptr test_varnishLayer =
      SliLayer::create(TestFiles::getPathToFile("v_nonexistent.tif"),
                       "This file doesn't exist", 0, 0);
  // Tif handling should fail here
  EXPECT_FALSE(test_varnishLayer->fillMetaFromTiff(8, 1));
  // Properties set correctly?
  EXPECT_EQ(test_varnishLayer->name, "This file doesn't exist");
  EXPECT_EQ(test_varnishLayer->filepath,
            TestFiles::getPathToFile("v_nonexistent.tif"));
  EXPECT_EQ(test_varnishLayer->width, 0);
  EXPECT_EQ(test_varnishLayer->height, 0);
  EXPECT_EQ(test_varnishLayer->xoffset, 0);
  EXPECT_EQ(test_varnishLayer->yoffset, 0);
  EXPECT_EQ(test_varnishLayer->xAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->yAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->bitmap, nullptr);
}

TEST(varnish_tests, varnish_load_1bps_tiff) { // NOLINT
  // This test file has 1 bps, which is not supported
  SliLayer::Ptr test_varnishLayer = SliLayer::create(
      TestFiles::getPathToFile("v_invalid1bps.tif"), "TitleGoesHere", 0, 0);
  // Tif handling should fail here
  EXPECT_FALSE(test_varnishLayer->fillMetaFromTiff(8, 1));
  // Properties set correctly?
  EXPECT_EQ(test_varnishLayer->name, "TitleGoesHere");
  EXPECT_EQ(test_varnishLayer->filepath,
            TestFiles::getPathToFile("v_invalid1bps.tif"));
  EXPECT_EQ(test_varnishLayer->width, 0);
  EXPECT_EQ(test_varnishLayer->height, 0);
  EXPECT_EQ(test_varnishLayer->xoffset, 0);
  EXPECT_EQ(test_varnishLayer->yoffset, 0);
  EXPECT_EQ(test_varnishLayer->xAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->yAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->bitmap, nullptr);
}

TEST(varnish_tests, varnish_load_invalid_no_bps_tiff) { // NOLINT
  // This test file doesn't have a bps tag, thus will default to 1 bps. 1bps is
  // not supported for varnish
  SliLayer::Ptr test_varnishLayer =
      SliLayer::create(TestFiles::getPathToFile("v_invalid_no_bps_tag.tif"),
                       "TitleGoesHere", 0, 0);
  // Tif handling should fail here
  EXPECT_FALSE(test_varnishLayer->fillMetaFromTiff(8, 1));
  // Properties set correctly?
  EXPECT_EQ(test_varnishLayer->name, "TitleGoesHere");
  EXPECT_EQ(test_varnishLayer->filepath,
            TestFiles::getPathToFile("v_invalid_no_bps_tag.tif"));
  EXPECT_EQ(test_varnishLayer->width, 0);
  EXPECT_EQ(test_varnishLayer->height, 0);
  EXPECT_EQ(test_varnishLayer->xoffset, 0);
  EXPECT_EQ(test_varnishLayer->yoffset, 0);
  EXPECT_EQ(test_varnishLayer->xAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->yAspect, 0.0f);
  EXPECT_EQ(test_varnishLayer->bitmap, nullptr);
}