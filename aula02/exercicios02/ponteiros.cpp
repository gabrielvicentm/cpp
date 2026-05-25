#include <iostream>

//ponteiro básico:
int health = 100;
int* healthPtr = &health;

//ponteiro nulo
void PrintHealth(const int* health)
{
    if(health == nullptr){
        std::cout << "No health";
    }
    else{
        std::cout << healthPtr << "\n";
    }
}
