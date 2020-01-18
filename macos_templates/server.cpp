
#include <netinet/in.h>  // For sockaddr_in
#include <omp.h>
#include <string.h>
#include <sys/socket.h>  // For socket functions
#include <sys/time.h>
#include <unistd.h>  // For read
#include <cstdlib>   // For exit() and EXIT_FAILURE
#include <iostream>  // For cout
#include <sstream>
#include <vector>

using namespace std;

typedef vector<vector<double>> vvd;
typedef vector<double> vd;

// Zwykłe mnożenie macierzy
vvd matrix_multiplication(vvd &a, vvd &b) {
  int n = a.size();
  vvd res(n, vd(n, 0));
  for (int j = 0; j < (int)a.size(); ++j)
    for (int i = 0; i < (int)a.size(); ++i)
      for (int k = 0; k < (int)a.size(); ++k) {
        res[i][j] += a[i][k] * b[k][j];
      }

  return res;
}

// Wielowątkowe mnożenie macierzy
vvd threaded_matrix_multiplication(vvd &a, vvd &b) {
  int n = a.size();
  vvd res(n, vd(n, 0));
  int i, j, k;
  omp_set_num_threads(omp_get_num_procs());
#pragma omp parallel for private(i, j, k) shared(a, b, res, n)
  for (i = 0; i < n; ++i)
    for (j = 0; j < n; ++j)
      for (k = 0; k < n; ++k) {
        res[i][j] += a[i][k] * b[k][j];
      }
  return res;
}

// Wyciąganie macierzy z buffera
vvd calculate_from_string(string strbuff) {
  cout << "Message size: " << strbuff.length() << endl;
  stringstream ss;
  double tmp;
  int matrix_size;
  ss << strbuff;
  ss >> matrix_size;
  cout << "matrix size value: " << matrix_size << endl;

  vvd a, b, res(matrix_size, vd(matrix_size, 0));
  for (int j = 0; j < matrix_size; j++) {
    a.push_back(vd());
    for (int i = 0; i < matrix_size; i++) {
      ss >> tmp;
      a[j].push_back(tmp);
    }
  }
  for (int j = 0; j < matrix_size; j++) {
    b.push_back(vd());
    for (int i = 0; i < matrix_size; i++) {
      ss >> tmp;
      b[j].push_back(tmp);
    }
  }

  res = threaded_matrix_multiplication(a, b);
  return res;
}

// Funkcja wypisująca wartość wektora, tylko do debugu
string debug_vector(vvd vec) {
  string s = "";
  for (int j = 0; j < (int)vec.size(); j++) {
    for (int i = 0; i < (int)vec.size(); i++) {
      s += to_string(vec[i][j]) + " ";
    }
    s += "\n";
  }
  return s;
}

int main() {
  // Tworzymy socket (IPv4, TCP)
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    cout << "Problem z socketem" << errno << endl;
    exit(1);
  }

  // Proba zajęcia portu 8080
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(8080);
  if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    cout << "Nie udało się dostać do portu 8080: " << errno << endl;
    exit(1);
  }
  cout << "Dostałem się do portu 8080." << endl;
  while (true) {
    // słuchamy, najwyżej 100 połączeń na raz
    if (listen(sockfd, 100) < 0) {
      cout << "Nie udało się zacząć słuchać " << errno << endl;
      exit(1);
    }
    cout << "Słucham..." << endl;

    // Wyciągamy połączenie z kolejki
    auto addrlen = sizeof(sockaddr);
    int connection =
        accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
    if (connection < 0) {
      cout << "Nie udało się wyciągnąć połączenia " << errno << endl;
      exit(1);
    }
    cout << "Mamy połączenie." << endl;

    if (fork() == 0) {
      // Czytamy
      char buffer[1000000];
      auto bytesRead = read(connection, buffer, 1000000);
      if ((int)bytesRead == 0) {
        cout << "Nie ma nic w środku" << endl;
      }
      // cout << buffer << endl;

      cout << "Liczę..." << endl;
      string strbuff = buffer;
      vvd res = calculate_from_string(strbuff);

      string response = "";
      for (int j = 0; j < (int)res.size(); j++) {
        for (int i = 0; i < (int)res.size(); i++) {
          response += to_string(res[i][j]) + " ";
        }
      }

      cout << "Policzone! Odsyłam odpowiedź..." << endl;
      cout << response << endl;
      send(connection, response.c_str(), response.size() + 1, 0);
      cout << "Odpowiedź odesłana." << endl;

      close(connection);
      exit(0);
    } else {
      close(connection);
    }
  }

  close(sockfd);
}
