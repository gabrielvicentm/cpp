# Guia Rapido - Programacao C++ na Unreal Engine 5

Este arquivo e uma consulta rapida para programar em C++ na UE5.

Ele nao substitui as aulas. A ideia e ser um guia de bolso para lembrar:

- macros principais
- classes base
- lifecycle
- tipos da Unreal
- ponteiros e GC
- componentes
- spawn/destroy
- Blueprint integration
- padroes comuns

---

## 1. Ideia central

UE5 usa C++ com uma camada propria:

- reflection
- macros
- garbage collection
- code generation
- editor
- Blueprints
- gameplay framework

Frase importante:

> C++ continua valendo, mas a Unreal precisa enxergar seu codigo. Macros como `UCLASS`, `UPROPERTY` e `UFUNCTION` dizem para a engine quais classes, variaveis e funcoes participam dos sistemas dela.

---

## 2. Estrutura basica de uma classe Unreal

### Header

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

### Source

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

---

## 3. Macros principais

### `UCLASS`

Marca classe para reflection da Unreal.

```cpp
UCLASS()
class MYPROJECT_API AMyActor : public AActor
{
    GENERATED_BODY()
};
```

Usada em classes derivadas de tipos Unreal, como `UObject`, `AActor`, `UActorComponent`.

### `GENERATED_BODY`

Injeta codigo gerado pela Unreal.

```cpp
GENERATED_BODY()
```

Obrigatorio em classes/structs/interfaces refletidas.

### `UPROPERTY`

Marca variavel para sistemas da Unreal.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
float MaxHealth = 100.0f;
```

Pode afetar:

- editor
- Blueprint
- serializacao
- garbage collection
- replicacao

### `UFUNCTION`

Marca funcao para reflection.

```cpp
UFUNCTION(BlueprintCallable, Category = "Health")
void ApplyDamage(float Amount);
```

Pode expor funcao para Blueprint, RPC, eventos etc.

### `USTRUCT`

Marca struct para reflection.

```cpp
USTRUCT(BlueprintType)
struct FWeaponStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 25.0f;
};
```

### `UENUM`

Marca enum para reflection.

```cpp
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle,
    Launcher,
    Melee
};
```

---

## 4. Especificadores comuns de `UPROPERTY`

```cpp
UPROPERTY(EditAnywhere)
```

Editavel no editor em defaults e instancias.

```cpp
UPROPERTY(EditDefaultsOnly)
```

Editavel nos defaults/classes.

```cpp
UPROPERTY(EditInstanceOnly)
```

Editavel por instancia colocada no mundo.

```cpp
UPROPERTY(VisibleAnywhere)
```

Visivel no editor, mas nao editavel.

```cpp
UPROPERTY(BlueprintReadOnly)
```

Blueprint pode ler.

```cpp
UPROPERTY(BlueprintReadWrite)
```

Blueprint pode ler e escrever.

```cpp
UPROPERTY(Category = "Combat")
```

Organiza no painel do editor.

Exemplo comum:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
float Damage = 20.0f;
```

---

## 5. Especificadores comuns de `UFUNCTION`

```cpp
UFUNCTION(BlueprintCallable)
```

Funcao vira node executavel em Blueprint.

```cpp
UFUNCTION(BlueprintPure)
```

Funcao vira node puro, normalmente para leitura/calculo sem efeito colateral.

```cpp
UFUNCTION(BlueprintImplementableEvent)
```

Declarada em C++, implementada no Blueprint.

```cpp
UFUNCTION(BlueprintNativeEvent)
```

Pode ter implementacao C++ e ser sobrescrita no Blueprint.

Exemplo:

```cpp
UFUNCTION(BlueprintPure, Category = "Health")
float GetHealthPercent() const;
```

---

## 6. Prefixos de tipos da Unreal

| Prefixo | Significado | Exemplo |
|---|---|---|
| `A` | Actor | `AActor`, `ACharacter` |
| `U` | UObject nao actor | `UObject`, `UActorComponent` |
| `F` | Struct/tipo de valor | `FVector`, `FString` |
| `E` | Enum | `EWeaponType` |
| `I` | Interface | `IInteractable` |
| `T` | Template/container Unreal | `TArray`, `TMap` |
| `b` | Booleano | `bIsAlive` |

---

## 7. Classes base importantes

### `UObject`

Base do sistema de objetos da Unreal.

Usado para objetos que precisam de:

- reflection
- garbage collection
- serializacao
- editor/Blueprint

Criacao:

```cpp
UInventoryItem* Item = NewObject<UInventoryItem>(this);
```

### `AActor`

Objeto que existe no mundo.

Pode:

- ter transform
- ter componentes
- ser colocado no level
- ser spawnado
- receber `BeginPlay`
- receber `Tick`
- ser destruido com `Destroy`

### `UActorComponent`

Componente anexado a actor.

Use para comportamento reutilizavel:

- vida
- inventario
- interacao
- arma
- status

### `APawn`

Actor que pode ser possuido por um controller.

### `ACharacter`

Pawn com estrutura pronta para personagem:

- capsule
- mesh
- movement component

### `APlayerController`

Controla input e possui pawn.

### `AGameModeBase`

Define regras da partida.

### `AGameStateBase`

Estado compartilhado da partida.

### `APlayerState`

Estado de um jogador.

### `UGameInstance`

Objeto persistente entre mapas durante a sessao.

---

## 8. Lifecycle basico de Actor

### Construtor

Use para:

- criar componentes default
- configurar valores padrao
- habilitar/desabilitar Tick

```cpp
AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = false;
}
```

### `BeginPlay`

Chamado quando o jogo comeca para aquele actor.

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
}
```

Use para logica runtime inicial.

### `Tick`

Chamado todo frame se habilitado.

```cpp
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
```

Use com cuidado. Tick demais custa performance.

### `EndPlay`

Chamado quando o actor sai do jogo.

```cpp
void AMyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}
```

Use para limpar timers, delegates e referencias temporarias.

---

## 9. Componentes

### Criar componente default

No header:

```cpp
class UStaticMeshComponent;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<UStaticMeshComponent> Mesh;
```

No cpp:

```cpp
#include "Components/StaticMeshComponent.h"

AMyActor::AMyActor()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
}
```

Use `CreateDefaultSubobject` no construtor.

---

## 10. Criacao e destruicao

### Criar componente default

```cpp
Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
```

### Criar `UObject`

```cpp
UInventoryItem* Item = NewObject<UInventoryItem>(this);
```

### Spawnar actor

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

Ou:

```cpp
TargetActor->Destroy();
```

Nunca use `delete` em `AActor`.

---

## 11. Ponteiros e GC

### Ponteiro cru

```cpp
AActor* Target = nullptr;
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

Referencia fraca. Nao mantem o objeto vivo.

Uso:

```cpp
if (CurrentTarget.IsValid())
{
    AActor* Target = CurrentTarget.Get();
}
```

### Validacao

```cpp
if (IsValid(TargetActor))
{
    TargetActor->Destroy();
}
```

`IsValid` e mais apropriado para muitos `UObject`/`AActor` do que apenas `!= nullptr`.

---

## 12. Tipos de texto

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

Texto para UI/localizacao.

```cpp
FText DisplayName = FText::FromString(TEXT("Health Potion"));
```

Regra:

```text
FString -> manipular texto
FName   -> identificador
FText   -> texto mostrado ao jogador
```

---

## 13. Containers Unreal

### `TArray`

Array dinamico.

```cpp
TArray<AActor*> Targets;
Targets.Add(SomeActor);
```

### `TMap`

Mapa chave/valor.

```cpp
TMap<FName, int32> Inventory;
Inventory.Add(TEXT("Ammo"), 30);
```

Buscar:

```cpp
if (int32* Count = Inventory.Find(TEXT("Ammo")))
{
    UE_LOG(LogTemp, Warning, TEXT("Ammo: %d"), *Count);
}
```

### `TSet`

Conjunto de valores unicos.

```cpp
TSet<FName> Tags;
Tags.Add(TEXT("Interactable"));
```

---

## 14. Classes configuraveis

Use `TSubclassOf` quando quiser escolher uma classe derivada no editor.

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Spawn")
TSubclassOf<AActor> ActorClassToSpawn;
```

Spawn:

```cpp
if (ActorClassToSpawn)
{
    GetWorld()->SpawnActor<AActor>(ActorClassToSpawn, GetActorLocation(), GetActorRotation());
}
```

Muito usado para:

- projetil
- pickup
- actor configuravel
- widget
- inimigo

---

## 15. Logs

```cpp
UE_LOG(LogTemp, Warning, TEXT("Hello"));
```

Com numero:

```cpp
UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
```

Com `FString`:

```cpp
FString Name = TEXT("Player");
UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *Name);
```

---

## 16. Exemplo rapido - Health Component

### Header

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float Amount);

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercent() const;

private:
    UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHealth = 100.0f;
};
```

### Source

```cpp
#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::ApplyDamage(float Amount)
{
    if (Amount <= 0.0f)
    {
        return;
    }

    CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
}

void UHealthComponent::Heal(float Amount)
{
    if (Amount <= 0.0f)
    {
        return;
    }

    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
}

float UHealthComponent::GetHealthPercent() const
{
    if (MaxHealth <= 0.0f)
    {
        return 0.0f;
    }

    return CurrentHealth / MaxHealth;
}
```

---

## 17. Exemplo rapido - Actor com componente

### Header

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthActor.generated.h"

class UHealthComponent;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AHealthActor : public AActor
{
    GENERATED_BODY()

public:
    AHealthActor();

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UHealthComponent> HealthComponent;
};
```

### Source

```cpp
#include "HealthActor.h"

#include "Components/StaticMeshComponent.h"
#include "HealthComponent.h"

AHealthActor::AHealthActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}
```

---

## 18. Blueprint integration

### Propriedade editavel

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
float Damage = 20.0f;
```

### Funcao chamavel

```cpp
UFUNCTION(BlueprintCallable, Category = "Combat")
void Fire();
```

### Funcao pura

```cpp
UFUNCTION(BlueprintPure, Category = "Health")
float GetHealthPercent() const;
```

### Evento implementado no Blueprint

```cpp
UFUNCTION(BlueprintImplementableEvent, Category = "Health")
void OnDeath();
```

### Evento com implementacao C++ e override em Blueprint

```cpp
UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
void Interact(AActor* InstigatorActor);
```

Implementacao C++:

```cpp
void AMyActor::Interact_Implementation(AActor* InstigatorActor)
{
}
```

---

## 19. Interfaces

Exemplo conceitual:

```cpp
UINTERFACE(BlueprintType)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class IInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* InstigatorActor);
};
```

Use interface quando classes diferentes precisam cumprir o mesmo contrato.

---

## 20. Gameplay framework em uma tela

```text
UGameInstance
    persiste entre mapas

AGameModeBase
    regras da partida, servidor

AGameStateBase
    estado compartilhado da partida

APlayerState
    estado de cada jogador

APlayerController
    input, UI, posse de pawn

APawn
    actor possuivel

ACharacter
    pawn com movimento de personagem

AActor
    entidade no mundo

UActorComponent
    comportamento/dados acoplados a actor
```

---

## 21. Includes e `.generated.h`

Header Unreal tipico:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"
```

Regra:

```text
O .generated.h deve ser o ultimo include do header.
```

Use forward declaration quando possivel:

```cpp
class UCameraComponent;
```

Inclua o header completo no `.cpp`:

```cpp
#include "Camera/CameraComponent.h"
```

---

## 22. Padroes de decisao rapida

### Preciso criar um actor no mundo?

Use:

```cpp
SpawnActor
```

### Preciso criar um UObject?

Use:

```cpp
NewObject
```

### Preciso criar componente default?

Use no construtor:

```cpp
CreateDefaultSubobject
```

### Preciso expor variavel no editor?

Use:

```cpp
UPROPERTY(EditAnywhere)
```

### Preciso expor funcao para Blueprint?

Use:

```cpp
UFUNCTION(BlueprintCallable)
```

### Preciso guardar referencia para UObject em classe Unreal?

Use:

```cpp
UPROPERTY()
TObjectPtr<UTipo> Valor;
```

### Preciso observar algo que pode sumir?

Use:

```cpp
TWeakObjectPtr<ATipo>
```

---

## 23. Erros comuns

### Usar `new` para `UObject`

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
delete TargetActor;
```

Correto:

```cpp
TargetActor->Destroy();
```

### Esquecer `UPROPERTY`

Suspeito:

```cpp
UInventoryItem* Item;
```

Melhor:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> Item;
```

### Colocar include depois do `.generated.h`

Errado:

```cpp
#include "MyActor.generated.h"
#include "OtherHeader.h"
```

### Habilitar Tick sem necessidade

Evite:

```cpp
PrimaryActorTick.bCanEverTick = true;
```

se nao precisa atualizar todo frame.

### Capturar `this` em lambda longa sem pensar

Cuidado:

```cpp
[this]()
{
    DoSomething();
}
```

Se rodar depois do objeto ser destruido, pode dar problema.

---

## 24. Mini checklist antes de commitar C++ UE5

Antes de considerar o codigo ok, confira:

- A classe tem `UCLASS`/`USTRUCT` quando precisa?
- Tem `GENERATED_BODY`?
- O `.generated.h` e o ultimo include do header?
- Ponteiros para `UObject` importantes estao com `UPROPERTY`?
- Componentes default foram criados com `CreateDefaultSubobject`?
- Actors sao criados com `SpawnActor`?
- UObjects sao criados com `NewObject`?
- Actors sao removidos com `Destroy`, nao `delete`?
- `Tick` esta realmente necessario?
- Funcoes de leitura sao `const`?
- Parametros grandes usam `const&`?
- Ponteiros opcionais sao validados?
- O header tem includes demais que poderiam virar forward declaration?
- Propriedades editaveis tem `Category`?
- Funcoes para Blueprint tem `UFUNCTION` correto?

---

## 25. Regra final

Quando estiver em duvida, pergunte:

```text
Isso e C++ puro ou objeto da Unreal?
Quem e dono?
Quem pode destruir?
O GC enxerga essa referencia?
Precisa aparecer no editor?
Precisa aparecer no Blueprint?
Isso precisa existir no mundo?
Isso precisa ser componente?
```

Essas perguntas evitam a maior parte dos erros iniciais em C++ na UE5.
