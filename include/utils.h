#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <map>
#include <unordered_map>
// #include <openssl/hmac.h>
#include <openssl/evp.h>
#include <sstream>
#include <string>
#include <vector>

#define HTTP_GET "GET"
#define HTTP_POST "POST"

#define PRINT_VALUE(x) cout << #x << " = " << x << endl
#define DBGPRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#define IS_DOUBLE_EQUAL(x, y) (abs((x) - (y)) < 0.000001)

class utils
{
public:

  // Modern OpenSSL 3.x HMAC-SHA256
  static void hmac_sha256(const unsigned char *key, int key_len,
                          const unsigned char *data, int data_len,
                          unsigned char *out, size_t *out_len)
  {
    EVP_MAC_CTX *ctx = nullptr;
    EVP_MAC *mac = EVP_MAC_fetch(nullptr, "HMAC", nullptr);

    OSSL_PARAM params[2];
    params[0] = OSSL_PARAM_construct_utf8_string("digest", (char *)"SHA256", 0);
    params[1] = OSSL_PARAM_construct_end();

    ctx = EVP_MAC_CTX_new(mac);
    EVP_MAC_init(ctx, key, key_len, params);
    EVP_MAC_update(ctx, data, data_len);
    EVP_MAC_final(ctx, out, out_len, EVP_MAX_MD_SIZE);

    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(mac);
  }

  static std::string to_hex_string(const unsigned char* data, unsigned int len) {
    std::ostringstream oss;
    for (unsigned int i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    return oss.str();
  }

  static std::string hmac_sha256_hex(const std::string &key, const std::string &msg)
  {
    unsigned char result[EVP_MAX_MD_SIZE];
    size_t result_len;

    hmac_sha256(reinterpret_cast<const unsigned char*>(key.c_str()), key.size(),
                reinterpret_cast<const unsigned char*>(msg.c_str()), msg.size(),
                result, &result_len);

    return to_hex_string(reinterpret_cast<const unsigned char*>(result), result_len);
  }

  static long get_milli()
  {
    using namespace std::chrono;
    milliseconds x =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return x.count();
  }

  static std::string read_file(std::string fname)
  {
    std::ifstream t(fname);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
  }
};

#endif // CPP_UTILS_H
