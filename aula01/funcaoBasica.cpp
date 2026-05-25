#include <iostream>

int Multiplicar(int a, int b); // eu uso int no lugar de 'function' pq a função retorna um int

int main()
{
    std::cout << Multiplicar(3, 4) << "\n";
    return 0;
}

int Multiplicar(int a, int b)
{
   return a * b;
}


