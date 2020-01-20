#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main() {
  stringstream messageStream;
  int matrixSize = 0, port;
  char ip[16];

  // wczytujemy wartości stałe
  cin >> ip;
  cin >> port;
  cin >> matrixSize;
  messageStream << matrixSize;

  int matrixFieldAmount = matrixSize * matrixSize * 2;
  double field;

  // wczytujemy macierz
  for (int x = 0; x < matrixFieldAmount; x++) {
    cin >> field;
    if (x % matrixSize == 0 || x == 0) {
      messageStream << endl << field;
    } else {
      messageStream << " " << field;
    }
  }
  messageStream << "#";

  WSADATA WSAData;
  SOCKET server;
  SOCKADDR_IN addr;

  // tworzymy socket
  WSAStartup(MAKEWORD(2, 0), &WSAData);
  server = socket(AF_INET, SOCK_STREAM, 0);

  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  // rozpoczynamy połączenie
  connect(server, (SOCKADDR *)&addr, sizeof(addr));

  cerr << "sending..." << endl;

  // wysyłamy wielkość macierzy i dwie macierze do pomnożenia
  string bufferString = messageStream.str();
  char buffer[2000000];
  strcpy(buffer, bufferString.c_str());
  send(server, buffer, sizeof(buffer), 0);

  cerr << "receiving..." << endl;

  // odbieramy macierz odpowiedzi
  stringstream responseStream;
  recv(server, buffer, sizeof(buffer), 0);
  responseStream << buffer;

  // zamykamy połączenie
  closesocket(server);
  WSACleanup();

  // wyświetlamy wynik
  for (int x = 0; x < matrixSize * matrixSize; x++) {
    responseStream >> field;
    cout << field << endl;
  }

  return 0;
}
