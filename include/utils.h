#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <openssl/hmac.h>
#include <sstream>
#include <string>
#include <vector>

#define HTTP_GET "GET"
#define HTTP_POST "POST"

#define PRINT_VALUE(x) cout << #x << " = " << x << endl
#define DBGPRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#define IS_DOUBLE_EQUAL(x, y) (abs((x) - (y)) < 0.000001)

class utils {
public:
  static std::string hmac_sha256_hex(std::string key, std::string msg) {
    unsigned char hash[32];

    // HMAC_CTX hmac;  // old version, error: variable has incomplete type
    // 'HMAC_CTX' (aka 'hmac_ctx_st')

    HMAC_CTX *pHmac = HMAC_CTX_new();
    HMAC_CTX_reset(pHmac);
    HMAC_Init_ex(pHmac, &key[0], key.length(), EVP_sha256(), NULL);
    HMAC_Update(pHmac, (unsigned char *)&msg[0], msg.length());
    unsigned int len = 32;
    HMAC_Final(pHmac, hash, &len);
    HMAC_CTX_free(pHmac);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < len; i++) {
      ss << std::hex << std::setw(2) << (unsigned int)hash[i];
    }

    return (ss.str());
  }

  static std::string hmac_sha256_str(std::string key, std::string msg) {
    unsigned char hash[32];

    HMAC_CTX *pHmac = HMAC_CTX_new();
    HMAC_CTX_reset(pHmac);
    HMAC_Init_ex(pHmac, &key[0], key.length(), EVP_sha256(), NULL);
    HMAC_Update(pHmac, (unsigned char *)&msg[0], msg.length());
    unsigned int len = 32;
    HMAC_Final(pHmac, hash, &len);
    HMAC_CTX_free(pHmac);

    std::stringstream ss;
    ss << std::setfill('0');
    for (int i = 0; i < len; i++) {
      ss << hash[i];
    }

    return (ss.str());
  }

  static long get_milli() {
    using namespace std::chrono;
    milliseconds x =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return x.count();
  }

  static std::string read_file(std::string fname) {
    std::ifstream t(fname);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
  }
};

#endif // CPP_UTILS_H
