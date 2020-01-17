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
    messageStream<<matrixSize;

    int matrixFieldAmount = matrixSize * matrixSize * 2;
    double field;

    for(int x=0;x<matrixFieldAmount;x++){
        cin>>field;
        if(x%matrixSize == 0 || x == 0){
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

    cerr<<"sending..."<<endl;

    string bufferString = messageStream.str();
    char buffer[1000000];
    strcpy(buffer, bufferString.c_str());
    send(server, buffer, sizeof(buffer), 0);

    cerr<<"receiving..."<<endl;

    char buffer2[1000000];
    stringstream responseStream;
    recv(server, buffer2, sizeof(buffer), 0);
    responseStream<<buffer2;

    closesocket(server);
    WSACleanup();

    for(int x=0;x<matrixSize*matrixSize;x++){
        responseStream>>field;
        if(x == 0){
            cout<<field;
        }else if(x%matrixSize==0){
            cout<<endl<<field;
        }else{
            cout<<" "<<field;
        }
    }
    cout<<endl;

    return 0;
}
