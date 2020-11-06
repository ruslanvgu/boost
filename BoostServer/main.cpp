#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/exception/all.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <list>
#include <iostream>
#include <string>
#include <exception>
#include <algorithm>
#include <list>


using namespace boost::asio;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
using std::cout;
using std::endl;
using std::cin;
using std::cerr;

enum class TYPEMSG
{
    HELLO = 1
};

class Clients
{
   std::list<int> listClient;
public:
        int genClientId(){
        int size=3;
        int beg=1000;
        static int value=beg;

        if(listClient.size() == size) return 0;
        do
        {
            if(++value > beg+size) value = beg + 1;
            if(value != *find(listClient.begin(),listClient.end(),value)){
                listClient.push_back(value);
                return value;
            }
        }while(1);
    }

    int countClients(){return listClient.size();}

    void removeClient(int id)
    {
        listClient.remove(id);
    }
};

Clients client;

void  session(socket_ptr sock, int count)
{
    try
    {
        char data[1024];
        boost::system::error_code error;
        sock->read_some(boost::asio::buffer(data), error);
        if (error)
            throw boost::system::system_error(error);

        if(stoi(std::string(data)) == (int)TYPEMSG::HELLO)
            write(*sock, boost::asio::buffer(std::to_string(count)));

        memset(data, 0, sizeof(data));

        for (;;)
        {  if(!sock->is_open()){
                sock->close();
                client.removeClient(count);
                break;
            }
            sock->read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            cout << boost::this_thread::get_id()<< " Text: " << data << endl;
            memset(data, 0, sizeof(data));
        }
    }
    catch (boost::system::system_error& e)
    {
        std::cerr << "Exception in thread "<<boost::this_thread::get_id()<<": " << e.what() << "\n";
    }
}

int main(int argc, char** argv)
{
    std::cout << "SERVER" << std::endl;

    typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
    io_service service;
    boost::system::error_code ec;
    ip::tcp::endpoint ep( ip::tcp::v4(), 2009); // listen on 2001

    ip::tcp::acceptor acc(service, ep);

    while (true)
    {
        socket_ptr sock(new ip::tcp::socket(service));
        acc.accept(*sock, ec);
        if(!ec){
            int idClient = client.genClientId();
            if(idClient)
                boost::thread( boost::bind(session, sock,boost::ref(idClient)));
            else
                cerr <<"ERR: Number of connections exceeded"<< endl;
        }
        else
            cerr <<"ACCEPT ERR: "<<ec.message()<< endl;
    }

    return 0;
}

