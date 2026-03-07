#pragma once

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

class TestFiles : public ::testing::Environment {
private:
  static std::filesystem::path dir;

public:
  void SetUp() override;

  static std::string getPath();
  static std::string getPathToFile(const std::string &filename);
};
