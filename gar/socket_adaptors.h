//
// Created by dc on 6/3/16.
//

#ifndef GAR_SOCKET_ADAPTORS_H
#define GAR_SOCKET_ADAPTORS_H

#include <boost/asio.hpp>
#ifdef GAR_OPENSSL_ENABLED
#include <boost/asio/ssl.hpp>
#endif


namespace gar {

    using boost::asio::ip::tcp;
    typedef tcp::endpoint   SocketEp;
    typedef boost::system::error_code   Boost_Err;

    class SocketAdaptor {
    public:
        using Context = void;
        typedef tcp::socket RawSocket;
        typedef tcp::socket Socket;

        SocketAdaptor(std::shared_ptr<boost::asio::io_service> ioService, Context*)
                : socket_(*ioService),
                  ioService_(ioService)
        {
        }

        SocketAdaptor::RawSocket&    rawSocket() {
            return socket_;
        }

        SocketAdaptor::Socket&        socket() {
            return socket_;
        }

        SocketEp   remoteEndpoint() {
            return socket_.remote_endpoint();
        }

        bool isOpen() {
            return socket_.is_open();
        }

        bool close() {
            socket_.close();
        }

        template <typename Start>
        void start(Start cb) {
            cb(Boost_Err());
        }

    private:
        SocketAdaptor::Socket   socket_;
        /*
         * @brief This is only here to keep a reference to the service
         * for the lifetime of this service
         * */
        std::shared_ptr<boost::asio::io_service> ioService_;
    };

#ifdef GAR_OPENSSL_ENABLED
    class SslSocketAdaptor {
    public:
        using Context = boost::asio::ssl::context;
        typedef boost::asio::ssl::stream<tcp::socket>   Socket;
        typedef tcp::socket::lowest_layer_type          RawSocket;

        SslSocketAdaptor(std::shared_ptr<boost::asio::io_service> ioService, Context* context)
                : socket_(*ioService, *context),
                  ioService_(ioService)
        {
        }

        SslSocketAdaptor::RawSocket&    rawSocket() {
            return socket_.lowest_layer();
        }

        SslSocketAdaptor::Socket&        socket() {
            return socket_;
        }

        SocketEp   remoteEndpoint() {
            return rawSocket().remote_endpoint();
        }

        bool isOpen() {
            return rawSocket().is_open();
        }

        bool close() {
            rawSocket().close();
        }

        template <typename Start>
        void start(Start cb) {
            socket_.async_handshake(boost::asio::ssl::stream_base::server,
            [cb](const Boost_Err& err) {
                cb(err);
            });
        }

    private:
        SslSocketAdaptor::Socket        socket_;
        /*
         * @brief This is only here to keep a reference to the service
         * for the lifetime of this service
         * */
        std::shared_ptr<boost::asio::io_service> ioService_;
    };
#endif

}
#endif //GAR_SOCKET_INTERFACE_H
