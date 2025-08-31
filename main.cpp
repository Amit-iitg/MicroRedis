#include "../headers/RedisServer.h"
#include "../headers/RedisDatabase.h"

#include <iostream>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <winsock2.h>
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    WSADATA wsaData;
    int wsaerr = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (wsaerr != 0) {
        std::cerr << "WSAStartup failed: " << wsaerr << std::endl;
        return 1;
    }
#endif
    int port = 6379; // default
    if (argc >=2) port = std::stoi(argv[1]); 

    if (RedisDatabase::getInstance().load("dump.my_rdb"))
        std::cout << "Database Loaded From dump.my_rdb\n";
    else 
        std::cout << "No dump found or load failed; starting with an empty database.\n";

    RedisServer server(port);

    // Backgroung persistance: dump the database every 300 seconds. (5 * 60 seconds save database)
    std::thread persistanceThread([](){
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(300));
            if (!RedisDatabase::getInstance().dump("dump.my_rdb"))
                std::cerr << "Error Dumping Database\n";
            else 
                std::cout << "Database Dumped to dump.my_rdb\n";
        }
    });
    persistanceThread.detach();
    
    server.run();
    return 0;
}