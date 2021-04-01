#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <string>
#include <array>
#include <iostream>
#include <fstream>

using IP_t = size_t;

// Raw data
struct Packet
{
    IP_t owner;
    size_t index;
    char payload[32];
};

class Network;

// Holds data that can be requested/modified
class Server
{
private:
    const char* source;

public:
    Server(const char* filename) : source(filename) {}

    // <- Server
    void Pull(Packet& output)
    {
        std::ifstream in;
        
        in.open(source, std::ifstream::in);

        in >> output;

        in.close();
    }

    // -> Server
    void Push()
    {
        std::ofstream out(source);

        out << "Hi";

        out.close();
    }
};

// Sends requests to download/upload server data
class Client
{
private:
    size_t ip;
    std::vector<char> data;

public:
    Client(size_t _ip) : ip(_ip) {}

    // Server -> Client
    void RecievePacket(Packet pk)
    {

    }
    //  Client -> Server
    Packet SendPacket()
    {

    }
};

// The space through which data must be passed
class Network
{
private:
    std::map<size_t, Packet> packets;

public:
    Network() : packets() {}

    void Push(Packet what)
    {
        static int index;
        packets[index = (index + 1) % 8] = what;
    }
    Packet Pull()
    {

    }
};

int main()
{
    Client user(0);
    Client interceptor(1);

    Network network(
        { Server("database.txt"), Server("gameserver.txt"), Server("messenger.txt"), Server("webpage.txt"), },
        { user, interceptor, }
    );

    user
}