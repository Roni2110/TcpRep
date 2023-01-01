#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sstream>
#include "Knn.h"

using namespace std;

/**
 * checking arguments.
 * @param k - should be a number > 0.
 * @param dis - should be one of the distance we got in knn.
 */
void checkingArg(int k, string dis) {
    if((k<=0) ||
       ((dis != "AUC") && (dis != "MAN") && (dis != "CHB")
        && (dis != "CAN") && (dis != "MIN"))) {
        cout << "Input not valid!" << endl;
        return;
    }
}

/**
 * checking buffer from client.
 * @param str - the string we got in buffer from client.
 * @param distance - MAN/AUC/CHB/MIN/CAN.
 * @param v1 - vector for calculating distance.
 * @param k - the k neighbors.
 */
void checkingStr(string str,string &distance, vector<double> &v1, int &k) {
    k = -1;
    double num;
    int checkInt;
    stringstream ss;
    string checkStr;
    stringstream stringstream1(str);
    while (stringstream1 >> num) {
        v1.push_back(num);
    }
    if (v1.empty()) {
        cout << "invalid input" << endl;
        return;
    }
    stringstream1.clear();
    if(stringstream1 >> checkStr) {
        ss << checkStr;
    }
    distance = ss.str();
    if (str.empty()) {
        cout << "invalid input" << endl;
        return;
    }
    stringstream1.clear();
    if (stringstream1 >> checkInt) {
        k = checkInt;
    }
    if (k == -1) {
        cout << "invalid input" << endl;
        return;
    }
    if(!stringstream1.eof()) {
        cout << "invalid input" << endl;
    }
    checkingArg(k, distance);
}

/**
 * checking arguments.
 * @param port - port.
 * @param fileName - should be .csv.
 */
void checkingArgv(int port, string fileName) {
    string str2 = "csv";
   if(port < 1 || port > 65535) {
       cout<< "invalid port number!"<< endl;
       exit(1);
   }
   if(strstr(fileName.c_str(), str2.c_str())) {
       cout << "invalid file!" << endl;
       exit(1);
   }
}

int main (int argc, char *argv[]) {
    string data;
    string message;
    vector<double> vector;
    string distance;
    int neighbor;
    string file = argv[1];
    const int server_port = atoi(argv[2]);
    checkingArgv(server_port, file);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        cout << "error creating socket" <<endl;
    }
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);
    if(bind(sock, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
        cout<<"error binding socket"<<endl;
    }
    if(listen(sock, 5) < 0) {
        cout<<"error listening to a socket"<<endl;
    }
    struct sockaddr client_sin;
    unsigned int addr_len = sizeof(client_sin);
    int client_sock = accept(sock, (struct sockaddr*) &client_sin, &addr_len);
    if(client_sock < 0) {
        cout<<"error accepting client"<<endl;
    }
    while(true) {
        char buffer[4096];
        int expected_data_len = sizeof(buffer);
        int read_bytes = recv(client_sock, buffer, expected_data_len, 0);
        if (read_bytes == 0) {
            cout << "no message from client" << endl;
        }
        if (read_bytes < 0) {
            cout << "error getting a message from client" << endl;
        }
        data = buffer;
        checkingStr(data, distance, vector, neighbor);
        Knn *knn = new Knn(neighbor, distance, vector);
        knn->uploadFiles(file);
        message = knn->getMessage();
        int length = message.size();
        char message_to_send[length + 1];
        strcpy(message_to_send, message.c_str());
        int send_bytes = send(client_sock, message_to_send, length, 0);
        if (send_bytes < 0) {
            cout << "error sending a message" << endl;
        }
        delete knn;
    }
}
