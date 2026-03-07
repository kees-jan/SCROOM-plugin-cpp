#include "testglobals.hh"

#include <iostream>

#include <scroom/gtk-test-helpers.hh>

namespace {
class GtkMainLoopEnvironment : public ::testing::Environment {
private:
  Scroom::GtkTestHelpers::GtkMainLoop loop;
};
} // namespace

std::filesystem::path TestFiles::dir;

void TestFiles::SetUp() {
  dir = TESTFILES_DIR;
  std::cout << "[----------] TestFiles directory: " << dir << "\n";
}

std::string TestFiles::getPath() { return dir.string(); }

std::string TestFiles::getPathToFile(const std::string &filename) {
  return (dir / filename).string();
}

namespace {
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
const bool environments_registered =
    (::testing::AddGlobalTestEnvironment(new TestFiles),
     ::testing::AddGlobalTestEnvironment(new GtkMainLoopEnvironment),
     true);
} // namespace
