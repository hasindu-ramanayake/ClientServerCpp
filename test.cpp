#include <iostream>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

using namespace asio;
using namespace std;


std::vector<char> vBuff(20*1024);
void asyncReadSomeData(ip::tcp::socket& sock_ ) {
    sock_.async_read_some( buffer(vBuff.data(), vBuff.size()), 
    [&](error_code ec, size_t length ){
        if (!ec ) {
            cout << "Read: " << length << " bytes\n";
            for ( int i=0; i< length; i++ ) cout << vBuff[i];
            asyncReadSomeData(sock_);
        }
    });
}


int main( ) {

    error_code ec;
    
    io_context dataContext;
    io_context::work dummyWorks(dataContext);
    std::thread threadContext = std::thread( [&]() { dataContext.run(); });
    ip::tcp::endpoint endPoint( ip::make_address("93.184.216.34", ec), 80);     // example.com for testing
    ip::tcp::socket socket(dataContext);
    

    socket.connect(endPoint, ec);

    if ( !ec ) {
        cout << "Connected..!!" << endl;
    } else {
        cout << "Fail to connect to address: " << ec.message() << endl;
    }

    if ( socket.is_open() ) {

        asyncReadSomeData(socket);
        ostringstream ss;
        ss << "GET /index.html HTTP/1.1\r\n";
        ss << "Host: example.com\r\n";
        ss << "Connection: close\r\n\r\n";
        string sRes = ss.str();
        
        socket.write_some( buffer(sRes.data(), sRes.size()), ec);

        // socket.wait(socket.wait_read);

        // size_t dataBytes = socket.available();
        // cout << " Data Available: " << dataBytes << endl;

        // if ( dataBytes > 0 ) {
        //     vector<char> vbuff(dataBytes);
        //     socket.read_some( buffer(vbuff.data(), vbuff.size()), ec);

        //     for ( auto data : vbuff) {
        //         cout << data;
        //     }

        // }
        sleep(20000); // to get many data as possible

        dataContext.stop();
        if ( threadContext.joinable() ) threadContext.join();

    }

    system("pause");
    return 0;
}