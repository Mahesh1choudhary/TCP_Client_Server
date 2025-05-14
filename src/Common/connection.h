#include "thread_safe_queue.h"
#include "messages.h"
#include "common_libraries.h"

template<typename T> class connection : public {
    public:
        connection(){}
        virtual ~connection(){}


        // connect_to_server will only be called by clients
        bool connect_to_server();

        bool disconnect();

        bool is_connected();

        bool send(const message<T>& msg);


    protected:
        // each connection will have its own socket
        asio::ip::tcp::socket connection_socket;

        // will be provided in constructor- shared with the whole asio instance
        asio::io_context& connection_io_context;

        // holds all messages that are to be sent
        thread_safe_queue<message<T>> outgoing_messages_queue;


        // will be provided. it reference to server/client queue
        thread_safe_queue<owned_message<T>> incoming_messages_queue;



        

};