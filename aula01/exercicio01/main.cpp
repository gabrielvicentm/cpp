#include <iostream>
#include <string>

void PrintStatus(const std::string& name, int health, int stamina)
{
std::cout << "Nome:" << name << "\n";

std::cout << "Vida:" << health << "\n";

std::cout << "Stamina:" << stamina << "\n";
}// oq mudaria se eu nao usase a referencia(&)

void ApplyDamage(int& health, int damage)
{
    health -= damage;

    if(health < 0){
        health = 0;
    }
}

bool canRun(int stamina)
{
    return stamina > 0;
}

void SpendStamina(int& stamina, int amount)
{
    stamina -= amount;
    
    if(stamina < 0)
    {
        stamina = 0;
    }
}

int main()
{
    std::string playername = "Paulo Kogos";
    int health = 100;
    int stamina = 40;

    PrintStatus(playername, health, stamina);

    ApplyDamage(health, 35);

    if (canRun(stamina))
    {
        SpendStamina(stamina, 15);
    }

    std::cout << "\n Depois da acao:\n";
    PrintStatus(playername, health, stamina);
    
    return 0;
}






