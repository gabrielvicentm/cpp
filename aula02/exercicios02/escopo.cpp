#include <iostream>

int main()
{
    int outer = 10;

    {
        int inner = 20;
        std::cout << outer << "\n";
        std::cout << inner << "\n";
    }

    std::cout << outer << "\n";
//  std::cout << inner << "\n";  -- dá erro pq inner não existe no escopo
    return 0;
} 
