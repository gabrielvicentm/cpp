#include <iostream>

//PASSAGEM POR VALOR
// Aqui a func tá recebendo uma cópia de vida, por isso o resultado vai ser 90 (100 - 10)
void ReceberDano(int vida)
{
    vida -= 10;
    std::cout << "Dentro da funcao:" << vida << "\n";
}
 
int main()
{
    int vida = 100;

    ReceberDano(vida);

   std::cout << "Fora da funcao:" << vida << "\n"; 
   return 0;
}
//SAÍDA:
//Dentro da funcao: 90
//Fora da funcao: 100

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

//PASSAGEM POR REFERÊNCIA
// referência permite acessar o objeto original, se eu fizer o mesmo código agora vi dar 90 nas 2 saídas
void ReceberDano2(int& vidaa)
{
    vidaa -= 10;
}


int main()
{
    int vidaa = 100;

    ReceberDano2(vidaa);

    std::cout << vidaa << "\n";
    return 0;
}
//SAÍDA: 90

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

//PASSAGEM POR PONTEIRO
void ReceberDanoPonteiro(int *life)
{
    if(life != nullptr)
    {
        *life -=10;
    }
}

int main(){

    int life = 100;

    ReceberDanoPonteiro(&life);

    std::cout << life << "\n";

    return 0;
}
//SAÍDA: 90











