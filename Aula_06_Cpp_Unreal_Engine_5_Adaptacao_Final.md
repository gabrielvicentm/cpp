# Aula 06 - Adaptacao do C++ para Unreal Engine 5

Esta aula cobre a Fase 6 do `Roadmap_Cpp_UE5.md`: o que muda no C++ da Unreal Engine 5.

Ate aqui voce construiu a base:

- Aula 01: fundamentos de C++
- Aula 02: memoria, ponteiros, lifetime, RAII e ownership
- Aula 03: orientacao a objetos
- Aula 04: C++ moderno, STL, smart pointers, move semantics, lambdas e templates
- Aula 05: headers, `.cpp`, preprocessador, linkagem e organizacao de projeto

Agora voce entra na camada especifica da Unreal.

O ponto central desta aula:

> UE5 nao substitui C++. UE5 estende C++ com macros, reflection, garbage collection, tipos proprios, code generation e um framework de gameplay.

Se voce tentar aprender Unreal C++ como se fosse C++ puro, vai estranhar.

Se voce tentar aprender Unreal C++ sem entender C++ puro, vai decorar sintaxe sem entender lifetime, ownership e build.

Esta aula serve para unir as duas coisas.

---

## O que voce vai aprender

Nesta aula voce vai estudar:

- o que e o "C++ da Unreal"
- por que existem macros como `UCLASS`, `UPROPERTY` e `UFUNCTION`
- o papel do Unreal Header Tool
- `generated.h` e `GENERATED_BODY`
- reflection system
- `UObject`
- `AActor`
- `UActorComponent`
- `APawn`
- `ACharacter`
- `GameMode`
- `PlayerController`
- `GameState`
- `PlayerState`
- lifecycle basico de actors
- construtor vs `BeginPlay`
- `Tick`
- `CreateDefaultSubobject`
- `SpawnActor`
- `NewObject`
- `Destroy`
- garbage collection da Unreal
- por que `UPROPERTY` importa para GC
- `TObjectPtr`
- `TWeakObjectPtr`
- `TArray`
- `TMap`
- `TSet`
- `FString`
- `FName`
- `FText`
- diferenca entre tipos STL e tipos Unreal
- exposicao para Blueprints
- erros conceituais comuns

---

## Como estudar esta aula

Esta aula e diferente das anteriores.

Voce nao deve tentar memorizar todas as macros e classes da Unreal de uma vez. O objetivo e entender o modelo.

Ordem recomendada:

1. Leia a parte de macros e reflection.
2. Entenda `UObject` e `AActor`.
3. Estude lifecycle: construtor, `BeginPlay`, `Tick`, `Destroy`.
4. Estude garbage collection e `UPROPERTY`.
5. Estude tipos da Unreal.
6. Estude componentes e gameplay framework.
7. Faca os exercicios teoricos.
8. Faca o mini-projeto conceitual em UE5.

Tempo sugerido:

- 60 min: macros, reflection e generated code
- 60 min: `UObject`, `AActor` e lifecycle
- 60 min: garbage collection, `UPROPERTY` e ponteiros Unreal
- 60 min: tipos da Unreal
- 60 min: gameplay framework
- 90 min: exercicios e mini-projeto

Se o tempo estiver curto, priorize:

- `UCLASS`
- `GENERATED_BODY`
- `UPROPERTY`
- `UFUNCTION`
- `AActor`
- `UActorComponent`
- `BeginPlay`
- `Tick`
- `TObjectPtr`
- garbage collection

---

## 1. O que e o C++ da Unreal

Unreal C++ e C++ com uma camada forte de infraestrutura da engine.

Voce ainda escreve C++:

```cpp
class AMyActor : public AActor
{
};
```

Mas tambem escreve macros:

```cpp
UCLASS()
class MYPROJECT_API AMyActor : public AActor
{
    GENERATED_BODY()
};
```

Essas macros nao sao enfeite.

Elas conectam sua classe a sistemas da Unreal:

- editor
- reflection
- garbage collection
- Blueprints
- serializacao
- propriedades editaveis
- networking
- detalhes de build

Em C++ puro, uma classe normal nao sabe nada sobre editor, Blueprint ou GC.

Na Unreal, uma classe marcada corretamente pode aparecer no editor, ter propriedades editaveis, ser usada em Blueprint e participar do sistema de objetos da engine.

---

## 2. Reflection system

Reflection e a capacidade de um sistema conhecer informacoes sobre tipos, propriedades e funcoes em runtime/editor.

C++ padrao tem reflection muito limitada.

A Unreal cria seu proprio sistema de reflection usando:

- macros
- Unreal Header Tool
- arquivos `.generated.h`
- codigo gerado

Exemplo:

```cpp
UCLASS()
class MYPROJECT_API AHealthPickup : public AActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealAmount = 25.0f;

    UFUNCTION(BlueprintCallable)
    void ActivatePickup();
};
```

Com isso, a Unreal consegue saber:

- existe uma classe chamada `AHealthPickup`
- ela deriva de `AActor`
- existe uma propriedade `HealAmount`
- essa propriedade pode ser editada no editor
- essa propriedade pode ser lida/escrita por Blueprint
- existe uma funcao `ActivatePickup`
- essa funcao pode ser chamada por Blueprint

Sem reflection, o editor nao teria como tratar suas classes C++ desse jeito.

---

## 3. Unreal Header Tool

Unreal Header Tool, ou UHT, e uma ferramenta da Unreal que processa headers antes/ao redor da compilacao C++ normal.

Ele procura macros como:

```cpp
UCLASS()
USTRUCT()
UENUM()
UPROPERTY()
UFUNCTION()
```

E gera codigo adicional.

Por isso voce inclui:

```cpp
#include "MyActor.generated.h"
```

Esse arquivo e gerado pela Unreal.

Regra importante:

> Em um header da Unreal que usa reflection, o `.generated.h` deve ser o ultimo include.

Exemplo:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"
```

Nao coloque includes depois de `HealthPickup.generated.h`.

---

## 4. `GENERATED_BODY`

`GENERATED_BODY()` injeta codigo gerado pela Unreal dentro da classe/struct.

Exemplo:

```cpp
UCLASS()
class MYPROJECT_API AHealthPickup : public AActor
{
    GENERATED_BODY()

public:
    AHealthPickup();
};
```

Voce nao ve diretamente todo o codigo que entra ali.

Mas ele e necessario para:

- reflection
- metadados da classe
- integracao com sistema de objetos
- construcao interna da Unreal
- serializacao
- Blueprint

Regra:

> Se a classe usa `UCLASS`, precisa de `GENERATED_BODY`.

O mesmo vale para:

```cpp
USTRUCT()
struct FWeaponStats
{
    GENERATED_BODY()
};
```

---

## 5. Prefixos de nomes na Unreal

A Unreal usa convencoes de prefixo.

Principais:

- `U`: classes derivadas de `UObject`, mas que nao sao actors
- `A`: classes derivadas de `AActor`
- `F`: structs e tipos de valor
- `E`: enums
- `I`: interfaces
- `T`: templates/tipos genericos da Unreal
- `b`: booleanos

Exemplos:

```cpp
UActorComponent
AActor
FVector
FRotator
EWeaponType
IInteractable
TArray
bool bIsAlive;
```

Esses prefixos nao sao aleatorios. Eles comunicam categoria e expectativa de uso.

Quando voce ve:

```cpp
AEnemyCharacter
```

ja sabe que e um actor.

Quando ve:

```cpp
UHealthComponent
```

ja sabe que e um `UObject`, provavelmente componente ou objeto da engine.

Quando ve:

```cpp
FWeaponStats
```

ja sabe que e struct/tipo de valor.

---

## 6. `UCLASS`

`UCLASS()` marca uma classe para o sistema de reflection da Unreal.

Exemplo:

```cpp
UCLASS()
class MYPROJECT_API AHealthPickup : public AActor
{
    GENERATED_BODY()
};
```

Essa classe agora pode participar do sistema da Unreal.

Voce pode adicionar especificadores:

```cpp
UCLASS(Blueprintable)
class MYPROJECT_API AHealthPickup : public AActor
{
    GENERATED_BODY()
};
```

Alguns especificadores comuns:

- `Blueprintable`: pode ser usada como base para Blueprint
- `BlueprintType`: pode ser usada como tipo em Blueprint
- `Abstract`: classe abstrata
- `NotBlueprintable`: nao pode virar Blueprint

Nao precisa decorar todos agora.

Entenda a ideia:

> `UCLASS` informa ao UHT e a Unreal como tratar essa classe.

---

## 7. `USTRUCT`

`USTRUCT()` marca uma struct para reflection.

Exemplo:

```cpp
USTRUCT(BlueprintType)
struct FWeaponStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireRate = 0.2f;
};
```

Use `USTRUCT` para dados que devem ser conhecidos pela Unreal:

- stats de arma
- dados de item
- configuracoes
- resultados de calculo
- informacoes serializaveis
- dados expostos ao editor/Blueprint

Comparacao:

```cpp
struct WeaponStats
{
    float Damage;
};
```

Essa struct existe para C++.

```cpp
USTRUCT(BlueprintType)
struct FWeaponStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float Damage;
};
```

Essa struct tambem participa da Unreal reflection.

---

## 8. `UENUM`

`UENUM()` marca enum para reflection.

Exemplo:

```cpp
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle UMETA(DisplayName = "Rifle"),
    Launcher UMETA(DisplayName = "Launcher"),
    Melee UMETA(DisplayName = "Melee")
};
```

Use quando quiser que o enum apareca no editor ou Blueprint.

Em C++ puro:

```cpp
enum class EWeaponType
{
    Rifle,
    Launcher,
    Melee
};
```

Na Unreal:

```cpp
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle,
    Launcher,
    Melee
};
```

O `: uint8` e comum em enums expostos na Unreal.

---

## 9. `UPROPERTY`

`UPROPERTY()` marca uma variavel membro para reflection.

Exemplo:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
float MaxHealth = 100.0f;
```

Isso pode permitir que a propriedade:

- apareca no editor
- seja serializada
- seja acessivel por Blueprint
- seja rastreada pelo garbage collector
- participe de networking, dependendo dos especificadores

`UPROPERTY` e uma das macros mais importantes da Unreal.

### Especificadores comuns

```cpp
UPROPERTY(EditAnywhere)
```

Editavel em varios contextos do editor.

```cpp
UPROPERTY(EditDefaultsOnly)
```

Editavel nos defaults/classes, nao por instancia colocada no mundo.

```cpp
UPROPERTY(EditInstanceOnly)
```

Editavel por instancia no mundo.

```cpp
UPROPERTY(VisibleAnywhere)
```

Visivel, mas nao editavel.

```cpp
UPROPERTY(BlueprintReadOnly)
```

Blueprint pode ler, mas nao escrever.

```cpp
UPROPERTY(BlueprintReadWrite)
```

Blueprint pode ler e escrever.

```cpp
UPROPERTY(Transient)
```

Nao deve ser salvo/serializado de forma persistente.

### Categoria no editor

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
float MaxHealth = 100.0f;
```

`Category` organiza no painel de detalhes.

---

## 10. `UPROPERTY` e garbage collection

Esta e a parte mais importante.

Para objetos derivados de `UObject`, a Unreal tem garbage collector.

O GC precisa saber quais referencias existem para nao destruir objetos ainda usados.

`UPROPERTY` ajuda a Unreal a rastrear referencias.

Exemplo:

```cpp
UPROPERTY()
TObjectPtr<UHealthComponent> HealthComponent;
```

Ou, em codigo legado:

```cpp
UPROPERTY()
UHealthComponent* HealthComponent;
```

Se voce guarda ponteiro para `UObject` sem `UPROPERTY` em um lugar que deveria ser rastreado, o GC pode nao conhecer essa referencia.

Isso pode causar bugs de lifetime.

Regra inicial:

> Se uma classe `UObject` guarda referencia forte para outro `UObject`, pense seriamente em `UPROPERTY`.

Em UE5 moderna, voce vera muito:

```cpp
TObjectPtr<AActor>
TObjectPtr<UActorComponent>
TObjectPtr<UTexture2D>
```

---

## 11. `UFUNCTION`

`UFUNCTION()` marca uma funcao para reflection.

Exemplo:

```cpp
UFUNCTION(BlueprintCallable)
void ApplyDamage(float Amount);
```

Isso permite chamar a funcao em Blueprint.

Especificadores comuns:

```cpp
UFUNCTION(BlueprintCallable)
```

Pode ser chamada em Blueprint.

```cpp
UFUNCTION(BlueprintPure)
```

Funcao sem efeito colateral aparente, usada como node puro.

```cpp
UFUNCTION(BlueprintImplementableEvent)
```

Declarada em C++, implementada em Blueprint.

```cpp
UFUNCTION(BlueprintNativeEvent)
```

Pode ter implementacao C++ e ser sobrescrita em Blueprint.

Exemplo:

```cpp
UFUNCTION(BlueprintPure)
float GetHealthPercent() const;
```

Como e `BlueprintPure`, a funcao deve ser conceitualmente leitura.

---

## 12. `UObject`

`UObject` e a base central do sistema de objetos da Unreal.

Muitos tipos da engine derivam de `UObject`.

`UObject` participa de:

- reflection
- garbage collection
- serializacao
- editor
- sistema de propriedades
- Blueprints

Exemplo:

```cpp
UCLASS(BlueprintType)
class MYPROJECT_API UInventoryItem : public UObject
{
    GENERATED_BODY()
};
```

Quando usar `UObject`?

Use para objetos que precisam:

- participar do sistema da Unreal
- ser refletidos
- ser usados por Blueprint
- ser serializados
- ser gerenciados pelo GC
- existir como objetos de dados/logica sem necessariamente estar no mundo

Mas se o objeto precisa existir no mundo, ter transform, ser colocado em level ou spawnado como entidade espacial, geralmente voce usa `AActor`.

---

## 13. Criando `UObject` com `NewObject`

Em Unreal, voce normalmente nao usa `new` para `UObject`.

Use:

```cpp
UInventoryItem* Item = NewObject<UInventoryItem>(this);
```

O parametro `this` costuma ser o Outer, ou seja, o objeto que contextualiza/possui aquele objeto no sistema da Unreal.

Versao com `TObjectPtr`:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> Item;

Item = NewObject<UInventoryItem>(this);
```

Regra:

> Para `UObject`, use `NewObject`, nao `new`.

E nao use `delete` manualmente.

---

## 14. `AActor`

`AActor` representa algo que pode existir no mundo da Unreal.

Actors podem:

- ser colocados em um level
- ser spawnados
- ter transform
- ter componentes
- receber `BeginPlay`
- receber `Tick`
- ser destruidos com `Destroy`
- participar de colisao, gameplay e networking

Exemplo:

```cpp
UCLASS()
class MYPROJECT_API AHealthPickup : public AActor
{
    GENERATED_BODY()

public:
    AHealthPickup();

protected:
    virtual void BeginPlay() override;
};
```

Se algo tem presenca no mundo, considere `AActor`.

Exemplos:

- pickup
- porta
- plataforma
- projetil
- trigger
- personagem
- item fisico no mapa

---

## 15. Lifecycle basico de `AActor`

O lifecycle da Unreal nao e igual ao de um objeto C++ puro simples.

Pontos importantes:

- construtor
- `OnConstruction`
- `BeginPlay`
- `Tick`
- `EndPlay`
- `Destroy`

### Construtor

Roda quando o objeto e construido.

Use para criar componentes default e configurar valores iniciais.

Exemplo:

```cpp
AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}
```

No construtor, o mundo pode ainda nao estar pronto do jeito que voce espera.

Evite fazer gameplay que depende do level, outros actors ou runtime.

### `BeginPlay`

Chamado quando o jogo comeca para aquele actor.

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
}
```

Use para logica inicial de gameplay.

### `Tick`

Chamado a cada frame se estiver habilitado.

```cpp
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
```

`DeltaTime` e o tempo desde o ultimo frame.

Nao habilite `Tick` sem necessidade. Muitos actors com Tick podem custar performance.

### `EndPlay`

Chamado quando o actor esta saindo do jogo por algum motivo.

```cpp
void AMyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}
```

Use para limpar logica de gameplay, cancelar timers, remover bindings etc.

---

## 16. `CreateDefaultSubobject`

Use no construtor para criar componentes default.

Exemplo:

```cpp
AMyCharacter::AMyCharacter()
{
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(RootComponent);
}
```

Propriedade:

```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
TObjectPtr<UCameraComponent> Camera;
```

Regra:

> Componentes default de actor normalmente sao criados no construtor com `CreateDefaultSubobject`.

Nao use `NewObject` para esse caso sem entender a diferenca.

---

## 17. `UActorComponent`

`UActorComponent` representa uma parte de comportamento que pode ser adicionada a um actor.

Exemplo:

```cpp
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    UFUNCTION(BlueprintCallable)
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintPure)
    float GetHealthPercent() const;

private:
    UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHealth = 100.0f;
};
```

Componentes sao composicao na pratica.

Em vez de criar uma arvore gigante de heranca, voce monta actors com componentes.

Exemplos:

- HealthComponent
- InventoryComponent
- InteractionComponent
- WeaponComponent
- ObjectiveComponent

Regra:

> Actor e a entidade no mundo. Componentes adicionam dados e comportamento.

---

## 18. `SpawnActor`

Para criar actor no mundo, use `SpawnActor`.

Exemplo:

```cpp
AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
    ProjectileClass,
    SpawnLocation,
    SpawnRotation
);
```

Geralmente `ProjectileClass` e:

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Combat")
TSubclassOf<AProjectile> ProjectileClass;
```

`TSubclassOf` representa uma classe derivada de um tipo base.

Exemplo:

```cpp
TSubclassOf<AProjectile>
```

significa:

> uma classe que e `AProjectile` ou deriva de `AProjectile`.

Regra:

> Para criar actor no mundo, use `GetWorld()->SpawnActor`, nao `new`.

---

## 19. `Destroy`

Para destruir actor, normalmente use:

```cpp
Destroy();
```

Ou:

```cpp
TargetActor->Destroy();
```

Isso marca o actor para destruicao pelo sistema da Unreal.

Nao faca:

```cpp
delete TargetActor;
```

Actors pertencem ao mundo/engine, nao ao seu `delete` manual.

Depois de chamar `Destroy`, ponteiros para esse actor precisam ser tratados com cuidado.

Use validacao apropriada:

```cpp
if (IsValid(TargetActor))
{
    TargetActor->Destroy();
}
```

---

## 20. Garbage collection da Unreal

Unreal tem garbage collector para `UObject`.

Ele rastreia objetos ainda referenciados pelo sistema.

Diferente de C++ puro:

- voce nao usa `delete` em `UObject`
- voce nao usa `std::unique_ptr<UObject>` como dono comum
- referencias precisam ser visiveis para o sistema quando devem manter objeto vivo
- `UPROPERTY` e tipos Unreal ajudam o GC a conhecer referencias

Exemplo importante:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> CurrentItem;
```

Isso permite que a Unreal saiba que `CurrentItem` referencia um `UObject`.

Sem `UPROPERTY`, em muitos contextos, o GC pode nao rastrear a referencia como voce espera.

---

## 21. `TObjectPtr`

`TObjectPtr<T>` e usado em UE5 para ponteiros para `UObject` rastreaveis pela engine.

Exemplo:

```cpp
UPROPERTY(VisibleAnywhere)
TObjectPtr<UHealthComponent> HealthComponent;
```

Outro:

```cpp
UPROPERTY(EditAnywhere)
TObjectPtr<UTexture2D> Icon;
```

Pense assim:

> `TObjectPtr` e uma forma moderna da Unreal representar referencia para `UObject` em propriedades.

Em codigo antigo voce vera:

```cpp
UHealthComponent* HealthComponent;
```

Mas em UE5 moderna, `TObjectPtr` aparece bastante em `UPROPERTY`.

---

## 22. `TWeakObjectPtr`

`TWeakObjectPtr<T>` referencia um `UObject` sem mantê-lo vivo.

Use quando voce quer observar algo que pode ser destruido.

Exemplo:

```cpp
TWeakObjectPtr<AActor> CurrentTarget;
```

Uso:

```cpp
if (CurrentTarget.IsValid())
{
    AActor* Target = CurrentTarget.Get();
}
```

Isso e util para alvos, referencias temporarias e objetos que podem desaparecer.

Conceito parecido com `std::weak_ptr`, mas integrado ao sistema da Unreal.

---

## 23. `IsValid`

Para `UObject`, voce frequentemente ve:

```cpp
if (IsValid(SomeActor))
{
}
```

Isso verifica mais do que apenas `nullptr` em alguns contextos, incluindo estado de destruicao pendente.

Comparacao:

```cpp
if (SomeActor != nullptr)
{
}
```

verifica apenas se o ponteiro nao e nulo.

```cpp
if (IsValid(SomeActor))
{
}
```

e mais apropriado em muitos casos com `UObject`.

Regra inicial:

> Para objetos Unreal, prefira validacao Unreal quando o objeto pode ter sido destruido ou estar pendente de destruicao.

---

## 24. Tipos de texto: `FString`, `FName`, `FText`

Na Unreal, texto nao e apenas `std::string`.

### `FString`

Texto mutavel geral.

Use para strings manipulaveis.

```cpp
FString PlayerName = TEXT("Player");
```

### `FName`

Nome/identificador eficiente.

Use para identificadores, keys e nomes comparados frequentemente.

```cpp
FName SocketName = TEXT("WeaponSocket");
```

`FName` nao e texto para UI. E identificador.

### `FText`

Texto para interface/usuario, com suporte a localizacao.

```cpp
FText DisplayName = FText::FromString(TEXT("Health Potion"));
```

Use para UI e texto mostrado ao jogador.

Regra pratica:

- `FString`: texto manipulado
- `FName`: identificador
- `FText`: texto exibido ao usuario

---

## 25. `TEXT()`

Voce vera:

```cpp
TEXT("Hello")
```

Isso cria literal compatível com o sistema de caracteres da Unreal.

Exemplo:

```cpp
UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
```

Use `TEXT()` com strings literais passadas para APIs Unreal que esperam `TCHAR`.

---

## 26. Containers Unreal

### `TArray`

Equivalente conceitual a `std::vector`.

```cpp
TArray<int32> DamageValues;
DamageValues.Add(10);
DamageValues.Add(20);
```

Iteracao:

```cpp
for (int32 Damage : DamageValues)
{
}
```

Para tipos grandes:

```cpp
for (const FWeaponStats& Stats : Weapons)
{
}
```

### `TMap`

Mapa chave/valor.

```cpp
TMap<FName, int32> Inventory;
Inventory.Add(TEXT("Ammo"), 30);
```

Busca:

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

Regra:

> Em codigo Unreal, prefira containers Unreal quando os dados precisam interagir com reflection, serialization, editor ou padroes da engine.

---

## 27. `TSubclassOf`

`TSubclassOf<T>` representa uma classe que deriva de `T`.

Exemplo:

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Spawning")
TSubclassOf<AActor> ActorToSpawn;
```

Uso:

```cpp
if (ActorToSpawn)
{
    GetWorld()->SpawnActor<AActor>(ActorToSpawn, Location, Rotation);
}
```

Muito comum para:

- projetil configuravel
- inimigo configuravel
- pickup configuravel
- widget class
- actor a ser spawnado

Isso permite escolher a classe no editor/Blueprint.

---

## 28. `APawn`, `ACharacter` e controle

### `APawn`

`APawn` e um actor que pode ser possuido por um controller.

Use quando voce precisa de uma entidade controlavel, mas nao precisa do pacote pronto de movimento de personagem.

### `ACharacter`

`ACharacter` deriva de `APawn` e traz:

- capsule collision
- skeletal mesh
- character movement component
- suporte pronto para movimento comum

Use para personagens humanoides ou entidades com movimento parecido com personagem.

### `APlayerController`

Controller do jogador.

Responsavel por:

- input
- possuir pawn
- interacao com UI
- comandos do jogador
- camera em muitos projetos

### `AAIController`

Controller para entidades controladas por IA.

### Relacao mental

```text
Controller controla Pawn
Character e um tipo especializado de Pawn
Pawn e um Actor possuivel
Actor existe no mundo
```

---

## 29. `GameMode`, `GameState`, `PlayerState`, `GameInstance`

Esses tipos fazem parte do gameplay framework.

### `GameMode`

Define regras do jogo.

Existe apenas no servidor em jogos multiplayer.

Exemplos:

- classe de pawn padrao
- regra de spawn
- condicao de vitoria
- fluxo de partida

### `GameState`

Estado compartilhado da partida.

Replicado para clientes em multiplayer.

Exemplos:

- tempo restante
- placar geral
- estado atual da rodada

### `PlayerState`

Estado de um jogador.

Exemplos:

- nome
- score
- equipe
- estatisticas

### `GameInstance`

Vive entre levels durante a execucao do jogo.

Exemplos:

- dados persistentes de sessao
- sistemas globais do jogo
- configuracoes carregadas

Regra inicial:

- regra da partida: `GameMode`
- estado replicado da partida: `GameState`
- estado do jogador: `PlayerState`
- persistencia entre mapas: `GameInstance`
- input/posse do jogador: `PlayerController`
- entidade controlavel: `Pawn`/`Character`

---

## 30. Expondo C++ para Blueprint

Blueprint e parte central da Unreal.

Voce pode escrever sistemas em C++ e expor pontos para designers ou para iteracao visual.

### Propriedade editavel

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
float MaxHealth = 100.0f;
```

### Funcao chamavel

```cpp
UFUNCTION(BlueprintCallable, Category = "Health")
void ApplyDamage(float Amount);
```

### Funcao pura

```cpp
UFUNCTION(BlueprintPure, Category = "Health")
float GetHealthPercent() const;
```

### Evento implementavel em Blueprint

```cpp
UFUNCTION(BlueprintImplementableEvent, Category = "Health")
void OnDeath();
```

C++ declara que o evento existe, Blueprint implementa o comportamento.

### Evento com implementacao nativa

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

Regra:

> Use C++ para base forte, sistemas e performance. Use Blueprint para configuracao, eventos visuais e iteracao, quando fizer sentido.

---

## 31. Logging

Para imprimir logs na Unreal:

```cpp
UE_LOG(LogTemp, Warning, TEXT("Hello from Unreal"));
```

Com valor:

```cpp
UE_LOG(LogTemp, Warning, TEXT("Health: %f"), CurrentHealth);
```

Com string:

```cpp
FString Name = TEXT("Player");
UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *Name);
```

O `*Name` converte `FString` para o formato esperado pelo log.

Categorias customizadas ficam para depois. No inicio, `LogTemp` e suficiente para estudo.

---

## 32. Exemplo: Health Component em UE5

### `HealthComponent.h`

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
    float GetCurrentHealth() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHealth = 100.0f;

    void ClampHealth();
};
```

### `HealthComponent.cpp`

```cpp
#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    ClampHealth();
}

void UHealthComponent::ApplyDamage(float Amount)
{
    if (Amount <= 0.0f)
    {
        return;
    }

    CurrentHealth -= Amount;
    ClampHealth();
}

void UHealthComponent::Heal(float Amount)
{
    if (Amount <= 0.0f)
    {
        return;
    }

    CurrentHealth += Amount;
    ClampHealth();
}

float UHealthComponent::GetCurrentHealth() const
{
    return CurrentHealth;
}

float UHealthComponent::GetHealthPercent() const
{
    if (MaxHealth <= 0.0f)
    {
        return 0.0f;
    }

    return CurrentHealth / MaxHealth;
}

bool UHealthComponent::IsDead() const
{
    return CurrentHealth <= 0.0f;
}

void UHealthComponent::ClampHealth()
{
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
}
```

Pontos importantes:

- `UHealthComponent` deriva de `UActorComponent`
- usa `UCLASS`
- usa `GENERATED_BODY`
- expõe funcoes para Blueprint
- usa `UPROPERTY` para dados editaveis/visiveis
- `BeginPlay` inicializa runtime
- `Tick` foi desabilitado
- `FMath::Clamp` e utilidade da Unreal

---

## 33. Exemplo: Actor com componente

### `HealthPickup.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AHealthPickup : public AActor
{
    GENERATED_BODY()

public:
    AHealthPickup();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float HealAmount = 25.0f;
};
```

### `HealthPickup.cpp`

```cpp
#include "HealthPickup.h"

#include "Components/StaticMeshComponent.h"

AHealthPickup::AHealthPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
}

void AHealthPickup::BeginPlay()
{
    Super::BeginPlay();
}
```

Pontos importantes:

- header usa forward declaration de `UStaticMeshComponent`
- `.cpp` inclui o header completo porque chama `CreateDefaultSubobject`
- componente e guardado com `UPROPERTY`
- `TObjectPtr` representa referencia para objeto Unreal
- `Tick` desabilitado por nao ser necessario

---

## 34. STL vs Unreal types

Voce aprendeu STL na Aula 04.

Agora precisa saber quando usar tipos Unreal.

### Em codigo Unreal gameplay, prefira tipos Unreal quando:

- propriedade precisa aparecer no editor
- propriedade precisa ser serializada
- dado precisa ser refletido
- dado precisa ser usado em Blueprint
- container precisa integrar com Unreal
- tipo aparece em `USTRUCT`, `UCLASS`, `UPROPERTY`

Exemplos:

```cpp
UPROPERTY(EditAnywhere)
TArray<FWeaponStats> Weapons;

UPROPERTY(EditAnywhere)
FText DisplayName;

UPROPERTY(EditAnywhere)
FName ItemId;
```

### STL ainda pode ser usada?

Sim, em codigo C++ interno, algoritmos, sistemas nao refletidos, ferramentas, codigo independente, etc.

Mas dentro de classes Unreal e propriedades expostas, tipos Unreal costumam ser o caminho correto.

Regra inicial:

> Se precisa aparecer para Unreal, use tipos Unreal. Se e logica C++ interna e isolada, STL pode ser aceitavel.

---

## 35. Ownership em Unreal vs C++ puro

Em C++ puro:

```cpp
std::unique_ptr<Item> Item;
```

comunica ownership exclusivo.

Em Unreal:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> Item;
```

comunica referencia rastreada para `UObject`.

Nao e a mesma coisa.

Actors sao gerenciados pelo mundo.

Components podem ser subobjetos de actors.

UObjects podem ter Outer e ser rastreados pelo GC.

Regra mental:

> Em Unreal, pergunte: isso e `UObject`? Isso e `AActor`? Isso e componente? Quem e o Outer? O GC consegue ver essa referencia?

Nao aplique automaticamente `std::unique_ptr` em objetos da Unreal.

---

## 36. Construtor vs `BeginPlay`

Erro comum: colocar gameplay runtime no construtor.

Construtor:

- cria componentes default
- define valores default
- configura propriedades basicas
- nao deve depender de outros actors do mundo

`BeginPlay`:

- jogo comecou para aquele actor
- mundo esta em estado mais apropriado para gameplay
- pode buscar referencias runtime com mais seguranca
- pode iniciar timers e logica

Exemplo bom:

```cpp
AMyActor::AMyActor()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
}

void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Actor started"));
}
```

---

## 37. Tick com cuidado

`Tick` roda todo frame.

```cpp
void AMyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
```

Habilitar Tick:

```cpp
PrimaryActorTick.bCanEverTick = true;
```

Desabilitar:

```cpp
PrimaryActorTick.bCanEverTick = false;
```

Use Tick quando precisa atualizar continuamente.

Evite Tick para coisas que podem ser:

- evento
- timer
- overlap
- delegate
- callback
- input action

Regra:

> Tick e ferramenta, nao padrao automatico.

---

## 38. Interfaces na Unreal

Voce viu interfaces em C++ puro na Aula 03.

Na Unreal, interface tem uma estrutura propria.

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

Uso permite que diferentes classes implementem um contrato de interacao.

Nao precisa dominar a sintaxe completa agora.

Entenda:

> Interface Unreal conecta contrato C++ com reflection e Blueprint.

---

## 39. Networking em alto nivel

Unreal tem sistema de replicacao.

Voce vai ver macros/especificadores como:

```cpp
UPROPERTY(Replicated)
float Health;
```

E funcoes RPC:

```cpp
UFUNCTION(Server, Reliable)
void ServerFire();
```

Nao aprofunde agora se ainda esta aprendendo base.

Mas saiba:

- `UPROPERTY` tambem participa de replicacao
- `UFUNCTION` tambem pode definir chamadas de rede
- `GameMode` existe so no servidor
- `GameState` replica estado da partida
- `PlayerState` replica estado por jogador

Quando estudar multiplayer, voce vai voltar a essas macros com mais profundidade.

---

## 40. Erros comuns de iniciante em UE5 C++

### Erro 1 - Usar `new` para `UObject`

Errado:

```cpp
UInventoryItem* Item = new UInventoryItem();
```

Correto:

```cpp
UInventoryItem* Item = NewObject<UInventoryItem>(this);
```

### Erro 2 - Usar `delete` em `AActor`

Errado:

```cpp
delete TargetActor;
```

Correto:

```cpp
TargetActor->Destroy();
```

### Erro 3 - Guardar `UObject*` importante sem `UPROPERTY`

Suspeito:

```cpp
UInventoryItem* CurrentItem;
```

Melhor em classe Unreal:

```cpp
UPROPERTY()
TObjectPtr<UInventoryItem> CurrentItem;
```

### Erro 4 - Colocar include depois do `.generated.h`

Errado:

```cpp
#include "MyActor.generated.h"
#include "SomeOtherHeader.h"
```

Correto:

```cpp
#include "SomeOtherHeader.h"
#include "MyActor.generated.h"
```

### Erro 5 - Esquecer `GENERATED_BODY`

Se usa `UCLASS`, `USTRUCT`, interface Unreal etc., precisa do corpo gerado.

### Erro 6 - Habilitar Tick por habito

Se nao precisa de atualizacao todo frame:

```cpp
PrimaryActorTick.bCanEverTick = false;
```

### Erro 7 - Confundir `FString`, `FName` e `FText`

Use:

- `FString` para manipular texto
- `FName` para identificador
- `FText` para UI/localizacao

### Erro 8 - Aplicar STL cegamente a propriedades Unreal

Em `UPROPERTY`, prefira tipos suportados pela Unreal.

---

## 41. Mini-projeto conceitual - Health Pickup em UE5

Objetivo:

Criar um pequeno sistema de pickup de vida usando C++ da Unreal.

Arquivos:

```text
HealthComponent.h
HealthComponent.cpp
HealthPickup.h
HealthPickup.cpp
```

### `UHealthComponent`

Responsabilidade:

> controlar vida de um actor.

Requisitos:

- derivar de `UActorComponent`
- usar `UCLASS`
- usar `GENERATED_BODY`
- `MaxHealth` editavel no editor
- `CurrentHealth` visivel no editor
- `ApplyDamage`
- `Heal`
- `GetHealthPercent`
- `IsDead`
- funcoes relevantes expostas para Blueprint

### `AHealthPickup`

Responsabilidade:

> actor no mundo que cura outro actor com `UHealthComponent`.

Requisitos:

- derivar de `AActor`
- ter componente visual `UStaticMeshComponent`
- ter `HealAmount`
- ter funcao `ApplyToActor(AActor* TargetActor)`
- procurar `UHealthComponent` no actor alvo
- chamar `Heal`
- destruir pickup depois de usado

Exemplo de funcao:

```cpp
void AHealthPickup::ApplyToActor(AActor* TargetActor)
{
    if (!IsValid(TargetActor))
    {
        return;
    }

    UHealthComponent* HealthComponent = TargetActor->FindComponentByClass<UHealthComponent>();

    if (HealthComponent == nullptr)
    {
        return;
    }

    HealthComponent->Heal(HealAmount);
    Destroy();
}
```

Pontos que essa funcao pratica:

- validacao de actor
- ponteiro possivelmente nulo
- componente por composicao
- chamada de funcao em componente
- `Destroy` em actor
- nada de `delete`

---

## 42. Exercicios teoricos

Responda com suas palavras:

1. Por que Unreal C++ nao e exatamente igual a C++ puro?
2. O que o reflection system permite?
3. Para que serve `UCLASS`?
4. Para que serve `USTRUCT`?
5. Para que serve `UENUM`?
6. Para que serve `UPROPERTY`?
7. Para que serve `UFUNCTION`?
8. Por que `GENERATED_BODY` e necessario?
9. Por que `.generated.h` deve ser o ultimo include?
10. O que e `UObject`?
11. O que e `AActor`?
12. Quando usar `UActorComponent`?
13. Qual a diferenca entre construtor e `BeginPlay`?
14. Por que Tick deve ser usado com cuidado?
15. Por que nao usar `new` para `UObject`?
16. Por que nao usar `delete` para `AActor`?
17. Como `UPROPERTY` se relaciona com garbage collection?
18. Quando usar `TObjectPtr`?
19. Quando usar `TWeakObjectPtr`?
20. Qual a diferenca entre `FString`, `FName` e `FText`?
21. Qual o paralelo entre `std::vector` e `TArray`?
22. Qual o papel de `GameMode`?
23. Qual o papel de `PlayerController`?
24. Por que componentes representam composicao?
25. O que significa expor uma funcao para Blueprint?

---

## 43. Exercicios praticos

### Exercicio 1 - Ler uma classe Unreal

Pegue uma classe C++ criada pela UE5 e identifique:

- `#pragma once`
- includes
- `.generated.h`
- `UCLASS`
- `GENERATED_BODY`
- construtor
- `BeginPlay`
- `Tick`
- propriedades
- funcoes

Explique o papel de cada parte.

### Exercicio 2 - Criar uma `USTRUCT`

Crie:

```cpp
USTRUCT(BlueprintType)
struct FWeaponData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 25.0f;
};
```

Explique por que:

- `WeaponId` usa `FName`
- `DisplayName` usa `FText`
- `Damage` usa `float`

### Exercicio 3 - Criar um enum exposto

Crie:

```cpp
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Common,
    Rare,
    Epic,
    Legendary
};
```

Use em uma `UPROPERTY`.

### Exercicio 4 - Health Component

Implemente `UHealthComponent` com:

- `MaxHealth`
- `CurrentHealth`
- `ApplyDamage`
- `Heal`
- `IsDead`
- `GetHealthPercent`

Exponha as funcoes certas para Blueprint.

### Exercicio 5 - Actor com componente

Crie um actor que tenha:

- `UStaticMeshComponent`
- `UHealthComponent`

Use `CreateDefaultSubobject` no construtor.

### Exercicio 6 - Spawn

Crie uma propriedade:

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Spawn")
TSubclassOf<AActor> ActorClassToSpawn;
```

Crie uma funcao:

```cpp
UFUNCTION(BlueprintCallable)
void SpawnConfiguredActor();
```

Dentro dela, use `GetWorld()->SpawnActor`.

### Exercicio 7 - Ponteiro fraco

Crie:

```cpp
TWeakObjectPtr<AActor> CurrentTarget;
```

Crie funcao que usa:

```cpp
if (CurrentTarget.IsValid())
{
}
```

Explique por que ponteiro fraco faz sentido para alvo temporario.

---

## 44. Perguntas obrigatorias finais do roadmap

Crie:

```text
respostas_aula_06.txt
```

Responda:

1. O que voce sabe agora sobre C++ puro que ajuda a entender UE5?
2. Onde lifetime aparece em Unreal?
3. Onde ownership aparece em Unreal?
4. Por que ponteiros continuam importantes?
5. Por que `UPROPERTY` nao e decoracao?
6. Por que `UFUNCTION` nao e decoracao?
7. Por que `UObject` nao deve ser tratado como objeto C++ comum?
8. Por que `AActor` deve ser destruido com `Destroy`?
9. Como componentes usam composicao?
10. Como heranca aparece em `ACharacter : public APawn`?
11. O que voce ainda precisa revisar antes de fazer gameplay C++ serio?
12. Qual parte do roadmap foi mais dificil?
13. Qual erro voce acha mais perigoso: ponteiro invalido, GC nao rastreando referencia, ou Tick excessivo? Explique.
14. Como voce explicaria `UCLASS` para alguem que so conhece C++ puro?
15. Como voce explicaria `UPROPERTY` para alguem que so conhece C++ puro?

---

## 45. Quando vale ver video

Para esta fase, videos sao muito uteis porque a Unreal tem editor, build tool e sistemas visuais.

Procure videos sobre:

- "Unreal Engine C++ UCLASS UPROPERTY UFUNCTION explained"
- "Unreal Engine UObject vs AActor"
- "Unreal Engine Actor lifecycle constructor BeginPlay Tick"
- "Unreal Engine garbage collection UPROPERTY"
- "Unreal Engine Actor Components C++"
- "Unreal Engine SpawnActor C++"
- "Unreal Engine C++ BlueprintCallable BlueprintPure"

O video mais importante desta fase e sobre:

```text
UObject, reflection, UPROPERTY e garbage collection
```

O segundo mais importante e sobre:

```text
Actor lifecycle: constructor, BeginPlay, Tick, Destroy
```

Assista com o codigo aberto. Pause e identifique macros, includes, lifecycle e ownership.

---

## 46. Checklist final antes de iniciar seu jogo

Antes de comecar seu jogo em UE5 com C++, voce deve conseguir explicar:

- o que e `UCLASS`
- o que e `GENERATED_BODY`
- o que e `UPROPERTY`
- o que e `UFUNCTION`
- o que e `UObject`
- o que e `AActor`
- o que e `UActorComponent`
- quando usar `CreateDefaultSubobject`
- quando usar `SpawnActor`
- quando usar `NewObject`
- por que nao usar `delete` em actor
- por que `UPROPERTY` importa para GC
- diferenca entre `TObjectPtr` e `TWeakObjectPtr`
- diferenca entre `FString`, `FName` e `FText`
- quando usar `TArray`, `TMap` e `TSet`
- diferenca entre construtor e `BeginPlay`
- por que `Tick` custa
- como expor propriedade ao editor
- como expor funcao ao Blueprint
- como componentes representam composicao
- onde C++ puro ainda importa dentro da Unreal

Se voce consegue responder isso, voce tem base suficiente para comecar gameplay C++ sem ficar apenas copiando codigo.

---

## 47. Plano pratico depois desta aula

Agora que o roadmap acabou, o proximo passo nao e estudar teoria infinitamente.

O proximo passo e criar sistemas pequenos em UE5.

Ordem recomendada:

1. Actor simples com mesh e log no `BeginPlay`.
2. Actor com `Tick` que move lentamente, so para entender.
3. `UHealthComponent`.
4. Actor que recebe dano.
5. Pickup que cura.
6. Projetil que aplica dano.
7. Character com input.
8. Weapon component.
9. Inventory simples.
10. Interface de interacao.

Cada sistema deve praticar um conceito:

- actor
- component
- pointer
- `UPROPERTY`
- `UFUNCTION`
- Blueprint integration
- lifetime
- ownership
- spawn
- destroy

---

## 48. Resumo final do roadmap

Voce agora tem uma base completa para entrar em UE5 C++:

- sabe sintaxe e tipos
- entende valor, referencia e ponteiro
- entende stack, heap e lifetime
- entende construtor, destrutor e RAII
- entende ownership
- entende OOP em C++
- entende heranca, `virtual` e `override`
- entende C++ moderno, STL, move e smart pointers
- entende headers, `.cpp`, preprocessador e linkagem
- entende por que Unreal adiciona macros, reflection e GC

O mais importante:

> Unreal C++ nao e magia. E C++ moderno somado a um grande framework com reflection, code generation, garbage collection e arquitetura de gameplay.

Quando um erro aparecer, pergunte:

- e erro de C++?
- e erro de include?
- e erro de linkagem?
- e erro do Unreal Header Tool?
- e erro de lifetime?
- e erro de GC?
- e erro de ownership?
- e erro de API da engine?

Essa forma de diagnosticar e o que separa tentativa e erro de engenharia.

---

## 49. Tarefa final da fase 6

Crie:

```text
respostas_aula_06.txt
```

E, se ja estiver com UE5 aberta, crie em um projeto de teste:

```text
UHealthComponent
AHealthPickup
```

Objetivo minimo:

- `UHealthComponent` controla vida
- `AHealthPickup` cura um actor com `UHealthComponent`
- propriedades aparecem no editor
- funcoes uteis aparecem em Blueprint
- nao usa `new`
- nao usa `delete`
- usa `UPROPERTY` para referencias relevantes
- usa `CreateDefaultSubobject` para componentes
- usa `Destroy` para remover pickup

So considere a fase 6 concluida quando voce conseguir explicar esta frase:

> Em UE5, C++ continua tendo lifetime, ponteiros, referencias e ownership, mas objetos da engine tambem participam de reflection, garbage collection e code generation. Por isso macros como `UCLASS`, `UPROPERTY` e `UFUNCTION` mudam como a engine enxerga o codigo.

Se essa frase esta clara, voce esta pronto para comecar seu jogo com uma base real.
