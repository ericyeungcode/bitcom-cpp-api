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

int main()
{
    std::string home_dir = getenv("HOME");
    cout << "home: " << home_dir << "\n";
    std::ifstream f(home_dir + "/cpp_twap.json");
    nlohmann::json jsonCfg = nlohmann::json::parse(f);
    string apiHost = jsonCfg["restAddr"];
    string ak = jsonCfg["accessKey"];
    string sk = jsonCfg["secretKey"];

    // testRetApi(apiHost, ak, sk);

    // testPublicWs("wss://betaws.bitexch.dev");

    testPrivateWs("wss://betaws.bitexch.dev", apiHost, ak, sk);

    return 0;
}