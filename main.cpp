#include <iostream>
#include <chrono>
#include <thread>
#include "utils.h"
#include "json.hpp"
#include "bitcom_rest.h"
#include "bitcom_ws.h"

using namespace std;

void testRetApi(const std::string &apiHost, const std::string &ak, const std::string &sk)
{
    BitcomRestApi bitRestClient(apiHost, ak, sk);

    // bitRestClient.queryUmAccount({});
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    bitRestClient.linearNewBatchNewOrders(
        {{"currency", "USD"},
         {"orders_data", {
            {{"instrument_id", "BTC-USD-PERPETUAL"}, {"price", "20000"}, {"qty", "1.2"}, {"side", "buy"}}, 
            {{"instrument_id", "ETH-USD-PERPETUAL"}, {"price", "1800"}, {"qty", "23"}, {"side", "buy"},}
        }}});

    std::this_thread::sleep_for(std::chrono::seconds(1));

    bitRestClient.linearNewOrder({
        {"instrument_id", "ETH-USD-PERPETUAL"},
        {"qty", "1.2"},
        {"side", "buy"},
        {"order_type", "market"},
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    bitRestClient.linearQueryOrders({{"currency", "USD"},
                                     {"instrument_id", "ETH-USD-PERPETUAL"},
                                     {"limit", "1"}});
}

void testPublicWs(const std::string & wsHost)
{
    run_ws_app(wsHost,
        [](client *c, websocketpp::connection_hdl, client::message_ptr msg)
        { std::cout << "on_message: " << msg->get_payload() << std::endl; },
        [](client *c, websocketpp::connection_hdl hdl)
        {
            nlohmann::json js;
            js["type"] = "subscribe";
            js["instruments"] = {"BTC-USD-PERPETUAL"};
            js["channels"] = {"depth1"};
            js["interval"] = "raw";
            auto req = js.dump();
            std::cout << "on_open, ws request: " << req <<  std::endl;            
            c->send(hdl, req, websocketpp::frame::opcode::text);
        });
}

void testPrivateWs(const std::string & wsHost, const std::string &restApiHost, const std::string &ak, const std::string &sk)
{
    BitcomRestApi bitRestClient(restApiHost, ak, sk);
    auto ret = bitRestClient.wsAuth({});
    auto authRet = nlohmann::json::parse(ret->body);
    cout << authRet.dump() << endl;
    auto token = authRet["data"]["token"];

    run_ws_app(
        wsHost,
        [](client *c, websocketpp::connection_hdl, client::message_ptr msg)
        { std::cout << "on_message: " << msg->get_payload() << std::endl; },
        [=](client *c, websocketpp::connection_hdl hdl)
        {
            nlohmann::json js;
            js["type"] = "subscribe";
            js["channels"] = {"um_account"};
            js["interval"] = "100ms";
            js["token"] = token;
            auto req = js.dump();
            std::cout << "on_open, ws request: " << req <<  std::endl;
            c->send(hdl, req, websocketpp::frame::opcode::text);
        });
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
        std::string val = getenv(x);
        if (val.empty()) {
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