#include "common_libraries.h"
#include "connection.h"

template<typename T> struct message_header{
    T id{};
    uint32_t size =0;
};

template<typename T> struct message{
    message_header<T> header{};
    std::vector<uint8_t> body;

    // total size of entire message packet in bytes - same as size member of message_header
    size_t size() const{
        return sizeof(message_header<T>) + body.size();
    }

    // overloading << for compatibility
    friend std::ostream& operator << (std::ostream& os, const message<T>& msg){
        os << "ID: "<< int(msg.header.id) << ", Size: "<< msg.header.size();
        return os;
    }


    // overloading << to put data in message
    template<typename dataType> friend message<T>& operator << (message<T>& msg, const dataType& data){
        // checking if the dataType is trivially copyable
        static_assert(std::is_standard_layout<dataType>::value, "Data is too complex to be pushed");

        size_t location = msg.body.size();  // start location in vector where we will push the data
        
        // adding data
        msg.body.resize(location + sizeof(dataType));
        memcpy(msg.body.data()+location, &data, sizeof(dataType));

        msg.header.size = msg.size();
        return msg;
    }

    // overloading >> to publish/remove data from message - notice we are publishin like a stack- last one first
    template<typename dataType> friend message<T>& operator >> (message<T>& msg, dataType& data){
        // checking if the dataType is trivially copyable
        static_assert(std::is_standard_layout<dataType>::value, "Data is too complex to be pushed");

        if(msg.body.size() < sizeof(dataType)){
            throw std::runtime_error("Not enough data in message body to extract the specified type");
        }

        size_t location = msg.body.size() - sizeof(dataType);  // location where the data to be published start

        // removing data from the end
        memcpy(&data, msg.body.data()+location, sizeof(dataType));
        msg.body.resize(location);

        msg.header.size = msg.size();
        return msg;
    }
};



// add/remove as per use case
enum class custom_msg_types: uint32_t{
    server_message,
    client_message
};


template<typename T> class connection;
template<typename T> struct owned_message{
    std::shared_ptr<connection> remote = nullptr;
    message<T> msg;

    friend std::ostream& operator << (std::ostream& os, const owned_message<T>& owned_msg){
        os << owned_message.msg;
        return os;
    }
}