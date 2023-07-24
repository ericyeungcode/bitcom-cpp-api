#ifndef BITCOM_REST_H
#define BITCOM_REST_H

// CPPHTTPLIB_OPENSSL_SUPPORT must be put before #include <httplib.h> to enable https
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <string>
#include <httplib.h>
#include "utils.h"

using namespace std;

class BitcomRestApi
{
public:
    BitcomRestApi(const std::string &apiHost, const std::string &ak, const std::string &sk);

    httplib::Result wsAuth(const unordered_map<string, string> &queryParamMap);
    httplib::Result queryAccountMode(const unordered_map<string, string> &queryParamMap);
    httplib::Result queryUmAccount(const unordered_map<string, string> &queryParamMap);
    httplib::Result queryUmTxLogs(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotQueryAccountConfigs(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotWsAuth(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotQueryClassicAccount(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotQueryClassicTxLogs(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotQueryOrders(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotQueryOpenOrders(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotQueryUserTrades(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotNewOrder(const nlohmann::json &jsonBody);
    httplib::Result spotAmendOrder(const nlohmann::json &jsonBody);
    httplib::Result spotCancelOrder(const nlohmann::json &jsonBody);
    httplib::Result spotNewBatchNewOrders(const nlohmann::json &jsonBody);
    httplib::Result spotAmendBatchNewOrders(const nlohmann::json &jsonBody);
    httplib::Result spotEnableCod(const nlohmann::json &jsonBody);
    httplib::Result spotQueryMmpState(const unordered_map<string, string> &queryParamMap);
    httplib::Result spotUpdateMmpConfig(const nlohmann::json &jsonBody);
    httplib::Result spotResetMmpConfig(const nlohmann::json &jsonBody);
    httplib::Result linearQueryAccountConfigs(const unordered_map<string, string> &queryParamMap);
    httplib::Result linearQueryOrders(const unordered_map<string, string> &queryParamMap);
    httplib::Result linearQueryOpenOrders(const unordered_map<string, string> &queryParamMap);
    httplib::Result linearQueryUserTrades(const unordered_map<string, string> &queryParamMap);
    httplib::Result linearNewOrder(const nlohmann::json &jsonBody);
    httplib::Result linearAmendOrder(const nlohmann::json &jsonBody);
    httplib::Result linearCancelOrder(const nlohmann::json &jsonBody);
    httplib::Result linearNewBatchNewOrders(const nlohmann::json &jsonBody);
    httplib::Result linearAmendBatchNewOrders(const nlohmann::json &jsonBody);
    httplib::Result linearQueryMmpState(const nlohmann::json &jsonBody);
    httplib::Result linearUpdateMmpConfig(const nlohmann::json &jsonBody);
    httplib::Result linearResetMmpConfig(const nlohmann::json &jsonBody);

private:
    httplib::Result doPrivateQuery(const string &httpMethod, const string &apiPath, const unordered_map<string, string> &paramMap);
    httplib::Result doPrivateCallWithJsonBody(const string &httpMethod, const string &apiPath, const nlohmann::json &jsonBody);

    std::string _apiHost;
    std::string _accessKey;
    std::string _privateKey;
    httplib::Client _restClient;
};

#endif // BITCOM_REST_H