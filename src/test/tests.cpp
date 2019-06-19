// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "cashaccount.hpp"
#include "test/testutil.h"
#include <boost/test/unit_test.hpp>
#include <set>

BOOST_AUTO_TEST_CASE(test_opreturn_prefix) {
  const char too_short[2] = {0, 0};
  const char valid_prefix[6] = {0x6a, 0x04, 0x01, 0x01, 0x01, 0x01};
  const char invalid_prefix[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  BOOST_CHECK(!cashacc_check_prefix(too_short, 2));
  BOOST_CHECK(!cashacc_check_prefix(invalid_prefix, 6));
  BOOST_CHECK(cashacc_check_prefix(valid_prefix, 6));
}

BOOST_AUTO_TEST_CASE(test_name_length) {
  const std::string too_long(100, 'a');
  const std::string too_short("");
  const std::string ok = "ok";

  BOOST_CHECK(!cashacc_check_name(too_long.c_str(), too_long.size()));
  BOOST_CHECK(!cashacc_check_name(too_short.c_str(), too_long.size()));
  BOOST_CHECK(cashacc_check_name(ok.c_str(), ok.size()));
}

BOOST_AUTO_TEST_CASE(test_name_characters) {
  const std::string valid_chars = "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVQXYZ"
                                  "0123456789";

  BOOST_CHECK(cashacc_check_name(valid_chars.c_str(), valid_chars.size()));

  // everything else is invalid
  const std::string some_invalid = "!@#¤%&/()=\\";
  for (char c : some_invalid) {
    BOOST_CHECK(!cashacc_check_name(&c, 1));
  }

  // hex contains name length(8) + name(Jonathan) + payload (garbage in this
  // context)
  const std::string ignore_garbage = ParseHex(
      "084a6f6e617468616e1501ebdeb6430f3d16a9c6758d6c0d7a400c8e6bbee4");
  const char *as_cstr = ignore_garbage.c_str();
  BOOST_CHECK(cashacc_check_name(as_cstr + 1, static_cast<int>(as_cstr[0])));
}

BOOST_AUTO_TEST_CASE(test_payload_type) {

  const std::set<uint8_t> valid = {0x01, 0x02, 0x03, 0x04,
                                   0x81, 0x82, 0x83, 0x84};

  for (uint8_t i = std::numeric_limits<uint8_t>::min();
       i < std::numeric_limits<uint8_t>::max(); ++i) {
    BOOST_CHECK_EQUAL(valid.count(i), cashacc_check_payload_type(i));
  }
}
