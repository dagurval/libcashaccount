// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "cashaccount.hpp"
#include "test/testutil.h"
#include <boost/test/unit_test.hpp>

// Test vectors from:
// https://gitlab.com/cash-accounts/specification/blob/master/test_data/op_return.yaml
// at commit 238fd418

static void test_payload(const std::string &expected_hex, size_t expected_len,
                         const Payload *p) {
  const std::string payload =
      std::string(p->payload, p->payload + p->payload_len);

  BOOST_CHECK_EQUAL(expected_len, p->payload_len);
  BOOST_CHECK_EQUAL(ParseHex(expected_hex), payload);
}

/// OP_RETURN <OP_PUSH(4)><protocol> <OP_PUSH(8)><account_name>
/// OP_PUSH(21=0x15)><payment_data>
BOOST_AUTO_TEST_CASE(opreturn_test1) {
  const std::string case1 = ParseHex(
      "6a0401010101084a6f6e617468616e1501ebdeb6430f3d16a9c6758d6c0d7a400c8e6"
      "bbee4");
  auto account = cashacc_account_init();
  int payloads =
      cashacc_parse_opreturn(case1.c_str(), case1.size(), true, account);
  BOOST_CHECK_EQUAL(1, payloads);
  BOOST_CHECK_EQUAL("Jonathan", account->name);
  test_payload("01ebdeb6430f3d16a9c6758d6c0d7a400c8e6bbee4", 21,
               account->payloads);
  cashacc_account_destroy(account);
}

/// OP_RETURN <OP_PUSH(4)><protocol> <OP_PUSHDATA1><99=0x63><account_name>
/// OP_PUSH(21=0x15)><payment_data>
BOOST_AUTO_TEST_CASE(opreturn_test2) {
  const std::string case2 = ParseHex(
      "6a04010101014c63495f616d5f796f75725f6b696e675f57656c6c5f495f6469646e"
      "745f766f74655f666f725f796f755f596f755f646f6e745f766f74655f666f725f6b"
      "696e67735f57656c6c5f686f775f6469645f796f755f6265636f6d655f6b696e675f"
      "7468656e5f15014685ab7ecdcee6addf8928cf684b47b07b0aa8f6");
  auto account = cashacc_account_init();
  int payloads =
      cashacc_parse_opreturn(case2.c_str(), case2.size(), true, account);
  BOOST_CHECK_EQUAL(1, payloads);
  BOOST_CHECK_EQUAL("I_am_your_king_Well_I_didnt_vote_for_you_You_dont_vote"
                    "_for_kings_Well_how_did_you_become_king_then_",
                    account->name);
  test_payload("014685ab7ecdcee6addf8928cf684b47b07b0aa8f6", 21,
               account->payloads);
  cashacc_account_destroy(account);
}

BOOST_AUTO_TEST_CASE(opreturn_test3) {
  const std::string case3 = ParseHex(
      "6a04010101010c4d6f6e7374657262697461724c5103010003c70482cfa903349d64"
      "1659cf407e9421d0569788bec2154910a6a5914e6c89b0992a77fedb78855f9c6c22"
      "ca070904577f7cab40910e63cf9cc18492e73071a600000000000000000000000000"
      "15018d4e356539eaac7d84b1216030bd9f12424bbb72");
  auto account = cashacc_account_init();
  int payloads =
      cashacc_parse_opreturn(case3.c_str(), case3.size(), true, account);
  BOOST_CHECK_EQUAL(2, payloads);
  BOOST_CHECK_EQUAL("Monsterbitar", account->name);

  test_payload(
      "03010003c70482cfa903349d641659cf407e9421d0569788bec2154910a6a5914e6c"
      "89b0992a77fedb78855f9c6c22ca070904577f7cab40910e63cf9cc18492e73071a6"
      "00000000000000000000000000",
      81, account->payloads);
  test_payload("018d4e356539eaac7d84b1216030bd9f12424bbb72", 21,
               account->payloads + 1);
  cashacc_account_destroy(account);
}

BOOST_AUTO_TEST_CASE(opreturn_ignore_payload) {
  const std::string case3 = ParseHex(
      "6a04010101010c4d6f6e7374657262697461724c5103010003c70482cfa903349d64"
      "1659cf407e9421d0569788bec2154910a6a5914e6c89b0992a77fedb78855f9c6c22"
      "ca070904577f7cab40910e63cf9cc18492e73071a600000000000000000000000000"
      "15018d4e356539eaac7d84b1216030bd9f12424bbb72");

  auto account = cashacc_account_init();
  int payloads =
      cashacc_parse_opreturn(case3.c_str(), case3.size(), false, account);
  BOOST_CHECK_EQUAL(2, payloads);
  BOOST_CHECK_EQUAL("Monsterbitar", account->name);
  BOOST_CHECK(nullptr == account->payloads);
}
