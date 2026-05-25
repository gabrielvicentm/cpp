#include <iostream>

int main()
{
    int dano = 7;
    int armadura = 2;

    float resultado1 = dano / armadura;

    // se eu converter um lado já converte o outro também, só precisa do static_cast uma vez
    float resultado2 = static_cast<float>(dano) / (armadura);
    
    //isso aqui é redundante, não precisa convertre os dois:
    float resultado3 = static_cast<float>(dano) / static_cast<float>(armadura);

}