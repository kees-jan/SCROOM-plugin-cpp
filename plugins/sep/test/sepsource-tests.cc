#include <gtest/gtest.h>

#include <boost/algorithm/string.hpp>

#include "../sepsource.hh"
#include "../sli/slilayer.hh"
#include "testglobals.hh"

TEST(SepSource_Tests, sepsource_create) { // NOLINT
  auto source = SepSource::create();
  EXPECT_NE(source, nullptr);
}

TEST(SepSource_Tests, sepsource_parent_dir) { // NOLINT
  auto abctest = SepSource::findParentDir("abc/test/removed");
  EXPECT_EQ(abctest, "abc/test");
}

TEST(SepSource_Tests, sepsource_parse_sep) { // NOLINT
  SepFile file = SepSource::parseSep(TestFiles::getPathToFile("sep_cmyk.sep"));
  EXPECT_EQ(file.files.size(), 4u);

  for (const std::string colour : {"C", "M", "Y", "K"}) {
    EXPECT_EQ(file.files[boost::algorithm::to_upper_copy(colour)],
              TestFiles::getPathToFile(colour + ".tif"));
  }

  EXPECT_EQ(file.width, 600);
  EXPECT_EQ(file.height, 400);
}

TEST(SepSource_Tests, sepsource_parse_sep_extra_lines) { // NOLINT
  SepFile file =
      SepSource::parseSep(TestFiles::getPathToFile("sep_extra_lines.sep"));
  EXPECT_EQ(file.files.size(), 4u);

  for (const std::string colour : {"C", "M", "Y", "K"}) {
    EXPECT_EQ(file.files[boost::algorithm::to_upper_copy(colour)],
              TestFiles::getPathToFile(colour + ".tif"));
  }

  EXPECT_EQ(file.width, 600);
  EXPECT_EQ(file.height, 400);
}

TEST(SepSource_Tests, sepsource_parse_sep_empty_line) { // NOLINT
  SepFile file =
      SepSource::parseSep(TestFiles::getPathToFile("sep_empty_line_2.sep"));
  EXPECT_EQ(file.files.size(), 4u);

  for (const std::string colour : {"C", "M", "Y", "K"}) {
    EXPECT_EQ(file.files[boost::algorithm::to_upper_copy(colour)],
              TestFiles::getPathToFile(colour + ".tif"));
  }

  EXPECT_EQ(file.width, 600);
  EXPECT_EQ(file.height, 400);
}

TEST(SepSource_Tests, sepsource_parse_sep_missing_channel) { // NOLINT
  // Y channel is not defined in this file
  SepFile file =
      SepSource::parseSep(TestFiles::getPathToFile("sep_missing_channel.sep"));
  EXPECT_EQ(file.files.size(), 3u);

  for (const std::string colour : {"C", "M", "K"}) {
    EXPECT_EQ(file.files[boost::algorithm::to_upper_copy(colour)],
              TestFiles::getPathToFile(colour + ".tif"));
  }

  EXPECT_TRUE(file.files["Y"].empty());
  EXPECT_EQ(file.width, 600);
  EXPECT_EQ(file.height, 400);
}

TEST(SepSource_Tests, sepsource_parse_sep_missing_channel_2) { // NOLINT
  // C channel is not defined in this file + empty line
  SepFile file =
      SepSource::parseSep(TestFiles::getPathToFile("sep_empty_line_3.sep"));
  EXPECT_EQ(file.files.size(), 3u);

  for (const std::string colour : {"M", "Y", "K"}) {
    EXPECT_EQ(file.files[boost::algorithm::to_upper_copy(colour)],
              TestFiles::getPathToFile(colour + ".tif"));
  }

  EXPECT_TRUE(file.files["C"].empty());
  EXPECT_EQ(file.width, 600);
  EXPECT_EQ(file.height, 400);
}

TEST(SepSource_Tests, sepsource_get_for_none) { // NOLINT
  uint16_t unit;
  float x_res, y_res;
  auto source = SepSource::create();

  source->getForOneChannel(nullptr, unit, x_res, y_res);

  EXPECT_EQ(unit, RESUNIT_NONE);
  EXPECT_NEAR(x_res, 1.0, 1e-4);
  EXPECT_NEAR(y_res, 1.0, 1e-4);
}

TEST(SepSource_Tests, sepsource_get_resolution_null) { // NOLINT
  uint16_t unit;
  float x_res, y_res;
  auto source = SepSource::create();

  source->channel_files["C"] = nullptr;
  source->channel_files["M"] = nullptr;
  source->channel_files["Y"] = nullptr;
  source->channel_files["K"] = nullptr;

  EXPECT_TRUE(source->getResolution(unit, x_res, y_res));
}

TEST(SepSource_Tests, sepsource_get_transformation) { // NOLINT
  auto source = SepSource::create();

  source->channel_files["C"] = nullptr;
  source->channel_files["M"] = nullptr;
  source->channel_files["Y"] = nullptr;
  source->channel_files["K"] = nullptr;

  auto res = source->getTransform()->getAspectRatio();
  EXPECT_NEAR(res.x, 1.0, 1e4);
  EXPECT_NEAR(res.y, 1.0, 1e4);
}

TEST(SepSource_Tests, sepsource_set_data) { // NOLINT
  // Preparation
  auto source = SepSource::create();
  SepFile file = SepSource::parseSep(TestFiles::getPathToFile("sep_cmyk.sep"));

  // Tested call
  source->setData(file);

  // Check properties, not the actual value, because we don't care if
  // setData copies the passed SepFile.
  EXPECT_EQ(source->sep_file.files.size(), 4u);

  for (const std::string colour : {"C", "M", "Y", "K"}) {
    EXPECT_EQ(source->sep_file.files[boost::algorithm::to_upper_copy(colour)],
              TestFiles::getPathToFile(colour + ".tif"));
  }

  EXPECT_EQ(source->sep_file.width, 600);
  EXPECT_EQ(source->sep_file.height, 400);
}

TEST(SepSource_Tests, sepsource_open_files) { // NOLINT
  // Preparation
  auto source = SepSource::create();
  SepFile file = SepSource::parseSep(TestFiles::getPathToFile("sep_cmyk.sep"));
  source->setData(file);

  // Tested call
  source->openFiles();

  // Check that all the CMYK files have been opened
  for (const std::string colour : {"C", "M", "Y", "K"}) {
    EXPECT_NE(source->channel_files[boost::algorithm::to_upper_copy(colour)],
              nullptr);
  }

  // Check that varnish is not opened
  EXPECT_EQ(source->varnish, nullptr);
}

TEST(SepSource_Tests, sepsource_open_files_twice) { // NOLINT
  // Preparation
  auto source = SepSource::create();
  SepFile file = SepSource::parseSep(TestFiles::getPathToFile("sep_cmyk.sep"));
  source->setData(file);
  source->openFiles();

  // Save the TIFF pointers to make sure they don't change
  std::map<std::string, tiff *> files;
  for (const std::string colour : {"C", "M", "Y", "K"}) {
    files[colour] =
        source->channel_files[boost::algorithm::to_upper_copy(colour)];
  }

  // Tested call
  source->openFiles();

  // Check that all the CMYK files have not been opened again
  for (const std::string colour : {"C", "M", "Y", "K"}) {
    EXPECT_EQ(source->channel_files[boost::algorithm::to_upper_copy(colour)],
              files[colour]);
  }
}

TEST(SepSource_Tests, sepsource_open_files_extra) { // NOLINT
  // Preparation
  auto source = SepSource::create();
  SepFile file = SepSource::parseSep(TestFiles::getPathToFile("sep_cmykv.sep"));
  source->setData(file);
  // Set white manually to avoid white choice popup dialog which
  // crashes when executed during tests.
  source->sep_file.files["W"] = TestFiles::getPathToFile("C.tif");

  // Tested call
  source->openFiles();

  // Check that all the CMYK files have been opened
  for (const std::string colour : {"C", "M", "Y", "K"}) {
    EXPECT_NE(source->channel_files[boost::algorithm::to_upper_copy(colour)],
              nullptr);
  }

  // Check that varnish has also been opened
  EXPECT_NE(source->varnish, nullptr);
}

TEST(SepSource_Tests, sepsource_check_files) { // NOLINT
  // Preparation
  auto source = SepSource::create();
  SepFile file = SepSource::parseSep(TestFiles::getPathToFile("sep_cmykv.sep"));
  source->setData(file);
  // Set white manually to avoid white choice popup dialog which
  // crashes when executed during tests.
  source->sep_file.files["W"] = TestFiles::getPathToFile("C.tif");
  source->openFiles();

  // Tested call — check that it does not crash
  source->checkFiles();
}

TEST(SepSource_Tests, sepsource_tiff_wrapper_nullptr) { // NOLINT
  auto res = SepSource::TIFFReadScanline_(nullptr, nullptr, 1);
  EXPECT_EQ(res, -1);
}

TEST(SepSource_Tests, sepsource_tiff_wrapper_2) { // NOLINT
  auto file = TIFFOpen(TestFiles::getPathToFile("M_9.tif").c_str(), "r");
  auto lines = std::vector<uint8_t>(TIFFScanlineSize(file));
  int res = SepSource::TIFFReadScanline_(file, lines.data(), 1);
  EXPECT_NE(res, -1);
}

TEST(SepSource_Tests, sepsource_fill_sli_empty) { // NOLINT
  SliLayer::Ptr sli = SliLayer::create("", "name", 0, 0);
  sli->height = 42;
  SepSource::Ptr sepSource = SepSource::create();
  sepSource->fillSliLayerMeta(sli);
  EXPECT_EQ(sli->height, 42);
}

TEST(SepSource_Tests, sepsource_fill_sli_1) { // NOLINT
  SliLayer::Ptr sli =
      SliLayer::create(TestFiles::getPathToFile("sep_cmyk.sep"), "name", 0, 0);
  SepSource::Ptr sepSource = SepSource::create();
  sepSource->fillSliLayerMeta(sli);
  sepSource->fillSliLayerBitmap(sli);
  EXPECT_EQ(sli->width, 600);
  EXPECT_EQ(sli->height, 400);
  EXPECT_EQ(sli->spp, 4); // This file specifically has 4 channels
  EXPECT_EQ(sli->bps, 8);
  EXPECT_NE(sli->bitmap, nullptr);
  EXPECT_NEAR(sli->xAspect, 1.0, 1e-4);
  EXPECT_NEAR(sli->yAspect, 1.0, 1e-4);
}

TEST(SepSource_Tests, sepsource_closeIfNeeded_1) { // NOLINT
  auto file = TIFFOpen(TestFiles::getPathToFile("M_9.tif").c_str(), "r");
  EXPECT_NE(file, nullptr);
  SepSource::closeIfNeeded(file);
  EXPECT_EQ(file, nullptr);
}

TEST(SepSource_Tests, sepsource_closeIfNeeded_2) { // NOLINT
  auto file = TIFFOpen(nullptr, "r");
  EXPECT_EQ(file, nullptr);
  SepSource::closeIfNeeded(file);
  EXPECT_EQ(file, nullptr);
}

TEST(SepSource_Tests, sepsource_fill_no_tiles) { // NOLINT
  // Preparation
  std::vector<Tile::Ptr> tiles;
  auto source = SepSource::create();
  SepFile file = SepSource::parseSep(TestFiles::getPathToFile("sep_cmyk.sep"));
  source->setData(file);
  source->openFiles();

  // Tested call — check that fillTiles does not crash
  source->fillTiles(0, 0, 4096, 0, tiles);
}
