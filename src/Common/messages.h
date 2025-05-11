#include "common_libraries.h"

template<typename T> struct message_header{
    T id{};
    uint32_t size =0;
};


template<typename T> struct message{
    message_header<T> header{};
    std::vector<uint8_t> body;

    // total size of entire message packet in bytes - different from size member of message_header
    size_t size() const{
        return sizeof(message_header<T>) + body.size();
    }

    // overloading << afor compatibility
    friend std::osream& operator << (std::ostream& os, const message<T>& msg){
        os << "ID: "<< int(msg.header.id) << ", Size: "<< msg.header.size();
        return os;
    }


    // overloading << to output the message stored in body vector in some fixed order
    template<typename dataType> friend message<T>& operator << (message<T>& msg, const dataType& data){

    }


};