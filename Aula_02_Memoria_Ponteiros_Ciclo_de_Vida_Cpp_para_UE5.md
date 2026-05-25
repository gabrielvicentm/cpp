# Aula 02 - Memoria, ponteiros e ciclo de vida em C++ para UE5

Esta aula cobre a Fase 2 do `Roadmap_Cpp_UE5.md`: memoria, ponteiros e ciclo de vida.

Essa e a fase mais importante da base de C++ antes de entrar na Unreal Engine. Se voce entende sintaxe mas nao entende lifetime, ownership e custo de copia, C++ parece imprevisivel. Se voce entende esses conceitos, ate os erros chatos da Unreal ficam mais explicaveis.

O objetivo desta aula e fazer voce enxergar:

- onde um objeto vive
- quando um objeto morre
- quem e responsavel por destruir um recurso
- por que ponteiros podem ficar invalidos
- por que referencias costumam ser mais seguras que ponteiros quando o valor e obrigatorio
- por que copiar objetos pode ser caro
- por que RAII e uma das ideias centrais de C++

Voce nao precisa virar especialista em memoria hoje. Mas precisa sair desta aula com um modelo mental solido.

---

## Como estudar esta aula

Esta aula deve ser estudada com codigo rodando.

Ordem recomendada:

1. Releia rapidamente passagem por valor, referencia e ponteiro da Aula 01.
2. Leia Stack vs Heap com calma.
3. Digite os exemplos pequenos.
4. Rode os exemplos de construtor/destrutor.
5. Faca os exercicios de ponteiro nulo e dangling pointer.
6. Faca o mini-projeto final.
7. Escreva respostas teoricas com suas palavras.

Tempo sugerido:

- 45 min: stack, heap e escopo
- 60 min: ponteiros e referencias
- 60 min: construtor, destrutor e ciclo de vida
- 60 min: ownership, RAII e bugs de memoria
- 60 min: copia, movimento e custo
- 90 min: exercicios e mini-projeto

Se voce so tiver pouco tempo, priorize:

- stack vs heap
- escopo
- ponteiro nulo
- dangling pointer
- RAII
- ownership

Esses conceitos aparecem direto na UE5.

---

## 1. O problema real desta fase

Em C++, uma variavel nao e apenas "um nome com um valor".

Uma variavel tem:

- tipo
- endereco
- tamanho
- tempo de vida
- regras de construcao
- regras de destruicao
- custo de copia
- dono conceitual

Exemplo simples:

```cpp
int health = 100;
```

Parece simples, mas o compilador precisa saber:

- `health` e um `int`
- ele ocupa um certo espaco em memoria
- ele existe dentro de um escopo
- ele sera destruido quando sair desse escopo
- se voce passar para uma funcao por valor, sera copiado

Em C++ isso e central.

Na UE5, voce vai lidar com objetos como:

```cpp
AActor* Enemy;
UCameraComponent* Camera;
UStaticMeshComponent* Mesh;
```

Todos esses ponteiros so fazem sentido quando voce entende:

- o objeto existe?
- quem criou?
- quem destroi?
- esse ponteiro ainda e valido?
- posso guardar isso?
- preciso marcar com `UPROPERTY`?

Ainda nao vamos estudar `UPROPERTY` a fundo, mas esta aula prepara o chao.

---

## 2. Stack vs Heap

C++ permite que objetos existam em regioes diferentes de memoria.

Para agora, pense em duas grandes areas:

- stack
- heap

Essa divisao e uma simplificacao util. O sistema real tem mais detalhes, mas esse modelo ja resolve a maior parte do entendimento inicial.

---

## 3. Stack

A stack e uma regiao de memoria usada para variaveis locais, chamadas de funcao e controle de escopo.

Exemplo:

```cpp
#include <iostream>

void Foo()
{
    int x = 10;
    std::cout << x << "\n";
}

int main()
{
    Foo();
    return 0;
}
```

O `x` existe enquanto `Foo` esta executando.

Quando `Foo` termina, `x` deixa de existir.

Modelo mental:

```text
main chama Foo
    Foo cria x
    Foo usa x
    Foo termina
    x morre
main continua
```

Isso e automatico.

Voce nao precisa destruir `x` manualmente.

### Escopo define lifetime

Escopo e a regiao onde um nome existe.

```cpp
int main()
{
    int a = 10;

    {
        int b = 20;
        std::cout << a << "\n";
        std::cout << b << "\n";
    }

    std::cout << a << "\n";
    // std::cout << b << "\n"; // erro: b nao existe mais aqui

    return 0;
}
```

`b` so existe dentro do bloco.

Quando o bloco termina, `b` morre.

Essa ideia e muito importante:

> objeto local morre ao sair do escopo.

### Stack e rapida

Criar objetos na stack geralmente e rapido porque a alocacao segue uma estrutura simples.

Exemplo:

```cpp
void Update()
{
    float delta = 0.016f;
    int enemiesVisible = 4;
}
```

Essas variaveis locais sao baratas.

Em gameplay, voce vai criar muitas variaveis locais pequenas. Isso e normal.

### Stack tem limite

A stack nao e infinita.

Evite colocar objetos enormes na stack sem pensar.

Exemplo perigoso:

```cpp
void BadIdea()
{
    int hugeArray[10000000];
}
```

Isso pode estourar a stack.

Na pratica moderna, para colecoes grandes voce normalmente usa containers como `std::vector` ou, na Unreal, `TArray`.

---

## 4. Heap

A heap e uma regiao de memoria usada para alocacoes dinamicas.

Em C++ puro, voce pode alocar manualmente com `new`:

```cpp
int* health = new int(100);
```

Aqui:

- `new int(100)` cria um `int` na heap
- retorna o endereco desse `int`
- `health` e um ponteiro para esse endereco

Para destruir:

```cpp
delete health;
health = nullptr;
```

Exemplo completo:

```cpp
#include <iostream>

int main()
{
    int* health = new int(100);

    std::cout << *health << "\n";

    delete health;
    health = nullptr;

    return 0;
}
```

Importante:

> Tudo que voce cria com `new` precisa ter uma estrategia clara de destruicao.

Hoje, em C++ moderno, voce deve evitar `new` e `delete` manuais na maior parte dos casos. Use RAII, containers e smart pointers. Mas entender `new` e `delete` ajuda a entender o problema que essas ferramentas resolvem.

### Heap e mais flexivel, mas custa mais

A heap permite objetos que vivem alem do escopo onde foram criados, mas alocar na heap costuma ser mais caro que criar uma variavel local na stack.

Compare:

```cpp
int x = 10;              // stack
int* y = new int(10);    // heap
delete y;
```

O primeiro e simples.

O segundo envolve pedir memoria ao alocador, guardar endereco e liberar depois.

Em jogos, alocacoes no runtime importam. Alocar e desalocar demais durante gameplay pode afetar performance e gerar pausas ou fragmentacao dependendo do contexto.

Na Unreal, muitos objetos sao criados por sistemas do engine, nao com `new` direto. Por exemplo:

```cpp
GetWorld()->SpawnActor<AEnemy>();
```

Mas a pergunta continua sendo:

> quem controla o lifetime desse objeto?

---

## 5. Stack vs Heap em uma frase

Stack:

- lifetime automatico por escopo
- geralmente mais rapida
- boa para variaveis locais e objetos temporarios
- nao deve guardar coisas enormes sem pensar

Heap:

- lifetime mais flexivel
- requer uma estrategia de ownership
- alocacao geralmente mais cara
- boa para objetos grandes, dinamicos ou que precisam viver alem do escopo atual

Regra inicial:

> Prefira stack quando puder. Use heap quando precisar. Em C++ moderno, use ferramentas que gerenciam heap com RAII.

---

## 6. Ponteiros de verdade

Ponteiro e uma variavel que guarda um endereco.

```cpp
int health = 100;
int* pointerToHealth = &health;
```

Aqui:

- `health` e um `int`
- `&health` pega o endereco de `health`
- `pointerToHealth` guarda esse endereco

Para acessar o valor pelo ponteiro:

```cpp
std::cout << *pointerToHealth << "\n";
```

O `*` nesse contexto significa:

> va ate o endereco guardado e acesse o valor de la.

Exemplo completo:

```cpp
#include <iostream>

int main()
{
    int health = 100;
    int* healthPtr = &health;

    std::cout << "Value: " << health << "\n";
    std::cout << "Address: " << healthPtr << "\n";
    std::cout << "Value through pointer: " << *healthPtr << "\n";

    *healthPtr = 75;

    std::cout << "New value: " << health << "\n";

    return 0;
}
```

Ponto principal:

```cpp
*healthPtr = 75;
```

altera o objeto original.

### `*` tem mais de um papel

Isso confunde no inicio.

Declarando ponteiro:

```cpp
int* p;
```

Aqui `*` faz parte do tipo: `p` e um ponteiro para `int`.

Desreferenciando ponteiro:

```cpp
*p = 10;
```

Aqui `*` acessa o valor apontado.

Leia pelo contexto.

### `&` tambem tem mais de um papel

Pegando endereco:

```cpp
int* p = &health;
```

Aqui `&health` significa endereco de `health`.

Declarando referencia:

```cpp
void Damage(int& health)
```

Aqui `int&` significa referencia para `int`.

Mesmos simbolos, contextos diferentes.

---

## 7. Ponteiro nulo

Um ponteiro pode apontar para nada:

```cpp
int* target = nullptr;
```

`nullptr` significa ausencia de endereco valido.

Antes de desreferenciar um ponteiro que pode ser nulo, verifique:

```cpp
void Heal(int* health)
{
    if (health != nullptr)
    {
        *health += 10;
    }
}
```

Se voce fizer:

```cpp
int* health = nullptr;
*health = 100;
```

o programa tem comportamento invalido. Pode quebrar imediatamente.

### Quando ponteiro nulo faz sentido

Ponteiro nulo e util quando a ausencia e uma possibilidade legitima.

Exemplo:

```cpp
AActor* CurrentTarget = nullptr;
```

No gameplay, isso pode significar:

> ainda nao tenho alvo.

Em C++ puro:

```cpp
struct Enemy
{
    int health = 100;
};

void Attack(Enemy* target)
{
    if (target == nullptr)
    {
        std::cout << "No target\n";
        return;
    }

    target->health -= 10;
}
```

Esse formato fica muito parecido com codigo real de jogo.

---

## 8. O operador `->`

Se voce tem um objeto:

```cpp
Enemy enemy;
enemy.health = 90;
```

usa ponto.

Se voce tem ponteiro para objeto:

```cpp
Enemy* enemyPtr = &enemy;
enemyPtr->health = 90;
```

usa seta.

Isto:

```cpp
enemyPtr->health
```

e equivalente a:

```cpp
(*enemyPtr).health
```

A seta existe porque `(*enemyPtr).health` e feio e facil de errar.

Na UE5 voce vera o tempo todo:

```cpp
Camera->SetupAttachment(RootComponent);
Mesh->SetVisibility(true);
Enemy->Destroy();
```

Leia assim:

> tenho um ponteiro para um objeto, entao acesso membros com `->`.

---

## 9. Referencias novamente, agora com lifetime

Referencia e um apelido para um objeto existente.

```cpp
int health = 100;
int& ref = health;

ref = 50;

std::cout << health << "\n"; // 50
```

Uma referencia comum precisa ser inicializada:

```cpp
int& ref; // erro
```

Ela tambem nao representa ausencia como `nullptr`.

### Referencia como parametro obrigatorio

```cpp
void ApplyDamage(int& health, int damage)
{
    health -= damage;
}
```

Essa funcao comunica:

> eu preciso de uma vida valida e vou altera-la.

### Referencia const como leitura sem copia

```cpp
void PrintName(const std::string& name)
{
    std::cout << name << "\n";
}
```

Essa funcao comunica:

> eu preciso de um nome valido, nao vou copiar e nao vou alterar.

Esse padrao e muito importante:

```cpp
const Tipo& value
```

Use para objetos maiores que voce so quer ler.

---

## 10. Ponteiro vs referencia

Pense assim:

Referencia:

- valor obrigatorio
- sintaxe simples
- nao representa nulo
- boa para parametros que sempre devem existir

Ponteiro:

- pode ser nulo
- guarda endereco explicitamente
- pode apontar para outro objeto
- exige cuidado antes de desreferenciar

Exemplo com referencia:

```cpp
void DamageRequiredTarget(Enemy& enemy)
{
    enemy.health -= 10;
}
```

Aqui nao existe "sem alvo".

Exemplo com ponteiro:

```cpp
void DamageOptionalTarget(Enemy* enemy)
{
    if (enemy == nullptr)
    {
        return;
    }

    enemy->health -= 10;
}
```

Aqui o alvo pode nao existir.

Regra inicial:

> Se precisa existir, prefira referencia. Se pode faltar, use ponteiro ou outro tipo que represente ausencia.

Mais tarde, em C++ moderno, voce tambem usara `std::optional`, smart pointers e referencias com semantica mais especifica.

---

## 11. Dangling pointer

Dangling pointer e um ponteiro que aponta para um objeto que ja morreu.

Esse e um dos bugs mais importantes de C++.

Exemplo perigoso:

```cpp
#include <iostream>

int* CreateBadPointer()
{
    int localHealth = 100;
    return &localHealth; // errado
}

int main()
{
    int* health = CreateBadPointer();

    std::cout << *health << "\n"; // comportamento indefinido

    return 0;
}
```

O problema:

```cpp
int localHealth = 100;
```

`localHealth` vive na stack dentro de `CreateBadPointer`.

Quando a funcao termina, `localHealth` morre.

Mas voce retornou o endereco dele.

O ponteiro fica apontando para uma memoria que nao pertence mais a esse objeto.

Isso e dangling pointer.

### Nao rode exemplos perigosos esperando resposta confiavel

Comportamento indefinido nao significa "sempre vai quebrar".

Pode:

- parecer funcionar
- imprimir lixo
- quebrar
- mudar de comportamento em outra compilacao
- falhar so em producao

Em C++, "parece funcionar" nao e prova de que esta correto.

### Versao correta retornando valor

```cpp
int CreateHealth()
{
    int localHealth = 100;
    return localHealth;
}
```

Aqui retorna uma copia do valor. Correto.

### Versao correta com ownership na heap

Por enquanto, apenas para entender:

```cpp
int* CreateHealthOnHeap()
{
    return new int(100);
}

int main()
{
    int* health = CreateHealthOnHeap();

    std::cout << *health << "\n";

    delete health;
    health = nullptr;

    return 0;
}
```

Funciona, mas exige `delete`.

C++ moderno prefere encapsular isso com RAII, como veremos abaixo.

---

## 12. Memory leak

Memory leak acontece quando voce aloca memoria e perde a chance de liberar.

Exemplo:

```cpp
void Leak()
{
    int* value = new int(10);
}
```

Quando `Leak` termina:

- `value`, o ponteiro local, morre
- mas o `int` alocado na heap continua vivo
- voce perdeu o endereco
- nao consegue mais chamar `delete`

Isso e vazamento de memoria.

Em programas pequenos talvez pareca irrelevante. Em jogos, vazamentos acumulados podem destruir estabilidade.

### Vazamento em loop

```cpp
void BadLoop()
{
    for (int i = 0; i < 1000; ++i)
    {
        int* value = new int(i);
    }
}
```

Isso vaza 1000 alocacoes.

Regra pratica:

> `new` manual deve acender alerta. Onde esta o `delete`? Quem e o dono? O que acontece se houver return antes?

---

## 13. Use-after-free

Use-after-free acontece quando voce usa memoria depois de libera-la.

Exemplo:

```cpp
int* value = new int(10);

delete value;

std::cout << *value << "\n"; // errado
```

Depois de `delete`, o ponteiro ainda contem um endereco, mas o objeto nao existe mais.

Uma pratica defensiva:

```cpp
delete value;
value = nullptr;
```

Isso nao resolve todos os problemas, mas reduz a chance de usar um ponteiro velho no mesmo escopo.

Na UE5, voce tambem precisa lidar com referencias a objetos que podem ter sido destruidos pelo engine. Por isso existem formas especificas de ponteiro e validacao no ecossistema da Unreal.

---

## 14. Double free

Double free acontece quando voce tenta liberar duas vezes o mesmo recurso.

Exemplo:

```cpp
int* value = new int(10);

delete value;
delete value; // errado
```

Isso e comportamento indefinido.

Pode quebrar imediatamente ou corromper memoria.

Esse tipo de bug e um dos motivos pelos quais C++ moderno evita `delete` manual quando possivel.

---

## 15. Construtor e destrutor

Agora vamos introduzir objetos com ciclo de vida visivel.

```cpp
#include <iostream>

struct Tracker
{
    Tracker()
    {
        std::cout << "Constructor\n";
    }

    ~Tracker()
    {
        std::cout << "Destructor\n";
    }
};

int main()
{
    std::cout << "Before block\n";

    {
        Tracker t;
        std::cout << "Inside block\n";
    }

    std::cout << "After block\n";

    return 0;
}
```

Saida esperada:

```text
Before block
Constructor
Inside block
Destructor
After block
```

O construtor roda quando o objeto nasce.

O destrutor roda quando o objeto morre.

### Isso e enorme em C++

Em C++, destruicao nao e apenas "liberar memoria".

Destrutor pode:

- fechar arquivo
- liberar memoria
- soltar lock
- devolver recurso
- avisar sistema
- encerrar conexao
- limpar estado

Essa ideia leva ao RAII.

---

## 16. Ordem de destruicao

Objetos locais sao destruidos na ordem inversa da criacao.

```cpp
#include <iostream>
#include <string>

struct Tracker
{
    std::string name;

    Tracker(const std::string& newName)
        : name(newName)
    {
        std::cout << "Construct " << name << "\n";
    }

    ~Tracker()
    {
        std::cout << "Destroy " << name << "\n";
    }
};

int main()
{
    Tracker a("A");
    Tracker b("B");
    Tracker c("C");

    return 0;
}
```

Saida:

```text
Construct A
Construct B
Construct C
Destroy C
Destroy B
Destroy A
```

Isso importa quando um objeto depende de outro.

Se `B` usa `A`, voce precisa garantir que `A` viva pelo menos enquanto `B` precisar.

---

## 17. RAII

RAII significa:

```text
Resource Acquisition Is Initialization
```

Traduzindo a ideia:

> um recurso deve ser adquirido na construcao do objeto e liberado na destruicao do objeto.

Exemplo didatico:

```cpp
#include <iostream>

struct FakeFile
{
    FakeFile()
    {
        std::cout << "Open file\n";
    }

    ~FakeFile()
    {
        std::cout << "Close file\n";
    }
};

void ProcessFile()
{
    FakeFile file;

    std::cout << "Processing\n";

    if (true)
    {
        return;
    }
}

int main()
{
    ProcessFile();
    return 0;
}
```

Mesmo com `return` antecipado, o destrutor roda.

Saida:

```text
Open file
Processing
Close file
```

Essa e a forca do RAII.

Voce amarra o recurso ao lifetime de um objeto.

Quando o objeto morre, o recurso e liberado automaticamente.

### Por que RAII e tao importante?

Porque evita esquecer limpeza em caminhos diferentes.

Sem RAII:

```cpp
Open();

if (error)
{
    Close();
    return;
}

DoWork();
Close();
```

Com RAII:

```cpp
Resource resource;

if (error)
{
    return;
}

DoWork();
```

O destrutor cuida.

C++ moderno usa essa ideia em muitos lugares:

- `std::string` gerencia memoria interna
- `std::vector` gerencia array dinamico
- `std::unique_ptr` gerencia objeto alocado
- locks gerenciam mutex
- streams gerenciam arquivos

Na Unreal, o modelo e diferente em objetos `UObject`, mas RAII ainda importa para C++ normal, structs, containers, handles e recursos nao gerenciados pelo garbage collector.

---

## 18. Ownership

Ownership significa:

> quem e responsavel por garantir que um recurso seja destruido corretamente?

Esse e um dos conceitos mais importantes de C++.

Exemplo com objeto local:

```cpp
void Foo()
{
    std::string name = "Player";
}
```

`name` e dono da memoria interna da string.

Quando `name` morre, a memoria interna e liberada.

Voce nao chama `delete`.

### Ownership com ponteiro cru

```cpp
int* value = new int(10);
```

Quem e dono?

Nao esta claro pelo tipo.

`int*` pode significar:

- eu sou dono e devo deletar
- eu so estou observando
- o objeto pertence a outro sistema
- pode ser nulo
- pode ficar invalido

Esse e o problema dos raw pointers.

Ponteiro cru comunica endereco, mas nao comunica ownership.

### Ponteiro observador

```cpp
void PrintEnemy(const Enemy* enemy)
{
    if (enemy != nullptr)
    {
        std::cout << enemy->health << "\n";
    }
}
```

Aqui a funcao nao e dona do `enemy`. Ela so observa.

O `const Enemy*` comunica que nao vai alterar o inimigo.

Mas ainda nao comunica quem e dono.

### Dono unico

Em C++ moderno, ownership exclusivo costuma ser expresso com `std::unique_ptr`.

Ainda vamos estudar smart pointers com calma na Aula 04, mas veja a ideia:

```cpp
#include <memory>

std::unique_ptr<int> value = std::make_unique<int>(10);
```

Quando `value` morre, o `int` e destruido.

Nao tem `delete` manual.

Isso e RAII.

---

## 19. Ponteiros crus na UE5

Na Unreal, voce vera muitos ponteiros crus:

```cpp
AActor* Target;
UCameraComponent* Camera;
UStaticMeshComponent* Mesh;
```

Mas cuidado: em Unreal, um ponteiro cru para `UObject` nao deve ser interpretado automaticamente como "eu sou dono e devo deletar".

Geralmente voce nao faz:

```cpp
delete Target;
```

Objetos da Unreal sao gerenciados pelo engine e pelo garbage collector em muitos contextos.

Mais tarde voce vai estudar:

- `UPROPERTY`
- `TObjectPtr`
- `TWeakObjectPtr`
- `IsValid`
- `Destroy`
- `SpawnActor`
- lifecycle de `AActor`

Por enquanto, leve esta regra:

> Em C++ puro, ponteiro cru nao explica ownership. Em UE5, ponteiro para `UObject` tambem depende das regras do engine.

---

## 20. Copia

Copiar um `int` e barato.

```cpp
int a = 10;
int b = a;
```

Agora existem dois inteiros independentes.

Copiar uma `std::string` pode custar mais:

```cpp
std::string a = "A very long name";
std::string b = a;
```

Agora `b` precisa ter seu proprio conteudo.

Copiar um vetor grande pode ser caro:

```cpp
std::vector<int> a = {1, 2, 3, 4, 5};
std::vector<int> b = a;
```

`b` recebe uma copia dos elementos.

### Exemplo visual de copia

```cpp
#include <iostream>
#include <string>

struct PlayerData
{
    std::string name;
    int health;
};

void PrintPlayer(PlayerData player)
{
    std::cout << player.name << " " << player.health << "\n";
}

int main()
{
    PlayerData player{"Knight", 100};

    PrintPlayer(player);

    return 0;
}
```

`PrintPlayer(PlayerData player)` recebe por valor.

Isso copia `PlayerData`.

Melhor para apenas leitura:

```cpp
void PrintPlayer(const PlayerData& player)
{
    std::cout << player.name << " " << player.health << "\n";
}
```

Agora nao copia e nao altera.

---

## 21. Copy constructor e copy assignment

Quando voce cria um objeto novo a partir de outro, acontece construcao por copia.

```cpp
PlayerData a{"Knight", 100};
PlayerData b = a;
```

Conceitualmente, isso usa copy constructor.

Quando voce atribui a um objeto que ja existe, acontece atribuicao por copia.

```cpp
PlayerData a{"Knight", 100};
PlayerData b{"Mage", 80};

b = a;
```

Conceitualmente, isso usa copy assignment.

Vamos observar com logs:

```cpp
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
```

Voce nao precisa decorar operadores hoje. O objetivo e perceber:

- criar `b` a partir de `a` e uma coisa
- atribuir `a` para `c` ja existente e outra
- copias podem executar codigo
- copia nao e sempre "barata"

---

## 22. Movimento

Movimento e uma ideia do C++ moderno para evitar copias desnecessarias quando um objeto temporario ou descartavel pode transferir seus recursos.

Exemplo conceitual:

```cpp
std::string a = "A very long string";
std::string b = std::move(a);
```

Depois disso, `b` pode reaproveitar os recursos de `a`.

`a` continua valido, mas seu conteudo fica em estado especificado apenas como "valido, mas nao conte com o valor antigo".

Regra importante:

> depois de mover de um objeto, voce pode destruir ou reatribuir esse objeto, mas nao deve depender do valor antigo dele.

### Por que isso existe?

Imagine um objeto que possui um buffer grande.

Copiar:

```text
cria outro buffer grande
copia todos os dados
```

Mover:

```text
transfere o ponteiro/recurso interno
evita copiar todo o conteudo
```

Movimento e sobre transferir ownership de recursos internos quando isso e seguro.

Na Aula 04 voce vai estudar `std::move`, `T&&`, lvalue/rvalue e smart pointers com mais calma. Aqui o objetivo e apenas entender que copy e move sao diferentes.

---

## 23. Move constructor e move assignment em nivel conceitual

Copy constructor:

```cpp
Tracker(const Tracker& other)
```

Cria um objeto copiando outro.

Move constructor:

```cpp
Tracker(Tracker&& other)
```

Cria um objeto movendo recursos de outro.

Copy assignment:

```cpp
Tracker& operator=(const Tracker& other)
```

Copia para um objeto que ja existe.

Move assignment:

```cpp
Tracker& operator=(Tracker&& other)
```

Move para um objeto que ja existe.

Nao tente dominar tudo agora. O que voce precisa gravar:

- copia duplica valor/recurso
- movimento transfere recurso quando possivel
- movimento existe para performance e ownership
- depois de `std::move`, nao conte com o conteudo antigo do objeto movido

---

## 24. Aritmetica de ponteiros em nivel conceitual

Ponteiros podem ser incrementados para apontar para o proximo elemento de um array.

Exemplo:

```cpp
#include <iostream>

int main()
{
    int values[3] = {10, 20, 30};

    int* p = values;

    std::cout << *p << "\n";       // 10
    std::cout << *(p + 1) << "\n"; // 20
    std::cout << *(p + 2) << "\n"; // 30

    return 0;
}
```

`p + 1` nao significa "endereco + 1 byte".

Significa:

> avance para o proximo `int`.

Se `int` ocupa 4 bytes, o endereco real avanca 4 bytes.

### Por que isso e perigoso?

Porque C++ nao impede automaticamente voce de ir alem do array.

```cpp
std::cout << *(p + 99) << "\n"; // errado
```

Isso e comportamento indefinido.

Em C++ moderno, voce vai preferir containers:

```cpp
std::vector<int> values = {10, 20, 30};
```

Na UE5:

```cpp
TArray<int32> Values;
```

Mas entender aritmetica de ponteiros ajuda a entender por que C++ e poderoso e perigoso.

---

## 25. Arrays, ponteiros e tamanho

Array cru:

```cpp
int values[3] = {10, 20, 30};
```

Em muitos contextos, o nome do array "decai" para ponteiro para o primeiro elemento.

```cpp
int* p = values;
```

Mas o ponteiro nao sabe o tamanho do array.

```cpp
void PrintValues(int* values)
{
    // Quantos elementos existem?
}
```

Nao da para saber apenas pelo ponteiro.

Por isso, funcoes antigas recebem ponteiro + tamanho:

```cpp
void PrintValues(const int* values, int count)
{
    for (int i = 0; i < count; ++i)
    {
        std::cout << values[i] << "\n";
    }
}
```

Em C++ moderno, containers carregam tamanho:

```cpp
std::vector<int> values = {10, 20, 30};
std::cout << values.size() << "\n";
```

Na UE5, `TArray` tambem sabe quantos elementos tem:

```cpp
Values.Num();
```

---

## 26. Boas decisoes de parametro

Depois desta aula, voce deve justificar cada parametro.

### Tipo pequeno, so leitura

```cpp
bool IsAlive(int health)
{
    return health > 0;
}
```

Passar `int` por valor e bom.

### Tipo grande, so leitura

```cpp
void PrintPlayer(const PlayerData& player)
{
}
```

Passar por `const&` evita copia.

### Precisa alterar original

```cpp
void ApplyDamage(PlayerData& player, int damage)
{
    player.health -= damage;
}
```

Referencia nao-const.

### Pode estar ausente

```cpp
void ApplyDamageIfValid(PlayerData* player, int damage)
{
    if (player == nullptr)
    {
        return;
    }

    player->health -= damage;
}
```

Ponteiro.

### Precisa transferir ownership

Mais tarde:

```cpp
void StoreEnemy(std::unique_ptr<Enemy> enemy)
{
}
```

Isso comunica transferencia de dono. Ainda nao precisa usar agora, mas guarde a ideia.

---

## 27. Relacao direta com UE5

Na UE5, voce vai encontrar codigo parecido com:

```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}
```

E:

```cpp
void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
```

`DeltaTime` e passado por valor porque `float` e pequeno.

Voce tambem vera:

```cpp
void SetTarget(AActor* NewTarget);
```

Aqui ponteiro faz sentido porque talvez nao exista alvo.

E:

```cpp
void ConfigureWeapon(const FWeaponData& WeaponData);
```

Aqui `const&` faz sentido porque dados de arma podem ser maiores, e a funcao so precisa ler.

### Objetos locais em funcoes da Unreal

```cpp
void AMyActor::SomeFunction()
{
    FVector Direction = FVector(1.0f, 0.0f, 0.0f);
}
```

`Direction` e local. Morre ao sair da funcao.

Se voce guardar ponteiro para isso e usar depois, esta errado.

### Actor destruido

Imagine:

```cpp
AActor* Target;
```

Se o actor for destruido pelo engine, seu ponteiro pode nao ser mais seguro de usar.

Por isso, em Unreal, voce aprende validacao e tipos proprios. Mas o conceito base e C++ puro:

> ponteiro pode apontar para algo que nao existe mais.

---

## 28. Quando vale ver video

Para esta aula, video pode ajudar muito. Procure videos com visualizacao de memoria sobre:

- "C++ stack vs heap visual explanation"
- "C++ pointers and references visual explanation"
- "C++ RAII explained"
- "C++ copy constructor move constructor explained visually"
- "C++ dangling pointer use after free memory leak"

O video mais importante para hoje e sobre stack vs heap com desenho de memoria.

O segundo mais importante e sobre ponteiros/referencias.

Nao assista como entretenimento. Enquanto assiste, pause e desenhe:

```text
variavel -> endereco -> valor -> lifetime
```

Se o video nao mostrar memoria, endereco ou escopo, talvez seja superficial demais para esta fase.

---

## 29. Exercicios teoricos

Responda com suas palavras, sem procurar resposta pronta:

1. Por que uma variavel local morre ao sair da funcao?
2. Por que retornar ponteiro para variavel local e errado?
3. Qual a diferenca entre ponteiro nulo e dangling pointer?
4. Por que `delete` duas vezes e perigoso?
5. Por que copiar uma `std::string` pode ser mais caro que copiar um `int`?
6. O que RAII tenta resolver?
7. O que significa ownership?
8. Um `int*` sozinho diz quem deve chamar `delete`?
9. Quando voce escolheria referencia em vez de ponteiro?
10. Quando voce escolheria ponteiro em vez de referencia?

Se voce nao conseguir responder 2, 3, 6 e 7, revise antes de avancar.

---

## 30. Exercicios praticos

### Exercicio 1 - Escopo

Escreva:

```cpp
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

    return 0;
}
```

Depois tente imprimir `inner` fora do bloco.

Pergunta:

> O erro acontece em compilacao ou runtime?

Resposta esperada:

> Compilacao. O nome `inner` nao existe naquele escopo.

### Exercicio 2 - Ponteiro basico

Crie:

```cpp
int health = 100;
int* healthPtr = &health;
```

Use o ponteiro para:

- imprimir endereco
- imprimir valor
- alterar vida para 70
- imprimir `health` diretamente

Explique por que alterar `*healthPtr` altera `health`.

### Exercicio 3 - Ponteiro nulo

Implemente:

```cpp
void PrintHealth(const int* health)
{
}
```

Regras:

- se `health == nullptr`, imprima `"No health"`
- senao, imprima o valor apontado
- nao altere a vida

Pergunta:

> Por que o parametro e `const int*`?

Resposta esperada:

> Porque a funcao recebe um ponteiro, mas nao deve modificar o `int` apontado.

### Exercicio 4 - Referencia obrigatoria

Implemente:

```cpp
void ApplyDamage(int& health, int damage)
{
}
```

Regras:

- subtrair dano
- nao deixar abaixo de 0

Pergunta:

> Por que referencia faz sentido aqui?

Resposta esperada:

> Porque a funcao exige uma vida valida e precisa alterar o original.

### Exercicio 5 - Ponteiro opcional

Implemente:

```cpp
void ApplyDamageIfValid(int* health, int damage)
{
}
```

Regras:

- se ponteiro for nulo, nao faca nada
- se for valido, aplique dano
- nao deixe abaixo de 0

Pergunta:

> Qual diferenca de intencao entre essa funcao e `ApplyDamage(int& health, int damage)`?

### Exercicio 6 - Construtor e destrutor

Crie:

```cpp
struct LifeLogger
{
    LifeLogger()
    {
        std::cout << "Born\n";
    }

    ~LifeLogger()
    {
        std::cout << "Destroyed\n";
    }
};
```

Use dentro de blocos diferentes e observe a ordem dos logs.

### Exercicio 7 - Ordem de destruicao

Crie tres objetos:

```cpp
LifeLogger a("A");
LifeLogger b("B");
LifeLogger c("C");
```

Adapte o construtor para receber nome.

Veja a ordem de construcao e destruicao.

Explique por que destrucao acontece de tras para frente.

### Exercicio 8 - Vazamento proposital, depois correcao

Leia este codigo:

```cpp
void Bad()
{
    int* value = new int(10);
}
```

Nao precisa rodar.

Explique:

- onde esta o vazamento?
- o que morreu?
- o que continuou vivo?
- por que nao da mais para chamar `delete`?

Depois escreva a versao com `delete`.

### Exercicio 9 - Evitando copia

Crie:

```cpp
struct PlayerData
{
    std::string name;
    int health;
    int stamina;
};
```

Implemente:

```cpp
void PrintPlayer(PlayerData player);
void PrintPlayerBetter(const PlayerData& player);
```

Perguntas:

1. Qual copia?
2. Qual nao copia?
3. Qual protege contra alteracao?

### Exercicio 10 - Copy logs

Use o exemplo de `Tracker` com copy constructor e copy assignment.

Depois crie uma funcao:

```cpp
void ReceiveByValue(Tracker tracker)
{
}
```

Chame:

```cpp
Tracker a("A");
ReceiveByValue(a);
```

Observe se ocorre copia.

Depois mude para:

```cpp
void ReceiveByConstReference(const Tracker& tracker)
{
}
```

Observe a diferenca.

---

## 31. Mini-projeto - Combat memory training

Crie um arquivo chamado:

```text
combat_memory_training.cpp
```

Objetivo:

Simular um pequeno sistema de combate usando stack, ponteiros, referencias, construtor, destrutor e decisoes corretas de parametro.

### Estrutura obrigatoria

```cpp
struct Combatant
{
    std::string name;
    int health;
    int stamina;

    Combatant(const std::string& newName, int newHealth, int newStamina)
        : name(newName), health(newHealth), stamina(newStamina)
    {
        std::cout << "Spawned " << name << "\n";
    }

    ~Combatant()
    {
        std::cout << "Destroyed " << name << "\n";
    }
};
```

### Funcoes obrigatorias

```cpp
void PrintCombatant(const Combatant& combatant);
bool IsAlive(const Combatant& combatant);
void ApplyDamage(Combatant& combatant, int damage);
void SpendStamina(Combatant& combatant, int amount);
void Attack(Combatant& attacker, Combatant* target);
Combatant* SelectTarget(Combatant* first, Combatant* second);
```

### Regras

`PrintCombatant`:

- recebe por `const&`
- nao altera o objeto
- imprime nome, vida e stamina

`IsAlive`:

- recebe por `const&`
- retorna `combatant.health > 0`

`ApplyDamage`:

- recebe por referencia
- altera o original
- nao deixa vida abaixo de 0

`SpendStamina`:

- recebe por referencia
- altera o original
- nao deixa stamina abaixo de 0

`Attack`:

- recebe atacante por referencia
- recebe alvo por ponteiro
- se alvo for `nullptr`, imprime `"No target"`
- se atacante nao tiver stamina, imprime `"No stamina"`
- se puder atacar, gasta stamina e aplica dano no alvo

`SelectTarget`:

- recebe dois ponteiros
- retorna o primeiro que estiver vivo
- se nenhum estiver vivo, retorna `nullptr`

### Esqueleto sugerido

```cpp
#include <iostream>
#include <string>

struct Combatant
{
    std::string name;
    int health;
    int stamina;

    Combatant(const std::string& newName, int newHealth, int newStamina)
        : name(newName), health(newHealth), stamina(newStamina)
    {
        std::cout << "Spawned " << name << "\n";
    }

    ~Combatant()
    {
        std::cout << "Destroyed " << name << "\n";
    }
};

void PrintCombatant(const Combatant& combatant)
{
    std::cout << combatant.name
              << " | Health: " << combatant.health
              << " | Stamina: " << combatant.stamina
              << "\n";
}

bool IsAlive(const Combatant& combatant)
{
    return combatant.health > 0;
}

void ApplyDamage(Combatant& combatant, int damage)
{
    combatant.health -= damage;

    if (combatant.health < 0)
    {
        combatant.health = 0;
    }
}

void SpendStamina(Combatant& combatant, int amount)
{
    combatant.stamina -= amount;

    if (combatant.stamina < 0)
    {
        combatant.stamina = 0;
    }
}

void Attack(Combatant& attacker, Combatant* target)
{
    if (target == nullptr)
    {
        std::cout << attacker.name << " has no target\n";
        return;
    }

    if (attacker.stamina <= 0)
    {
        std::cout << attacker.name << " has no stamina\n";
        return;
    }

    std::cout << attacker.name << " attacks " << target->name << "\n";

    SpendStamina(attacker, 10);
    ApplyDamage(*target, 25);
}

Combatant* SelectTarget(Combatant* first, Combatant* second)
{
    if (first != nullptr && IsAlive(*first))
    {
        return first;
    }

    if (second != nullptr && IsAlive(*second))
    {
        return second;
    }

    return nullptr;
}

int main()
{
    Combatant player("Player", 100, 30);
    Combatant enemyA("EnemyA", 50, 10);
    Combatant enemyB("EnemyB", 50, 10);

    for (int turn = 0; turn < 5; ++turn)
    {
        std::cout << "\nTurn " << turn + 1 << "\n";

        Combatant* target = SelectTarget(&enemyA, &enemyB);
        Attack(player, target);

        PrintCombatant(player);
        PrintCombatant(enemyA);
        PrintCombatant(enemyB);
    }

    return 0;
}
```

### Perguntas obrigatorias depois de terminar

Responda no seu caderno ou em um `.txt`:

1. Quais objetos estao na stack?
2. Quando `player`, `enemyA` e `enemyB` sao destruidos?
3. Por que `Attack` recebe `Combatant& attacker`?
4. Por que `Attack` recebe `Combatant* target`?
5. Por que `SelectTarget` retorna `Combatant*`?
6. O que aconteceria se `SelectTarget` retornasse endereco de uma variavel local?
7. Onde existe risco de ponteiro nulo?
8. Onde usamos `const&` para evitar copia?
9. Onde alteramos o objeto original?
10. Qual parte deste mini-projeto lembra codigo de gameplay na UE5?

---

## 32. Desafio extra - arena com escopo

Crie uma funcao:

```cpp
void RunArena()
{
    Combatant player("Player", 100, 30);
    Combatant enemy("Enemy", 50, 10);

    Attack(player, &enemy);
}
```

No `main`:

```cpp
int main()
{
    std::cout << "Before arena\n";
    RunArena();
    std::cout << "After arena\n";

    return 0;
}
```

Observe quando os destrutores rodam.

Pergunta:

> Por que `player` e `enemy` nao existem depois que `RunArena` termina?

Essa pergunta parece simples, mas e o coracao do lifetime.

---

## 33. Desafio extra - nao copie sem querer

Adicione copy constructor em `Combatant`:

```cpp
Combatant(const Combatant& other)
    : name(other.name), health(other.health), stamina(other.stamina)
{
    std::cout << "Copied " << name << "\n";
}
```

Depois crie uma funcao ruim:

```cpp
void BadPrint(Combatant combatant)
{
    std::cout << combatant.name << "\n";
}
```

Chame:

```cpp
BadPrint(player);
```

Veja o log de copia.

Depois compare com:

```cpp
void GoodPrint(const Combatant& combatant)
{
    std::cout << combatant.name << "\n";
}
```

Objetivo:

> aprender a perceber copia invisivel.

Na UE5, copiar structs grandes ou containers sem perceber pode custar caro.

---

## 34. Checklist de dominio

Antes de ir para orientacao a objetos em C++, voce deve conseguir responder sem decorar:

- O que e stack?
- O que e heap?
- Quando uma variavel local morre?
- O que um ponteiro guarda?
- O que `&x` faz?
- O que `*p` faz?
- O que `nullptr` representa?
- Por que ponteiro nulo deve ser verificado?
- O que e dangling pointer?
- O que e memory leak?
- O que e use-after-free?
- O que e double free?
- O que construtor faz?
- O que destrutor faz?
- Em que ordem objetos locais sao destruidos?
- O que RAII resolve?
- O que ownership significa?
- Por que `int*` nao comunica ownership?
- Quando usar `T&`?
- Quando usar `const T&`?
- Quando usar `T*`?
- Por que copiar objeto grande pode ser caro?
- Qual a diferenca conceitual entre copy e move?

Se voce responder isso bem, voce esta construindo a parte mais importante da base para Unreal.

---

## 35. Resumo final

A Aula 01 te ensinou a escrever e ler C++ basico.

Esta Aula 02 te ensina a pensar como C++ pensa:

- objetos existem em algum lugar
- objetos nascem e morrem
- escopo controla lifetime automatico
- ponteiros guardam enderecos
- referencias sao apelidos para objetos existentes
- ponteiros podem ser nulos ou invalidos
- `new` sem dono claro e perigoso
- `delete` manual exige disciplina
- RAII amarra recurso a objeto
- ownership responde quem limpa o recurso
- copia tem custo real
- movimento existe para transferir recursos sem copia desnecessaria

Para UE5, o mais importante desta aula e:

> todo objeto tem lifetime, todo ponteiro precisa ser interpretado, e ownership nunca deve ser assumido sem entender quem criou e quem destroi.

Quando voce chegar em `AActor*`, `UObject`, `UPROPERTY`, `TObjectPtr`, `TWeakObjectPtr`, `SpawnActor` e `Destroy`, esta aula vai ser a base mental que impede voce de decorar sintaxe sem entender o que esta acontecendo.

---

## 36. Tarefa final da aula

Crie dois arquivos:

```text
combat_memory_training.cpp
respostas_aula_02.txt
```

No `.cpp`, implemente o mini-projeto da arena.

No `.txt`, responda:

1. O que vive na stack no seu programa?
2. Voce usou heap diretamente? Se nao, por que isso e bom neste momento?
3. Onde existe ponteiro?
4. Onde existe referencia?
5. Onde existe `const&`?
6. Onde poderia haver ponteiro nulo?
7. Onde um dangling pointer poderia aparecer se voce programasse errado?
8. Onde acontece destruicao automatica?
9. Onde aconteceria copia se voce removesse `const&`?
10. Como esse exercicio prepara voce para entender ponteiros de Actors na UE5?

Se voce conseguir responder isso com clareza, voce aprendeu a parte que mais separa C++ de linguagens gerenciadas.
