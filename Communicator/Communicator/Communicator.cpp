#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdarg>
using namespace std;

using Username = string;
using UserID = size_t;

struct Message
{
    UserID sender;
    float timestamp;
    string content;
};

using Conversation = vector<Message>;
using DMs = unordered_map<UserID, Conversation>;

struct UserData
{
    string icon;
    string displayname;
    DMs messages;
};

unordered_map<UserID, UserData> g_users;
unordered_map<Username, UserID> g_hash;

bool CreateNewUser(Username name)
{
    auto it = g_hash.find(name);
    bool nameIsFree = (it == g_hash.end());
    if (nameIsFree)
    {
        UserID id = g_users.size();
        g_hash.insert({ name, id });
    }
    return nameIsFree;
}

int Menu(string prompt, ...)
{
    va_list args;
    va_start(args, prompt);

    string str = va_arg(args, string);

    va_end(args);

    int i;

    va_start(args, prompt);

    string str = va_arg(args, string);

    va_end(args);
}

int main()
{
    CreateNewUser("xXgamerXx");
    CreateNewUser("urmumlmao");
    while (true)
    {
        cout << "Create a username: ";
        Username name;
        cin >> name;
        if (CreateNewUser(name)) break;
        cout << "That name is taken.\n";
    }
    cout << "Success!\n";

    cout << "Send a dm:\n";

    return 0;
}