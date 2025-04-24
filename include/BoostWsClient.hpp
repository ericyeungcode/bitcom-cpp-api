#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

class BoostWsClient : public std::enable_shared_from_this<BoostWsClient> {
public:
    using OnOpen    = std::function<void()>;
    using OnMessage = std::function<void(const std::string&)>;
    using OnClose   = std::function<void()>;
    using OnError   = std::function<void(const beast::error_code&)>;

    BoostWsClient(net::io_context& ioc, ssl::context& ssl_ctx)
        : resolver_(net::make_strand(ioc)),
          ws_(net::make_strand(ioc), ssl_ctx) {}

    void connect(const std::string& host, const std::string& port, const std::string& target = "/") {
        host_ = host;
        target_ = target;

        if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host.c_str()))
        {
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            throw boost::system::system_error{ec};
        }

        resolver_.async_resolve(host, port,
            beast::bind_front_handler(&BoostWsClient::on_resolve, shared_from_this()));
    }

    void send(const std::string& message) {
        ws_.async_write(net::buffer(message),
            beast::bind_front_handler(&BoostWsClient::on_write, shared_from_this()));
    }

    void close() {
        ws_.async_close(websocket::close_code::normal,
            beast::bind_front_handler(&BoostWsClient::on_close_handler, shared_from_this()));
    }

    // Callbacks
    OnOpen    on_open;
    OnMessage on_message;
    OnClose   on_close;
    OnError   on_error;

private:
    tcp::resolver resolver_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string target_;

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
        if (ec) return fail(ec);
        beast::get_lowest_layer(ws_).async_connect(results,
            beast::bind_front_handler(&BoostWsClient::on_connect, shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
        if (ec) return fail(ec);
        ws_.next_layer().async_handshake(ssl::stream_base::client,
            beast::bind_front_handler(&BoostWsClient::on_ssl_handshake, shared_from_this()));
    }

    void on_ssl_handshake(beast::error_code ec) {
        if (ec) return fail(ec);
        ws_.async_handshake(host_, target_,
            beast::bind_front_handler(&BoostWsClient::on_handshake, shared_from_this()));
    }

    void on_handshake(beast::error_code ec) {
        if (ec) return fail(ec);
        if (on_open) on_open();
        do_read();
    }

    void do_read() {
        ws_.async_read(buffer_,
            beast::bind_front_handler(&BoostWsClient::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        if (ec) return fail(ec);
        std::string message = beast::buffers_to_string(buffer_.data());
        buffer_.consume(bytes_transferred);
        if (on_message) on_message(message);
        do_read();
    }

    void on_write(beast::error_code ec, std::size_t) {
        if (ec) fail(ec);
    }

    void on_close_handler(beast::error_code ec) {
        if (ec) return fail(ec);
        if (on_close) on_close();
    }

    void fail(beast::error_code ec) {
        if (on_error) on_error(ec);
    }
};
