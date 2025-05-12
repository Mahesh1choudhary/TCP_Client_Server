#include "messages.h"

void test_push_pop_data(){
    message<custom_msg_types> msg;
    msg.header.id= custom_msg_types::client_message;
    
    int in_a= 10; float in_b= 3.14f;
    struct st{
        int c=5;
        double d=6;
    } in_s;
    msg << in_a << in_b << in_s;
    assert(msg.size() == sizeof(msg.header)+ sizeof(int) + sizeof(float) + sizeof(st));
    assert(msg.header.size == msg.size());

    int out_a; float out_b; st out_s;
    msg >> out_s >> out_b >> out_a;
    assert(in_a == out_a);
    assert(in_b == out_b);
    assert(in_s.c == out_s.c);
    assert(in_s.d == out_s.d);
}

void test_underflow_error(){
    message<custom_msg_types> msg;
    try{
        int val;
        msg >> val;

    } catch (const std::runtime_error& e){
        assert(std::string(e.what()) == "Not enough data in message body to extract the specified type");
    }
    
}

int main(){
    test_push_pop_data();
    test_underflow_error();
    std::cout << "All tests passed!" << std::endl;

    return 0;
}