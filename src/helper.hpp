#pragma once

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>


#define DEF_BUFFERLEN 1024
#define ECHO_COMMAND_LEN 6
#define USER_AGENT_LEN 12
#define FILES_COMMAND_LEN 7

std::string getURL(std::string_view request);
std::string HandleUserAgent(std::string_view request);
std::string getRequestBody(std::string* client_message);
int GetDirectory(std::string* directory, int argc, char** argv);
int HandleFileRquest(std::filesystem::path* path, std::ifstream* file, std::stringstream* buffer);
int HandleGETFileRequest(int client_socket, std::filesystem::path* path, std::ifstream* file, std::stringstream* buffer,std::string* url);
int HandlePOSTFileRequest(int client_socket, std::filesystem::path* path, std::ifstream* file, std::string* client_message,std::string* url);