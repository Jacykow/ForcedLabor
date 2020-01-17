
#include <netinet/in.h>  // For sockaddr_in
#include <string.h>
#include <sys/socket.h>  // For socket functions
#include <unistd.h>      // For read
// #include <boost/interprocess/shared_memory_object.hpp>
#include <cstdlib>   // For exit() and EXIT_FAILURE
#include <iostream>  // For cout
#include <sstream>
#include <vector>

using namespace std;

double multiply_field(int i, int j, vector<vector<double>> &a,
                      vector<vector<double>> &b) {
  double result = 0;
  for (int k = 0; k < (int)a.size(); ++k) {
    result += a[i][k] * b[k][j];
  }
  return result;
}

vector<double> multiply_row(int j, vector<vector<double>> &a,
                            vector<vector<double>> &b) {
  vector<double> res;
  for (int i = 0; i < (int)a.size(); ++i) {
    res.push_back(multiply_field(i, j, a, b));
  }
  return res;
}

vector<vector<double>> matrix_multiplication(vector<vector<double>> &a,
                                             vector<vector<double>> &b) {
  int n = a.size();
  vector<vector<double>> res(n, vector<double>(n));
  for (int j = 0; j < (int)a.size(); ++j) {
    res[j] = (multiply_row(j, a, b));
  }
  return res;
}

vector<vector<double>> calculate_from_string(string strbuff) {
  cout << "Message size: " << strbuff.length() << endl;
  stringstream ss;
  double tmp;
  int matrix_size;
  ss << strbuff;
  ss >> matrix_size;
  cout << "matrix size value: " << matrix_size << endl;

  vector<vector<double>> a, b, res;
  for (int j = 0; j < matrix_size; j++) {
    a.push_back(vector<double>());
    for (int i = 0; i < matrix_size; i++) {
      ss >> tmp;
      a[j].push_back(tmp);
    }
  }
  for (int j = 0; j < matrix_size; j++) {
    b.push_back(vector<double>());
    for (int i = 0; i < matrix_size; i++) {
      ss >> tmp;
      b[j].push_back(tmp);
    }
  }

  res = matrix_multiplication(a, b);
  return res;
}

string debug_vector(vector<vector<double>> vec) {
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
    cout << "Grabbed a connection!" << endl;

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

      vector<vector<double>> res = calculate_from_string(strbuff);

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
