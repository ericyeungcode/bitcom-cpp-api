#ifndef BITCOM_WS_H
#define BITCOM_WS_H

#include <functional>
#include <iostream>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

using namespace std;

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

context_ptr on_tls_init(websocketpp::connection_hdl);
int run_ws_app(string wsHost, std::function<void(client *, websocketpp::connection_hdl, client::message_ptr)>,
               std::function<void(client *, websocketpp::connection_hdl)>);

#endif // BITCOM_WS_H