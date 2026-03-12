#include"helper.hpp"

std::string getURL(std::string_view request)
{
  bool foundPath = false;
  std::string url = "";
  int begin = 0;
  for(int i = 0; i < request.length(); i++)
  {
    if (request[i] == ' ' && !foundPath)
    {
      foundPath = true;
      begin = i+1;
      continue;
    }
    else if (request[i] == ' ' && foundPath)
    {
      url = request.substr(begin, i-begin);
      break;
    }
  }
  return url;

}

std::string HandleUserAgent(std::string_view request)
{

  int request_len = request.length();
  int begin = request.find("User-Agent:");
  std::string_view body = request.substr(begin,request_len - begin);
  int end = body.find("\r\n");
  std::string userAgentBody = "";
  userAgentBody = body.substr(USER_AGENT_LEN,end - USER_AGENT_LEN);

  return userAgentBody;
}

int HandleFileRquest(std::filesystem::path* path, std::ifstream* file, std::stringstream* buffer)
{
  file->open(path->c_str(),std::ios::binary);

  if(file->fail())
  {
    std::cout<<"file open failed\n";
    return 1;
  }
  
  *buffer << file->rdbuf();
  file->close();
  return 0;
}

int GetDirectory(std::string* directory, int argc, char** argv)
{
  if(argc < 3)
  {
    return 1;
  }
  if(strcmp(argv[2],"--directory"))
  {
    *directory = argv[2];
    return 0;
  }
  return 0;
}
int HandleGETFileRequest(int client_socket, std::filesystem::path* path, std::ifstream* file, std::stringstream* buffer,std::string* url)
{
      *path /= url->substr(FILES_COMMAND_LEN);
      if(HandleFileRquest(path,file,buffer) != 0)
      {
        std::string_view notFound = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_socket,notFound.data(),notFound.length(),0);
        return 0;
      }
      else
      {
        std::string file_content = buffer->str();
        // HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: 13\r\n\r\nHello, World!
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: " + std::to_string(file_content.size())
                                +"\r\n\r\n"+file_content;
        send(client_socket,response.data(),response.length(),0);
        return 0;
      }
  return 0;
}

int HandlePOSTFileRequest(int client_socket, std::filesystem::path* path, std::ifstream* file, std::string* client_message,std::string* url)
{
    *path /= url->substr(FILES_COMMAND_LEN);
    std::string body = getRequestBody(client_message);
    std::ofstream newFile (*path);
    newFile<<body;
    newFile.close();
    std::string_view createdSend = "HTTP/1.1 201 Created\r\n\r\n";
    send(client_socket,createdSend.data(),createdSend.size(),0);
    return 0;
}
std::string getRequestBody(std::string* client_message)
{
    std::string body;

    int end = client_message->size() - 1;
    
    for (int i = end; i >= 0; i--)
    {
        if (client_message->at(i) != '\n')
        {
            continue;
        }
        body = client_message->substr(i+1);
        break;
    }
    return body;
}