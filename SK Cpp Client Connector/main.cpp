#include <iostream>
#include <winsock2.h>
#include <sstream>
#include <string>

using namespace std;

int main()
{
    stringstream messageStream;
    int matrixSize=0;
    cin>>matrixSize;
    messageStream<<endl<<matrixSize;

    int matrixFieldAmount = matrixSize * matrixSize * 2;
    double field;

    for(int x=0;x<matrixFieldAmount;x++){
        cin>>field;
        if(x*2 == matrixSize || x == 0){
            messageStream<<endl<<field;
        }else{
            messageStream<<" "<<field;
        }
    }

    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;

    WSAStartup(MAKEWORD(2,0), &WSAData);
    server = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_addr.s_addr = inet_addr("192.168.0.26");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);

    connect(server, (SOCKADDR *)&addr, sizeof(addr));

    string bufferString = messageStream.str();
    char buffer[1000000];
    strcpy(buffer, bufferString.c_str());
    send(server, buffer, sizeof(buffer), 0);

    cerr<<"waiting"<<endl;

    stringstream responseStream;
    recv(server, buffer, sizeof(buffer), 0);

    closesocket(server);
    WSACleanup();

    cerr<<"received"<<endl;

    for(int x=0;!responseStream.eof();x++){
        responseStream>>field;
        cerr<<x<<endl;
        if(x == 0){
            cout<<field;
        }else if(x%matrixSize==0){
            cout<<endl<<field;
        }else{
            cout<<" "<<field;
        }
    }

    return 0;
}
