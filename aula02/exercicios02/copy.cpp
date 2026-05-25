#include <iostream>
#include <string>

struct Tracker
{
    std::string name;

    Tracker(const std::string& newName)
        : name(newName)
    {
        std::cout << "Constructor: " << name << "\n";
    }

    Tracker(const Tracker& other)
        : name(other.name)
    {
        std::cout << "Copy constructor: " << name << "\n";
    }

    Tracker& operator=(const Tracker& other)
    {
        std::cout << "Copy assignment: " << other.name << "\n";

        if (this != &other)
        {
            name = other.name;
        }

        return *this;
    }

    ~Tracker()
    {
        std::cout << "Destructor: " << name << "\n";
    }
};

int main()
{
    Tracker a("A");
    Tracker b = a;

    Tracker c("C");
    c = a;

    return 0;
}