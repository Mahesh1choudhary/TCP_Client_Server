#include "common_libraries.h"
#include "thread_safe_queue.h"
#include "connection.h"

template<typename T> class client{

    public:

        client():client_socket(client_io_context){

        }

        virtual ~client(){
            disconnect();
        }

        bool connect(const std::string& host, const unint16_t port){
            try{
                client_connection = std::make_unique<connection<T>>();
                asio::ip::tcp::resolver resolver(client_io_context);
                client_endpoint = resolver.resolve(host, std::to_string(port));

                client_connetion->connect_to_server();

            }
            catch(std::exception& e){
                std::cerr << "client exception : "<<e.what()<<endl;
            }
        }

        void disconnect(){
            if(is_connected()){
                client_connection -> disconnect();
            }
            client_io_context.stop();
            if(thread_io_context.joinable()) thread_io_context.join();
            client_connection.release();
        }

        bool is_connected(){
            if(client_connection){
                return client_connection->is_connected();
            }
        }


    protected:
        asio::io_context client_io_context;
        // thread to run the io_context
        std::thread thread_io_context;

        asio::ip::tcp::socket client_socket;

        std::unique_ptr<connection<T>> client_connection;


    private:
        // will store incoming messages
        thread_safe_queue<message<T>> incoming_messages_queue;


};

int main(){
    
    
    

    return 0;
}