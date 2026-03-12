lightweight multithreaded HTTP server written in C++ using POSIX sockets.
The server supports multiple concurrent clients and basic HTTP request handling.

This project uses vcpkg as the dependency manager.

The project was made following codecrafters guides and tests.


Features:
- TCP server using POSIX sockets
- Multithreaded client handling (std::thread)
- Basic HTTP parsing
- Echo endpoint
- User-Agent header extraction
- File upload and retrieval
- Dependency management using vcpkg

To build and run the project simply run ./your_program.sh
If you wish to pass a directory run ./your_program.sh --directory "directory name"
