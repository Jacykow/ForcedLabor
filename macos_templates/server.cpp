
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

double multiply_field(int i, int j, vvd &a, vvd &b) {
  double result = 0;
  for (int k = 0; k < (int)a.size(); ++k) {
    result += a[i][k] * b[k][j];
  }
  return result;
}

vd multiply_row(int j, vvd &a, vvd &b) {
  vd res(a.size());
  for (int i = 0; i < (int)a.size(); ++i) {
    res[i] = multiply_field(i, j, a, b);
  }
  return res;
}

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

double eval_time(vvd (*func)(vvd &, vvd &)) {
  vvd g(1000, vd(1000, 1));
  vvd h(1000, vd(1000, 1));
  struct timeval tv1, tv2;
  struct timezone tz;
  double elapsed;
  gettimeofday(&tv1, &tz);
  vvd res = func(g, h);
  gettimeofday(&tv2, &tz);
  elapsed = (double)(tv2.tv_sec - tv1.tv_sec) +
            (double)(tv2.tv_usec - tv1.tv_usec) * 1.e-6;
  return elapsed;
}

int main() {
  // Create a socket (IPv4, TCP)
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    cout << "Failed to create socket. errno: " << errno << endl;
    exit(EXIT_FAILURE);
  }

  // Listen to port 8080 on any address
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(8080);  // htons is necessary to convert a number to
                                    // network byte order
  if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    cout << "Failed to bind to port 8080. errno: " << errno << endl;
    exit(EXIT_FAILURE);
  }
  cout << "Bind successful." << endl;
  while (true) {
    // Start listening. Hold at most 10 connections in the queue
    if (listen(sockfd, 10) < 0) {
      cout << "Failed to listen on socket. errno: " << errno << endl;
      exit(EXIT_FAILURE);
    }
    cout << "Listening..." << endl;

    // Grab a connection from the queue
    auto addrlen = sizeof(sockaddr);
    int connection =
        accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
    if (connection < 0) {
      cout << "Failed to grab connection. errno: " << errno << endl;
      exit(EXIT_FAILURE);
    }
    cout << "Grabbed a connection: " << connection << endl;

    if (fork() == 0) {
      // Read from the connection
      char buffer[1000000];
      auto bytesRead = read(connection, buffer, 1000000);
      if ((int)bytesRead == 0) {
        cout << "No message inside" << endl;
      }
      // cout << "The message was: " << buffer << endl;

      cout << "Counting..." << endl;

      string strbuff = buffer;

      vvd res = calculate_from_string(strbuff);

      string response = "";
      for (int j = 0; j < (int)res.size(); j++) {
        for (int i = 0; i < (int)res.size(); i++) {
          response += to_string(res[i][j]) + " ";
        }
      }

      cout << "Counted! Sending response..." << endl;
      cout << response << endl;
      // Send a message to the connection
      send(connection, response.c_str(), response.size() + 1, 0);

      cout << "Response sent." << endl;
      close(connection);
      exit(0);
    } else {
      close(connection);
    }
  }

  // Close the connections
  close(sockfd);
}
