#include "cashaccount.hpp"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <regex>
#include <vector>

namespace {
static const char OP_RETURN = 0x6a;
static const char OP_PUSHDATA1 = 0x4c;
static const char OP_PUSHDATA2 = 0x4d;
static const char OP_PUSHDATA4 = 0x4e;
static const unsigned int CASHACCOUNT_PREFIX_LENGTH = 6;
static const char CASHACCOUNT_PREFIX[6] = {
    OP_RETURN, /* push */ 0x04, 0x01, 0x01, 0x01, 0x01};
} // namespace

CashAccount *cashacc_account_init() {
  CashAccount *account =
      static_cast<CashAccount *>(std::malloc(sizeof(CashAccount)));
  memset(account, 0, sizeof(CashAccount));
  return account;
}

void cashacc_account_clear(CashAccount *acc) {
  free(acc->name);

  if (acc->payloads != nullptr) {
    for (size_t i = 0; i < acc->payloads_len; ++i) {
      free(acc->payloads[i].payload);
    }
    free(acc->payloads);
  }
  acc->name = nullptr;
  acc->payloads = nullptr;
  acc->payloads_len = 0;
}

void cashacc_account_destroy(CashAccount *acc) {
  cashacc_account_clear(acc);
  free(acc);
}

#define INCREMENT_I                                                            \
  if (++i >= len)                                                              \
    return CASHACC_ERR_PUSHDATA_ERROR;

int cashacc_parse_opreturn(const char *opreturn, unsigned int len,
                           bool store_payload, CashAccount *account) {
  assert(account != nullptr);

  if (!cashacc_check_prefix(opreturn, len)) {
    return CASHACC_ERR_WRONG_PREFIX;
  }

  // Locate the individual parts of the registration.
  // parts.first is the position of the first character.
  // parts.second is the length of the part
  std::vector<std::pair<int, int>> parts;

  for (int i = CASHACCOUNT_PREFIX_LENGTH; i < len; ++i) {
    uint32_t push = 0;

    const char &opcode = *(opreturn + i);
    if (opcode >= 0x01 && opcode <= 0x4b) {
      push = opcode;
      INCREMENT_I;
    } else if (opcode == OP_PUSHDATA1) {
      INCREMENT_I;
      push = *(opreturn + i);
      INCREMENT_I;
    } else if (opcode == OP_PUSHDATA2) {
      char byte1, byte2;
      INCREMENT_I;
      byte1 = (*opreturn + i);
      INCREMENT_I;
      byte2 = (*opreturn + i);
      INCREMENT_I;
      // order is little-endien
      push = byte1 | byte2 << 8;
    } else if (opcode == OP_PUSHDATA4) {
      char byte1, byte2, byte3, byte4;
      INCREMENT_I;
      byte1 = (*opreturn + i);
      INCREMENT_I;
      byte2 = (*opreturn + i);
      INCREMENT_I;
      byte3 = (*opreturn + i);
      INCREMENT_I;
      byte4 = (*opreturn + i);
      INCREMENT_I;
      // order is little-endien
      push = byte1 | byte2 << 8 | byte3 << 16 | byte4 << 24;
    } else {
      // no push?
      return CASHACC_ERR_NO_PUSHDATA;
    }

    if (i + push > len) {
      // less data left than the push claims there is
      return CASHACC_ERR_PUSHDATA_ERROR;
    }

    parts.push_back({i, push});
    i += push - 1 /* -1 because we incremented already */;
  }

  if (parts.size() < 2) {
    // needs at minimum a name + one payload
    return CASHACC_ERR_TOO_FEW_PARTS;
  }

  const char *name_start = opreturn + parts[0].first;
  const size_t name_len = parts[0].second;

  if (!cashacc_check_name(name_start, name_len)) {
    return CASHACC_ERR_INVALID_ACCOUNT_NAME;
  }

  account->payloads = nullptr;
  account->name = static_cast<char *>(std::malloc(name_len + 1));
  if (account->name == nullptr) {
    return CASHACC_ERR_MALLOC_FAILED;
  }
  memcpy(account->name, name_start, name_len);
  account->name[name_len] = '\0';

  if (store_payload) {
    // realloc requires that there already is an allocation
    account->payloads = static_cast<Payload *>(std::malloc(sizeof(Payload)));
  }
  account->payloads_len = 0;

  for (size_t i = 1; i < parts.size(); ++i) {
    const size_t payload_len = parts[i].second;
    const uint8_t payload_type = *(opreturn + parts[i].first);

    if (!cashacc_check_payload_type(payload_type)) {
      // uknown type, ignore for forward compatibility
      continue;
    }
    ++account->payloads_len;
    if (!store_payload) {
      continue;
    }

    Payload *payloads_tmp = static_cast<Payload *>(std::realloc(
        account->payloads, sizeof(Payload) * account->payloads_len));

    if (payloads_tmp == nullptr) {
      // realloc failed, keep old ptr so it can be freed
      return CASHACC_ERR_MALLOC_FAILED;
    }
    account->payloads = payloads_tmp;

    Payload *current = account->payloads + (account->payloads_len - 1);
    current->type = payload_type;
    current->payload_len = payload_len;
    current->payload = static_cast<char *>(std::malloc(payload_len));
    if (current->payload == nullptr) {
      return CASHACC_ERR_MALLOC_FAILED;
    }
    memcpy(current->payload, opreturn + parts[i].first, payload_len);
  }
  return account->payloads_len;
}

int cashacc_check_name(const char *name, unsigned int len) {
  static const std::regex VALID_USERNAME("^[a-zA-Z0-9_]{1,99}$");

  if (len < 1 || len > 99) {
    return false;
  }
  std::cmatch m;
  return std::regex_match(name, name + len, m, VALID_USERNAME);
}

int cashacc_check_prefix(const char *opreturn, unsigned int len) {
  if (len < CASHACCOUNT_PREFIX_LENGTH) {
    return false;
  }
  for (size_t i = 0; i < CASHACCOUNT_PREFIX_LENGTH; ++i) {
    if (*(opreturn + i) != CASHACCOUNT_PREFIX[i]) {
      return false;
    }
  }
  return true;
}

int cashacc_check_payload_type(unsigned char type) {
  switch (type) {
  case CASHACC_PAYMENT_KEYHASH:
  case CASHACC_PAYMENT_SCRIPTHASH:
  case CASHACC_PAYMENT_PAYMENTCODE:
  case CASHACC_PAYMENT_STEALTHKEYS:
  case CASHACC_PAYMENT_TOKEN_KEYHASH:
  case CASHACC_PAYMENT_TOKEN_SCRIPTHASH:
  case CASHACC_PAYMENT_TOKEN_PAYMENTCODE:
  case CASHACC_PAYMENT_TOKEN_STEALTHKEYS:
    return true;
  default:
    return false;
  }
  return false;
}
