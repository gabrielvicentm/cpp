# Aula 03 - Orientacao a objetos em C++ para UE5

Esta aula cobre a Fase 3 do `Roadmap_Cpp_UE5.md`: orientacao a objetos em C++.

Aqui voce vai aprender OOP do jeito que C++ realmente implementa, nao como uma ideia abstrata de "classe tem metodo". Em C++, objetos tem custo, lifetime, construcao, destruicao, copia, heranca, despacho dinamico e regras de acesso que afetam diretamente como voce escreve gameplay em UE5.

O objetivo desta aula e voce entender:

- diferenca entre `struct` e `class`
- membros publicos e privados
- encapsulamento real
- construtores e destrutores dentro de tipos
- initializer list
- invariantes de objeto
- heranca publica
- classes base e derivadas
- `virtual`
- `override`
- polimorfismo
- classes abstratas
- interfaces em C++ puro
- por que composicao muitas vezes e melhor que heranca
- como isso aparece em `UObject`, `AActor`, `APawn`, `ACharacter` e componentes da Unreal

Se a Aula 02 ensinou "quando objetos nascem e morrem", esta aula ensina "como objetos carregam comportamento e se relacionam".

---

## Como estudar esta aula

Estude com calma. Orientacao a objetos em C++ tem duas camadas:

1. sintaxe
2. custo e comportamento real

Nao basta saber escrever:

```cpp
class Player
{
};
```

Voce precisa saber:

- o objeto nasce valido?
- quem pode alterar os dados?
- essa funcao modifica o estado?
- existe copia sem querer?
- o destrutor precisa ser virtual?
- a chamada de metodo e resolvida em compile time ou runtime?
- heranca esta modelando uma relacao real?

Tempo sugerido:

- 45 min: `struct`, `class`, membros e metodos
- 60 min: encapsulamento, getters/setters e invariantes
- 60 min: construtores, initializer list e destrutores
- 75 min: heranca, `virtual`, `override` e polimorfismo
- 45 min: classes abstratas e interfaces
- 60 min: composicao vs heranca e conexao com UE5
- 90 min: exercicios e mini-projeto

Se o tempo estiver curto, priorize:

- classe vs struct
- public/private
- construtor com initializer list
- `virtual` e `override`
- polimorfismo por ponteiro/referencia
- composicao vs heranca

---

## 1. O que e um objeto em C++

Um objeto em C++ e uma instancia concreta de um tipo.

Exemplo:

```cpp
struct Player
{
    int health;
    int stamina;
};

int main()
{
    Player player;
    player.health = 100;
    player.stamina = 50;

    return 0;
}
```

`Player` e o tipo.

`player` e o objeto.

O objeto tem memoria, lifetime e estado.

Se `player` esta dentro de `main`, ele vive ate sair do escopo de `main`.

Isso conecta esta aula diretamente com a Aula 02:

> classe nao e so organizacao de codigo; classe define objetos que nascem, vivem, podem ser copiados e morrem.

---

## 2. `struct` vs `class`

Em C++, `struct` e `class` sao quase iguais.

A diferenca padrao e o nivel de acesso.

Em `struct`, membros sao `public` por padrao:

```cpp
struct Player
{
    int health;
};
```

Equivale a:

```cpp
struct Player
{
public:
    int health;
};
```

Em `class`, membros sao `private` por padrao:

```cpp
class Player
{
    int health;
};
```

Equivale a:

```cpp
class Player
{
private:
    int health;
};
```

### Quando usar `struct`

Use `struct` quando o tipo e principalmente um pacote simples de dados, com pouca ou nenhuma regra interna.

Exemplo:

```cpp
struct Vector2
{
    float x;
    float y;
};
```

Ou:

```cpp
struct DamageInfo
{
    int amount;
    bool isCritical;
};
```

Na Unreal, voce vai ver `USTRUCT` para dados:

```cpp
USTRUCT(BlueprintType)
struct FWeaponStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float Damage;

    UPROPERTY(EditAnywhere)
    float FireRate;
};
```

Ainda nao precisa estudar macros agora. O ponto e: struct costuma carregar dados.

### Quando usar `class`

Use `class` quando o tipo tem regras, comportamento e precisa proteger seu estado.

Exemplo:

```cpp
class HealthComponent
{
public:
    void ApplyDamage(int amount);
    int GetHealth() const;

private:
    int health = 100;
};
```

Aqui `health` nao deve ser alterado de qualquer jeito. A classe controla como o estado muda.

Regra pratica:

- `struct`: dados simples, acesso direto aceitavel
- `class`: comportamento, invariantes e encapsulamento

---

## 3. Membros e metodos

Uma classe pode ter dados e funcoes.

Dados dentro da classe sao membros.

Funcoes dentro da classe sao metodos.

```cpp
#include <iostream>
#include <string>

class Player
{
public:
    std::string name;
    int health;

    void PrintStatus()
    {
        std::cout << name << " has " << health << " health\n";
    }
};

int main()
{
    Player player;
    player.name = "Knight";
    player.health = 100;

    player.PrintStatus();

    return 0;
}
```

`PrintStatus` e um metodo porque pertence a `Player`.

Dentro de `PrintStatus`, voce acessa `name` e `health` diretamente porque eles pertencem ao objeto atual.

Mentalmente:

```cpp
player.PrintStatus();
```

significa:

> execute `PrintStatus` usando o objeto `player` como contexto.

---

## 4. `this`

Dentro de um metodo, existe um ponteiro especial chamado `this`.

Ele aponta para o objeto atual.

```cpp
class Player
{
public:
    int health = 100;

    void SetHealth(int health)
    {
        this->health = health;
    }
};
```

Aqui existem dois `health`:

- parametro `health`
- membro `health`

`this->health` significa:

> o `health` do objeto atual.

Voce nao precisa usar `this` o tempo todo:

```cpp
void Damage(int amount)
{
    health -= amount;
}
```

Mas ele ajuda quando existe conflito de nomes ou quando voce quer deixar claro que esta acessando membro.

Na UE5 voce tambem vera `this` em alguns contextos:

```cpp
GetWorld()->SpawnActor<AEnemy>(EnemyClass, GetActorLocation(), GetActorRotation());
```

Mesmo quando `this` nao aparece, metodos como `GetWorld()` estao sendo chamados no objeto atual.

---

## 5. `public` e `private`

Controle de acesso define quem pode mexer no que.

```cpp
class Player
{
public:
    void ApplyDamage(int amount)
    {
        health -= amount;

        if (health < 0)
        {
            health = 0;
        }
    }

    int GetHealth() const
    {
        return health;
    }

private:
    int health = 100;
};
```

Agora, fora da classe, isto funciona:

```cpp
Player player;
player.ApplyDamage(25);
std::cout << player.GetHealth() << "\n";
```

Mas isto nao funciona:

```cpp
player.health = -999; // erro: health e private
```

Esse erro e bom.

Ele protege a classe contra estados invalidos.

### Encapsulamento nao e burocracia

Encapsulamento significa controlar acesso para proteger regras.

Se qualquer parte do codigo pode fazer:

```cpp
player.health = -500;
```

entao voce nao tem garantia nenhuma sobre o estado do objeto.

Com metodo:

```cpp
player.ApplyDamage(500);
```

a classe pode garantir:

```cpp
if (health < 0)
{
    health = 0;
}
```

Isso cria uma invariante:

> health nunca deve ser menor que zero.

---

## 6. Invariantes

Invariante e uma regra que deve ser sempre verdadeira enquanto o objeto esta valido.

Exemplos:

- vida nunca e menor que 0
- stamina nunca e menor que 0
- municao atual nunca passa da municao maxima
- velocidade maxima nao pode ser negativa
- ponteiro obrigatorio nao pode ser nulo

Classe boa protege invariantes.

Exemplo ruim:

```cpp
struct Weapon
{
    int ammo;
    int maxAmmo;
};

int main()
{
    Weapon weapon;
    weapon.ammo = 999;
    weapon.maxAmmo = 30;
}
```

Objeto esta invalido: `ammo > maxAmmo`.

Versao melhor:

```cpp
class Weapon
{
public:
    Weapon(int initialAmmo, int maximumAmmo)
        : ammo(initialAmmo), maxAmmo(maximumAmmo)
    {
        if (maxAmmo < 0)
        {
            maxAmmo = 0;
        }

        if (ammo < 0)
        {
            ammo = 0;
        }

        if (ammo > maxAmmo)
        {
            ammo = maxAmmo;
        }
    }

    void Reload()
    {
        ammo = maxAmmo;
    }

    bool CanFire() const
    {
        return ammo > 0;
    }

    void Fire()
    {
        if (ammo > 0)
        {
            --ammo;
        }
    }

    int GetAmmo() const
    {
        return ammo;
    }

private:
    int ammo;
    int maxAmmo;
};
```

Agora `Weapon` protege suas regras.

---

## 7. `const` em metodos

Este detalhe e muito importante.

```cpp
int GetHealth() const
{
    return health;
}
```

O `const` depois dos parenteses significa:

> este metodo nao altera o estado observavel do objeto.

Exemplo:

```cpp
class Player
{
public:
    int GetHealth() const
    {
        return health;
    }

    void ApplyDamage(int amount)
    {
        health -= amount;
    }

private:
    int health = 100;
};
```

`GetHealth` e metodo de leitura.

`ApplyDamage` modifica o objeto.

Se voce tem:

```cpp
void PrintPlayer(const Player& player)
{
    std::cout << player.GetHealth() << "\n";
}
```

`PrintPlayer` so consegue chamar metodos `const` em `player`, porque recebeu `const Player&`.

Se `GetHealth` nao fosse marcado como `const`, o compilador poderia reclamar.

Regra:

> metodo que nao altera o objeto deve ser `const`.

Isso torna seu codigo mais claro e mais facil de usar corretamente.

---

## 8. Construtores

Construtor e uma funcao especial chamada quando o objeto nasce.

```cpp
class Player
{
public:
    Player()
    {
        health = 100;
        stamina = 50;
    }

private:
    int health;
    int stamina;
};
```

Uso:

```cpp
Player player;
```

O construtor garante que o objeto comece valido.

### Construtor com parametros

```cpp
class Player
{
public:
    Player(const std::string& newName, int initialHealth)
    {
        name = newName;
        health = initialHealth;

        if (health < 0)
        {
            health = 0;
        }
    }

private:
    std::string name;
    int health;
};
```

Uso:

```cpp
Player player("Knight", 100);
```

Mas existe uma forma melhor para inicializar membros: initializer list.

---

## 9. Initializer list

Initializer list inicializa membros antes do corpo do construtor rodar.

```cpp
class Player
{
public:
    Player(const std::string& newName, int initialHealth)
        : name(newName), health(initialHealth)
    {
        if (health < 0)
        {
            health = 0;
        }
    }

private:
    std::string name;
    int health;
};
```

Esta parte:

```cpp
: name(newName), health(initialHealth)
```

e a initializer list.

Ela e preferivel porque:

- inicializa diretamente
- evita criar primeiro e atribuir depois
- e obrigatoria para certos tipos
- comunica intencao melhor

### Inicializacao vs atribuicao

Sem initializer list:

```cpp
Player(const std::string& newName)
{
    name = newName;
}
```

`name` e construido primeiro e depois recebe atribuicao.

Com initializer list:

```cpp
Player(const std::string& newName)
    : name(newName)
{
}
```

`name` ja nasce com o valor correto.

Em C++, isso importa.

### Ordem real de inicializacao

Os membros sao inicializados na ordem em que aparecem na classe, nao na ordem da initializer list.

```cpp
class Example
{
public:
    Example()
        : b(2), a(1)
    {
    }

private:
    int a;
    int b;
};
```

Mesmo listando `b` antes, `a` inicializa primeiro porque aparece primeiro na classe.

Regra pratica:

> escreva a initializer list na mesma ordem dos membros.

---

## 10. Destrutores

Destrutor e chamado quando o objeto morre.

```cpp
#include <iostream>

class Player
{
public:
    Player()
    {
        std::cout << "Player created\n";
    }

    ~Player()
    {
        std::cout << "Player destroyed\n";
    }
};

int main()
{
    Player player;
    return 0;
}
```

Saida:

```text
Player created
Player destroyed
```

Destrutor e parte do ciclo de vida.

Voce viu isso na Aula 02 com RAII.

### Quando escrever destrutor?

Nem toda classe precisa de destrutor escrito manualmente.

Se a classe so usa tipos que se limpam sozinhos:

```cpp
class Player
{
private:
    std::string name;
    std::vector<int> inventory;
};
```

voce provavelmente nao precisa escrever destrutor.

`std::string` e `std::vector` ja fazem RAII.

Escreva destrutor quando a classe gerencia recurso diretamente ou precisa executar alguma acao ao morrer.

### Regra pratica moderna

Evite gerenciar `new`/`delete` manualmente dentro de classes no comeco.

Prefira:

- `std::string`
- `std::vector`
- `std::unique_ptr`
- objetos locais
- containers

Na Unreal, objetos derivados de `UObject` seguem regras proprias do engine. Nao saia colocando `delete` em objeto da Unreal.

---

## 11. Separando interface e implementacao em conceito

Em C++, classes geralmente sao declaradas em `.h` e implementadas em `.cpp`.

Por enquanto, veja em um unico arquivo.

```cpp
class Player
{
public:
    Player(const std::string& name, int health);
    void ApplyDamage(int amount);
    int GetHealth() const;

private:
    std::string name;
    int health;
};

Player::Player(const std::string& newName, int initialHealth)
    : name(newName), health(initialHealth)
{
}

void Player::ApplyDamage(int amount)
{
    health -= amount;

    if (health < 0)
    {
        health = 0;
    }
}

int Player::GetHealth() const
{
    return health;
}
```

O `Player::` significa:

> esta funcao pertence a classe `Player`.

Na Unreal isso aparece o tempo todo:

```cpp
AMyCharacter::AMyCharacter()
{
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}
```

Voce declara no `.h` e implementa no `.cpp`.

---

## 12. Heranca

Heranca permite criar uma classe derivada a partir de uma classe base.

Exemplo:

```cpp
class Character
{
public:
    void Move()
    {
        std::cout << "Character moves\n";
    }
};

class Player : public Character
{
public:
    void Attack()
    {
        std::cout << "Player attacks\n";
    }
};
```

Uso:

```cpp
Player player;
player.Move();
player.Attack();
```

`Player` herdou `Move` de `Character`.

### Heranca publica significa "e um"

```cpp
class Player : public Character
```

Leia:

> Player e um Character.

Esse e o teste mental.

Bom:

```text
Player e um Character
Enemy e um Character
Boss e um Enemy
```

Suspeito:

```text
Inventory e um Player
Weapon e um Character
Door e uma HealthBar
```

Se a frase "X e um Y" nao fizer sentido, talvez heranca esteja errada.

---

## 13. Heranca nao e so reutilizacao de codigo

Erro comum:

> "Quero usar uma funcao dessa classe, entao vou herdar dela."

Isso costuma gerar design ruim.

Heranca deve modelar relacao real.

Se voce so quer reaproveitar uma funcionalidade, composicao pode ser melhor.

Exemplo ruim:

```cpp
class Inventory : public Player
{
};
```

Inventario nao e jogador.

Melhor:

```cpp
class Inventory
{
};

class Player
{
private:
    Inventory inventory;
};
```

Agora `Player` tem um `Inventory`.

Essa diferenca e central:

- heranca: "e um"
- composicao: "tem um"

Na UE5, componentes usam muito composicao:

```text
ACharacter tem CameraComponent
ACharacter tem SkeletalMeshComponent
AActor tem SceneComponent
```

---

## 14. Sobrescrita de comportamento

Suponha:

```cpp
class Enemy
{
public:
    void Attack()
    {
        std::cout << "Enemy attacks\n";
    }
};

class Boss : public Enemy
{
public:
    void Attack()
    {
        std::cout << "Boss attacks hard\n";
    }
};
```

Se voce fizer:

```cpp
Boss boss;
boss.Attack();
```

imprime:

```text
Boss attacks hard
```

Mas isso ainda nao e polimorfismo dinamico.

Veja:

```cpp
Enemy* enemy = &boss;
enemy->Attack();
```

Sem `virtual`, isso chama `Enemy::Attack`, nao `Boss::Attack`.

Para polimorfismo dinamico, voce precisa de `virtual`.

---

## 15. `virtual`

`virtual` permite que uma chamada por ponteiro ou referencia da classe base execute a versao da classe derivada.

```cpp
#include <iostream>

class Enemy
{
public:
    virtual void Attack()
    {
        std::cout << "Enemy attacks\n";
    }
};

class Boss : public Enemy
{
public:
    void Attack() override
    {
        std::cout << "Boss attacks hard\n";
    }
};

int main()
{
    Boss boss;
    Enemy* enemy = &boss;

    enemy->Attack();

    return 0;
}
```

Saida:

```text
Boss attacks hard
```

Isso e polimorfismo.

O ponteiro tem tipo `Enemy*`, mas o objeto real e `Boss`.

Como `Attack` e virtual, C++ resolve a chamada em runtime.

### Sem `virtual`

Se `Attack` nao fosse virtual:

```cpp
Enemy* enemy = &boss;
enemy->Attack();
```

chamaria `Enemy::Attack`.

Regra:

> Se voce quer sobrescrita polimorfica por ponteiro/referencia base, o metodo da base precisa ser `virtual`.

---

## 16. `override`

Use `override` quando uma classe derivada sobrescreve um metodo virtual.

```cpp
class Boss : public Enemy
{
public:
    void Attack() override
    {
        std::cout << "Boss attacks hard\n";
    }
};
```

`override` pede ao compilador:

> verifique se eu realmente estou sobrescrevendo um metodo virtual da classe base.

Isso evita bugs.

Exemplo:

```cpp
class Enemy
{
public:
    virtual void Attack(int damage)
    {
    }
};

class Boss : public Enemy
{
public:
    void Attack() override
    {
    }
};
```

Isso da erro, porque `Attack()` nao sobrescreve `Attack(int)`.

Sem `override`, voce poderia achar que sobrescreveu, mas na verdade criou outra funcao.

Regra:

> Sempre use `override` ao sobrescrever metodo virtual.

Na UE5 isso e obrigatorio na pratica:

```cpp
virtual void BeginPlay() override;
virtual void Tick(float DeltaTime) override;
virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
```

---

## 17. Polimorfismo por referencia

Polimorfismo tambem funciona por referencia.

```cpp
void MakeEnemyAttack(Enemy& enemy)
{
    enemy.Attack();
}

int main()
{
    Enemy enemy;
    Boss boss;

    MakeEnemyAttack(enemy);
    MakeEnemyAttack(boss);

    return 0;
}
```

Se `Attack` for virtual:

- `enemy` chama `Enemy::Attack`
- `boss` chama `Boss::Attack`

Esse padrao e comum:

```cpp
void ProcessActor(AActor& Actor)
{
}
```

Ou com ponteiro quando pode ser nulo:

```cpp
void ProcessActor(AActor* Actor)
{
    if (Actor == nullptr)
    {
        return;
    }
}
```

---

## 18. Custo de `virtual`

Metodo virtual tem custo.

Normalmente existe uma indirecao em runtime para descobrir qual implementacao chamar.

Voce nao precisa ter medo disso. O custo geralmente e pequeno, mas nao e zero.

O ponto e:

> use `virtual` quando precisar de polimorfismo real, nao em todo metodo por habito.

Na Unreal, muitos metodos do engine sao virtuais porque a engine precisa chamar comportamento definido nas suas classes derivadas.

Exemplo:

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
}
```

O engine chama `BeginPlay` de varios tipos concretos de actor. Isso e polimorfismo aplicado a gameplay.

---

## 19. Destrutor virtual

Quando voce pretende deletar objetos derivados atraves de ponteiro para base, o destrutor da base deve ser virtual.

Exemplo perigoso:

```cpp
class Base
{
public:
    ~Base()
    {
        std::cout << "Base destroyed\n";
    }
};

class Derived : public Base
{
public:
    ~Derived()
    {
        std::cout << "Derived destroyed\n";
    }
};

int main()
{
    Base* object = new Derived();
    delete object; // problema: destrutor da base nao e virtual
}
```

Versao correta:

```cpp
class Base
{
public:
    virtual ~Base()
    {
        std::cout << "Base destroyed\n";
    }
};
```

Agora:

```cpp
Base* object = new Derived();
delete object;
```

chama corretamente:

```text
Derived destroyed
Base destroyed
```

Regra:

> Classe base polimorfica deve ter destrutor virtual.

Na UE5, voce normalmente nao da `delete` em `UObject`/`AActor`, mas o conceito de destrucao correta em hierarquia continua importante.

---

## 20. Classes abstratas

Classe abstrata e uma classe que nao pode ser instanciada porque possui pelo menos um metodo virtual puro.

Metodo virtual puro:

```cpp
virtual void Attack() = 0;
```

Exemplo:

```cpp
class IAttackable
{
public:
    virtual ~IAttackable() = default;

    virtual void ReceiveDamage(int amount) = 0;
    virtual bool IsAlive() const = 0;
};
```

`IAttackable` define um contrato:

- quem implementa precisa ter `ReceiveDamage`
- quem implementa precisa ter `IsAlive`

Nao da para fazer:

```cpp
IAttackable target; // erro
```

Mas da para fazer:

```cpp
class Enemy : public IAttackable
{
public:
    Enemy(int initialHealth)
        : health(initialHealth)
    {
    }

    void ReceiveDamage(int amount) override
    {
        health -= amount;

        if (health < 0)
        {
            health = 0;
        }
    }

    bool IsAlive() const override
    {
        return health > 0;
    }

private:
    int health;
};
```

Uso:

```cpp
void DamageTarget(IAttackable& target)
{
    target.ReceiveDamage(10);
}
```

`DamageTarget` nao precisa saber se o alvo e `Enemy`, `Boss`, `Barrel` ou outra coisa. So precisa saber que ele cumpre o contrato `IAttackable`.

---

## 21. Interfaces em C++ puro

C++ nao tem uma palavra `interface` como algumas linguagens.

Voce cria uma interface usando classe abstrata com metodos virtuais puros.

Convencao comum:

```cpp
class IDamageable
{
public:
    virtual ~IDamageable() = default;

    virtual void ApplyDamage(int amount) = 0;
    virtual bool IsDead() const = 0;
};
```

O `I` no comeco e convencao para interface.

Na Unreal, interfaces tem um sistema proprio com macros:

```cpp
UINTERFACE()
class UDamageable : public UInterface
{
    GENERATED_BODY()
};

class IDamageable
{
    GENERATED_BODY()
};
```

Voce nao precisa dominar isso agora. Mas o conceito base e o mesmo:

> interface define contrato sem fixar implementacao concreta.

---

## 22. Heranca na UE5

Na UE5, heranca aparece imediatamente.

Exemplo comum:

```cpp
class AMyActor : public AActor
{
};
```

Leia:

> `AMyActor` e um `AActor`.

Outro:

```cpp
class AMyCharacter : public ACharacter
{
};
```

Leia:

> `AMyCharacter` e um `ACharacter`.

Hierarquia simplificada:

```text
UObject
  AActor
    APawn
      ACharacter
```

Conceitualmente:

- `UObject`: base de muitos objetos da Unreal
- `AActor`: objeto que pode existir no mundo
- `APawn`: actor que pode ser possuido/controlado
- `ACharacter`: pawn com movimento de personagem pronto

Isso e OOP na pratica.

Sem entender heranca, o codigo da Unreal vira decoracao.

---

## 23. `Super`

Na Unreal, voce vera:

```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}
```

`Super` e uma convencao/macro da Unreal para se referir a classe base.

Em C++ puro, voce poderia escrever:

```cpp
BaseClass::BeginPlay();
```

Na Unreal, `Super::BeginPlay()` significa:

> chame a implementacao da classe base antes/depois da minha logica.

Isso importa porque a classe base pode fazer inicializacoes internas do engine.

Regra pratica na UE5:

> Em overrides de metodos do engine, geralmente chame `Super::Metodo()` salvo quando voce souber exatamente por que nao deve.

---

## 24. Componentes e composicao na UE5

Apesar da Unreal usar muita heranca, ela tambem usa composicao fortemente.

Um `ACharacter` pode ter:

- `USkeletalMeshComponent`
- `UCameraComponent`
- `USpringArmComponent`
- `UCharacterMovementComponent`
- componentes customizados

Isso e composicao:

```text
Character tem Camera
Character tem Mesh
Character tem MovementComponent
```

Nao e:

```text
Character e uma Camera
```

Exemplo conceitual em C++ puro:

```cpp
class CameraComponent
{
public:
    void Activate()
    {
        std::cout << "Camera active\n";
    }
};

class Character
{
public:
    void BeginPlay()
    {
        camera.Activate();
    }

private:
    CameraComponent camera;
};
```

Essa forma evita criar uma arvore de heranca enorme para cada combinacao de comportamento.

### Regra de design

Use heranca quando:

- existe relacao real "e um"
- voce precisa de polimorfismo
- a API/framework exige

Use composicao quando:

- um objeto "tem um" outro objeto
- voce quer montar comportamento por partes
- heranca criaria combinacoes rigidas

Na Unreal, voce vai herdar de classes do engine, mas criar gameplay frequentemente envolve componentes.

---

## 25. Exemplo completo: inimigos com polimorfismo

```cpp
#include <iostream>
#include <string>
#include <vector>

class Enemy
{
public:
    Enemy(const std::string& newName, int initialHealth)
        : name(newName), health(initialHealth)
    {
    }

    virtual ~Enemy() = default;

    virtual void Attack()
    {
        std::cout << name << " attacks normally\n";
    }

    void ApplyDamage(int amount)
    {
        health -= amount;

        if (health < 0)
        {
            health = 0;
        }
    }

    bool IsAlive() const
    {
        return health > 0;
    }

    const std::string& GetName() const
    {
        return name;
    }

protected:
    std::string name;
    int health;
};

class GoblinEnemy : public Enemy
{
public:
    GoblinEnemy()
        : Enemy("Goblin", 30)
    {
    }

    void Attack() override
    {
        std::cout << name << " attacks quickly\n";
    }
};

class BossEnemy : public Enemy
{
public:
    BossEnemy()
        : Enemy("Boss", 150)
    {
    }

    void Attack() override
    {
        std::cout << name << " uses a heavy attack\n";
    }
};

void MakeAttack(Enemy& enemy)
{
    enemy.Attack();
}

int main()
{
    GoblinEnemy goblin;
    BossEnemy boss;

    MakeAttack(goblin);
    MakeAttack(boss);

    return 0;
}
```

Pontos importantes:

- `Enemy` e classe base
- `GoblinEnemy` e `BossEnemy` sao derivadas
- `Attack` e virtual
- derivadas usam `override`
- `MakeAttack` recebe `Enemy&`
- a chamada executa o comportamento real do objeto derivado

### Observacao sobre `protected`

`protected` permite acesso na propria classe e nas derivadas.

```cpp
protected:
    std::string name;
    int health;
```

Use com cuidado. `protected` pode ser util em hierarquias, mas tambem aumenta acoplamento entre base e derivadas.

Regra inicial:

- prefira `private`
- use `protected` quando derivadas realmente precisam acessar
- nao deixe tudo `public` por comodidade

---

## 26. Exemplo melhor com interface

Agora vamos separar "pode receber dano" de "e inimigo".

```cpp
#include <iostream>
#include <string>

class IDamageable
{
public:
    virtual ~IDamageable() = default;

    virtual void ApplyDamage(int amount) = 0;
    virtual bool IsAlive() const = 0;
};

class Enemy : public IDamageable
{
public:
    Enemy(const std::string& newName, int initialHealth)
        : name(newName), health(initialHealth)
    {
    }

    void ApplyDamage(int amount) override
    {
        health -= amount;

        if (health < 0)
        {
            health = 0;
        }
    }

    bool IsAlive() const override
    {
        return health > 0;
    }

    void Print() const
    {
        std::cout << name << " health: " << health << "\n";
    }

private:
    std::string name;
    int health;
};

class BreakableCrate : public IDamageable
{
public:
    BreakableCrate()
        : durability(20)
    {
    }

    void ApplyDamage(int amount) override
    {
        durability -= amount;

        if (durability < 0)
        {
            durability = 0;
        }
    }

    bool IsAlive() const override
    {
        return durability > 0;
    }

private:
    int durability;
};

void DamageSomething(IDamageable& target, int amount)
{
    target.ApplyDamage(amount);
}
```

Agora `DamageSomething` funciona com qualquer coisa que implemente `IDamageable`.

Isso e design por contrato.

---

## 27. Armadilhas comuns

### Armadilha 1 - Classe que deixa estado invalido

Ruim:

```cpp
struct Player
{
    int health;
};
```

Qualquer lugar pode fazer:

```cpp
player.health = -100;
```

Melhor:

```cpp
class Player
{
public:
    void ApplyDamage(int amount);
    int GetHealth() const;

private:
    int health = 100;
};
```

### Armadilha 2 - Getter e setter sem regra

Getter e setter automatico para tudo nao e encapsulamento forte.

```cpp
void SetHealth(int newHealth)
{
    health = newHealth;
}
```

Se isso aceita `-999`, voce so moveu o problema.

Melhor:

```cpp
void SetHealth(int newHealth)
{
    if (newHealth < 0)
    {
        health = 0;
        return;
    }

    health = newHealth;
}
```

Ou melhor ainda: exponha a operacao do dominio:

```cpp
void ApplyDamage(int amount);
void Heal(int amount);
```

Em vez de deixar qualquer codigo definir vida diretamente.

### Armadilha 3 - Esquecer `override`

Ruim:

```cpp
void Beginplay();
```

Talvez voce queria sobrescrever `BeginPlay`, mas escreveu errado.

Com `override`, o compilador protege:

```cpp
void Beginplay() override; // erro, nao existe metodo base com esse nome
```

### Armadilha 4 - Heranca para tudo

Se voce cria:

```text
FireFlyingPoisonBossEnemy
IceFlyingPoisonBossEnemy
FireGroundPoisonBossEnemy
```

talvez voce esteja usando heranca para combinacoes que deveriam ser componentes/dados.

Prefira compor:

```text
Enemy tem MovementBehavior
Enemy tem AttackBehavior
Enemy tem ElementType
```

Na Unreal, isso vira:

```text
AActor + componentes + dados configuraveis
```

---

## 28. Quando vale ver video

Para esta aula, video ajuda principalmente em:

- "C++ virtual functions vtable explained"
- "C++ inheritance and polymorphism explained"
- "C++ abstract classes and interfaces"
- "C++ constructors initializer list explained"
- "composition over inheritance game development"

O video mais importante e sobre `virtual` e polimorfismo com diagrama.

Procure um video que mostre:

- ponteiro da classe base
- objeto real da classe derivada
- chamada virtual
- diferenca com/sem `virtual`

Se o video nao mostra esse contraste, provavelmente vai ficar superficial.

---

## 29. Exercicios teoricos

Responda com suas palavras:

1. Qual a diferenca pratica entre `struct` e `class` em C++?
2. Por que deixar todos os membros `public` pode ser perigoso?
3. O que e uma invariante?
4. Por que construtor ajuda a criar objeto valido?
5. Por que initializer list e melhor que atribuir tudo no corpo do construtor?
6. O que o `const` depois de um metodo significa?
7. O que significa heranca publica?
8. Por que heranca deve representar "e um"?
9. Qual a diferenca entre heranca e composicao?
10. O que `virtual` muda em uma chamada por ponteiro da classe base?
11. Por que usar `override`?
12. Quando uma classe e abstrata?
13. O que uma interface representa?
14. Por que uma classe base polimorfica deve ter destrutor virtual?
15. Como `ACharacter : public APawn` se encaixa no conceito de heranca?

Se voce travar nas perguntas 10, 11 e 14, revise `virtual`, `override` e destrutor virtual antes de avancar.

---

## 30. Exercicios praticos

### Exercicio 1 - Classe simples com encapsulamento

Crie uma classe `Health`.

Requisitos:

- membro privado `int current`
- membro privado `int maximum`
- construtor recebe `initial` e `max`
- vida nunca pode ser menor que 0
- vida nunca pode ser maior que `maximum`
- metodo `ApplyDamage(int amount)`
- metodo `Heal(int amount)`
- metodo `GetCurrent() const`
- metodo `GetMaximum() const`
- metodo `IsDead() const`

Perguntas:

1. Qual e a invariante da classe?
2. Por que `current` e `maximum` devem ser privados?
3. Quais metodos devem ser `const`?

### Exercicio 2 - Weapon com initializer list

Crie uma classe `Weapon`.

Membros:

- `std::string name`
- `int ammo`
- `int maxAmmo`
- `int damage`

Requisitos:

- construtor com initializer list
- `Fire()` reduz ammo se puder atirar
- `Reload()` restaura ammo
- `CanFire() const`
- `GetDamage() const`
- `PrintStatus() const`

Regra:

- `ammo` nunca pode ser negativo
- `ammo` nunca pode passar de `maxAmmo`
- `maxAmmo` nunca pode ser negativo

### Exercicio 3 - Heranca basica

Crie:

```cpp
class Character
class Player : public Character
class Enemy : public Character
```

`Character` deve ter:

- nome
- vida
- `Move()`
- `ApplyDamage()`
- `IsAlive() const`

`Player` deve ter:

- `UsePotion()`

`Enemy` deve ter:

- `DropLoot()`

Teste no `main`.

Pergunta:

> `Player` e um `Character`? `Enemy` e um `Character`?

### Exercicio 4 - Metodo virtual

Em `Character`, crie:

```cpp
virtual void Attack();
```

Em `Player`, sobrescreva:

```cpp
void Attack() override;
```

Em `Enemy`, sobrescreva tambem.

Crie:

```cpp
void MakeAttack(Character& character)
{
    character.Attack();
}
```

Passe `Player` e `Enemy`.

Observe o polimorfismo.

### Exercicio 5 - Sem virtual

Remova `virtual` da base e `override` das derivadas.

Teste uma chamada por ponteiro:

```cpp
Character* character = &player;
character->Attack();
```

Compare com a versao virtual.

Pergunta:

> O que mudou?

### Exercicio 6 - Classe abstrata

Crie uma interface:

```cpp
class IDamageable
{
public:
    virtual ~IDamageable() = default;
    virtual void ApplyDamage(int amount) = 0;
    virtual bool IsAlive() const = 0;
};
```

Faca `Enemy` implementar `IDamageable`.

Crie tambem `BreakableBarrel` implementando `IDamageable`.

Crie:

```cpp
void DamageTarget(IDamageable& target, int amount)
{
    target.ApplyDamage(amount);
}
```

Teste com inimigo e barril.

### Exercicio 7 - Composicao

Crie:

```cpp
class Inventory
class Player
```

`Player` deve ter um `Inventory`, nao herdar de `Inventory`.

Pergunta:

> Por que `Player : public Inventory` seria conceitualmente errado?

### Exercicio 8 - Componente de vida

Crie:

```cpp
class HealthComponent
```

Depois crie:

```cpp
class Enemy
{
private:
    HealthComponent health;
};
```

Objetivo:

> simular a ideia de composicao que voce vera na Unreal com Actor Components.

---

## 31. Mini-projeto - Combat OOP Training

Crie um arquivo:

```text
combat_oop_training.cpp
```

Objetivo:

Criar uma pequena arquitetura de combate usando classes, encapsulamento, heranca, polimorfismo e composicao.

### Classes obrigatorias

Voce deve criar:

```text
HealthComponent
Weapon
Character
PlayerCharacter
EnemyCharacter
BossCharacter
```

### `HealthComponent`

Responsabilidade:

> controlar vida e proteger invariante.

Requisitos:

- `currentHealth`
- `maxHealth`
- construtor com initializer list
- `ApplyDamage(int amount)`
- `Heal(int amount)`
- `IsDead() const`
- `GetCurrent() const`
- `GetMax() const`

Regras:

- vida nunca menor que 0
- vida nunca maior que max
- max nunca menor que 0

### `Weapon`

Responsabilidade:

> controlar nome, dano e municao.

Requisitos:

- `name`
- `damage`
- `ammo`
- `maxAmmo`
- `CanFire() const`
- `Fire()`
- `Reload()`
- `GetDamage() const`
- `GetName() const`

Regras:

- ammo nunca menor que 0
- ammo nunca maior que maxAmmo
- dano nao deve ser negativo

### `Character`

Classe base.

Responsabilidade:

> representar comportamento comum de personagens.

Requisitos:

- nome
- `HealthComponent`
- `Weapon`
- construtor com initializer list
- destrutor virtual
- `virtual void Attack(Character& target)`
- `virtual void PrintStatus() const`
- `bool IsAlive() const`
- `const std::string& GetName() const`
- `void ReceiveDamage(int amount)`

### `PlayerCharacter`

Deriva de `Character`.

Requisitos:

- sobrescrever `Attack`
- metodo `UsePotion()`

### `EnemyCharacter`

Deriva de `Character`.

Requisitos:

- sobrescrever `Attack`
- metodo `Taunt() const`

### `BossCharacter`

Deriva de `EnemyCharacter` ou `Character`.

Escolha uma das duas e explique sua decisao.

Requisitos:

- sobrescrever `Attack`
- ataque deve causar dano maior ou ter mensagem diferente
- metodo `Enrage()`

### Funcoes livres obrigatorias

```cpp
void RunTurn(Character& attacker, Character& defender);
void PrintBattleState(const Character& first, const Character& second);
```

`RunTurn` deve chamar `attacker.Attack(defender)`.

Como `Attack` e virtual, o comportamento real depende do tipo concreto do atacante.

### Exemplo de fluxo no `main`

```cpp
int main()
{
    PlayerCharacter player("Knight");
    EnemyCharacter enemy("Skeleton");
    BossCharacter boss("Dragon");

    RunTurn(player, enemy);
    RunTurn(enemy, player);
    RunTurn(boss, player);

    PrintBattleState(player, enemy);
    PrintBattleState(player, boss);

    return 0;
}
```

### Perguntas obrigatorias

Responda em:

```text
respostas_aula_03.txt
```

1. Quais classes usam encapsulamento?
2. Quais membros ficaram `private` e por que?
3. Quais metodos sao `const`?
4. Onde voce usou initializer list?
5. Qual classe base voce criou?
6. Quais classes derivadas voce criou?
7. Onde existe polimorfismo?
8. Por que `Attack` e virtual?
9. Onde voce usou `override`?
10. Onde voce usou composicao?
11. Por que `HealthComponent` nao deveria ser classe base de `Character`?
12. O que no seu codigo lembra `ACharacter` da UE5?
13. O que no seu codigo lembra Actor Components da UE5?
14. Se voce removesse `virtual`, o que quebraria conceitualmente?
15. Por que uma classe base com metodos virtuais deve ter destrutor virtual?

---

## 32. Esqueleto inicial opcional

Use este esqueleto se quiser comecar mais rapido, mas complete por conta propria.

```cpp
#include <iostream>
#include <string>

class HealthComponent
{
public:
    HealthComponent(int initialHealth, int maximumHealth)
        : currentHealth(initialHealth), maxHealth(maximumHealth)
    {
        if (maxHealth < 0)
        {
            maxHealth = 0;
        }

        Clamp();
    }

    void ApplyDamage(int amount)
    {
        if (amount < 0)
        {
            return;
        }

        currentHealth -= amount;
        Clamp();
    }

    void Heal(int amount)
    {
        if (amount < 0)
        {
            return;
        }

        currentHealth += amount;
        Clamp();
    }

    bool IsDead() const
    {
        return currentHealth <= 0;
    }

    int GetCurrent() const
    {
        return currentHealth;
    }

    int GetMax() const
    {
        return maxHealth;
    }

private:
    void Clamp()
    {
        if (currentHealth < 0)
        {
            currentHealth = 0;
        }

        if (currentHealth > maxHealth)
        {
            currentHealth = maxHealth;
        }
    }

    int currentHealth;
    int maxHealth;
};

class Weapon
{
public:
    Weapon(const std::string& newName, int newDamage, int initialAmmo, int maximumAmmo)
        : name(newName), damage(newDamage), ammo(initialAmmo), maxAmmo(maximumAmmo)
    {
        if (damage < 0)
        {
            damage = 0;
        }

        if (maxAmmo < 0)
        {
            maxAmmo = 0;
        }

        ClampAmmo();
    }

    bool CanFire() const
    {
        return ammo > 0;
    }

    void Fire()
    {
        if (ammo > 0)
        {
            --ammo;
        }
    }

    void Reload()
    {
        ammo = maxAmmo;
    }

    int GetDamage() const
    {
        return damage;
    }

    const std::string& GetName() const
    {
        return name;
    }

private:
    void ClampAmmo()
    {
        if (ammo < 0)
        {
            ammo = 0;
        }

        if (ammo > maxAmmo)
        {
            ammo = maxAmmo;
        }
    }

    std::string name;
    int damage;
    int ammo;
    int maxAmmo;
};
```

Continue criando `Character`, `PlayerCharacter`, `EnemyCharacter` e `BossCharacter`.

---

## 33. Relacao com a proxima fase

Depois desta aula, voce vai estar pronto para estudar C++ moderno com mais qualidade:

- `std::vector`
- `std::string`
- `std::optional`
- smart pointers
- `std::unique_ptr`
- `std::shared_ptr`
- `std::weak_ptr`
- copy vs move com mais profundidade
- lambdas
- templates introdutorios

Orientacao a objetos tambem prepara diretamente para a Unreal:

- herdar de classes do engine
- sobrescrever `BeginPlay`, `Tick` e callbacks
- entender `Super`
- modelar gameplay com Actors e Components
- evitar heranca exagerada
- usar composicao para montar comportamento

---

## 34. Checklist de dominio

Antes de avancar, voce deve conseguir responder:

- O que diferencia `struct` e `class`?
- O que `public` permite?
- O que `private` protege?
- O que e encapsulamento?
- O que e uma invariante?
- Para que serve construtor?
- Para que serve initializer list?
- Quando o destrutor roda?
- O que significa `const` em um metodo?
- O que significa `class Player : public Character`?
- Quando heranca faz sentido?
- Quando composicao e melhor?
- O que `virtual` faz?
- O que `override` protege?
- O que e polimorfismo?
- O que e classe abstrata?
- O que e interface em C++ puro?
- Por que destrutor virtual importa em classe base polimorfica?
- Como isso aparece em `AActor`, `APawn` e `ACharacter`?

Se voce entende isso, a hierarquia da Unreal deixa de parecer magia e passa a parecer uma aplicacao grande de conceitos conhecidos.

---

## 35. Resumo final

Orientacao a objetos em C++ nao e apenas juntar dados e funcoes.

E sobre criar tipos que:

- nascem validos
- protegem seu estado
- comunicam intencao
- controlam modificacoes
- podem ser usados por contrato
- podem participar de hierarquias
- tem comportamento sobrescrito com custo real
- sao destruidos corretamente

Para UE5, esta aula e essencial porque a engine e organizada em torno de classes, heranca, componentes e overrides.

Quando voce escrever:

```cpp
class AMyCharacter : public ACharacter
```

voce deve enxergar:

- `AMyCharacter` e um `ACharacter`
- `ACharacter` ja traz comportamento da engine
- voce vai sobrescrever metodos virtuais
- `Super::BeginPlay()` chama a base
- componentes adicionam comportamento por composicao
- ponteiros para objetos da engine exigem entendimento de lifetime e ownership

Essa e a base para parar de copiar codigo da Unreal sem entender e comecar a ler a arquitetura por tras dele.

---

## 36. Tarefa final da aula

Crie:

```text
combat_oop_training.cpp
respostas_aula_03.txt
```

No `.cpp`, implemente o mini-projeto.

No `.txt`, responda as perguntas obrigatorias da secao 31.

So considere a aula concluida quando voce conseguir explicar em voz alta:

1. por que `HealthComponent` e composicao
2. por que `PlayerCharacter` herda de `Character`
3. por que `Attack` e virtual
4. por que `override` protege contra erro
5. por que membros privados ajudam a manter invariantes
6. por que isso se parece com UE5

Se conseguir explicar isso sem olhar resposta pronta, voce esta pronto para seguir para C++ moderno.
