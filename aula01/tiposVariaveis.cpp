//SE AS VARIAVEIS ESTIVEREM NO MESMO ESCOPO não posso ter 2 variaveis com o mesmo nome, mesmo sendo de tipos diferentes

int vida = 99; // inteiro 'normal'

float velocidade = 600.0f; // float só tem 1 casa decimal, usado quando nao precisa de precisao extrema, se esquecer o f toma gap
double velocidadePrecisa = 600.0623664343; // usado quando precisão importa

char letrinha = 'W'; //guarda um caractere, aspas simples

bool estaVivo = true; // true or false

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

// SIGNED S UNSIGNED

int moedas = -10; //int 'normal' aceita vvalores negativos

unsigned int moedinhas = 10; //unsigned int NÃO ACEITA VALORES NEGATIVOS

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

// CONST

const int danoBase = 150; // não posso mduar o valor depois que for compilado

//CONST COM PARÂMETROS:
#include <iostream>
void ImprimirNome(const std::string& nome)
{
    std::cout << nome << "\n";
} // isso aqui significa: 'Receba sem copiar, mas não modifique"


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

//AUTO

 // quando eu uso auto o compilador vai deduzir o tipo
 // igual o := do golang, só que com algumas particularidades
auto stamina = 100;
auto dano = 623.0f;
auto nomezinho = std::string("Paulo Kogos");




