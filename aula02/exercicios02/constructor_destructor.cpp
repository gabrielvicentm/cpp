#include <iostream>

struct LifeLogger
{
    LifeLogger(){
        std::cout << "Nasceu\n";
    }

    ~LifeLogger(){
        std::cout << "Destruido\n";
    }

};
