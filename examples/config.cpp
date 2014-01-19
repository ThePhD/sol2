#include <sol.hpp>
#include <string>
#include <iostream>

// shows how to load basic data to a struct

struct config {
    std::string name;
    int width;
    int height;

    void print() {
        std::cout << "Name: "   << name   << '\n'
                  << "Width: "  << width  << '\n'
                  << "Height: " << height << '\n';
    }
};

int main() {
    sol::state lua;
    config screen;
    lua.open_file("config.lua");
    screen.name = lua.get<std::string>("name");
    screen.width = lua.get<int>("width");
    screen.height = lua.get<int>("height");
    screen.print();
}
