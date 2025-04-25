#include <iostream>
#include <chrono>
#include <thread>
#include "utils.h"
#include "json.hpp"
#include "bitcom_rest.h"
#include "BoostWsClient.hpp"

using namespace std;

std::string trimWssPrefix(const std::string& url) {
    const std::string prefix = "wss://";
    if (url.rfind(prefix, 0) == 0) { // rfind returns 0 if prefix is at the beginning
        return url.substr(prefix.length());
    }
    return url;
}

void testRetApi(const std::string &apiHost, const std::string &ak, const std::string &sk)
{
    BitcomRestApi bitRestClient(apiHost, ak, sk);

    // bitRestClient.queryUmAccount({});
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    bitRestClient.linearNewBatchOrders(
        {{"currency", "USDT"},
         {"orders_data", {
            {{"instrument_id", "BTC-USDT-PERPETUAL"}, {"price", "20000"}, {"qty", "1.2"}, {"side", "buy"}}, 
            {{"instrument_id", "ETH-USDT-PERPETUAL"}, {"price", "1800"}, {"qty", "23"}, {"side", "buy"},}
        }}});

    std::this_thread::sleep_for(std::chrono::seconds(1));

    bitRestClient.linearNewOrder({
        {"instrument_id", "ETH-USDT-PERPETUAL"},
        {"qty", "1.2"},
        {"side", "buy"},
        {"order_type", "market"},
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    bitRestClient.linearQueryOrders({{"currency", "USDT"},
                                     {"instrument_id", "ETH-USDT-PERPETUAL"},
                                     {"limit", "1"}});
}


void testPublicWs(const std::string & wsHost)
{
    net::io_context ioc;
    ssl::context ssl_ctx(ssl::context::tlsv12_client);

    // Skip cert verification (demo only!)
    ssl_ctx.set_verify_mode(ssl::verify_none);
    auto client = std::make_shared<BoostWsClient>(ioc, ssl_ctx);
    client->on_open = [&]() {
        std::cout << "Ws Connected: " << wsHost << "\n";

        nlohmann::json js;
        js["type"] = "subscribe";
        js["instruments"] = {"BTC-USDT-PERPETUAL"};
        js["channels"] = {"depth1"};
        js["interval"] = "raw";
        auto req = js.dump();
        std::cout << "on_open, ws request: " << req <<  std::endl;
        client->send(req);
    };

    client->on_message = [](const std::string& msg) {
        std::cout << "Ws Received: " << msg << std::endl;
    };

    client->on_close = []() {
        std::cout << "Ws Connection closed.\n";
    };

    client->on_error = [](const beast::error_code& ec) {
        std::cerr << "Ws Error: " << ec.message() << std::endl;
    };

    auto noPrefixHost = trimWssPrefix(wsHost);
    std::cout << "Connecting " << noPrefixHost << "\n";
    client->connect(noPrefixHost, "443", "/");

    ioc.run();
}

void testPrivateWs(const std::string & wsHost, const std::string &restApiHost, const std::string &ak, const std::string &sk)
{
    BitcomRestApi bitRestClient(restApiHost, ak, sk);
    auto ret = bitRestClient.wsAuth({});
    auto authRet = nlohmann::json::parse(ret->body);
    cout << authRet.dump() << endl;
    auto token = authRet["data"]["token"];


    net::io_context ioc;
    ssl::context ssl_ctx(ssl::context::tlsv12_client);

    // Skip cert verification (demo only!)
    ssl_ctx.set_verify_mode(ssl::verify_none);
    auto client = std::make_shared<BoostWsClient>(ioc, ssl_ctx);
    client->on_open = [&]() {
        std::cout << "Ws Connected: " << wsHost << "\n";

        nlohmann::json js;
        js["type"] = "subscribe";
        js["channels"] = {"um_account"};
        js["interval"] = "100ms";
        js["token"] = token;
        auto req = js.dump();
        std::cout << "on_open, ws request: " << req <<  std::endl;
        client->send(req);
    };

    client->on_message = [](const std::string& msg) {
        std::cout << "Ws Received: " << msg << std::endl;
    };

    client->on_close = []() {
        std::cout << "Ws Connection closed.\n";
    };

    client->on_error = [](const beast::error_code& ec) {
        std::cerr << "Ws Error: " << ec.message() << std::endl;
    };

    auto noPrefixHost = trimWssPrefix(wsHost);
    std::cout << "Connecting " << noPrefixHost << "\n";
    client->connect(noPrefixHost, "443", "/");

    ioc.run();
}


int main(int argc, char **argv)
{
    string mode;
    if (argc != 2) {
        cout << "Invalid argument: input mode[rest/public-ws/private-ws]" << endl;
        return -1;
    }
    mode = string(argv[1]);

    map<string,string> varMap;
    for(const auto &x: {"BITCOM_REST_HOST", "BITCOM_WS_HOST", "BITCOM_AK", "BITCOM_SK"}) {
        auto val = getenv(x);
        if (!val) {
            cout << "Failed to lookup environment variable: " << x << endl;
            return -1;
        }
        varMap[x] = val;
    }

    if (mode == "rest") {
        testRetApi(varMap["BITCOM_REST_HOST"], varMap["BITCOM_AK"], varMap["BITCOM_SK"]);
    } else if (mode == "public-ws") {
        testPublicWs(varMap["BITCOM_WS_HOST"]); 
    } else if (mode == "private-ws") {
        testPrivateWs(varMap["BITCOM_WS_HOST"], varMap["BITCOM_REST_HOST"], varMap["BITCOM_AK"], varMap["BITCOM_SK"]);
    } else {
        cout << "Invalid mode " << mode << endl;
        return -1;
    }

    return 0;
}