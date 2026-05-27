# Checklist Final - O que devemos saber sobre C++ na Unreal Engine 5

Este arquivo serve como revisao final depois das aulas 1 a 6.

Use assim:

1. Leia cada secao.
2. Marque mentalmente: "sei explicar", "sei usar", "ja vi mas nao domino", "nao entendi".
3. Tudo que cair em "nao entendi" deve virar revisao antes de implementar sistemas importantes no jogo.

O objetivo nao e decorar a Unreal inteira. O objetivo e garantir que voces sabem o suficiente para escrever gameplay C++ com consciencia, sem depender apenas de copiar codigo.

---

## 1. Ideia central

Unreal Engine 5 usa C++, mas nao e apenas C++ puro.

E C++ somado a:

- macros
- reflection
- garbage collection
- code generation
- editor
- Blueprints
- gameplay framework
- tipos proprios
- sistema de build proprio

Frase que todos devem conseguir explicar:

> UE5 nao substitui C++; ela estende C++ com sistemas proprios para que o codigo converse com editor, Blueprints, serializacao, garbage collection e gameplay framework.

Se essa frase nao ficou clara, revise a Aula 06.

---

## 2. Base de C++ que realmente importa para UE5

Antes de escrever gameplay C++ serio, voces devem saber:

- declarar variaveis
- escolher tipos basicos
- usar `const`
- usar `auto` sem esconder entendimento
- escrever funcoes
- entender passagem por valor
- entender passagem por referencia
- entender passagem por ponteiro
- entender `nullptr`
- entender escopo
- entender lifetime
- entender stack vs heap em nivel conceitual
- entender classes e structs
- entender construtores
- entender destrutores em C++ puro
- entender encapsulamento
- entender heranca
- entender `virtual`
- entender `override`
- entender `const` em metodos
- entender headers `.h`
- entender sources `.cpp`
- entender `#include`
- entender forward declaration

Nao precisa ser especialista em C++ antes da UE5, mas precisa entender esses pontos.

---

## 3. Valor, referencia e ponteiro

Esse e um dos blocos mais importantes.

### Valor

```cpp
void Foo(int Value);
```

Passa uma copia.

Bom para:

- `int`
- `float`
- `bool`
- enums
- tipos pequenos

### Referencia

```cpp
void ApplyDamage(FHealthData& Health);
```

Passa o objeto original.

Use quando:

- o valor precisa existir
- a funcao deve alterar o original

### Referencia constante

```cpp
void PrintData(const FWeaponData& Data);
```

Le sem copiar e sem alterar.

Use muito para:

- structs grandes
- strings
- arrays
- objetos de dados

### Ponteiro

```cpp
void SetTarget(AActor* Target);
```

Pode representar ausencia.

Use quando:

- o objeto pode ser nulo
- voce esta referenciando objeto gerenciado por outro sistema
- a API da Unreal pede ponteiro

Pergunta obrigatoria:

> Se esse parametro pode ser nulo, por que ele nao deveria ser uma referencia?

Resposta esperada:

> Porque referencia representa melhor um valor obrigatorio. Ponteiro pode representar ausencia com `nullptr`.

---

## 4. Lifetime e ownership

Todos devem saber responder:

- quando uma variavel local morre?
- o que e dangling pointer?
- o que e ponteiro nulo?
- o que e ownership?
- quem e dono desse objeto?
- quem pode destruir esse objeto?
- quem apenas observa esse objeto?

Em C++ puro:

```cpp
std::unique_ptr<Item>
```

comunica ownership exclusivo.

Em UE5:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> Item;
```

comunica referencia rastreada pelo sistema da Unreal.

Nao sao a mesma coisa.

Frase essencial:

> Em UE5, ponteiro para `UObject` ou `AActor` normalmente nao significa "eu sou dono e vou dar delete".

---

## 5. Headers, cpp e build

Todos precisam entender:

```text
.h   -> declara interface
.cpp -> implementa comportamento
```

Exemplo:

```cpp
// HealthComponent.h
class UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    void ApplyDamage(float Amount);
};
```

```cpp
// HealthComponent.cpp
void UHealthComponent::ApplyDamage(float Amount)
{
}
```

### Saber diagnosticar

Erro de compilacao:

- sintaxe errada
- tipo desconhecido
- include faltando
- metodo chamado errado

Erro de linkagem:

- declarou mas nao implementou
- assinatura diferente
- `.cpp` nao entrou no build
- simbolo duplicado

Erro do Unreal Header Tool:

- macro errada
- `.generated.h` fora de ordem
- tipo nao suportado em `UPROPERTY`
- falta de `GENERATED_BODY`

Pergunta obrigatoria:

> O que significa unresolved external symbol?

Resposta:

> O codigo declarou/chamou algo, mas o linker nao encontrou uma definicao valida.

---

## 6. Includes e forward declaration

Devem saber:

```cpp
#include "Weapon.h"
```

cola o conteudo do header antes da compilacao.

Forward declaration:

```cpp
class AWeapon;
```

diz apenas que o tipo existe.

### Quando forward declaration basta

Basta para:

```cpp
AWeapon* Weapon;
AWeapon& WeaponRef;
```

em muitos headers.

### Quando nao basta

Nao basta quando:

- guarda objeto por valor
- herda da classe
- chama metodo no header
- precisa saber tamanho do tipo

Exemplo:

```cpp
class AWeapon;

class APlayerCharacter
{
private:
    AWeapon Weapon; // errado com tipo incompleto
};
```

Precisa incluir o header completo.

---

## 7. Macros fundamentais da Unreal

Todos devem saber o papel destas macros:

```cpp
UCLASS()
USTRUCT()
UENUM()
UPROPERTY()
UFUNCTION()
GENERATED_BODY()
```

### `UCLASS`

Marca classe para reflection da Unreal.

```cpp
UCLASS()
class MYPROJECT_API AMyActor : public AActor
{
    GENERATED_BODY()
};
```

### `USTRUCT`

Marca struct para reflection.

```cpp
USTRUCT(BlueprintType)
struct FWeaponStats
{
    GENERATED_BODY()
};
```

### `UENUM`

Marca enum para reflection.

```cpp
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle,
    Launcher
};
```

### `UPROPERTY`

Marca propriedade para editor, Blueprint, serializacao, GC e outros sistemas.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
float MaxHealth = 100.0f;
```

### `UFUNCTION`

Marca funcao para reflection.

```cpp
UFUNCTION(BlueprintCallable, Category = "Health")
void ApplyDamage(float Amount);
```

### `GENERATED_BODY`

Injeta codigo gerado pela Unreal.

Obrigatorio em classes/structs/interfaces refletidas.

---

## 8. `.generated.h`

Todo header Unreal com macros de reflection costuma ter:

```cpp
#include "MyActor.generated.h"
```

Regra:

> O `.generated.h` deve ser o ultimo include do header.

Correto:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"
```

Errado:

```cpp
#include "MyActor.generated.h"
#include "SomeOtherHeader.h"
```

Todos devem saber que esse arquivo e gerado pelo Unreal Header Tool.

---

## 9. Prefixos de tipos da Unreal

Devem reconhecer:

- `A`: actor
- `U`: UObject nao actor
- `F`: struct/tipo de valor
- `E`: enum
- `I`: interface
- `T`: template/tipo generico Unreal
- `b`: bool

Exemplos:

```cpp
AActor
ACharacter
UObject
UActorComponent
FVector
FRotator
FString
EWeaponType
IInteractable
TArray
TMap
bool bIsAlive;
```

Pergunta obrigatoria:

> Se uma classe comeca com `A`, o que voce espera dela?

Resposta:

> Que derive de `AActor` e possa existir no mundo.

---

## 10. `UObject`

Todos devem saber:

`UObject` e a base do sistema de objetos da Unreal.

Participa de:

- reflection
- garbage collection
- serializacao
- propriedades
- Blueprints
- editor

Criacao:

```cpp
UInventoryItem* Item = NewObject<UInventoryItem>(this);
```

Nao fazer:

```cpp
new UInventoryItem();
delete Item;
```

Pergunta obrigatoria:

> Por que `UObject` nao deve ser tratado como objeto C++ comum?

Resposta:

> Porque participa do sistema da Unreal, incluindo GC, reflection, Outer e code generation.

---

## 11. `AActor`

Todos devem saber:

`AActor` e algo que pode existir no mundo.

Pode:

- ser colocado no level
- ser spawnado
- ter transform
- ter componentes
- receber `BeginPlay`
- receber `Tick`
- ser destruido com `Destroy`

Criacao:

```cpp
GetWorld()->SpawnActor<AMyActor>(ActorClass, Location, Rotation);
```

Destruicao:

```cpp
Destroy();
```

Nao fazer:

```cpp
delete SomeActor;
```

---

## 12. Componentes

`UActorComponent` adiciona comportamento/dados a actors.

Exemplos:

- `UHealthComponent`
- `UInventoryComponent`
- `UInteractionComponent`
- `UWeaponComponent`
- `UCameraComponent`
- `UStaticMeshComponent`

Componentes representam composicao:

```text
Character tem HealthComponent
Character tem InventoryComponent
Character tem CameraComponent
```

Nao:

```text
Character e HealthComponent
```

Criar componente default no construtor:

```cpp
HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
```

Com propriedade:

```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<UHealthComponent> HealthComponent;
```

---

## 13. Lifecycle de Actor

Todos devem entender a diferenca:

### Construtor

Usado para:

- criar componentes default
- configurar defaults
- definir Tick

```cpp
AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = false;
}
```

### `BeginPlay`

Chamado quando o jogo comeca para o actor.

Usado para:

- iniciar logica de gameplay
- buscar referencias runtime
- iniciar timers

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
}
```

### `Tick`

Chamado todo frame, se habilitado.

```cpp
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
```

Use com cuidado.

### `EndPlay`

Chamado quando o actor sai do jogo.

Usado para:

- limpar timers
- remover bindings
- soltar referencias temporarias

Pergunta obrigatoria:

> Por que nao devemos colocar qualquer logica de gameplay no construtor?

Resposta:

> Porque o mundo e outros actors podem ainda nao estar prontos. O construtor e para defaults e subobjetos.

---

## 14. Garbage Collection da Unreal

Todos devem saber:

Unreal tem GC para `UObject`.

Isso significa:

- voce nao da `delete` em `UObject`
- referencias importantes devem ser visiveis para o GC
- `UPROPERTY` ajuda o GC a rastrear referencias

Exemplo:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> CurrentItem;
```

Risco:

```cpp
UInventoryItem* CurrentItem;
```

Se isso deveria manter o objeto vivo e nao esta marcado, pode virar problema.

Frase obrigatoria:

> `UPROPERTY` nao e so para aparecer no editor; tambem pode ser importante para serializacao e garbage collection.

---

## 15. Ponteiros Unreal

Devem reconhecer:

### Ponteiro cru

```cpp
AActor* Target;
```

Pode ser usado, mas nao comunica ownership.

### `TObjectPtr`

```cpp
UPROPERTY()
TObjectPtr<AActor> Target;
```

Referencia para `UObject` integrada ao modelo moderno da UE5.

### `TWeakObjectPtr`

```cpp
TWeakObjectPtr<AActor> CurrentTarget;
```

Referencia fraca: observa sem manter vivo.

Uso:

```cpp
if (CurrentTarget.IsValid())
{
    AActor* Target = CurrentTarget.Get();
}
```

### `TSubclassOf`

```cpp
UPROPERTY(EditDefaultsOnly)
TSubclassOf<AProjectile> ProjectileClass;
```

Representa uma classe derivada de `AProjectile`.

Usado para spawn/configuracao pelo editor.

---

## 16. Validacao

Devem saber diferenciar:

```cpp
if (Target != nullptr)
{
}
```

de:

```cpp
if (IsValid(Target))
{
}
```

`nullptr` verifica apenas se o ponteiro nao e nulo.

`IsValid` e mais apropriado para muitos objetos Unreal, pois considera estado do `UObject`.

Regra pratica:

> Para `UObject`/`AActor` que pode ter sido destruido, pense em `IsValid`.

---

## 17. Tipos de texto

Devem saber:

### `FString`

Texto mutavel/manipulavel.

```cpp
FString PlayerName = TEXT("Player");
```

### `FName`

Identificador eficiente.

```cpp
FName ItemId = TEXT("HealthPotion");
```

### `FText`

Texto para usuario/UI/localizacao.

```cpp
FText DisplayName = FText::FromString(TEXT("Health Potion"));
```

Regra:

```text
FString -> manipular texto
FName   -> identificador
FText   -> texto para jogador
```

---

## 18. Containers Unreal

Devem reconhecer:

### `TArray`

Array dinamico.

```cpp
TArray<AActor*> Targets;
```

### `TMap`

Mapa chave/valor.

```cpp
TMap<FName, int32> Inventory;
```

### `TSet`

Conjunto de valores unicos.

```cpp
TSet<FName> Tags;
```

### Paralelos com STL

```text
std::vector        -> TArray
std::unordered_map -> TMap
std::set           -> TSet, conceitualmente
std::string        -> FString, dependendo do caso
std::optional      -> TOptional
```

Em propriedades Unreal, prefira tipos Unreal.

---

## 19. Exposicao para editor e Blueprint

Devem saber usar no minimo:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
float MaxHealth = 100.0f;
```

```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<UHealthComponent> HealthComponent;
```

```cpp
UFUNCTION(BlueprintCallable, Category = "Health")
void ApplyDamage(float Amount);
```

```cpp
UFUNCTION(BlueprintPure, Category = "Health")
float GetHealthPercent() const;
```

### Saber diferenciar

`BlueprintCallable`:

- vira node executavel
- pode ter efeito colateral

`BlueprintPure`:

- node puro
- deve ser leitura/calculo sem efeito colateral relevante

`BlueprintReadOnly`:

- Blueprint le

`BlueprintReadWrite`:

- Blueprint le e escreve

---

## 20. Gameplay Framework

Devem saber a funcao geral:

### `AActor`

Entidade no mundo.

### `APawn`

Actor que pode ser possuido.

### `ACharacter`

Pawn com movimento de personagem pronto.

### `APlayerController`

Controla input e possui pawn.

### `AAIController`

Controller para IA.

### `AGameModeBase` / `AGameMode`

Regras da partida. No multiplayer, existe no servidor.

### `AGameStateBase` / `AGameState`

Estado compartilhado da partida.

### `APlayerState`

Estado de cada jogador.

### `UGameInstance`

Vive entre mapas durante a sessao.

Resumo:

```text
Controller controla Pawn
Character e um Pawn especializado
Pawn e um Actor possuivel
Actor existe no mundo
GameMode define regras
GameState guarda estado da partida
PlayerState guarda estado do jogador
GameInstance persiste entre mapas
```

---

## 21. Spawn e criacao

Devem saber diferenciar:

### Criar componente default

No construtor:

```cpp
Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
```

### Criar `UObject`

```cpp
UInventoryItem* Item = NewObject<UInventoryItem>(this);
```

### Criar actor no mundo

```cpp
AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
    ProjectileClass,
    SpawnLocation,
    SpawnRotation
);
```

### Destruir actor

```cpp
Destroy();
```

Pergunta obrigatoria:

> Quando uso `CreateDefaultSubobject`, `NewObject` e `SpawnActor`?

Resposta:

> `CreateDefaultSubobject` para componentes default no construtor, `NewObject` para `UObject`, `SpawnActor` para actor no mundo.

---

## 22. Logs

Devem saber usar:

```cpp
UE_LOG(LogTemp, Warning, TEXT("Hello"));
```

Com numero:

```cpp
UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
```

Com `FString`:

```cpp
UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *PlayerName);
```

Log e ferramenta basica de debug.

---

## 23. Lambdas na UE5

Nao precisa dominar lambdas avancadas, mas precisa reconhecer usos comuns:

- sort
- filtros
- timers
- callbacks
- async

Exemplo com sort:

```cpp
Enemies.Sort([](const AEnemy* A, const AEnemy* B)
{
    return A->GetHealth() < B->GetHealth();
});
```

Exemplo com timer:

```cpp
GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
{
    DoSomething();
}, 1.0f, false);
```

Cuidado:

> Capturar `this` pode ser perigoso se a lambda rodar depois que o actor/componente morreu.

Alternativa:

```cpp
TWeakObjectPtr<AMyActor> WeakThis = this;
```

---

## 24. Templates na UE5

Mesmo sem criar templates proprios, voces vao usar templates o tempo todo:

```cpp
TArray<AActor*>
TMap<FName, int32>
TObjectPtr<UCameraComponent>
TSubclassOf<AProjectile>
```

Funcoes template comuns:

```cpp
FindComponentByClass<UHealthComponent>()
```

```cpp
CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"))
```

```cpp
NewObject<UInventoryItem>(this)
```

```cpp
SpawnActor<AProjectile>(...)
```

Todos devem entender:

> O tipo entre `< >` informa para a funcao/classe generica qual tipo concreto ela deve usar.

Nao precisam criar templates proprios no inicio.

---

## 25. O que pode ficar para depois

Nao precisa dominar agora:

- templates avancados
- metaprogramacao
- move semantics profundo
- escrever allocators
- macros C++ complexas
- networking avancado
- replicacao profunda
- GAS
- multithreading avancado
- Slate C++ profundo
- modules/build avancado
- editor tooling

Mas e bom saber que existem.

---

## 26. O que nao pode ficar fraco

Esses pontos nao podem ficar mal entendidos:

- ponteiro vs referencia
- `nullptr`
- lifetime
- ownership
- `UPROPERTY`
- `UObject` vs `AActor`
- `CreateDefaultSubobject` vs `NewObject` vs `SpawnActor`
- `Destroy` vs `delete`
- `BeginPlay` vs construtor
- `Tick` com custo
- headers e includes
- `.generated.h`
- `GENERATED_BODY`
- `UCLASS`
- `UPROPERTY`
- `UFUNCTION`
- `TArray`
- `FString`, `FName`, `FText`
- `TObjectPtr`
- `TWeakObjectPtr`
- `TSubclassOf`

Se alguem do grupo nao entende esses pontos, essa pessoa ainda precisa revisar antes de mexer em sistemas centrais do jogo.

---

## 27. Erros comuns que devemos evitar

### Usar `new` em `UObject`

Errado:

```cpp
UInventoryItem* Item = new UInventoryItem();
```

Correto:

```cpp
UInventoryItem* Item = NewObject<UInventoryItem>(this);
```

### Usar `delete` em actor

Errado:

```cpp
delete Enemy;
```

Correto:

```cpp
Enemy->Destroy();
```

### Esquecer `UPROPERTY`

Suspeito:

```cpp
UInventoryItem* CurrentItem;
```

Melhor:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> CurrentItem;
```

### Colocar include depois do `.generated.h`

Errado:

```cpp
#include "MyActor.generated.h"
#include "Weapon.h"
```

### Habilitar Tick sem precisar

Ruim:

```cpp
PrimaryActorTick.bCanEverTick = true;
```

sem motivo claro.

### Guardar ponteiro e assumir que sempre sera valido

Errado conceitualmente:

```cpp
Target->Destroy();
Target->DoSomething();
```

Depois de destruir, nao trate como objeto normal valido.

---

## 28. Checklist de leitura de uma classe UE5

Ao abrir qualquer classe C++ da Unreal, identifique:

- qual classe base?
- tem `UCLASS`?
- tem `GENERATED_BODY`?
- quais includes existem?
- o `.generated.h` esta por ultimo?
- quais propriedades sao `UPROPERTY`?
- quais funcoes sao `UFUNCTION`?
- quais membros sao componentes?
- quais membros sao referencias para actors/objects?
- usa `TObjectPtr`?
- usa `TWeakObjectPtr`?
- o construtor cria subobjetos?
- `BeginPlay` chama `Super::BeginPlay()`?
- `Tick` esta habilitado por necessidade?
- existe `Destroy`?
- existe `SpawnActor`?
- existe `NewObject`?
- existe lambda capturando `this`?
- existe ponteiro que pode ser nulo?
- existe include que poderia ser forward declaration?

Esse checklist ajuda a ler codigo real da engine/projeto.

---

## 29. Mini prova final

Cada membro do grupo deve conseguir responder sem olhar:

1. O que `UCLASS` faz?
2. O que `UPROPERTY` faz?
3. O que `UFUNCTION` faz?
4. O que `GENERATED_BODY` faz?
5. Por que existe `.generated.h`?
6. O que e `UObject`?
7. O que e `AActor`?
8. O que e `UActorComponent`?
9. Quando uso `CreateDefaultSubobject`?
10. Quando uso `NewObject`?
11. Quando uso `SpawnActor`?
12. Por que nao dou `delete` em actor?
13. O que `Destroy` faz?
14. Por que `UPROPERTY` importa para GC?
15. O que e `TObjectPtr`?
16. O que e `TWeakObjectPtr`?
17. O que e `TSubclassOf`?
18. Diferenca entre `FString`, `FName`, `FText`?
19. O que e `TArray`?
20. O que e `TMap`?
21. O que e `BeginPlay`?
22. O que e `Tick`?
23. Por que Tick pode ser perigoso para performance?
24. O que e `PlayerController`?
25. O que e `GameMode`?
26. O que e `GameInstance`?
27. Quando usar ponteiro?
28. Quando usar referencia?
29. Quando usar `const&`?
30. O que e forward declaration?

Se alguem nao consegue responder pelo menos 24 dessas 30, essa pessoa deve revisar antes de pegar tarefas C++ mais arriscadas.

---

## 30. Sinais de que podemos comecar o jogo

Voces estao prontos para comecar o jogo em C++ se conseguem:

- criar uma classe C++ derivada de `AActor`
- criar uma classe C++ derivada de `UActorComponent`
- expor propriedade no editor
- expor funcao para Blueprint
- criar componente no construtor
- usar `BeginPlay`
- usar `Tick` so quando necessario
- criar um actor com `SpawnActor`
- destruir actor com `Destroy`
- validar ponteiro antes de usar
- guardar referencia Unreal com `UPROPERTY`
- usar `TArray`
- usar `FName` para identificador
- usar `FText` para texto de UI
- separar `.h` e `.cpp`
- resolver erro simples de include
- entender erro simples do UHT

Nao precisa saber tudo para comecar.

Mas precisa saber o suficiente para nao construir sistemas em cima de conceitos errados.

---

## 31. Plano de revisao para o grupo

Para alinhar todos:

### Quem esta na Aula 1

Foco:

- tipos
- funcoes
- valor/referencia/ponteiro
- `const`

### Quem esta na Aula 2

Foco:

- lifetime
- stack/heap
- dangling pointer
- ownership
- RAII em conceito

### Quem esta na Aula 3

Foco:

- classe/struct
- encapsulamento
- heranca
- `virtual`
- `override`
- composicao

### Quem esta na Aula 4

Foco:

- `TArray` paralelo com `std::vector`
- `const&`
- `std::move` em conceito
- lambda basica
- template como uso, nao criacao avancada

### Quem esta na Aula 5

Foco:

- `.h` e `.cpp`
- include
- forward declaration
- linkagem
- `.generated.h`

### Quem esta na Aula 6

Foco:

- macros Unreal
- `UObject`
- `AActor`
- componentes
- GC
- tipos Unreal
- gameplay framework

---

## 32. Primeiro conjunto de sistemas recomendado

Depois das aulas, implementem estes sistemas pequenos antes de partir para sistemas grandes:

1. `AInteractableDoor`
2. `UHealthComponent`
3. `AHealthPickup`
4. `AProjectile`
5. `AWeaponActor`
6. `UInventoryComponent`
7. `IInteractable` interface
8. `AEnemyCharacter`
9. `APlayerCharacter`
10. `UGameInstance` custom simples

Cada um treina uma parte:

- actor
- component
- interface
- pointer
- spawn
- destroy
- Blueprint integration
- propriedade editavel
- lifecycle
- composicao

---

## 33. Frase final

Se ao final das aulas voces conseguirem explicar isto, a base esta boa:

> Em UE5, C++ continua exigindo entendimento de tipo, ponteiro, referencia, lifetime, ownership, classe, heranca e compilacao. A Unreal adiciona reflection, macros, garbage collection, tipos proprios e gameplay framework. Programar bem na UE5 e saber onde termina o C++ puro e onde comecam as regras da engine.

Esse e o criterio final.

Se essa frase parece obvia, voces estao prontos para comecar.

Se essa frase parece confusa, revisem antes de construir sistemas centrais do jogo.
