#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "json.hpp"
#include "utils.h"
#include <boost/algorithm/string.hpp>
#include <httplib.h>
#include <iostream>
#include <sstream>
#include "bitcom_rest.h"

using namespace std;
using namespace boost::algorithm;

#define CONTENT_TYPE_JSON "application/json"

// ws auth
string V1_WS_AUTH = "/v1/ws/auth";

// SPOT
string V1_SPOT_INSTRUMENTS = "/spot/v1/instruments";
string V1_SPOT_ACCOUNTS = "/spot/v1/accounts";
string V1_SPOT_ORDERS = "/spot/v1/orders";
string V1_SPOT_CANCEL_ORDERS = "/spot/v1/cancel_orders";
string V1_SPOT_OPENORDERS = "/spot/v1/open_orders";
string V1_SPOT_USER_TRADES = "/spot/v1/user/trades";
string V1_SPOT_AMEND_ORDERS = "/spot/v1/amend_orders";
string V1_SPOT_TRANSACTION_LOGS = "/spot/v1/transactions";
string V1_SPOT_WS_AUTH = "/spot/v1/ws/auth";
string V1_SPOT_BATCH_ORDERS = "/spot/v1/batchorders";
string V1_SPOT_AMEND_BATCH_ORDERS =
    "/spot/v1/amend_batchorders";
string V1_SPOT_MMP_STATE = "/spot/v1/mmp_state";
string V1_SPOT_MMP_UPDATE_CONFIG = "/spot/v1/update_mmp_config";
string V1_SPOT_RESET_MMP = "/spot/v1/reset_mmp";
string V1_SPOT_ACCOUNT_CONFIGS_COD =
    "/spot/v1/account_configs/cod";
string V1_SPOT_ACCOUNT_CONFIGS = "/spot/v1/account_configs";
string V1_SPOT_AGG_TRADES = "/spot/v1/aggregated/trades";

// UM
string V1_UM_ACCOUNT_MODE = "/um/v1/account_mode";
string V1_UM_ACCOUNTS = "/um/v1/accounts";
string V1_UM_TRANSACTIONS = "/um/v1/transactions";
string V1_UM_INTEREST_RECORDS = "/um/v1/interest_records";

// LINEAR;
string V1_LINEAR_POSITIONS = "/linear/v1/positions";
string V1_LINEAR_ORDERS = "/linear/v1/orders";
string V1_LINEAR_CANCEL_ORDERS = "/linear/v1/cancel_orders";
string V1_LINEAR_OPENORDERS = "/linear/v1/open_orders";
string V1_LINEAR_USER_TRADES = "/linear/v1/user/trades";
string V1_LINEAR_AMEND_ORDERS = "/linear/v1/amend_orders";
string V1_LINEAR_EST_MARGINS = "/linear/v1/margins";
string V1_LINEAR_CLOSE_POS = "/linear/v1/close_positions";
string V1_LINEAR_BATCH_ORDERS = "/linear/v1/batchorders";
string V1_LINEAR_AMEND_BATCH_ORDERS =
    "/linear/v1/amend_batchorders";
string V1_LINEAR_BLOCK_TRADES = "/linear/v1/blocktrades";
string V1_LINEAR_USER_INFO = "/linear/v1/user/info";
string V1_LINEAR_PLATFORM_BLOCK_TRADES =
    "/linear/v1/platform_blocktrades";
string V1_LINEAR_ACCOUNT_CONFIGS = "/linear/v1/account_configs";
string V1_LINEAR_LEVERAGE_RATIO = "/linear/v1/leverage_ratio";
string V1_LINEAR_AGG_POSITIONS =
    "/linear/v1/aggregated/positions";
string V1_LINEAR_MMP_STATE = "/linear/v1/mmp_state";
string V1_LINEAR_MMP_UPDATE_CONFIG =
    "/linear/v1/update_mmp_config";
string V1_LINEAR_RESET_MMP = "/linear/v1/reset_mmp";

string encode_object(const unordered_map<string, string> &paramMap);

string encode_vector(const vector<unordered_map<string, string>> &itemVec)
{
    std::vector<string> strVec;
    for (const auto &item : itemVec)
    {
        strVec.push_back(encode_object(item));
    }

    std::sort(strVec.begin(), strVec.end());
    return "[" + boost::algorithm::join(strVec, "&") + "]";
}

string encode_object(const unordered_map<string, string> &paramMap)
{
    std::vector<string> sortedKeys;
    for (const auto &x : paramMap)
    {
        sortedKeys.push_back(x.first);
    }

    std::sort(sortedKeys.begin(), sortedKeys.end());
    std::vector<string> strVec;
    for (const auto &ikey : sortedKeys)
    {
        std::cout << "processing " << ikey << "\n";
        auto val = paramMap.at(ikey);
        strVec.push_back(ikey + "=" + val);
    }

    std::sort(strVec.begin(), strVec.end());
    return boost::algorithm::join(strVec, "&");
}

httplib::Result doPrivateQueryGeneral(httplib::Client *client, const string &accessKey, const string &secretKey,
                                      const string &httpMethod, const string &host, const string &path,
                                      const unordered_map<string, string> &argMap)
{
    auto paramMap = argMap; // clone
    long nonce = utils::get_milli();
    paramMap.insert(std::make_pair("timestamp", to_string(nonce)));

    string strToSign = path + "&" + encode_object(paramMap);
    auto sig = utils::hmac_sha256_hex(secretKey, strToSign);

    auto url = host + path;
    ostringstream oss;
    for (const auto &item : paramMap)
    {
        oss << item.first << "=" << item.second << "&";
    }
    oss << "signature=" << sig;
    auto queryString = oss.str();
    url += "?" + queryString;
    httplib::Headers headers = {{"X-Bit-Access-Key", accessKey}};

    std::cout << "------------- Make http request --------------\n";
    std::cout << "Request: " << httpMethod << " " << url << "\n";

    try
    {
        auto res = client->Get(url, headers);
        if (res) {
            cout << "status: " << res->status << "\n";
            cout << "body: " << res->body << "\n";
            return res;
        } else {
            cout << "GET failed: " << res.error() << ", url:" << url << "\n";
        }
    }
    catch (...)
    {
        cout << "Exception occurs...\n";
    }
    return httplib::Result();
}

httplib::Result doPrivateCallWithJsonBodyGeneral(httplib::Client *client, const string &accessKey, const string &secretKey,
                                                 const string &httpMethod, const string &host, const string &path,
                                                 const nlohmann::json &jsonBody)
{
    unordered_map<string, string> paramMap;

    // encode array(if any) before we call encode_object()
    for (auto &el : jsonBody.items())
    {
        if (el.value().is_array())
        {
            vector<unordered_map<string, string>> vecJs;
            for (auto &sub : el.value())
            {
                vecJs.emplace_back(sub.get<unordered_map<string, string>>());
            }
            auto arrayVal = encode_vector(vecJs);
            paramMap.insert(std::make_pair(el.key(), arrayVal));
        }
        else
        {
            std::string str = el.value();
            paramMap.insert(std::make_pair(el.key(), str));
        }
    }
    long nonce = utils::get_milli();
    paramMap.insert(std::make_pair("timestamp", to_string(nonce)));

    string strToSign = path + "&" + encode_object(paramMap);
    auto sig = utils::hmac_sha256_hex(secretKey, strToSign);

    auto url = host + path;
    auto sendOutJsBody = jsonBody;
    sendOutJsBody["timestamp"] = nonce;
    sendOutJsBody["signature"] = sig;
    auto jsonBodyStr = sendOutJsBody.dump();

    httplib::Headers headers = {{"X-Bit-Access-Key", accessKey}};
    std::cout << "------------- Make http request --------------\n";
    std::cout << "Request: " << httpMethod << " " << url << "\n";
    std::cout << "Json body: " << jsonBodyStr << "\n";
    std::cout << "strToSign: " << strToSign << "\n";

    try
    {
        auto res = client->Post(url, headers, jsonBodyStr, CONTENT_TYPE_JSON);
        if (res) {
            cout << "status: " << res->status << "\n";
            cout << "body: " << res->body << "\n";
            return res;
        } else {
            cout << "POST failed: " << res.error() << ", url:" << url << "\n";
        }
    }
    catch (...)
    {
        cout << "Exception occurs...\n";
    }
    return httplib::Result();
}

BitcomRestApi::BitcomRestApi(const std::string &apiHost, const std::string &ak, const std::string &sk) : _apiHost(apiHost), _accessKey(ak), _privateKey(sk), _restClient(apiHost)
{
}

httplib::Result BitcomRestApi::doPrivateQuery(const string &httpMethod, const string &apiPath, const unordered_map<string, string> &paramMap)
{
    return doPrivateQueryGeneral(&this->_restClient, this->_accessKey, this->_privateKey, httpMethod,
                                 this->_apiHost, apiPath, paramMap);
}

httplib::Result BitcomRestApi::doPrivateCallWithJsonBody(const string &httpMethod, const string &apiPath, const nlohmann::json &jsonBody)
{
    return doPrivateCallWithJsonBodyGeneral(&this->_restClient, this->_accessKey, this->_privateKey, httpMethod,
                                            this->_apiHost, apiPath, jsonBody);
}

////////////////////////////////////////////////////
// rest api endpoints
////////////////////////////////////////////////////
httplib::Result BitcomRestApi::wsAuth(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_WS_AUTH, queryParamMap);
}

////////////////////////////////////////////////////
// UM
////////////////////////////////////////////////////
httplib::Result BitcomRestApi::queryAccountMode(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_UM_ACCOUNT_MODE, queryParamMap);
}

httplib::Result BitcomRestApi::queryUmAccount(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_UM_ACCOUNTS, queryParamMap);
}

httplib::Result BitcomRestApi::queryUmTxLogs(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_UM_TRANSACTIONS, queryParamMap);
}

////////////////////////////////////////////////////
// SPOT
////////////////////////////////////////////////////
httplib::Result BitcomRestApi::spotQueryAccountConfigs(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_ACCOUNT_CONFIGS, queryParamMap);
}

httplib::Result BitcomRestApi::spotWsAuth(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_WS_AUTH, queryParamMap);
}

httplib::Result BitcomRestApi::spotQueryClassicAccount(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_ACCOUNTS, queryParamMap);
}

httplib::Result BitcomRestApi::spotQueryClassicTxLogs(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_TRANSACTION_LOGS, queryParamMap);
}

httplib::Result BitcomRestApi::spotQueryOrders(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_ORDERS, queryParamMap);
}

httplib::Result BitcomRestApi::spotQueryOpenOrders(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_OPENORDERS, queryParamMap);
}

httplib::Result BitcomRestApi::spotQueryUserTrades(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_USER_TRADES, queryParamMap);
}

httplib::Result BitcomRestApi::spotNewOrder(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::spotAmendOrder(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_AMEND_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::spotCancelOrder(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_CANCEL_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::spotNewBatchNewOrders(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_BATCH_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::spotAmendBatchNewOrders(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_AMEND_BATCH_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::spotEnableCod(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_ACCOUNT_CONFIGS_COD, jsonBody);
}

httplib::Result BitcomRestApi::spotQueryMmpState(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_SPOT_MMP_STATE, queryParamMap);
}

httplib::Result BitcomRestApi::spotUpdateMmpConfig(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_MMP_UPDATE_CONFIG, jsonBody);
}

httplib::Result BitcomRestApi::spotResetMmpConfig(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_SPOT_RESET_MMP, jsonBody);
}

////////////////////////////////////////////////////
// LINEAR
////////////////////////////////////////////////////

httplib::Result BitcomRestApi::linearQueryAccountConfigs(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_LINEAR_ACCOUNT_CONFIGS, queryParamMap);
}

httplib::Result BitcomRestApi::linearQueryOrders(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_LINEAR_ORDERS, queryParamMap);
}

httplib::Result BitcomRestApi::linearQueryOpenOrders(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_LINEAR_OPENORDERS, queryParamMap);
}

httplib::Result BitcomRestApi::linearQueryUserTrades(const unordered_map<string, string> &queryParamMap)
{
    return this->doPrivateQuery(HTTP_GET, V1_LINEAR_USER_TRADES, queryParamMap);
}

httplib::Result BitcomRestApi::linearNewOrder(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_LINEAR_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::linearAmendOrder(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_LINEAR_AMEND_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::linearCancelOrder(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_LINEAR_CANCEL_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::linearNewBatchOrders(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_LINEAR_BATCH_ORDERS, jsonBody);
}

httplib::Result BitcomRestApi::linearAmendBatchOrders(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_LINEAR_AMEND_BATCH_ORDERS,
                                           jsonBody);
}

httplib::Result BitcomRestApi::linearQueryMmpState(const nlohmann::json &jsonBody)
{
    return this->doPrivateQuery(HTTP_GET, V1_LINEAR_MMP_STATE, jsonBody);
}

httplib::Result BitcomRestApi::linearUpdateMmpConfig(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_LINEAR_MMP_UPDATE_CONFIG, jsonBody);
}

httplib::Result BitcomRestApi::linearResetMmpConfig(const nlohmann::json &jsonBody)
{
    return this->doPrivateCallWithJsonBody(HTTP_POST, V1_LINEAR_RESET_MMP, jsonBody);
}
