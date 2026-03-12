
#include "helper.hpp"

int ClientConnection(int client_socket, int server_fd, int argc, char **argv);


int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  //Socket takes 3 arguments: domain, type of socket and a protocol and it creates an unbound socket in the communications domain
  // AF_INET is the domain set for IPv4 addresses
  // SOCK_STREAM is the type of socket used for TCP
  // 0 is the default protocol
  // It returns a file desciptor (int)
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
      std::cerr << "Failed to create server socket\n";
      return 1;
     }
  //
  // // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  // This structure contains the address to the bound to the socket
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY; // Used to tell socket to accept connections from any network interface
  server_addr.sin_port = htons(4221);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  int (*ClientFucntionPtr)(int,int,int,char**) = &ClientConnection; // Function Pointer to be used in the thread as a callback
  while(true)
  {
      struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
  
    std::cout << "Waiting for a client to connect...\n";
  
    int client_socket = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
    if (client_socket < 0)
    {
      std::cout<<"Could not connect with client\n";
      close(server_fd);
      return 1;
    }
    else
    {
      std::thread newCLientThread(ClientFucntionPtr,client_socket,server_fd,argc,argv);
      newCLientThread.detach();
    }
  }
  
  close(server_fd);

  return 0;
}


int ClientConnection(int client_socket, int server_fd, int argc, char** argv)
{
  ssize_t rcvResult;
    std::cout<<"client connected\n";
    std::string client_message(DEF_BUFFERLEN,'\0');
    rcvResult = recv(client_socket,&client_message[0],DEF_BUFFERLEN,0);
    client_message.resize(rcvResult);
    if (rcvResult < 0)
    {
      std::cout<<"recv error\n";
      close(server_fd);
      close(client_socket);
      return 1;
    }

    std::string url = getURL(client_message);
    if (url == "/")
    {
      std::string_view ok = "HTTP/1.1 200 OK\r\n\r\n";
      send(client_socket,ok.data(),ok.length(),0);
      return 0;
    }
    else if(url.find("/echo/") == 0)
    {
      std::string str = url.substr(ECHO_COMMAND_LEN);
      std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "+ std::to_string(str.size())+"\r\n\r\n" + str;
      send(client_socket,response.data(),response.length(),0);
      return 0;
    }
    else if(url.find("/user-agent") == 0)
    {
      std::string userAgent = HandleUserAgent(client_message);
      std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(userAgent.size())+"\r\n\r\n" + userAgent;
      send(client_socket,response.data(),response.length(),0);
      return 0;
    }
    else if (url.find("/files/") == 0)
    {
      
      std::ifstream file;
      std::stringstream buffer;
      std::string directory;
      if (GetDirectory(&directory,argc,argv) != 0)
      {
        std::cout<<"Could not get directory\n";
        std::string_view notFound = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_socket,notFound.data(),notFound.length(),0);
        return 0;
      }
      std::filesystem::path path = directory;
      if (client_message.find("POST") != 0)
      {
        HandleGETFileRequest(client_socket,&path,&file,&buffer,&url);
        return 0;
      }
      else
      {
        HandlePOSTFileRequest(client_socket,&path,&file,&client_message,&url);
      }
    }
    else
    {
      std::string_view notFound = "HTTP/1.1 404 Not Found\r\n\r\n";
      send(client_socket,notFound.data(),notFound.length(),0);
      return 0;
    }
}



  

