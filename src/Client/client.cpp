#include <iostream>
#include <asio.hpp>
#include <thread>
#include <cmath>


using asio::ip::tcp;

std::vector<char> vBuffer(1024*10);

void grabSomeData(tcp::socket& socket){
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](asio::error_code ec, std::size_t len){
            if(!ec){
                for(int i=0; i<len; ++i){
                    std::cout<<vBuffer[i];
                }
                grabSomeData(socket);
            }
        });
}

int main(){
    try
    {
        asio::error_code ec;
        asio::io_context io_context;
        auto idlework = asio::make_work_guard(io_context);
        std::thread context_thread = std::thread([&](){ io_context.run();});

        tcp::endpoint endpoint(asio::ip::make_address("142.250.195.78", ec), 80);
        if(ec){
            std::cout<<"could not convert the ip address"<<std::endl;
        }
        tcp::socket socket(io_context);

        socket.connect(endpoint,ec);
        if(ec){
            std::cout<<"socket could not connect to edpoint = "<<ec.message()<<std::endl;
        }

        if(socket.is_open()){
            grabSomeData(socket);
            std::string query =  "what is networking";
            std::string request = "GET /search?q=" + query + " HTTP/1.1\r\n"
                              "Host: www.google.com\r\n"
                              "Connection: close\r\n"
                              "\r\n";
            socket.write_some(asio::buffer(request.data(), request.size()),ec);
            if(ec){
                std::cout<<"could not send the request with error= "<<ec.message()<<std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

        context_thread.join();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    

    return 0;
}