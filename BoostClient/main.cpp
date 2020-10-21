#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <boost/asio/io_service.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;
using std::cout;
using std::endl;
using std::cin;
using std::cerr;

enum class TYPEMSG
{
    HELLO = 1
};

bool start_message(tcp::socket &socket, boost::system::error_code &err, tcp::endpoint &endpoint )
{
    try{
        char data[1024];
        socket.connect(endpoint,err);
        if(err == boost::system::errc::errc_t::success)
        cout << "CONENCT TO SERVER: " << endl;

        int type = static_cast<int>(TYPEMSG::HELLO);
        write(socket, buffer(std::to_string(type)), err);
        memset(data, 0, sizeof(data));
        socket.read_some(boost::asio::buffer(data), err);
        cout << "CLIENT №"<< data << endl;
        memset(data, 0, sizeof(data));
        return 1;

    }
    catch(boost::system::system_error& e)
    {
        std::cerr << "Exception " << ": " << e.what() << "\n";
    }
    return 0;
}

int main()
{
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    boost::system::error_code err;
    tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"),2009);

    if(!start_message(socket, err, endpoint)){
        return 0;
    }

    std::string msg;
    while(true){
        std::getline(std::cin,msg);
        boost::asio::write( socket, boost::asio::buffer(msg));

    }

    return 0;
}
