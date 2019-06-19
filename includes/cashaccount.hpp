// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
///
/// libcashaccount is a support library for handling Cash Account as per
/// v1.0 of the specification.
/// https://gitlab.com/cash-accounts/specification/blob/master/SPECIFICATION.md
#ifndef LIBCASHACCOUNT_H
#define LIBCASHACCOUNT_H

static const int CASHACC_ERR_INVALID_ACCOUNT_NAME = -1;
///
/// Parsing failed to to out of memory.
static const int CASHACC_ERR_MALLOC_FAILED = -2;
///
/// PUSHDATA requested more data parsed than remains in OP_RETURN array.
static const int CASHACC_ERR_PUSHDATA_ERROR = -3;
///
/// PUSHDATA op code missing
static const int CASHACC_ERR_NO_PUSHDATA = -4;

///
/// Registration contains too few registration parts. For example, it may
/// have a account name, but no payloads.
static const int CASHACC_ERR_TOO_FEW_PARTS = -5;

///
/// OP_RETURN does not have the correct cashaccount prefix.
static const int CASHACC_ERR_WRONG_PREFIX = -6;

/// Known (and valid) payment types
static const unsigned char CASHACC_PAYMENT_KEYHASH = 0x01;
static const unsigned char CASHACC_PAYMENT_SCRIPTHASH = 0x02;
static const unsigned char CASHACC_PAYMENT_PAYMENTCODE = 0x03;
static const unsigned char CASHACC_PAYMENT_STEALTHKEYS = 0x04;
static const unsigned char CASHACC_PAYMENT_TOKEN_KEYHASH = 0x81;
static const unsigned char CASHACC_PAYMENT_TOKEN_SCRIPTHASH = 0x82;
static const unsigned char CASHACC_PAYMENT_TOKEN_PAYMENTCODE = 0x83;
static const unsigned char CASHACC_PAYMENT_TOKEN_STEALTHKEYS = 0x84;

/// A cash account playload. One registration can contain multiple payloads.
///
struct Payload {
  char type;                ///< Payment type. See CASHACC_PAYMENT* constants.
  char *payload;            ///< The payload itself.
  unsigned int payload_len; ///< Length of the payload.
};

/// A cash account registration
///
struct CashAccount {
  char *name;                ///< The account name
  Payload *payloads;         ///< Payloads, nullptr if payloads are ignored
  unsigned int payloads_len; ///< Number of payloads in registration
};

/// Allocate the cash account result data.
/// Free using cashacc_account_destroy.
CashAccount *cashacc_account_init();

/// Clear name/payloads from CashAccount. The struct can be reused
/// after calling this.
void cashacc_account_clear(CashAccount *);

///
/// Free all memory, including payloads.
void cashacc_account_destroy(CashAccount *);

/// Parse CashAccount from an OP_RETURN script
/// \param opreturn Pointer to script
/// \param opreturn_len Length of the script
/// \param store_payload If payload is to be stored. If you just need to verify
///                      that script contains a cashaccount and get the name,
///                      you can set to false for increased performance.
/// \param account The variable where the result is stored
/// \return Number of payloads found, or error code on error
int cashacc_parse_opreturn(const char *opreturn, unsigned int len,
                           bool store_payload, CashAccount *account);

/// Check if string is a valid cashaccount name (used in
/// cashacc_parse_opreturn)
int cashacc_check_name(const char *name, unsigned int len);
/// Check if opreturn has the correct cashaccount prefix (used in
/// cashacc_parse_opreturn)
int cashacc_check_prefix(const char *opreturn, unsigned int len);

/// Check if payload type is a know type. Paylods with unknown types will
/// be ignored.
int cashacc_check_payload_type(unsigned char type);

#endif // LIBCASHACCOUNT_H
