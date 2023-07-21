#include "bitcom_ws.h"

context_ptr on_tls_init(websocketpp::connection_hdl)
{
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try
    {
        // don't verify SSL server
        ctx->set_verify_mode(boost::asio::ssl::verify_none);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    return ctx;
}

int run_ws_app(string wsHost, std::function<void(client *, websocketpp::connection_hdl, client::message_ptr)> onWsMsg,
               std::function<void(client *, websocketpp::connection_hdl)> onWsOpen)
{
    client c;

    try
    {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        c.set_error_channels(websocketpp::log::elevel::all);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(bind(onWsMsg, &c, ::_1, ::_2));
        c.set_open_handler(bind(onWsOpen, &c, ::_1));
        c.set_tls_init_handler(&on_tls_init);

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(wsHost, ec);
        if (ec)
        {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();

        return 0;
    }
    catch (websocketpp::exception const &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        return -1;
    }
}