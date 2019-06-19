#include "test/testutil.h"
#include <cassert>
#include <sstream>

std::string ParseHex(const std::string &hex) {
  assert(hex.size() % 2 == 0);
  std::string res(hex.size() / 2, '0');
  for (size_t i = 0; i < hex.size(); i += 2) {
    std::stringstream ss;
    std::string curr(begin(hex) + i, begin(hex) + (i + 2));
    res[i / 2] = static_cast<char>(stoi(curr, nullptr, 16));
  }
  return res;
}
