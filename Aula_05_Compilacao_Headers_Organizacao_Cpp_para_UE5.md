# Aula 05 - Compilacao, headers e organizacao de projeto em C++ para UE5

Esta aula cobre a Fase 5 do `Roadmap_Cpp_UE5.md`: compilacao, headers e organizacao de projeto.

Ate agora voce estudou:

- Aula 01: fundamentos da linguagem
- Aula 02: memoria, ponteiros, lifetime, RAII e ownership
- Aula 03: orientacao a objetos
- Aula 04: C++ moderno, STL, smart pointers, move semantics, lambdas e templates

Agora voce vai estudar uma parte que muita gente ignora ate comecar a sofrer:

> como C++ organiza codigo em varios arquivos e como o compilador/linker juntam tudo.

Na UE5 isso e obrigatorio. A Unreal usa headers, macros, code generation, reflection, includes, modulos, build tool e convencoes proprias. Se voce nao entende o modelo basico de C++, erros da Unreal parecem absurdos.

O objetivo desta aula e voce entender:

- o que vai em `.h`
- o que vai em `.cpp`
- declaracao vs definicao em projeto real
- `#include`
- include guards
- `#pragma once`
- preprocessador
- macros em nivel conceitual
- linkagem
- simbolos
- multiplas definicoes
- undefined reference
- forward declaration
- dependencias circulares
- por que headers ruins deixam projetos lentos
- como isso se conecta com `UCLASS`, `GENERATED_BODY`, `UPROPERTY` e Unreal Build Tool

---

## Como estudar esta aula

Esta aula deve ser praticada criando varios arquivos.

Voce nao vai entender linkagem apenas lendo. Voce precisa causar alguns erros de proposito:

- erro de include faltando
- erro de simbolo indefinido
- erro de multiplas definicoes
- erro de dependencia circular

Esses erros sao parte do aprendizado.

Tempo sugerido:

- 45 min: modelo de compilacao em alto nivel
- 60 min: `.h` e `.cpp`
- 45 min: include guards e `#pragma once`
- 45 min: preprocessador e macros
- 60 min: linkagem e erros comuns
- 60 min: forward declaration e dependencia circular
- 90 min: mini-projeto

Se o tempo estiver curto, priorize:

- header declara, `.cpp` implementa
- `#include` copia texto antes da compilacao
- `#pragma once`
- undefined reference
- multiple definition
- forward declaration

Esses conceitos aparecem direto em UE5.

---

## 1. Por que C++ usa varios arquivos

Programas pequenos cabem em um arquivo:

```cpp
#include <iostream>

int main()
{
    std::cout << "Hello\n";
    return 0;
}
```

Mas projetos reais ficam grandes.

Voce precisa separar:

- classes
- funcoes
- sistemas
- componentes
- interfaces
- implementacoes
- testes

Em C++, a separacao classica e:

```text
Arquivo.h    -> declaracoes
Arquivo.cpp  -> definicoes/implementacoes
```

Exemplo:

```text
HealthComponent.h
HealthComponent.cpp
main.cpp
```

Na UE5, isso e padrao:

```text
MyActor.h
MyActor.cpp
MyCharacter.h
MyCharacter.cpp
WeaponComponent.h
WeaponComponent.cpp
```

O `.h` diz para outros arquivos:

> isto existe e pode ser usado assim.

O `.cpp` diz:

> aqui esta como isso funciona.

---

## 2. Declaracao vs definicao revisado

Declaracao informa existencia.

Definicao cria corpo, armazenamento ou implementacao.

### Funcao

Declaracao:

```cpp
int Add(int a, int b);
```

Definicao:

```cpp
int Add(int a, int b)
{
    return a + b;
}
```

### Classe

Declaracao/definicao de classe geralmente aparece no header:

```cpp
class Player
{
public:
    void Attack();

private:
    int health = 100;
};
```

Isso define a estrutura da classe: tamanho, membros, metodos declarados.

Mas o metodo pode ser implementado no `.cpp`:

```cpp
void Player::Attack()
{
}
```

### Variavel global

Declaracao:

```cpp
extern int GlobalScore;
```

Definicao:

```cpp
int GlobalScore = 0;
```

Variavel global em header sem cuidado causa multiplas definicoes. Evite globais enquanto aprende.

---

## 3. O que e header

Header e um arquivo que normalmente termina em `.h` ou `.hpp`.

Ele contem declaracoes que outros arquivos precisam conhecer.

Exemplo:

```cpp
// MathUtils.h
#pragma once

int Add(int a, int b);
int Multiply(int a, int b);
```

Esse header diz:

> existem funcoes Add e Multiply.

Outro arquivo pode incluir:

```cpp
#include "MathUtils.h"

int result = Add(2, 3);
```

Sem o header, o compilador nao sabe a assinatura de `Add`.

### Header nao e lugar para qualquer coisa

Coloque no header aquilo que outros arquivos precisam saber:

- declaracao de classes
- declaracao de funcoes publicas
- enums compartilhados
- structs compartilhadas
- constantes apropriadas
- templates, quando necessario

Evite colocar:

- implementacoes grandes sem motivo
- `using namespace std;`
- includes desnecessarios
- variaveis globais definidas
- codigo que muda muito e força recompilacao ampla

---

## 4. O que e `.cpp`

Arquivo `.cpp` contem implementacao.

Exemplo:

```cpp
// MathUtils.cpp
#include "MathUtils.h"

int Add(int a, int b)
{
    return a + b;
}

int Multiply(int a, int b)
{
    return a * b;
}
```

O `.cpp` inclui seu proprio header para garantir consistencia.

Se o header declarar:

```cpp
int Add(int a, int b);
```

mas o `.cpp` implementar:

```cpp
float Add(float a, float b)
{
    return a + b;
}
```

voce vai perceber o problema.

Regra:

> Todo `.cpp` deve incluir primeiro o header correspondente quando existir.

Na Unreal, isso tambem e boa pratica:

```cpp
#include "MyActor.h"
```

normalmente aparece no topo de `MyActor.cpp`.

---

## 5. Exemplo completo com tres arquivos

### `HealthComponent.h`

```cpp
#pragma once

class HealthComponent
{
public:
    HealthComponent(int initialHealth, int maxHealth);

    void ApplyDamage(int amount);
    void Heal(int amount);
    int GetCurrent() const;
    int GetMax() const;
    bool IsDead() const;

private:
    void Clamp();

    int currentHealth;
    int maximumHealth;
};
```

### `HealthComponent.cpp`

```cpp
#include "HealthComponent.h"

HealthComponent::HealthComponent(int initialHealth, int maxHealth)
    : currentHealth(initialHealth), maximumHealth(maxHealth)
{
    if (maximumHealth < 0)
    {
        maximumHealth = 0;
    }

    Clamp();
}

void HealthComponent::ApplyDamage(int amount)
{
    if (amount < 0)
    {
        return;
    }

    currentHealth -= amount;
    Clamp();
}

void HealthComponent::Heal(int amount)
{
    if (amount < 0)
    {
        return;
    }

    currentHealth += amount;
    Clamp();
}

int HealthComponent::GetCurrent() const
{
    return currentHealth;
}

int HealthComponent::GetMax() const
{
    return maximumHealth;
}

bool HealthComponent::IsDead() const
{
    return currentHealth <= 0;
}

void HealthComponent::Clamp()
{
    if (currentHealth < 0)
    {
        currentHealth = 0;
    }

    if (currentHealth > maximumHealth)
    {
        currentHealth = maximumHealth;
    }
}
```

### `main.cpp`

```cpp
#include <iostream>

#include "HealthComponent.h"

int main()
{
    HealthComponent health(100, 100);

    health.ApplyDamage(25);

    std::cout << health.GetCurrent() << "\n";

    return 0;
}
```

Aqui:

- `main.cpp` precisa saber que `HealthComponent` existe
- por isso inclui `HealthComponent.h`
- `HealthComponent.cpp` contem a implementacao dos metodos
- o linker junta tudo no executavel

---

## 6. Modelo de compilacao em alto nivel

Imagine estes arquivos:

```text
main.cpp
HealthComponent.cpp
Weapon.cpp
```

Cada `.cpp` e uma translation unit.

O compilador compila cada `.cpp` separadamente.

Modelo:

```text
main.cpp             -> main.obj
HealthComponent.cpp  -> HealthComponent.obj
Weapon.cpp           -> Weapon.obj
```

Depois o linker junta:

```text
main.obj + HealthComponent.obj + Weapon.obj -> programa.exe
```

Por isso existem dois tipos de erro muito comuns:

- erro de compilacao
- erro de linkagem

### Erro de compilacao

O compilador nao conseguiu entender/validar um `.cpp`.

Exemplos:

- tipo nao declarado
- sintaxe errada
- metodo chamado com parametros errados
- falta de include

### Erro de linkagem

O compilador aceitou as declaracoes, mas o linker nao encontrou a definicao final.

Exemplos:

- funcao declarada mas nao implementada
- `.cpp` nao foi incluido no build
- assinatura da definicao nao bate com a declaracao
- simbolo definido mais de uma vez

Essa separacao e essencial.

---

## 7. `#include`

`#include` e uma diretiva de preprocessador.

Ele roda antes da compilacao propriamente dita.

Quando voce escreve:

```cpp
#include "HealthComponent.h"
```

o preprocessador basicamente cola o conteudo do header naquele ponto.

Isso e conceitualmente simples e tambem perigoso.

Se um header inclui outro, que inclui outro, que inclui outro, seu `.cpp` pode acabar compilando muito mais codigo do que parece.

### Include com aspas

```cpp
#include "HealthComponent.h"
```

Usado para headers do seu projeto.

### Include com angle brackets

```cpp
#include <iostream>
#include <vector>
#include <string>
```

Usado para bibliotecas e headers do sistema/biblioteca padrao.

---

## 8. O problema de incluir o mesmo header varias vezes

Imagine:

### `Player.h`

```cpp
#include "HealthComponent.h"

class Player
{
private:
    HealthComponent health;
};
```

### `Enemy.h`

```cpp
#include "HealthComponent.h"

class Enemy
{
private:
    HealthComponent health;
};
```

### `main.cpp`

```cpp
#include "Player.h"
#include "Enemy.h"
```

Agora `HealthComponent.h` pode ser incluido duas vezes no mesmo `.cpp`.

Sem protecao, isso pode causar redefinicao da classe.

Por isso usamos include guards ou `#pragma once`.

---

## 9. Include guards

Include guard evita que um header seja processado mais de uma vez na mesma translation unit.

```cpp
#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

class HealthComponent
{
};

#endif
```

Como funciona:

1. Se `HEALTH_COMPONENT_H` nao foi definido, entra.
2. Define `HEALTH_COMPONENT_H`.
3. Processa o header.
4. Se incluir de novo, `HEALTH_COMPONENT_H` ja existe, entao pula.

Isso e C++ tradicional.

---

## 10. `#pragma once`

Forma moderna e simples:

```cpp
#pragma once

class HealthComponent
{
};
```

Significa:

> inclua este arquivo no maximo uma vez por translation unit.

Hoje e amplamente suportado pelos compiladores usados em projetos comuns.

Na UE5, voce vai ver muito:

```cpp
#pragma once
```

Regra pratica:

> Use `#pragma once` nos seus headers.

---

## 11. Preprocessador

O preprocessador roda antes da compilacao.

Ele lida com diretivas como:

```cpp
#include
#define
#if
#ifdef
#ifndef
#endif
```

Ele nao entende C++ do mesmo jeito que o compilador. Ele trabalha mais como transformacao textual.

Exemplo:

```cpp
#define MAX_HEALTH 100
```

Depois:

```cpp
int health = MAX_HEALTH;
```

O preprocessador substitui `MAX_HEALTH` por `100`.

Resultado conceitual:

```cpp
int health = 100;
```

### Macros sao poderosas e perigosas

Exemplo ruim:

```cpp
#define SQUARE(x) x * x
```

Uso:

```cpp
int result = SQUARE(2 + 3);
```

Expande para:

```cpp
int result = 2 + 3 * 2 + 3;
```

Resultado errado por precedencia.

Melhor, mas ainda macro:

```cpp
#define SQUARE(x) ((x) * (x))
```

Em C++ moderno, prefira funcoes:

```cpp
int Square(int x)
{
    return x * x;
}
```

Ou templates quando precisar generico.

### Por que macros importam na Unreal?

Porque Unreal usa macros pesadas:

```cpp
UCLASS()
USTRUCT()
UENUM()
UPROPERTY()
UFUNCTION()
GENERATED_BODY()
```

Essas macros integram seu codigo com:

- reflection
- editor
- Blueprints
- garbage collection
- serializacao
- networking
- Unreal Header Tool

Elas nao sao decoracao. Elas participam do pipeline de build da engine.

Mas antes de entender as macros da Unreal, voce precisa entender:

> macro acontece antes/ao redor da compilacao normal e pode alterar o codigo que o compilador realmente ve.

---

## 12. Macros vs `constexpr`

Para constantes, evite macro quando puder.

Ruim:

```cpp
#define MAX_HEALTH 100
```

Melhor:

```cpp
constexpr int MaxHealth = 100;
```

`constexpr` tem tipo, escopo e participa melhor do C++.

Em header:

```cpp
#pragma once

constexpr int DefaultMaxHealth = 100;
```

Para valores constantes simples, prefira `constexpr`.

Macros ainda existem para casos que C++ normal nao cobre, como o sistema da Unreal.

---

## 13. Linkagem e simbolos

Depois que cada `.cpp` compila, o linker junta tudo.

Um simbolo e, simplificando, um nome que precisa ser resolvido:

- funcao
- variavel global
- metodo
- objeto com linkage externo

Exemplo:

```cpp
int Add(int a, int b);
```

Quando `main.cpp` chama:

```cpp
Add(2, 3);
```

o compilador acredita que `Add` existe porque viu a declaracao.

Mas o linker precisa encontrar a definicao:

```cpp
int Add(int a, int b)
{
    return a + b;
}
```

Se nao encontrar, erro de linkagem.

---

## 14. Undefined reference / unresolved external symbol

Esse erro significa:

> alguem prometeu que uma funcao/variavel existe, mas o linker nao encontrou a definicao.

Exemplo:

### `MathUtils.h`

```cpp
#pragma once

int Add(int a, int b);
```

### `main.cpp`

```cpp
#include <iostream>
#include "MathUtils.h"

int main()
{
    std::cout << Add(2, 3) << "\n";
    return 0;
}
```

Se voce nao criar `MathUtils.cpp` com a definicao, compila a translation unit, mas falha na linkagem.

Erro pode aparecer como:

```text
undefined reference to Add(int, int)
```

ou no MSVC:

```text
unresolved external symbol
```

### Causas comuns

- esqueceu de implementar a funcao
- implementou com assinatura diferente
- `.cpp` nao entrou no build
- metodo declarado na classe mas nao definido
- esqueceu namespace correto

Exemplo de assinatura diferente:

Header:

```cpp
int Add(int a, int b);
```

Cpp:

```cpp
float Add(float a, float b)
{
    return a + b;
}
```

Para o linker, isso nao e a mesma funcao.

---

## 15. Multiple definition

Esse erro significa:

> o mesmo simbolo foi definido mais de uma vez.

Exemplo ruim:

### `MathUtils.h`

```cpp
#pragma once

int Add(int a, int b)
{
    return a + b;
}
```

### `A.cpp`

```cpp
#include "MathUtils.h"
```

### `B.cpp`

```cpp
#include "MathUtils.h"
```

Agora a funcao `Add` foi definida em `A.cpp` e `B.cpp`, porque o header foi colado nos dois.

O linker ve duas definicoes do mesmo simbolo.

### Solucao

No header, declare:

```cpp
#pragma once

int Add(int a, int b);
```

No `.cpp`, defina:

```cpp
#include "MathUtils.h"

int Add(int a, int b)
{
    return a + b;
}
```

### E funcoes pequenas no header?

Podem existir, mas precisam de cuidado.

Funcoes definidas dentro da classe:

```cpp
class Player
{
public:
    int GetHealth() const
    {
        return health;
    }

private:
    int health = 100;
};
```

sao tratadas de forma especial como inline.

Funcoes livres em header geralmente devem ser `inline` se forem definidas ali:

```cpp
inline int Add(int a, int b)
{
    return a + b;
}
```

Mas no inicio, regra simples:

> declare no `.h`, implemente no `.cpp`, salvo templates e funcoes muito pequenas dentro da classe.

---

## 16. Templates em headers

Templates normalmente precisam ficar no header.

Exemplo:

```cpp
template <typename T>
T Clamp(T value, T min, T max)
{
    if (value < min)
    {
        return min;
    }

    if (value > max)
    {
        return max;
    }

    return value;
}
```

Por que?

Porque o compilador precisa ver a implementacao do template quando instancia para um tipo.

Exemplo:

```cpp
int x = Clamp(10, 0, 5);
float y = Clamp(1.5f, 0.0f, 1.0f);
```

O compilador gera versoes especificas para `int` e `float`.

Se so tiver a declaracao e nao a implementacao visivel, pode falhar.

Regra:

> Templates geralmente vivem em headers.

Isso tambem explica por que bibliotecas template-heavy podem aumentar tempo de compilacao.

---

## 17. Forward declaration

Forward declaration declara que um tipo existe sem incluir o header completo.

Exemplo:

```cpp
class Weapon;
```

Isso diz:

> existe uma classe chamada `Weapon`.

Voce pode usar forward declaration quando so precisa de ponteiro ou referencia no header.

### Exemplo bom

### `Player.h`

```cpp
#pragma once

class Weapon;

class Player
{
public:
    void EquipWeapon(Weapon* newWeapon);

private:
    Weapon* weapon = nullptr;
};
```

Aqui o header nao precisa saber o tamanho de `Weapon`, porque so guarda ponteiro.

### `Player.cpp`

```cpp
#include "Player.h"
#include "Weapon.h"

void Player::EquipWeapon(Weapon* newWeapon)
{
    weapon = newWeapon;
}
```

O `.cpp` inclui `Weapon.h` porque ali talvez precise acessar membros de `Weapon`.

---

## 18. Quando forward declaration nao basta

Forward declaration nao basta quando o compilador precisa saber o tamanho ou detalhes do tipo.

### Membro por valor

```cpp
class Weapon;

class Player
{
private:
    Weapon weapon; // erro: tipo incompleto
};
```

Para guardar por valor, o compilador precisa saber o tamanho de `Weapon`.

Entao precisa incluir:

```cpp
#include "Weapon.h"
```

### Heranca

```cpp
class Base;

class Derived : public Base // erro: precisa definicao completa
{
};
```

Para herdar, precisa incluir o header da base.

### Chamar metodo no header

```cpp
class Weapon;

class Player
{
public:
    void Fire()
    {
        weapon->Fire(); // erro no header: Weapon incompleto
    }

private:
    Weapon* weapon = nullptr;
};
```

Se voce quer chamar metodo de `Weapon`, o compilador precisa conhecer a classe.

Solucao: implemente `Fire` no `.cpp` e inclua `Weapon.h` la.

---

## 19. Por que forward declaration importa

Headers espalham dependencia.

Se `Player.h` inclui `Weapon.h`, todo arquivo que inclui `Player.h` tambem recebe `Weapon.h`.

Isso aumenta:

- tempo de compilacao
- acoplamento
- chance de dependencia circular
- recompilacoes desnecessarias

Forward declaration reduz isso.

Regra pratica:

> Em headers, prefira forward declaration quando ponteiro/referencia bastar. Inclua o header completo no `.cpp`.

Na Unreal, isso e muito comum para reduzir tempo de build.

---

## 20. Dependencia circular

Dependencia circular acontece quando dois headers precisam um do outro.

Exemplo ruim:

### `Player.h`

```cpp
#pragma once

#include "Weapon.h"

class Player
{
private:
    Weapon* weapon;
};
```

### `Weapon.h`

```cpp
#pragma once

#include "Player.h"

class Weapon
{
private:
    Player* owner;
};
```

`Player.h` inclui `Weapon.h`, que inclui `Player.h`, que inclui `Weapon.h`...

`#pragma once` evita loop infinito, mas voce ainda pode ter tipo incompleto ou arquitetura acoplada demais.

### Solucao com forward declarations

### `Player.h`

```cpp
#pragma once

class Weapon;

class Player
{
private:
    Weapon* weapon = nullptr;
};
```

### `Weapon.h`

```cpp
#pragma once

class Player;

class Weapon
{
private:
    Player* owner = nullptr;
};
```

Nos `.cpp`, inclua os headers completos quando precisar acessar detalhes.

---

## 21. Include what you use

Regra:

> Cada arquivo deve incluir diretamente aquilo de que precisa.

Nao dependa de include indireto.

Ruim:

```cpp
// main.cpp
#include "Player.h"

std::vector<int> values; // funciona so porque Player.h inclui vector
```

Melhor:

```cpp
#include <vector>
#include "Player.h"

std::vector<int> values;
```

Se `main.cpp` usa `std::vector`, ele deve incluir `<vector>`.

Isso deixa dependencias claras.

---

## 22. Ordem de includes

Uma ordem comum em `.cpp`:

```cpp
#include "Player.h"

#include <iostream>
#include <vector>

#include "Weapon.h"
#include "Inventory.h"
```

No C++ puro, estilos variam.

Na Unreal, normalmente:

```cpp
#include "MyActor.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
```

O mais importante:

- inclua o header correspondente primeiro
- mantenha includes necessarios
- remova includes nao usados
- prefira forward declaration no header quando possivel

---

## 23. Namespaces

Namespaces agrupam nomes e evitam conflito.

```cpp
namespace Combat
{
    int CalculateDamage(int baseDamage, int bonus)
    {
        return baseDamage + bonus;
    }
}
```

Uso:

```cpp
int damage = Combat::CalculateDamage(10, 5);
```

Evite em header:

```cpp
using namespace std;
```

Isso polui todos os arquivos que incluem o header.

Ruim:

```cpp
// Player.h
using namespace std;
```

Melhor:

```cpp
#include <string>

class Player
{
private:
    std::string name;
};
```

Regra:

> Nunca coloque `using namespace std;` em header.

---

## 24. `static` em arquivo e linkage interno

Em C++, `static` em escopo de arquivo pode limitar um simbolo ao arquivo atual.

```cpp
static int HelperValue = 10;
```

Ou:

```cpp
static int HelperFunction()
{
    return 42;
}
```

Isso significa que o simbolo tem linkage interno: nao e visivel para outros `.cpp`.

Hoje, uma forma mais moderna para funcoes auxiliares e namespace anonimo:

```cpp
namespace
{
    int HelperFunction()
    {
        return 42;
    }
}
```

Use para helpers internos de um `.cpp`.

Nao precisa aprofundar agora. O ponto e:

> nem tudo em um `.cpp` precisa ser exposto no header.

---

## 25. `inline` em headers

`inline` tem dois sentidos praticos:

1. Pode permitir multiplas definicoes identicas em translation units diferentes.
2. Pode sugerir/permitir expansao inline, mas o compilador decide otimizacao.

Exemplo de funcao livre em header:

```cpp
#pragma once

inline int Add(int a, int b)
{
    return a + b;
}
```

Sem `inline`, se esse header for incluido em varios `.cpp`, pode gerar multiple definition.

Para iniciantes:

- metodos pequenos dentro da classe no header sao ok
- templates no header sao ok
- funcoes livres implementadas em header devem ser `inline`
- implementacoes maiores devem ir para `.cpp`

---

## 26. `extern`

`extern` declara uma variavel definida em outro arquivo.

Exemplo:

### `Globals.h`

```cpp
#pragma once

extern int GlobalScore;
```

### `Globals.cpp`

```cpp
#include "Globals.h"

int GlobalScore = 0;
```

### `main.cpp`

```cpp
#include "Globals.h"

int main()
{
    GlobalScore = 10;
    return 0;
}
```

Isso funciona, mas use globais com muito cuidado.

Em jogos, estado global mal controlado vira fonte de bugs.

Na UE5, existem sistemas proprios para estado global ou semi-global:

- GameInstance
- GameMode
- GameState
- subsystems
- singletons em casos especificos

Nao abuse de global em C++ puro.

---

## 27. Build manual em conceito

Se voce estivesse usando compilador via terminal, poderia compilar varios `.cpp`.

Com g++ conceitualmente:

```text
g++ main.cpp HealthComponent.cpp Weapon.cpp -o game
```

Com MSVC conceitualmente:

```text
cl main.cpp HealthComponent.cpp Weapon.cpp
```

Se voce esquecer `HealthComponent.cpp`, pode receber unresolved external.

Em IDEs e sistemas de build, a ferramenta decide quais arquivos entram.

Na Unreal, quem gerencia isso e:

- Unreal Build Tool
- arquivos `.Build.cs`
- modulos
- plugins
- generated code

Mas o conceito continua:

> os `.cpp` precisam entrar no build para suas definicoes existirem no link.

---

## 28. Como isso aparece na UE5

Um actor C++ comum tem:

```text
MyActor.h
MyActor.cpp
```

### `MyActor.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class MYPROJECT_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
```

Pontos importantes:

- `#pragma once` protege o header
- `CoreMinimal.h` traz tipos basicos da Unreal
- `GameFramework/Actor.h` e necessario porque voce herda de `AActor`
- `MyActor.generated.h` e gerado pela Unreal Header Tool
- `UCLASS()` marca a classe para reflection
- `GENERATED_BODY()` injeta codigo gerado
- `MYPROJECT_API` controla exportacao/importacao entre modulos

### `MyActor.cpp`

```cpp
#include "MyActor.h"

AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay()
{
    Super::BeginPlay();
}

void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
```

Isso e exatamente o modelo:

- header declara classe e metodos
- `.cpp` implementa
- engine/build tool processa macros e gera codigo adicional

---

## 29. `generated.h` na Unreal

Em headers da Unreal com macros de reflection, voce vera:

```cpp
#include "MyActor.generated.h"
```

Regra importante:

> O include `.generated.h` deve ser o ultimo include do header.

Exemplo:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"
```

Nao coloque outro include depois dele.

O Unreal Header Tool gera esse arquivo com codigo necessario para reflection.

Se voce baguncar isso, os erros podem ser confusos.

---

## 30. Forward declaration na Unreal

Exemplo:

```cpp
class UCameraComponent;
class USpringArmComponent;
```

Header:

```cpp
UPROPERTY(VisibleAnywhere)
TObjectPtr<UCameraComponent> Camera;

UPROPERTY(VisibleAnywhere)
TObjectPtr<USpringArmComponent> SpringArm;
```

Dependendo do uso, forward declaration pode bastar no `.h`, e o `.cpp` inclui:

```cpp
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
```

Mas existe cuidado: macros como `UPROPERTY` e reflection podem precisar de tipos completos em alguns contextos.

Regra pragmatica inicial para UE5:

- se for ponteiro/referencia simples no header, forward declaration pode funcionar
- se herda da classe, precisa incluir header
- se usa membros/metodos no `.cpp`, inclua header no `.cpp`
- se o Unreal Header Tool reclamar, leia o erro e ajuste include/tipo

---

## 31. Dependencias em headers da Unreal

Headers da UE5 podem ficar pesados rapidamente.

Exemplo ruim:

```cpp
// MyCharacter.h
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon.h"
#include "Inventory.h"
#include "Enemy.h"
#include "QuestSystem.h"
```

Se muitos desses tipos sao usados apenas como ponteiros, voce pode forward declarar.

Melhor:

```cpp
class UCameraComponent;
class USpringArmComponent;
class AWeapon;
class UInventoryComponent;
```

E incluir os headers completos no `.cpp`.

Isso reduz acoplamento e pode reduzir tempo de compilacao.

---

## 32. Erros comuns e como ler

### Erro: tipo desconhecido

Mensagem parecida:

```text
unknown type name 'Weapon'
```

Possiveis causas:

- faltou include
- faltou forward declaration
- namespace errado
- ordem de includes ruim
- dependencia circular

### Erro: invalid use of incomplete type

Mensagem:

```text
invalid use of incomplete type 'Weapon'
```

Significa:

> o compilador sabe que `Weapon` existe, mas nao conhece os detalhes.

Provavel causa:

- voce so forward declarou, mas tentou acessar membro/metodo ou guardar por valor

Solucao:

- inclua `Weapon.h` no `.cpp`
- ou no `.h` se realmente precisar do tipo completo ali

### Erro: unresolved external symbol

Significa:

> declarou, chamou, mas nao existe definicao linkada.

Cheque:

- implementou o metodo?
- assinatura bate?
- namespace bate?
- `.cpp` entrou no build?

### Erro: multiple definition

Significa:

> mesmo simbolo definido em mais de uma translation unit.

Cheque:

- implementou funcao livre no header sem `inline`?
- definiu variavel global no header?
- incluiu `.cpp` por engano?

Importante:

> Nunca inclua arquivo `.cpp` com `#include`.

Inclua headers.

---

## 33. Nunca inclua `.cpp`

Errado:

```cpp
#include "HealthComponent.cpp"
```

Isso pode causar:

- multiplas definicoes
- build confuso
- arquitetura errada

Correto:

```cpp
#include "HealthComponent.h"
```

O `.cpp` entra no build pelo sistema de build/IDE.

Regra:

> `#include` e para headers, nao para `.cpp`.

---

## 34. Organizacao simples de projeto

Para estudos, use:

```text
project/
    main.cpp
    HealthComponent.h
    HealthComponent.cpp
    Weapon.h
    Weapon.cpp
    Player.h
    Player.cpp
```

Em cada par:

### Header

- `#pragma once`
- includes minimos
- forward declarations
- declaracao da classe

### Cpp

- inclui seu header
- inclui headers extras usados na implementacao
- implementa metodos

Exemplo:

### `Player.h`

```cpp
#pragma once

#include <string>

class Weapon;

class Player
{
public:
    Player(std::string name);

    void EquipWeapon(Weapon* newWeapon);
    void Attack();

private:
    std::string name;
    Weapon* weapon = nullptr;
};
```

### `Player.cpp`

```cpp
#include "Player.h"

#include <iostream>
#include <utility>

#include "Weapon.h"

Player::Player(std::string newName)
    : name(std::move(newName))
{
}

void Player::EquipWeapon(Weapon* newWeapon)
{
    weapon = newWeapon;
}

void Player::Attack()
{
    if (weapon == nullptr)
    {
        std::cout << name << " has no weapon\n";
        return;
    }

    weapon->Fire();
}
```

`Player.h` nao precisa incluir `Weapon.h` porque so guarda ponteiro.

`Player.cpp` precisa incluir `Weapon.h` porque chama `weapon->Fire()`.

---

## 35. Quando colocar implementacao no header

Coloque no header quando:

- e template
- metodo pequeno definido dentro da classe
- funcao `inline`
- tipo precisa estar totalmente visivel para performance/genericidade

Coloque no `.cpp` quando:

- implementacao e maior
- nao precisa ser template
- quer reduzir dependencias
- quer esconder detalhes
- quer reduzir recompilacao

Exemplo bom no header:

```cpp
int GetHealth() const
{
    return health;
}
```

Exemplo melhor no `.cpp`:

```cpp
void Player::Attack()
{
    // varias regras, logs, validacoes, chamadas
}
```

---

## 36. Recompilacao e custo de include

Se voce muda um `.cpp`, geralmente so aquele `.cpp` precisa recompilar.

Se voce muda um `.h`, todo `.cpp` que inclui esse header direta ou indiretamente pode precisar recompilar.

Isso e enorme em projetos grandes.

Exemplo:

```text
CoreTypes.h incluido por 200 arquivos
```

Se voce muda `CoreTypes.h`, muitos arquivos recompilam.

Por isso:

- headers devem ser estaveis
- evite includes desnecessarios em headers
- use forward declarations
- coloque implementacao no `.cpp` quando possivel
- mantenha acoplamento baixo

Na UE5, tempo de build pode virar um problema real.

---

## 37. Mini-projeto - Multi-file combat training

Crie uma pasta:

```text
aula05/
```

Dentro dela, crie:

```text
main.cpp
HealthComponent.h
HealthComponent.cpp
Weapon.h
Weapon.cpp
Player.h
Player.cpp
Enemy.h
Enemy.cpp
respostas_aula_05.txt
```

Objetivo:

> transformar o mini-sistema de combate das aulas anteriores em um projeto com headers e sources separados.

### `HealthComponent`

Header:

- `#pragma once`
- classe `HealthComponent`
- construtor
- `ApplyDamage`
- `Heal`
- `IsDead`
- `GetCurrent`
- `GetMax`
- `Clamp` privado
- membros privados

Cpp:

- implementa tudo

### `Weapon`

Header:

- `#pragma once`
- inclui `<string>`
- classe `Weapon`
- construtor
- `Fire`
- `Reload`
- `CanFire`
- `GetDamage`
- `GetName`
- membros privados

Cpp:

- implementa tudo

### `Player`

Header:

- `#pragma once`
- inclui `<string>`
- inclui `"HealthComponent.h"` porque guarda por valor
- forward declaration de `Weapon`
- classe `Player`
- construtor
- `EquipWeapon(Weapon* weapon)`
- `Attack(Enemy& enemy)` ou `Attack(Enemy* enemy)`
- `ReceiveDamage`
- `PrintStatus`
- membros privados

Ponto importante:

Se `Player.h` mencionar `Enemy&` ou `Enemy*`, use forward declaration:

```cpp
class Enemy;
```

Cpp:

- inclui `"Player.h"`
- inclui `"Weapon.h"`
- inclui `"Enemy.h"`
- implementa metodos

### `Enemy`

Header:

- `#pragma once`
- inclui `<string>`
- inclui `"HealthComponent.h"`
- classe `Enemy`
- construtor
- `ReceiveDamage`
- `IsDead`
- `PrintStatus`
- membros privados

Cpp:

- implementa tudo

### `main.cpp`

Inclui:

```cpp
#include "Enemy.h"
#include "Player.h"
#include "Weapon.h"
```

Fluxo:

- cria `Player`
- cria `Enemy`
- cria `Weapon`
- equipa arma no player
- player ataca enemy
- imprime status

---

## 38. Exercicios de erro proposital

Faca estes testes e anote o erro.

### Exercicio 1 - Esquecer include

Remova `#include "Weapon.h"` de `Player.cpp`.

Se `Player.cpp` chama `weapon->Fire()`, veja o erro.

Explique:

> Por que forward declaration no header nao basta para chamar metodo no `.cpp`?

### Exercicio 2 - Esquecer implementacao

Declare em `Weapon.h`:

```cpp
int GetAmmo() const;
```

Chame em algum lugar, mas nao implemente em `Weapon.cpp`.

Veja o erro de linkagem.

Explique:

> Por que o compilador aceitou, mas o linker reclamou?

### Exercicio 3 - Assinatura diferente

No header:

```cpp
void Reload();
```

No cpp, escreva por engano:

```cpp
void Weapon::Reload(int amount)
{
}
```

Explique o erro.

### Exercicio 4 - Multiple definition

Crie uma funcao livre no header sem `inline`:

```cpp
int Add(int a, int b)
{
    return a + b;
}
```

Inclua esse header em dois `.cpp`.

Veja o erro.

Depois corrija movendo a definicao para `.cpp` ou adicionando `inline`.

### Exercicio 5 - Dependencia circular

Faca `Player.h` incluir `Enemy.h` e `Enemy.h` incluir `Player.h`.

Veja o problema.

Depois resolva com forward declarations.

---

## 39. Perguntas obrigatorias

Responda em `respostas_aula_05.txt`:

1. Qual a diferenca entre `.h` e `.cpp`?
2. O que e uma translation unit?
3. O que `#include` faz conceitualmente?
4. Para que serve `#pragma once`?
5. O que o preprocessador faz?
6. Por que macros podem ser perigosas?
7. O que e um simbolo em linkagem?
8. O que significa unresolved external symbol?
9. O que significa multiple definition?
10. Por que nao devemos incluir `.cpp`?
11. O que e forward declaration?
12. Quando forward declaration nao basta?
13. Por que headers com muitos includes aumentam tempo de compilacao?
14. Por que templates geralmente ficam em headers?
15. Por que `using namespace std;` em header e ruim?
16. Como `MyActor.h` e `MyActor.cpp` da UE5 seguem esse modelo?
17. Para que serve `MyActor.generated.h` em alto nivel?
18. Por que o `.generated.h` deve ser o ultimo include no header da Unreal?
19. Quando voce incluiria um header no `.h`?
20. Quando voce preferiria incluir no `.cpp`?

---

## 40. Checklist de dominio

Antes de ir para a Fase 6, voce deve conseguir responder:

- Header declara ou implementa?
- `.cpp` declara ou implementa?
- O que e declaracao?
- O que e definicao?
- O que `#include` realmente faz?
- O que `#pragma once` evita?
- O que include guard evita?
- O que e preprocessador?
- O que macro faz?
- Por que macro nao e igual funcao?
- O que e linkagem?
- O que e unresolved external?
- O que e multiple definition?
- O que e forward declaration?
- Quando preciso do tipo completo?
- Como evitar dependencia circular?
- Por que headers ruins deixam build lento?
- Por que template fica no header?
- Por que Unreal usa `.generated.h`?
- Por que macros da Unreal nao sao enfeite?

Se voce entende isso, esta preparado para entrar na Fase 6 sem achar que os erros de build da Unreal sao aleatorios.

---

## 41. Quando vale ver video

Para esta aula, video ajuda bastante se mostrar fluxo visual:

- "C++ header and cpp files explained"
- "C++ compilation process preprocessing compiling linking"
- "C++ linker errors undefined reference explained"
- "C++ forward declaration explained"
- "C++ include guards pragma once"
- "Unreal Engine generated.h UCLASS UPROPERTY explained"

O video mais importante e sobre:

```text
preprocess -> compile -> link
```

Procure um video que desenhe varios `.cpp` virando arquivos objeto e depois sendo unidos pelo linker.

Se voce entender esse desenho, metade dos erros de C++ fica menos assustadora.

---

## 42. Resumo final

C++ nao compila um projeto inteiro como um bloco unico simples.

Ele compila translation units separadas e depois linka tudo.

Por isso:

- headers expõem declaracoes
- `.cpp` contem implementacoes
- `#include` cola headers antes da compilacao
- `#pragma once` evita inclusao repetida
- preprocessador roda antes do compilador
- macros sao transformacoes antes/ao redor da compilacao normal
- linker resolve simbolos entre arquivos
- declaracao sem definicao gera unresolved external
- definicao duplicada gera multiple definition
- forward declaration reduz dependencia
- headers mal planejados aumentam acoplamento e tempo de build

Para UE5, esta aula prepara voce para entender:

- por que cada classe tem `.h` e `.cpp`
- por que `generated.h` existe
- por que macros como `UCLASS` e `UPROPERTY` participam do build
- por que includes e forward declarations importam
- por que erros de build da Unreal misturam C++, Unreal Header Tool e linker

O ponto mais importante:

> Um header e uma promessa. Um `.cpp` cumpre a promessa. O linker confere se todas as promessas foram cumpridas exatamente uma vez.

---

## 43. Tarefa final da aula

Crie:

```text
aula05/
    main.cpp
    HealthComponent.h
    HealthComponent.cpp
    Weapon.h
    Weapon.cpp
    Player.h
    Player.cpp
    Enemy.h
    Enemy.cpp
    respostas_aula_05.txt
```

Implemente o mini-projeto multi-file.

Depois faca os exercicios de erro proposital.

No `respostas_aula_05.txt`, explique:

1. quais headers cada `.cpp` inclui
2. quais includes poderiam virar forward declaration
3. onde voce precisou de tipo completo
4. qual erro apareceu ao esquecer implementacao
5. qual erro apareceu ao causar multiple definition
6. como voce resolveu dependencia circular
7. como esse modelo aparece em uma classe C++ da UE5

So considere esta aula concluida quando voce conseguir explicar esta frase:

> `#include` nao importa biblioteca como em linguagens dinamicas; ele cola texto antes da compilacao. Depois, cada `.cpp` compila separado, e o linker junta as definicoes.

Se essa frase ficou clara, voce deu um passo enorme para entender C++ em projetos reais e, principalmente, C++ dentro da Unreal Engine.
