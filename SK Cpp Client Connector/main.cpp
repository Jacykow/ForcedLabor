#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    int matrixSize=0;
    //cin>>matrixSize;
    double matrixA[matrixSize][matrixSize];
    double matrixB[matrixSize][matrixSize];
    double matrixC[matrixSize][matrixSize];
    for(int y=0;y<matrixSize;y++){
        for(int x=0;x<matrixSize;x++){
            cin>>matrixA[y][x];
        }
    }
    for(int y=0;y<matrixSize;y++){
        for(int x=0;x<matrixSize;x++){
            cin>>matrixB[y][x];
        }
    }



    for(int y=0;y<matrixSize;y++){
        for(int x=0;x<matrixSize;x++){
            matrixC[y][x]=matrixA[y][x]+matrixB[y][x];
        }
    }

    cout << "Start" << endl;

    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;

    WSAStartup(MAKEWORD(2,0), &WSAData);
    server = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_addr.s_addr = inet_addr("192.168.0.26"); // replace the ip with your futur server ip address. If server AND client are running on the same computer, you can use the local ip 127.0.0.1
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);

    connect(server, (SOCKADDR *)&addr, sizeof(addr));

    char buffer[1024]="kaugorzata morda";
    send(server, buffer, sizeof(buffer), 0);
    cout << "Message sent!" << endl;

    recv(server, buffer, sizeof(buffer), 0);
    cout << "response: " << buffer << endl;

    closesocket(server);
    WSACleanup();
    cout << "Socket closed." << endl << endl;



    for(int y=0;y<matrixSize;y++){
        for(int x=0;x<matrixSize;x++){
            if(x != 0)cout<<" ";
            cout<<matrixC[y][x];
        }
        cout<<endl;
    }

    return 0;
}
