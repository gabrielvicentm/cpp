# Aula 04 - C++ moderno, STL, smart pointers e move semantics para UE5

Esta aula cobre a Fase 4 do `Roadmap_Cpp_UE5.md`: C++ moderno.

Ate agora voce estudou:

- Aula 01: sintaxe, tipos, funcoes, referencias e ponteiros
- Aula 02: memoria, stack, heap, lifetime, RAII e ownership
- Aula 03: orientacao a objetos, encapsulamento, heranca, `virtual` e composicao

Agora a meta e sair do C++ "manual demais" e entrar no C++ moderno que voce deve preferir em codigo real.

C++ moderno nao elimina custo, memoria ou ownership. Ele te da ferramentas melhores para expressar essas ideias com menos risco.

O objetivo desta aula e voce entender:

- `const` com mais rigor
- lvalue e rvalue em nivel pratico
- `T&`, `const T&` e `T&&`
- `std::move`
- copy vs move
- `std::unique_ptr`
- `std::shared_ptr`
- `std::weak_ptr`
- `std::vector`
- `std::string`
- `std::array`
- `std::unordered_map`
- `std::optional`
- templates em nivel introdutorio
- lambdas
- tratamento de erro em C++ moderno
- o que disso se aplica diretamente a UE5 e o que precisa cuidado

---

## Como estudar esta aula

Esta aula e grande porque C++ moderno e um conjunto de ferramentas. Nao tente decorar tudo de uma vez.

Estude nesta ordem:

1. Reforce `const`, referencia e ownership.
2. Entenda copy vs move conceitualmente.
3. Estude `std::vector`, `std::string` e `std::optional`.
4. Estude `std::unique_ptr`.
5. Depois veja `std::shared_ptr` e `std::weak_ptr`.
6. Estude lambdas.
7. Termine com templates.

Tempo sugerido:

- 45 min: `const`, lvalue/rvalue e `std::move`
- 75 min: containers da STL
- 75 min: smart pointers
- 45 min: lambdas
- 60 min: templates introdutorios
- 45 min: tratamento de erro
- 90 min: exercicios e mini-projeto

Se o tempo estiver curto, priorize:

- `std::vector`
- `std::string`
- `std::optional`
- `std::unique_ptr`
- `std::move`
- lambdas basicas

Esses conceitos vao aparecer constantemente quando voce ler C++ moderno e tambem vao ajudar a entender tipos equivalentes da Unreal.

---

## 1. O que significa C++ moderno

C++ moderno normalmente se refere ao estilo de C++ depois do C++11.

Ele inclui recursos como:

- move semantics
- smart pointers
- `auto` mais util
- lambdas
- `nullptr`
- `std::optional`
- melhorias na STL
- range-based for
- inicializacao uniforme
- templates mais expressivos

O ponto nao e usar recurso novo porque e novo.

O ponto e escrever codigo que:

- deixa ownership claro
- evita `new` e `delete` manuais
- evita copias desnecessarias
- usa containers seguros
- expressa ausencia de valor melhor
- reduz chance de vazamento de memoria
- comunica intencao ao compilador e ao leitor

Em C++ antigo, voce veria muito:

```cpp
Enemy* enemy = new Enemy();
// ...
delete enemy;
```

Em C++ moderno, voce prefere:

```cpp
auto enemy = std::make_unique<Enemy>();
```

O objeto ainda esta na heap. A diferenca e que agora existe ownership explicito e destruicao automatica.

---

## 2. `const` com mais rigor

Voce ja viu `const`, mas agora precisa tratar como ferramenta de design.

`const` comunica:

> esta coisa nao deve ser modificada por este caminho.

### Variavel constante

```cpp
const int MaxHealth = 100;
```

Depois de inicializada, nao muda.

### Parametro por valor constante

```cpp
void PrintDamage(const int damage)
{
    std::cout << damage << "\n";
}
```

Isso impede alterar `damage` dentro da funcao, mas como `int` foi copiado, nao muda nada para quem chamou.

E aceitavel, mas nao e tao importante quanto `const&`.

### Parametro por referencia constante

```cpp
void PrintName(const std::string& name)
{
    std::cout << name << "\n";
}
```

Isso e muito importante.

Significa:

- nao copia a string
- nao altera a string original
- exige que exista uma string valida

Padrao muito comum:

```cpp
void Foo(const BigType& value);
```

### Metodo constante

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

O `const` depois dos parenteses significa que o metodo nao altera o objeto.

### Ponteiro para constante

```cpp
const int* value;
```

Leia:

> ponteiro para int constante.

Voce pode mudar para onde o ponteiro aponta, mas nao pode alterar o valor apontado por esse ponteiro.

```cpp
int a = 10;
int b = 20;

const int* p = &a;
p = &b;   // ok
*p = 30;  // erro
```

### Ponteiro constante

```cpp
int* const value = &someInt;
```

Leia:

> ponteiro constante para int.

Voce pode alterar o valor apontado, mas nao pode mudar o ponteiro para outro endereco.

```cpp
int a = 10;
int b = 20;

int* const p = &a;
*p = 30; // ok
p = &b;  // erro
```

### Ponteiro constante para constante

```cpp
const int* const value = &someInt;
```

Nao pode mudar o valor por esse ponteiro e nao pode mudar para onde o ponteiro aponta.

Nao precisa usar isso toda hora, mas precisa conseguir ler.

---

## 3. Lvalue e rvalue em nivel pratico

Esse assunto pode ficar profundo demais. Hoje voce precisa de uma versao pratica.

### Lvalue

Um lvalue e algo que tem identidade e geralmente tem nome/endereco estavel.

```cpp
int x = 10;
```

`x` e lvalue.

Voce pode pegar o endereco:

```cpp
int* p = &x;
```

### Rvalue

Um rvalue e geralmente um valor temporario.

```cpp
int x = 10 + 20;
```

`10 + 20` produz um valor temporario.

Outro exemplo:

```cpp
std::string GetName()
{
    return "Player";
}

std::string name = GetName();
```

O resultado de `GetName()` e temporario.

### Por que isso importa?

Porque C++ moderno consegue tratar temporarios de forma mais eficiente.

Se um objeto temporario vai morrer, talvez seus recursos internos possam ser movidos em vez de copiados.

Essa e a base de move semantics.

---

## 4. `T&`, `const T&` e `T&&`

### `T&`

Referencia nao constante.

Use quando quer alterar o objeto original.

```cpp
void ApplyDamage(Health& health, int amount)
{
    health.ApplyDamage(amount);
}
```

### `const T&`

Referencia constante.

Use quando quer ler sem copiar.

```cpp
void PrintPlayer(const Player& player)
{
    player.PrintStatus();
}
```

### `T&&`

Referencia de rvalue.

Ela aparece quando uma funcao aceita um objeto temporario ou um objeto que pode ter seus recursos movidos.

Exemplo conceitual:

```cpp
void SetName(std::string&& newName)
{
    name = std::move(newName);
}
```

Nao tente usar `T&&` em todo lugar agora. No inicio, o mais importante e reconhecer que:

- `T&` geralmente altera objeto existente
- `const T&` le objeto existente sem copiar
- `T&&` participa de move semantics

---

## 5. Copy vs move

Copiar duplica conteudo.

Mover transfere recurso quando possivel.

Exemplo:

```cpp
std::string a = "A long player name";
std::string b = a; // copia
```

Depois disso:

- `a` tem o texto
- `b` tem uma copia do texto

Agora:

```cpp
std::string a = "A long player name";
std::string b = std::move(a); // move
```

Depois disso:

- `b` recebeu o conteudo
- `a` continua valido, mas voce nao deve depender do valor antigo

Regra importante:

> objeto movido continua valido, mas seu conteudo anterior nao deve ser assumido.

Voce pode:

```cpp
a = "New name";
```

Mas nao deve escrever logica que dependa de `a` ainda conter `"A long player name"` depois do move.

---

## 6. `std::move`

`std::move` nao move sozinho.

Ele converte uma expressao para uma forma que permite movimento.

Exemplo:

```cpp
std::string source = "Player";
std::string target = std::move(source);
```

`std::move(source)` diz:

> source pode ser tratado como movivel.

Quem realmente move e o construtor/operador de movimento do tipo.

### Erro comum

```cpp
std::string name = "Player";
std::cout << name << "\n";

std::string other = std::move(name);

std::cout << name << "\n"; // evite depender disso
```

Depois de mover, use `name` apenas para:

- destruir
- receber novo valor
- chamar metodos que aceitam estado valido mas indeterminado

### Quando usar `std::move`

Use quando voce quer transferir recurso e nao precisa mais do valor antigo.

Exemplo:

```cpp
class Player
{
public:
    void SetName(std::string newName)
    {
        name = std::move(newName);
    }

private:
    std::string name;
};
```

Aqui `newName` e uma copia/valor local da funcao. Mover dele para `name` faz sentido porque `newName` vai morrer ao sair da funcao.

---

## 7. STL

STL significa Standard Template Library.

Na pratica, quando falamos de STL, falamos de containers e algoritmos da biblioteca padrao.

Tipos centrais:

- `std::string`
- `std::vector`
- `std::array`
- `std::unordered_map`
- `std::optional`

Na Unreal existem tipos proprios:

- `FString`
- `TArray`
- `TMap`
- `TSet`
- `TOptional`

Mas aprender STL primeiro e excelente porque ensina o modelo moderno de containers, ownership e custo.

---

## 8. `std::string`

`std::string` representa texto.

```cpp
#include <string>

std::string name = "Player";
```

Concatenacao:

```cpp
std::string first = "Player";
std::string full = first + " One";
```

Tamanho:

```cpp
std::cout << name.size() << "\n";
```

Comparacao:

```cpp
if (name == "Player")
{
}
```

Passagem para funcao:

```cpp
void PrintName(const std::string& name)
{
    std::cout << name << "\n";
}
```

Se voce passar por valor, copia:

```cpp
void BadPrint(std::string name)
{
}
```

Use por valor quando voce realmente quer uma copia ou quando vai mover para dentro de um objeto.

### Relacao com UE5

Na Unreal voce vera:

- `FString`: texto mutavel geral
- `FName`: identificador/nome eficiente para comparacao
- `FText`: texto localizado para UI

`std::string` e importante para C++ puro, mas em codigo UE5 voce frequentemente usa os tipos da engine.

---

## 9. `std::vector`

`std::vector` e um array dinamico.

Ele guarda elementos contiguos na memoria e pode crescer.

```cpp
#include <vector>

std::vector<int> damages = {10, 20, 30};
```

Adicionar:

```cpp
damages.push_back(40);
```

Tamanho:

```cpp
std::cout << damages.size() << "\n";
```

Acesso:

```cpp
std::cout << damages[0] << "\n";
```

Iteracao:

```cpp
for (int damage : damages)
{
    std::cout << damage << "\n";
}
```

Para tipos grandes:

```cpp
for (const Player& player : players)
{
    player.PrintStatus();
}
```

Ou:

```cpp
for (const auto& player : players)
{
    player.PrintStatus();
}
```

### `push_back` e `emplace_back`

`push_back` adiciona um objeto existente ou temporario:

```cpp
std::vector<std::string> names;
names.push_back("Player");
```

`emplace_back` constroi o objeto diretamente dentro do vector:

```cpp
names.emplace_back("Player");
```

Para tipos simples, a diferenca pratica pode ser pequena. Para objetos mais complexos, `emplace_back` pode evitar construcoes intermediarias.

### Cuidado com referencias e ponteiros para elementos

`std::vector` pode realocar memoria quando cresce.

Exemplo perigoso:

```cpp
std::vector<int> values;
values.push_back(10);

int* p = &values[0];

values.push_back(20);
values.push_back(30);
values.push_back(40);

// p pode estar invalido se o vector realocou
```

Quando o vector precisa crescer alem da capacidade atual, ele pode mover os elementos para outro bloco de memoria.

Isso invalida ponteiros/referencias/iteradores antigos para elementos.

Esse e um ponto muito importante para jogos.

### `reserve`

Se voce sabe aproximadamente quantos elementos vai ter:

```cpp
std::vector<int> values;
values.reserve(100);
```

Isso reduz realocacoes.

### Relacao com UE5

Na Unreal, o equivalente comum e:

```cpp
TArray<int32> Values;
```

`TArray` tambem pode realocar e invalidar ponteiros/referencias para elementos.

O conceito aprendido com `std::vector` continua util.

---

## 10. `std::array`

`std::array` representa array de tamanho fixo conhecido em compile time.

```cpp
#include <array>

std::array<int, 3> damages = {10, 20, 30};
```

Tamanho:

```cpp
damages.size();
```

Iteracao:

```cpp
for (int damage : damages)
{
    std::cout << damage << "\n";
}
```

Use `std::array` quando:

- tamanho e fixo
- voce quer semantica melhor que array cru
- nao precisa crescer dinamicamente

Exemplo:

```cpp
std::array<float, 3> position = {0.0f, 1.0f, 2.0f};
```

Na UE5, para vetores matematicos voce normalmente usa tipos proprios como `FVector`, nao `std::array<float, 3>`.

---

## 11. `std::unordered_map`

`std::unordered_map` e uma tabela hash: associa chave a valor.

```cpp
#include <unordered_map>
#include <string>

std::unordered_map<std::string, int> inventory;

inventory["Potion"] = 3;
inventory["Arrow"] = 20;
```

Acesso:

```cpp
std::cout << inventory["Potion"] << "\n";
```

Cuidado: `operator[]` cria valor se a chave nao existir.

```cpp
std::cout << inventory["Unknown"] << "\n";
```

Isso cria `"Unknown"` com valor padrao `0`.

Para verificar sem criar:

```cpp
auto it = inventory.find("Potion");

if (it != inventory.end())
{
    std::cout << it->second << "\n";
}
```

Iteracao:

```cpp
for (const auto& pair : inventory)
{
    std::cout << pair.first << ": " << pair.second << "\n";
}
```

Em C++17, voce pode usar structured binding:

```cpp
for (const auto& [itemName, count] : inventory)
{
    std::cout << itemName << ": " << count << "\n";
}
```

### Relacao com UE5

Na Unreal, o equivalente comum e:

```cpp
TMap<FName, int32> Inventory;
```

O conceito e o mesmo:

> chave aponta para valor.

---

## 12. `std::optional`

`std::optional<T>` representa:

> talvez exista um `T`, talvez nao.

Inclua:

```cpp
#include <optional>
```

Exemplo:

```cpp
std::optional<int> FindAmmo(const std::string& weaponName)
{
    if (weaponName == "Rifle")
    {
        return 30;
    }

    return std::nullopt;
}
```

Uso:

```cpp
auto ammo = FindAmmo("Rifle");

if (ammo.has_value())
{
    std::cout << ammo.value() << "\n";
}
```

Ou:

```cpp
if (ammo)
{
    std::cout << *ammo << "\n";
}
```

### Por que isso e melhor que valor sentinela?

Ruim:

```cpp
int FindIndex()
{
    return -1; // significa nao encontrado
}
```

O `-1` e uma convencao. Alguem pode esquecer.

Melhor:

```cpp
std::optional<int> FindIndex()
{
    return std::nullopt;
}
```

Agora o tipo diz que pode nao existir valor.

### Optional vs ponteiro

Use `std::optional<T>` quando a ausencia e do valor em si.

Use ponteiro quando:

- voce quer referenciar objeto existente em outro lugar
- o objeto pode ser nulo
- voce nao quer copiar
- existe relacao com lifetime externo

Exemplo:

```cpp
std::optional<int> ammoCount; // talvez tenha numero
Enemy* target = nullptr;      // talvez aponte para um inimigo existente
```

### Relacao com UE5

Unreal tem `TOptional`.

Em codigo da engine, voce pode ver:

```cpp
TOptional<float>
```

O conceito e o mesmo: valor opcional.

---

## 13. Smart pointers

Smart pointers sao objetos que gerenciam ponteiros e ownership usando RAII.

Eles reduzem:

- memory leak
- double free
- use-after-free em alguns cenarios
- confusao sobre quem deve deletar

Principais:

- `std::unique_ptr`
- `std::shared_ptr`
- `std::weak_ptr`

Inclua:

```cpp
#include <memory>
```

Importante:

> Smart pointer e para ownership em C++ puro. Em UE5, nao use `std::unique_ptr` ou `std::shared_ptr` para gerenciar lifetime de `UObject`/`AActor` como se fossem objetos comuns. A Unreal tem garbage collector e tipos proprios.

---

## 14. `std::unique_ptr`

`std::unique_ptr<T>` representa ownership exclusivo.

So existe um dono do objeto.

Exemplo:

```cpp
#include <memory>

std::unique_ptr<Weapon> weapon = std::make_unique<Weapon>();
```

Quando `weapon` morre, o objeto apontado e destruido automaticamente.

Nao precisa:

```cpp
delete weapon;
```

Na verdade isso nem faz sentido com `unique_ptr`.

### Exemplo completo

```cpp
#include <iostream>
#include <memory>

class Weapon
{
public:
    Weapon()
    {
        std::cout << "Weapon created\n";
    }

    ~Weapon()
    {
        std::cout << "Weapon destroyed\n";
    }

    void Fire()
    {
        std::cout << "Fire\n";
    }
};

int main()
{
    std::unique_ptr<Weapon> weapon = std::make_unique<Weapon>();
    weapon->Fire();

    return 0;
}
```

Ao sair de `main`, `weapon` morre e destroi o `Weapon`.

Isso e RAII.

### `unique_ptr` nao copia

```cpp
std::unique_ptr<Weapon> a = std::make_unique<Weapon>();
std::unique_ptr<Weapon> b = a; // erro
```

Por que erro?

Porque se os dois fossem donos, quem deletaria?

`unique_ptr` obriga ownership exclusivo.

### Transferindo ownership com move

```cpp
std::unique_ptr<Weapon> a = std::make_unique<Weapon>();
std::unique_ptr<Weapon> b = std::move(a);
```

Agora:

- `b` e dono
- `a` fica vazio

Verifique:

```cpp
if (a == nullptr)
{
    std::cout << "a no longer owns weapon\n";
}
```

### Passando `unique_ptr` para funcao

Se a funcao deve apenas usar, passe referencia:

```cpp
void UseWeapon(Weapon& weapon)
{
    weapon.Fire();
}

UseWeapon(*weaponPtr);
```

Se a funcao deve receber ownership:

```cpp
void StoreWeapon(std::unique_ptr<Weapon> weapon)
{
}

StoreWeapon(std::move(weaponPtr));
```

Isso comunica transferencia de dono.

---

## 15. `std::shared_ptr`

`std::shared_ptr<T>` representa ownership compartilhado.

O objeto continua vivo enquanto existir pelo menos um `shared_ptr` apontando para ele.

Exemplo:

```cpp
std::shared_ptr<Weapon> a = std::make_shared<Weapon>();
std::shared_ptr<Weapon> b = a;
```

Agora `a` e `b` compartilham ownership.

Quando o ultimo `shared_ptr` morrer, o objeto e destruido.

### Quando usar

Use quando realmente houver ownership compartilhado.

Nao use como padrao so porque parece conveniente.

Ownership compartilhado torna lifetime menos obvio.

Pergunte:

> quem realmente deve ser dono desse objeto?

Se houver um dono claro, prefira `unique_ptr`.

### Custo

`shared_ptr` tem custo extra:

- contagem de referencias
- controle compartilhado
- possivel overhead atomico

Nao e absurdo, mas nao e gratis.

---

## 16. `std::weak_ptr`

`std::weak_ptr<T>` observa um objeto gerenciado por `shared_ptr` sem aumentar a contagem forte.

Ele evita ciclos.

Problema com ciclo:

```cpp
struct A
{
    std::shared_ptr<B> b;
};

struct B
{
    std::shared_ptr<A> a;
};
```

Se `A` e `B` apontam um para o outro com `shared_ptr`, talvez nunca sejam destruidos.

Use `weak_ptr` em uma das direcoes:

```cpp
struct B
{
    std::weak_ptr<A> a;
};
```

Para usar:

```cpp
std::weak_ptr<Weapon> weakWeapon = sharedWeapon;

if (auto locked = weakWeapon.lock())
{
    locked->Fire();
}
```

`lock()` tenta obter um `shared_ptr`.

Se o objeto ainda existe, voce usa.

Se ja foi destruido, retorna vazio.

### Relacao com UE5

Na Unreal, o equivalente conceitual para observar `UObject` sem manter vivo e muitas vezes:

```cpp
TWeakObjectPtr<AActor>
```

Mas isso pertence ao sistema da Unreal, nao a STL.

---

## 17. Smart pointers e UE5: cuidado importante

Em C++ puro:

```cpp
std::unique_ptr<MyObject>
std::shared_ptr<MyObject>
std::weak_ptr<MyObject>
```

sao ferramentas excelentes.

Em Unreal, objetos derivados de `UObject` e `AActor` seguem o sistema do engine.

Geralmente voce nao faz:

```cpp
std::unique_ptr<AActor> Actor;
std::shared_ptr<UObject> Object;
```

E tambem nao faz:

```cpp
delete Actor;
```

Para UE5, voce vai aprender tipos como:

- `UPROPERTY`
- `TObjectPtr`
- `TWeakObjectPtr`
- `TSharedPtr`
- `TSharedRef`
- `TUniquePtr`

Regra inicial:

> Use smart pointers da STL para C++ puro. Para objetos da Unreal, siga o modelo da Unreal.

Exemplo:

```cpp
AActor* Target = nullptr;
```

Esse ponteiro nao diz ownership. Em UE5, o lifetime do actor pertence ao engine/world.

---

## 18. Lambdas

Lambda e uma funcao anonima.

Exemplo:

```cpp
auto add = [](int a, int b)
{
    return a + b;
};

std::cout << add(2, 3) << "\n";
```

Estrutura:

```cpp
[captura](parametros)
{
    corpo
};
```

### Captura por valor

```cpp
int bonus = 10;

auto calculateDamage = [bonus](int baseDamage)
{
    return baseDamage + bonus;
};
```

`bonus` e copiado para dentro da lambda.

Se `bonus` mudar depois, a copia dentro da lambda nao muda.

### Captura por referencia

```cpp
int total = 0;

auto addToTotal = [&total](int value)
{
    total += value;
};
```

A lambda altera o `total` original.

Cuidado: se a lambda viver mais que a variavel capturada por referencia, voce pode criar referencia pendurada.

### Capturar tudo por valor ou referencia

```cpp
[=]() { } // captura usados por valor
[&]() { } // captura usados por referencia
```

Use com cuidado. Captura explicita costuma ser mais clara:

```cpp
[bonus, &total](int value)
{
}
```

### Lambda com STL

```cpp
#include <algorithm>
#include <vector>

std::vector<int> values = {5, 1, 9, 3};

std::sort(values.begin(), values.end(), [](int a, int b)
{
    return a < b;
});
```

Lambda define criterio de ordenacao.

### Relacao com UE5

Lambdas aparecem em callbacks, timers, delegates, async e utilitarios.

Mas em Unreal voce precisa ter muito cuidado ao capturar `this`:

```cpp
[this]()
{
    // usa objeto atual
}
```

Se a lambda executar depois que o objeto foi destruido, voce tem problema de lifetime.

Este e o mesmo conceito da Aula 02: referencia/ponteiro para objeto morto.

---

## 19. Templates

Template permite escrever codigo generico em compile time.

Exemplo:

```cpp
template <typename T>
T Max(T a, T b)
{
    return a > b ? a : b;
}
```

Uso:

```cpp
int a = Max(2, 5);
float b = Max(2.5f, 1.0f);
```

O compilador gera versoes para os tipos usados.

### Template nao e igual generics simples

Em C++, templates participam fortemente do compile time.

Eles podem:

- gerar codigo para tipos especificos
- causar mensagens de erro longas
- aumentar tempo de compilacao
- permitir abstracoes sem custo em runtime

### Template de classe

```cpp
template <typename T>
class Box
{
public:
    Box(const T& newValue)
        : value(newValue)
    {
    }

    const T& Get() const
    {
        return value;
    }

private:
    T value;
};
```

Uso:

```cpp
Box<int> intBox(10);
Box<std::string> stringBox("Player");
```

### STL usa templates

```cpp
std::vector<int>
std::vector<std::string>
std::optional<float>
std::unordered_map<std::string, int>
std::unique_ptr<Weapon>
```

Todos esses sao templates.

Na Unreal:

```cpp
TArray<int32>
TMap<FName, int32>
TOptional<float>
TObjectPtr<AActor>
```

Tambem sao tipos genericos no estilo da engine.

---

## 20. Zero-cost abstractions

C++ valoriza a ideia de abstracao sem custo extra desnecessario.

Isso nao significa que tudo e gratis.

Significa:

> uma abstracao bem desenhada pode compilar para codigo tao eficiente quanto codigo manual equivalente.

Exemplo:

```cpp
std::vector<int> values;
```

`std::vector` abstrai array dinamico, mas ainda e eficiente.

Range-based for:

```cpp
for (const auto& value : values)
{
}
```

Abstrai iteracao sem necessariamente adicionar custo relevante.

Templates podem gerar codigo especifico para tipos, evitando polimorfismo dinamico quando nao e necessario.

Mas cuidado:

- copia ainda custa
- alocacao ainda custa
- `shared_ptr` tem overhead
- virtual dispatch tem custo
- `std::function` pode ter custo
- containers podem realocar

C++ moderno nao elimina custo; ele torna custo mais controlavel.

---

## 21. Tratamento de erro

C++ tem varias formas de lidar com erro.

### Excecoes

```cpp
throw std::runtime_error("Invalid weapon");
```

E:

```cpp
try
{
}
catch (const std::exception& error)
{
    std::cout << error.what() << "\n";
}
```

Voce precisa entender que excecoes existem, mas nem todo codigo C++ usa excecoes.

### Codigos de erro

```cpp
bool TryFire()
{
    if (ammo <= 0)
    {
        return false;
    }

    --ammo;
    return true;
}
```

Simples e comum em gameplay.

### `std::optional`

Quando a operacao pode nao retornar valor:

```cpp
std::optional<Weapon> FindWeapon(const std::string& name);
```

### Precondicoes

Precondicao e algo que precisa ser verdade antes de chamar uma funcao.

```cpp
void ApplyDamage(Character& target, int amount)
{
    // precondicao: amount >= 0
}
```

Voce pode:

- validar e ignorar valor ruim
- usar assert em debug
- retornar erro
- documentar claramente

### Invariantes

Invariante e regra que deve continuar verdadeira.

Exemplo:

```cpp
health >= 0
ammo >= 0
ammo <= maxAmmo
```

C++ moderno ajuda, mas voce ainda precisa desenhar tipos que protegem suas regras.

### UE5 e excecoes

Em projetos Unreal, voce normalmente nao estrutura gameplay com excecoes C++.

Voce tende a usar:

- retornos `bool`
- checks/asserts da engine
- logs
- validacoes
- `ensure`
- estados explicitos
- `TOptional`

Por enquanto, saiba: excecoes existem em C++ padrao, mas Unreal tem convencoes proprias.

---

## 22. Boas decisoes de parametro em C++ moderno

Use esta tabela mental.

### Tipo pequeno, so leitura

```cpp
void SetDamage(int damage);
void Tick(float deltaTime);
```

Passe por valor.

### Tipo grande, so leitura

```cpp
void PrintPlayer(const Player& player);
void SetConfig(const Config& config);
```

Passe por `const&`.

### Precisa alterar original

```cpp
void ApplyDamage(Player& player, int amount);
```

Passe por `&`.

### Pode nao existir

```cpp
void SetTarget(Actor* target);
```

Use ponteiro, `std::optional`, ou tipo apropriado.

### Vai assumir ownership exclusivo

```cpp
void AddWeapon(std::unique_ptr<Weapon> weapon);
```

Receba `unique_ptr` por valor e chame com `std::move`.

### Vai guardar copia

```cpp
void SetName(std::string newName)
{
    name = std::move(newName);
}
```

Receber por valor pode ser bom quando voce vai armazenar internamente.

---

## 23. Mini-exemplo integrado

```cpp
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Weapon
{
public:
    Weapon(std::string newName, int newDamage)
        : name(std::move(newName)), damage(newDamage)
    {
        if (damage < 0)
        {
            damage = 0;
        }
    }

    const std::string& GetName() const
    {
        return name;
    }

    int GetDamage() const
    {
        return damage;
    }

private:
    std::string name;
    int damage;
};

class Inventory
{
public:
    void AddWeapon(std::unique_ptr<Weapon> weapon)
    {
        weapons.push_back(std::move(weapon));
    }

    Weapon* FindWeapon(const std::string& name)
    {
        for (const auto& weapon : weapons)
        {
            if (weapon != nullptr && weapon->GetName() == name)
            {
                return weapon.get();
            }
        }

        return nullptr;
    }

private:
    std::vector<std::unique_ptr<Weapon>> weapons;
};

std::optional<int> FindItemCount(const std::unordered_map<std::string, int>& items, const std::string& name)
{
    auto it = items.find(name);

    if (it == items.end())
    {
        return std::nullopt;
    }

    return it->second;
}

int main()
{
    Inventory inventory;
    inventory.AddWeapon(std::make_unique<Weapon>("Rifle", 25));
    inventory.AddWeapon(std::make_unique<Weapon>("Launcher", 80));

    Weapon* weapon = inventory.FindWeapon("Rifle");

    if (weapon != nullptr)
    {
        std::cout << weapon->GetName() << " damage: " << weapon->GetDamage() << "\n";
    }

    std::unordered_map<std::string, int> items;
    items["Potion"] = 3;
    items["Ammo"] = 90;

    auto ammoCount = FindItemCount(items, "Ammo");

    if (ammoCount)
    {
        std::cout << "Ammo: " << *ammoCount << "\n";
    }

    return 0;
}
```

O que este exemplo usa:

- `std::string`
- `std::move`
- `std::unique_ptr`
- `std::make_unique`
- `std::vector`
- `std::unordered_map`
- `std::optional`
- ponteiro observador retornado por `FindWeapon`
- `const&` para leitura sem copia

---

## 24. Exercicios teoricos

Responda com suas palavras:

1. O que `const T&` comunica?
2. O que acontece conceitualmente quando voce copia uma `std::string`?
3. O que `std::move` realmente faz?
4. Depois de mover de um objeto, o que voce pode fazer com ele?
5. Por que `std::unique_ptr` nao pode ser copiado?
6. Quando `std::shared_ptr` faz sentido?
7. Para que serve `std::weak_ptr`?
8. Por que `std::vector` pode invalidar ponteiros para elementos?
9. Qual a vantagem de `std::optional` sobre retornar `-1`?
10. O que uma lambda captura por valor?
11. O que uma lambda captura por referencia?
12. Por que capturar por referencia pode ser perigoso?
13. O que e template?
14. Por que templates podem aumentar tempo de compilacao?
15. Por que voce nao deve usar `std::unique_ptr<AActor>` para gerenciar Actor da Unreal?

---

## 25. Exercicios praticos

### Exercicio 1 - `std::vector`

Crie um `std::vector<int>` com danos.

Requisitos:

- adicionar 5 valores
- imprimir todos
- calcular dano total
- calcular dano medio como `float`
- usar range-based for

Pergunta:

> Onde poderia ocorrer copia desnecessaria se o vector guardasse objetos grandes?

### Exercicio 2 - `std::string` e move

Crie uma classe:

```cpp
class PlayerProfile
{
public:
    PlayerProfile(std::string newName)
        : name(std::move(newName))
    {
    }

    const std::string& GetName() const
    {
        return name;
    }

private:
    std::string name;
};
```

Perguntas:

1. Por que `newName` e recebido por valor?
2. Por que faz sentido usar `std::move(newName)`?
3. Voce deve usar `newName` depois do move?

### Exercicio 3 - `std::optional`

Implemente:

```cpp
std::optional<int> FindScore(const std::unordered_map<std::string, int>& scores, const std::string& playerName);
```

Regras:

- se encontrar, retorna score
- se nao encontrar, retorna `std::nullopt`
- nao use `operator[]` para buscar

### Exercicio 4 - `std::unique_ptr`

Crie uma classe `Weapon` com logs no construtor e destrutor.

No `main`:

```cpp
std::unique_ptr<Weapon> weapon = std::make_unique<Weapon>();
```

Chame um metodo.

Observe quando o destrutor roda.

Depois tente copiar:

```cpp
std::unique_ptr<Weapon> other = weapon;
```

Veja o erro do compilador e explique por que ele e bom.

### Exercicio 5 - Transferencia de ownership

Crie:

```cpp
void StoreWeapon(std::unique_ptr<Weapon> weapon)
{
}
```

Chame:

```cpp
auto weapon = std::make_unique<Weapon>();
StoreWeapon(std::move(weapon));
```

Depois verifique:

```cpp
if (weapon == nullptr)
{
    std::cout << "Ownership transferred\n";
}
```

### Exercicio 6 - `shared_ptr` e `weak_ptr`

Crie:

```cpp
std::shared_ptr<Weapon> sharedWeapon = std::make_shared<Weapon>();
std::weak_ptr<Weapon> weakWeapon = sharedWeapon;
```

Use:

```cpp
if (auto locked = weakWeapon.lock())
{
    locked->Fire();
}
```

Depois faça:

```cpp
sharedWeapon.reset();
```

Tente `lock()` de novo.

Explique o resultado.

### Exercicio 7 - Lambda

Crie um vector de damages:

```cpp
std::vector<int> damages = {10, 30, 20, 5};
```

Use `std::sort` com lambda para ordenar do maior para o menor.

```cpp
std::sort(damages.begin(), damages.end(), [](int a, int b)
{
    return a > b;
});
```

Depois imprima.

### Exercicio 8 - Captura de lambda

Crie:

```cpp
int bonus = 5;
```

Uma lambda que captura por valor e outra por referencia.

Altere `bonus` depois.

Compare os resultados.

### Exercicio 9 - Template simples

Crie:

```cpp
template <typename T>
T Clamp(T value, T min, T max)
{
}
```

Regras:

- se `value < min`, retorna `min`
- se `value > max`, retorna `max`
- senao retorna `value`

Teste com `int` e `float`.

### Exercicio 10 - Optional vs ponteiro

Crie dois exemplos:

```cpp
std::optional<int> FindAmmoCount(...);
Weapon* FindWeapon(...);
```

Explique:

- por que ammo count como optional faz sentido
- por que weapon como ponteiro pode fazer sentido

---

## 26. Mini-projeto - Modern Inventory Training

Crie:

```text
modern_inventory_training.cpp
respostas_aula_04.txt
```

Objetivo:

Criar um pequeno inventario moderno usando STL, smart pointers, optional, lambdas e move semantics.

### Classes obrigatorias

```text
Item
Weapon
Inventory
```

### `Item`

Classe base simples.

Requisitos:

- `std::string name`
- construtor recebe `std::string`
- destrutor virtual
- `const std::string& GetName() const`
- `virtual void Print() const`

### `Weapon`

Deriva de `Item`.

Requisitos:

- `int damage`
- construtor recebe nome e dano
- `int GetDamage() const`
- sobrescreve `Print() const`

### `Inventory`

Responsabilidade:

> possuir itens.

Requisitos:

- usar `std::vector<std::unique_ptr<Item>>`
- `void AddItem(std::unique_ptr<Item> item)`
- `Item* FindItem(const std::string& name)`
- `std::optional<int> FindWeaponDamage(const std::string& name) const`
- `void PrintAll() const`
- `void SortByName()`

### Regras importantes

`Inventory` deve ser dono dos itens.

Por isso:

```cpp
std::vector<std::unique_ptr<Item>>
```

`AddItem` deve receber ownership:

```cpp
void AddItem(std::unique_ptr<Item> item)
{
    items.push_back(std::move(item));
}
```

`FindItem` pode retornar ponteiro observador:

```cpp
Item* FindItem(const std::string& name)
```

Esse ponteiro nao transfere ownership.

### Exemplo de uso

```cpp
int main()
{
    Inventory inventory;

    inventory.AddItem(std::make_unique<Weapon>("Rifle", 25));
    inventory.AddItem(std::make_unique<Weapon>("Launcher", 80));
    inventory.AddItem(std::make_unique<Item>("Keycard"));

    inventory.SortByName();
    inventory.PrintAll();

    auto damage = inventory.FindWeaponDamage("Rifle");

    if (damage)
    {
        std::cout << "Rifle damage: " << *damage << "\n";
    }

    Item* item = inventory.FindItem("Keycard");

    if (item != nullptr)
    {
        item->Print();
    }

    return 0;
}
```

### Dica para `SortByName`

Use lambda:

```cpp
std::sort(items.begin(), items.end(), [](const auto& a, const auto& b)
{
    return a->GetName() < b->GetName();
});
```

Inclua:

```cpp
#include <algorithm>
```

### Dica para descobrir se `Item*` e `Weapon*`

Para este exercicio, voce pode usar `dynamic_cast` porque `Item` tem metodo virtual.

```cpp
Weapon* weapon = dynamic_cast<Weapon*>(item.get());
```

Se for weapon, retorna ponteiro valido.

Se nao for, retorna `nullptr`.

Nao precisa aprofundar RTTI agora. O objetivo e praticar polimorfismo e optional.

---

## 27. Perguntas obrigatorias do mini-projeto

Responda em `respostas_aula_04.txt`:

1. Quem e dono dos itens?
2. Por que `Inventory` usa `std::unique_ptr<Item>`?
3. Por que `AddItem` recebe `std::unique_ptr<Item>` por valor?
4. Por que precisa usar `std::move` em `AddItem`?
5. Depois de chamar `AddItem(std::move(item))`, o ponteiro original ainda e dono?
6. `FindItem` retorna ownership ou apenas observacao?
7. Por que `FindWeaponDamage` retorna `std::optional<int>`?
8. Onde voce usou lambda?
9. O que a lambda captura?
10. Onde poderia ocorrer copia desnecessaria?
11. Onde voce usou `const&`?
12. Onde voce usou polimorfismo?
13. Por que `Item` precisa de destrutor virtual?
14. Que parte desse projeto parece com inventario em jogo?
15. O que mudaria se isso fosse implementado dentro da UE5 com `UObject`?

---

## 28. Relação com UE5

Esta aula e C++ padrao, mas prepara muita coisa da Unreal.

### STL vs tipos Unreal

Em C++ puro:

```cpp
std::vector
std::string
std::unordered_map
std::optional
```

Na Unreal:

```cpp
TArray
FString
FName
FText
TMap
TSet
TOptional
```

O conceito aprendido continua util:

- array dinamico
- string
- mapa chave/valor
- valor opcional
- ownership
- invalidacao por realocacao
- custo de copia

### Smart pointers

Em C++ puro:

```cpp
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

Na Unreal, para objetos nao-`UObject`, a engine tambem tem:

```cpp
TUniquePtr
TSharedPtr
TWeakPtr
```

Para `UObject`/`AActor`, entram regras de GC e reflection:

```cpp
UPROPERTY()
TObjectPtr<AActor>
TWeakObjectPtr<AActor>
```

O mais importante:

> nao misture ownership de C++ puro com ownership do sistema de objetos da Unreal sem entender o modelo.

### Lambdas na Unreal

Lambdas aparecem em:

- timers
- delegates
- async
- callbacks
- algoritmos

Mas capturar `this` exige cuidado:

```cpp
[this]()
{
    DoSomething();
}
```

Se isso executar depois que o objeto foi destruido, voce tem problema.

Esse e um problema de lifetime, nao de sintaxe.

---

## 29. Quando vale ver video

Para esta aula, vale ver video quando o tema for visual ou dificil de imaginar:

- "C++ move semantics visual explanation"
- "C++ std::unique_ptr explained"
- "C++ shared_ptr weak_ptr cycle explained"
- "C++ vector reallocation invalidates pointers"
- "C++ lambdas captures by value reference"
- "C++ templates explained for beginners"

O video mais importante e sobre `std::unique_ptr` e ownership.

O segundo mais importante e sobre move semantics.

Procure videos que mostrem:

- objeto antes/depois do move
- ownership mudando de lugar
- `unique_ptr` ficando nulo depois de `std::move`
- `shared_ptr` contando referencias
- `weak_ptr.lock()`

---

## 30. Erros comuns

### Erro 1 - Usar `std::move` e depois depender do valor antigo

```cpp
std::string a = "Player";
std::string b = std::move(a);

std::cout << a << "\n"; // nao dependa disso
```

### Erro 2 - Tentar copiar `unique_ptr`

```cpp
auto a = std::make_unique<Weapon>();
auto b = a; // erro
```

Esse erro protege ownership.

### Erro 3 - Usar `shared_ptr` para tudo

`shared_ptr` nao e "ponteiro moderno padrao".

Ele comunica ownership compartilhado. Se nao existe ownership compartilhado, nao use.

### Erro 4 - Guardar ponteiro para elemento de vector e depois crescer o vector

```cpp
int* p = &values[0];
values.push_back(10); // pode realocar
```

`p` pode ficar invalido.

### Erro 5 - Capturar referencia em lambda que vive demais

```cpp
auto MakeLambda()
{
    int value = 10;

    return [&value]()
    {
        return value;
    };
}
```

`value` morre ao sair da funcao. A lambda guarda referencia invalida.

### Erro 6 - Usar STL para gerenciar `UObject`

```cpp
std::unique_ptr<AActor> actor;
```

Nao use esse modelo para actor da Unreal.

---

## 31. Checklist de dominio

Antes de seguir para headers, compilacao e organizacao de projeto, voce deve conseguir responder:

- O que `const T&` evita?
- O que e lvalue?
- O que e rvalue?
- O que `std::move` faz?
- Qual diferenca entre copy e move?
- O que acontece com um objeto depois de ser movido?
- O que `std::vector` faz?
- Por que `std::vector` pode invalidar ponteiros?
- Quando usar `std::array`?
- Quando usar `std::unordered_map`?
- Quando usar `std::optional`?
- O que `std::unique_ptr` comunica?
- Por que `std::unique_ptr` nao copia?
- Quando usar `std::shared_ptr`?
- Para que serve `std::weak_ptr`?
- O que e lambda?
- Qual diferenca entre captura por valor e por referencia?
- O que e template?
- Por que templates sao resolvidos em compile time?
- Por que smart pointers da STL nao substituem o GC da Unreal?

---

## 32. Resumo final

C++ moderno nao e uma linguagem diferente. E um estilo de C++ que usa recursos da linguagem e da biblioteca padrao para deixar ownership, lifetime e custo mais claros.

O centro desta aula:

- use `const&` para ler objetos grandes sem copiar
- use `std::move` quando quiser transferir recursos e nao precisar mais do valor antigo
- use `std::vector` para arrays dinamicos
- use `std::string` para texto em C++ puro
- use `std::optional` para valor talvez ausente
- use `std::unique_ptr` para ownership exclusivo
- use `std::shared_ptr` apenas quando ownership realmente for compartilhado
- use `std::weak_ptr` para observar sem manter vivo
- use lambdas para funcoes locais e callbacks
- use templates para codigo generico em compile time
- nao use tipos da STL cegamente para objetos gerenciados pela Unreal

Para UE5, esta aula te prepara para reconhecer paralelos:

```text
std::vector        -> TArray
std::string        -> FString / FName / FText
std::unordered_map -> TMap
std::optional      -> TOptional
std::weak_ptr      -> TWeakPtr ou TWeakObjectPtr, dependendo do caso
```

Mas o mais importante nao e decorar equivalencias. E entender o modelo:

> quem possui, quem observa, quem pode estar ausente, quem pode ser copiado e quem pode ser movido.

---

## 33. Tarefa final da aula

Crie:

```text
modern_inventory_training.cpp
respostas_aula_04.txt
```

Implemente o mini-projeto `Modern Inventory Training`.

No arquivo de respostas, explique:

1. ownership dos itens
2. uso de `std::unique_ptr`
3. uso de `std::move`
4. uso de `std::optional`
5. uso de lambdas
6. uso de `const&`
7. onde existe polimorfismo
8. onde poderia haver copia desnecessaria
9. como isso mudaria em UE5

So considere a aula concluida quando voce conseguir explicar por que esta linha faz sentido:

```cpp
std::vector<std::unique_ptr<Item>> items;
```

Explicacao esperada:

> `Inventory` possui uma colecao dinamica de itens polimorficos. Como cada item tem ownership exclusivo do inventario e pode ser uma classe derivada de `Item`, usamos `std::unique_ptr<Item>` dentro de `std::vector`. O vector guarda os donos, e os objetos sao destruidos automaticamente quando o inventario morre.

Se essa frase ficou clara, voce entendeu uma parte essencial do C++ moderno.
