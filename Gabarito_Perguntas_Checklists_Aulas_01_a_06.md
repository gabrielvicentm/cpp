# Gabarito - Perguntas obrigatorias e checklists das Aulas 01 a 06

Este arquivo reune respostas-modelo para as perguntas obrigatorias, tarefas finais e checklists de dominio das aulas 1 a 6.

Use como revisao, nao como substituto de fazer os exercicios. Se uma resposta aqui parece confusa, volte para a aula correspondente e rode exemplos pequenos.

---

## Aula 01 - Fundamentos de C++

### Checklist de dominio

#### 1. O que `#include` faz em alto nivel?

`#include` pede ao preprocessador para inserir o conteudo de outro arquivo naquele ponto antes da compilacao. Em geral, usamos para trazer declaracoes de bibliotecas ou headers do projeto.

```cpp
#include <iostream>
#include "HealthComponent.h"
```

`<iostream>` vem da biblioteca padrao. `"HealthComponent.h"` vem do projeto.

#### 2. O que e `main`?

`main` e o ponto de entrada de um programa C++ comum.

```cpp
int main()
{
    return 0;
}
```

Na UE5 voce quase nunca escreve `main`, porque a engine fornece o ponto de entrada.

#### 3. Qual a diferenca entre declaracao e definicao?

Declaracao diz que algo existe.

```cpp
int Add(int A, int B);
```

Definicao implementa/cria aquilo.

```cpp
int Add(int A, int B)
{
    return A + B;
}
```

Em C++ multi-file, o header normalmente declara e o `.cpp` define.

#### 4. Por que `int`, `float`, `double`, `char` e `bool` nao sao intercambiaveis?

Porque representam tipos diferentes, com tamanhos, faixas, operacoes e significados diferentes.

- `int`: inteiro
- `float`: decimal de precisao simples
- `double`: decimal de maior precisao
- `char`: caractere
- `bool`: verdadeiro/falso

Converter sem pensar pode perder informacao ou mudar comportamento.

#### 5. O que pode dar errado com `unsigned`?

`unsigned` nao representa numeros negativos. Subtrair abaixo de zero pode fazer o valor "dar a volta" para um numero enorme.

```cpp
unsigned int Coins = 0;
Coins = Coins - 1; // nao vira -1
```

Tambem pode gerar comparacoes confusas entre signed e unsigned.

#### 6. Quando usar `static_cast`?

Use quando quer deixar uma conversao explicita e intencional.

```cpp
float Ratio = static_cast<float>(CurrentHealth) / MaxHealth;
```

E util quando a conversao pode perder informacao ou quando voce quer evitar divisao inteira.

#### 7. O que `const` comunica?

Comunica que algo nao deve ser modificado por aquele caminho.

```cpp
const int MaxHealth = 100;
void PrintName(const std::string& Name);
int GetHealth() const;
```

`const` protege intencao e ajuda o compilador a impedir alteracoes erradas.

#### 8. Quando usar `auto` e quando evitar?

Use quando o tipo e obvio ou muito verboso.

```cpp
auto Count = Items.size();
```

Evite quando esconder o tipo atrapalha o entendimento.

```cpp
auto Value = 10u; // pode esconder unsigned
```

Enquanto aprende, prefira tipos explicitos quando o tipo for parte importante do raciocinio.

#### 9. Qual a diferenca entre passar por valor e por referencia?

Por valor copia:

```cpp
void Damage(int Health);
```

Alterar `Health` dentro da funcao nao altera o original.

Por referencia acessa o original:

```cpp
void Damage(int& Health);
```

Alterar `Health` altera o valor original.

#### 10. Quando um ponteiro pode ser melhor que uma referencia?

Quando a ausencia de valor e possivel.

```cpp
void Attack(AActor* Target);
```

`Target` pode ser `nullptr`. Referencia representa melhor valor obrigatorio.

#### 11. O que e `nullptr`?

`nullptr` representa um ponteiro nulo, ou seja, que nao aponta para objeto valido.

```cpp
AActor* Target = nullptr;
```

Antes de usar um ponteiro que pode ser nulo, valide.

#### 12. O que e sobrecarga de funcao?

E ter funcoes com o mesmo nome, mas parametros diferentes.

```cpp
int Add(int A, int B);
float Add(float A, float B);
```

O compilador escolhe pela assinatura.

#### 13. O que argumentos default fazem?

Dão valor padrao para parametros omitidos na chamada.

```cpp
int CalculateDamage(int BaseDamage, int Bonus = 0);
```

Pode chamar:

```cpp
CalculateDamage(10);
CalculateDamage(10, 5);
```

### Tarefa final da Aula 01 - Respostas

#### 1. Quais parametros sao passados por valor e por que?

`int health`, `int stamina`, `float movementSpeed`, `bool isAlive`, `int damage`, `int amount`, `int maxHealth` podem ser por valor porque sao tipos pequenos e baratos de copiar. Em funcoes como `IsAlive(int health)`, a funcao so precisa ler uma copia.

#### 2. Quais parametros sao passados por referencia e por que?

`int& health` em `ApplyDamage` e `int& stamina` em `SpendStamina` sao referencias porque as funcoes precisam alterar o valor original.

#### 3. Qual parametro usa ponteiro e por que?

`int* health` em `HealIfValid` usa ponteiro porque a funcao aceita ausencia de valor. Se `health == nullptr`, ela nao deve curar nada.

#### 4. Onde voce precisou usar `static_cast`?

Em `GetHealthRatio`, para evitar divisao inteira.

```cpp
float Ratio = static_cast<float>(health) / maxHealth;
```

#### 5. Onde `const` protegeu seu codigo?

Em `PrintStatus(const std::string& name, ...)`, `const` evita que a funcao altere o nome e `&` evita copiar a string.

---

## Aula 02 - Memoria, ponteiros e ciclo de vida

### Perguntas obrigatorias do mini-projeto

#### 1. Quais objetos estao na stack?

No mini-projeto, objetos locais como `player`, `enemyA`, `enemyB` e variaveis como `turn` estao na stack, assumindo que foram criados diretamente dentro de `main` ou outra funcao.

```cpp
Combatant player("Player", 100, 30);
```

#### 2. Quando `player`, `enemyA` e `enemyB` sao destruidos?

Sao destruidos automaticamente ao sair do escopo onde foram criados. Se foram criados em `main`, morrem no fim de `main`, em ordem inversa da construcao.

#### 3. Por que `Attack` recebe `Combatant& attacker`?

Porque o atacante precisa existir obrigatoriamente e a funcao pode alterar seu estado, por exemplo gastando stamina.

#### 4. Por que `Attack` recebe `Combatant* target`?

Porque o alvo pode estar ausente. Um ponteiro permite `nullptr`, que representa "sem alvo".

#### 5. Por que `SelectTarget` retorna `Combatant*`?

Porque pode retornar um dos combatentes existentes ou `nullptr` se nenhum alvo valido existir.

#### 6. O que aconteceria se `SelectTarget` retornasse endereco de uma variavel local?

Retornaria um ponteiro pendurado, porque a variavel local morreria ao sair da funcao. Usar esse ponteiro depois seria comportamento indefinido.

#### 7. Onde existe risco de ponteiro nulo?

Em qualquer uso de `Combatant* target`, principalmente dentro de `Attack` e no retorno de `SelectTarget`. Antes de usar `target->`, e preciso verificar `target != nullptr`.

#### 8. Onde usamos `const&` para evitar copia?

Em funcoes que so leem um `Combatant`, como:

```cpp
void PrintCombatant(const Combatant& Combatant);
bool IsAlive(const Combatant& Combatant);
```

#### 9. Onde alteramos o objeto original?

Em funcoes que recebem referencia nao-const, como `ApplyDamage(Combatant& ...)`, `SpendStamina(Combatant& ...)` e no atacante de `Attack(Combatant& attacker, ...)`.

#### 10. Qual parte lembra codigo de gameplay na UE5?

Alvo opcional como ponteiro, validacao de alvo, dano, stamina, status e selecao de target lembram gameplay com `AActor*`, componentes e funcoes de combate.

### Checklist de dominio

#### 1. O que e stack?

Regiao de memoria usada para variaveis locais e chamadas de funcao. Objetos na stack vivem ate o fim do escopo.

#### 2. O que e heap?

Regiao de memoria para alocacoes dinamicas. Em C++ puro pode ser usada com `new`, mas C++ moderno prefere RAII e smart pointers. Na Unreal, objetos da engine seguem regras proprias.

#### 3. Quando uma variavel local morre?

Quando a execucao sai do escopo onde ela foi criada.

#### 4. O que um ponteiro guarda?

Um endereco de memoria.

#### 5. O que `&x` faz?

Pega o endereco de `x`, quando usado em expressao.

```cpp
int* P = &x;
```

#### 6. O que `*p` faz?

Desreferencia o ponteiro, acessando o objeto apontado.

```cpp
*p = 10;
```

#### 7. O que `nullptr` representa?

Um ponteiro que nao aponta para objeto valido.

#### 8. Por que ponteiro nulo deve ser verificado?

Porque desreferenciar `nullptr` e invalido e pode quebrar o programa.

#### 9. O que e dangling pointer?

Ponteiro que aponta para um objeto que ja morreu ou nao e mais valido.

#### 10. O que e memory leak?

Vazamento de memoria: memoria/recurso alocado que nao e liberado e cujo endereco foi perdido ou esquecido.

#### 11. O que e use-after-free?

Usar memoria depois que ela ja foi liberada/destruida.

#### 12. O que e double free?

Liberar duas vezes o mesmo recurso. E comportamento indefinido.

#### 13. O que construtor faz?

Roda quando o objeto nasce e deve inicializar o objeto em estado valido.

#### 14. O que destrutor faz?

Roda quando o objeto morre e libera/fecha/limpa recursos.

#### 15. Em que ordem objetos locais sao destruidos?

Na ordem inversa da construcao.

#### 16. O que RAII resolve?

Amarra recurso ao lifetime de um objeto: o recurso e adquirido na construcao e liberado no destrutor.

#### 17. O que ownership significa?

Significa quem e responsavel por manter e liberar/destruir um recurso.

#### 18. Por que `int*` nao comunica ownership?

Porque um ponteiro cru so guarda endereco. Ele nao diz se deve deletar, se apenas observa, se pode ser nulo ou se pertence a outro sistema.

#### 19. Quando usar `T&`?

Quando o objeto deve existir e voce quer alterar o original.

#### 20. Quando usar `const T&`?

Quando o objeto deve existir, voce quer ler sem copiar e sem alterar.

#### 21. Quando usar `T*`?

Quando o objeto pode estar ausente ou quando a API usa ponteiros. Em Unreal, muitos objetos aparecem como ponteiros.

#### 22. Por que copiar objeto grande pode ser caro?

Porque pode duplicar memoria interna, containers, strings ou dados pesados.

#### 23. Qual a diferenca conceitual entre copy e move?

Copy duplica valor/recurso. Move transfere recurso quando possivel, deixando o objeto de origem valido, mas sem valor antigo garantido.

### Tarefa final da Aula 02 - Respostas

#### 1. O que vive na stack no seu programa?

Objetos locais criados diretamente, como `Combatant player`, `Combatant enemyA`, `Combatant enemyB`, variaveis `int turn` e ponteiros locais como `Combatant* target`.

#### 2. Voce usou heap diretamente? Se nao, por que isso e bom neste momento?

Nao, se o exercicio foi feito com objetos locais. Isso e bom porque evita `new/delete`, vazamento e ownership manual enquanto o foco e entender lifetime por escopo.

#### 3. Onde existe ponteiro?

Em `Combatant* target`, parametros de `Attack` e retorno de `SelectTarget`.

#### 4. Onde existe referencia?

Em parametros como `Combatant& attacker`, `Combatant& combatant`.

#### 5. Onde existe `const&`?

Em funcoes de leitura, como `PrintCombatant(const Combatant&)` e `IsAlive(const Combatant&)`.

#### 6. Onde poderia haver ponteiro nulo?

No retorno de `SelectTarget` e no parametro `target` de `Attack`.

#### 7. Onde um dangling pointer poderia aparecer se voce programasse errado?

Se retornasse endereco de variavel local, guardasse ponteiro para objeto que saiu de escopo ou usasse ponteiro depois do objeto morrer.

#### 8. Onde acontece destruicao automatica?

No fim do escopo dos objetos locais. Os destrutores rodam automaticamente.

#### 9. Onde aconteceria copia se voce removesse `const&`?

Em funcoes como `PrintCombatant(Combatant combatant)` ou `IsAlive(Combatant combatant)`, que receberiam uma copia.

#### 10. Como prepara para ponteiros de Actors na UE5?

Ensina que ponteiro pode ser nulo ou invalido, que lifetime importa e que voce precisa validar antes de acessar.

---

## Aula 03 - Orientacao a objetos

### Perguntas obrigatorias

#### 1. Quais classes usam encapsulamento?

Principalmente `HealthComponent`, `Weapon` e `Character`, porque mantem dados privados e expõem operacoes controladas.

#### 2. Quais membros ficaram `private` e por que?

Vida, vida maxima, municao, dano, nome e componentes internos devem ficar privados para proteger invariantes e impedir alteracoes arbitrarias.

#### 3. Quais metodos sao `const`?

Metodos que apenas leem estado:

```cpp
GetCurrent() const
GetMax() const
IsDead() const
CanFire() const
GetDamage() const
GetName() const
PrintStatus() const
```

#### 4. Onde voce usou initializer list?

Nos construtores:

```cpp
HealthComponent(int Initial, int Max)
    : currentHealth(Initial), maxHealth(Max)
{
}
```

Tambem em `Weapon`, `Character`, `PlayerCharacter`, `EnemyCharacter` e `BossCharacter`.

#### 5. Qual classe base voce criou?

No mini-projeto, `Character` e a classe base principal.

#### 6. Quais classes derivadas voce criou?

`PlayerCharacter`, `EnemyCharacter` e `BossCharacter`.

#### 7. Onde existe polimorfismo?

Quando uma funcao recebe `Character&` ou `Character*` e chama metodo virtual como `Attack`, executando a versao real da classe derivada.

```cpp
void RunTurn(Character& Attacker, Character& Defender)
{
    Attacker.Attack(Defender);
}
```

#### 8. Por que `Attack` e virtual?

Porque cada derivada deve atacar de forma diferente, mas o codigo pode chamar pelo tipo base `Character`.

#### 9. Onde voce usou `override`?

Nas classes derivadas ao sobrescrever `Attack` e possivelmente `PrintStatus`.

```cpp
void Attack(Character& Target) override;
```

#### 10. Onde voce usou composicao?

`Character` tem `HealthComponent` e `Weapon`. Ele nao herda deles.

#### 11. Por que `HealthComponent` nao deveria ser classe base de `Character`?

Porque um personagem nao e uma vida. Um personagem tem um componente de vida. Isso e relacao "tem um", logo composicao.

#### 12. O que no codigo lembra `ACharacter` da UE5?

Uma classe base com comportamento comum e derivadas especializadas, como player, inimigo ou boss.

#### 13. O que lembra Actor Components da UE5?

`HealthComponent` e componentes internos que adicionam comportamento/dados a uma entidade.

#### 14. Se removesse `virtual`, o que quebraria conceitualmente?

Chamadas por `Character&` ou `Character*` deixariam de despachar para a versao derivada. O polimorfismo dinamico seria perdido.

#### 15. Por que classe base com metodos virtuais deve ter destrutor virtual?

Para garantir que, se um objeto derivado for destruido por ponteiro da base, o destrutor derivado tambem rode corretamente.

### Checklist de dominio

#### 1. O que diferencia `struct` e `class`?

Em C++, `struct` tem membros `public` por padrao; `class` tem membros `private` por padrao.

#### 2. O que `public` permite?

Permite acesso de fora da classe.

#### 3. O que `private` protege?

Protege membros contra acesso direto externo, ajudando a manter invariantes.

#### 4. O que e encapsulamento?

Controlar acesso ao estado interno, expondo operacoes seguras em vez de dados soltos.

#### 5. O que e uma invariante?

Regra que deve permanecer verdadeira enquanto o objeto esta valido, como `health >= 0`.

#### 6. Para que serve construtor?

Para inicializar o objeto em estado valido quando ele nasce.

#### 7. Para que serve initializer list?

Para inicializar membros diretamente antes do corpo do construtor.

#### 8. Quando o destrutor roda?

Quando o objeto morre: fim de escopo, `delete` em C++ puro, ou destruicao do dono/RAII.

#### 9. O que significa `const` em um metodo?

Que o metodo promete nao alterar o estado observavel do objeto.

#### 10. O que significa `class Player : public Character`?

`Player` herda publicamente de `Character`; conceitualmente, `Player` e um `Character`.

#### 11. Quando heranca faz sentido?

Quando existe relacao real "e um" e/ou necessidade de polimorfismo.

#### 12. Quando composicao e melhor?

Quando a relacao e "tem um" ou quando voce quer montar comportamento por partes.

#### 13. O que `virtual` faz?

Permite despacho dinamico: chamada por base executa implementacao da classe derivada real.

#### 14. O que `override` protege?

Faz o compilador verificar que voce realmente esta sobrescrevendo um metodo virtual da base.

#### 15. O que e polimorfismo?

Capacidade de tratar objetos diferentes por uma interface/base comum, executando comportamento especifico de cada tipo.

#### 16. O que e classe abstrata?

Classe com pelo menos um metodo virtual puro (`= 0`), que nao pode ser instanciada diretamente.

#### 17. O que e interface em C++ puro?

Uma classe abstrata usada como contrato, normalmente so com metodos virtuais puros e destrutor virtual.

#### 18. Por que destrutor virtual importa em classe base polimorfica?

Para destruicao correta de objetos derivados por ponteiro da base.

#### 19. Como isso aparece em `AActor`, `APawn` e `ACharacter`?

Como hierarquia de heranca. `ACharacter` deriva de `APawn`, que deriva de `AActor`, e voce sobrescreve metodos virtuais como `BeginPlay` e `Tick`.

### Tarefa final da Aula 03 - Respostas

#### 1. Por que `HealthComponent` e composicao?

Porque o personagem tem vida, mas nao e uma vida. `HealthComponent` e uma parte do personagem.

#### 2. Por que `PlayerCharacter` herda de `Character`?

Porque um player character e um character especializado.

#### 3. Por que `Attack` e virtual?

Porque cada tipo de character pode atacar de forma diferente usando a mesma chamada pela base.

#### 4. Por que `override` protege contra erro?

Porque detecta assinatura errada ou nome errado ao tentar sobrescrever.

#### 5. Por que membros privados ajudam a manter invariantes?

Porque impedem alteracao externa arbitraria e forcam mudancas por metodos controlados.

#### 6. Por que isso se parece com UE5?

Porque UE5 usa heranca (`ACharacter : APawn`), overrides (`BeginPlay`, `Tick`) e composicao por componentes.

---

## Aula 04 - C++ moderno

### Perguntas obrigatorias do mini-projeto

#### 1. Quem e dono dos itens?

`Inventory` e dono dos itens, porque guarda `std::unique_ptr<Item>`.

#### 2. Por que `Inventory` usa `std::unique_ptr<Item>`?

Porque cada item tem ownership exclusivo do inventario e pode ser uma classe derivada de `Item`.

#### 3. Por que `AddItem` recebe `std::unique_ptr<Item>` por valor?

Porque receber por valor comunica transferencia de ownership para a funcao/inventario.

#### 4. Por que precisa usar `std::move` em `AddItem`?

Porque `std::unique_ptr` nao copia. Para transferir ownership para o vector, precisa mover.

#### 5. Depois de `AddItem(std::move(item))`, o ponteiro original ainda e dono?

Nao. O ownership foi transferido. O `unique_ptr` original fica vazio/nulo.

#### 6. `FindItem` retorna ownership ou apenas observacao?

Apenas observacao. Um `Item*` retornado nao e dono e nao deve dar `delete`.

#### 7. Por que `FindWeaponDamage` retorna `std::optional<int>`?

Porque talvez a arma nao exista ou o item encontrado nao seja arma. `optional` representa "talvez tenha valor".

#### 8. Onde voce usou lambda?

Em `SortByName`, por exemplo:

```cpp
items.Sort(...)
```

No C++ padrao:

```cpp
std::sort(items.begin(), items.end(), [](const auto& A, const auto& B)
{
    return A->GetName() < B->GetName();
});
```

#### 9. O que a lambda captura?

Nesse exemplo, nada. A captura e `[]`. Ela usa apenas os parametros `A` e `B`.

#### 10. Onde poderia ocorrer copia desnecessaria?

Se funcoes recebessem `std::string`, `Item`, `Weapon` ou containers por valor sem necessidade. Use `const&` para leitura.

#### 11. Onde voce usou `const&`?

Em parametros como:

```cpp
Item* FindItem(const std::string& Name);
```

E em iteracoes:

```cpp
for (const auto& Item : Items)
```

#### 12. Onde voce usou polimorfismo?

No `std::unique_ptr<Item>` apontando para objetos derivados como `Weapon`, e ao chamar `Print()` virtual.

#### 13. Por que `Item` precisa de destrutor virtual?

Porque objetos derivados (`Weapon`) podem ser destruidos atraves de ponteiro para base (`Item*`/`unique_ptr<Item>`).

#### 14. Que parte parece inventario em jogo?

Colecao de itens, busca por nome, item polimorfico, arma com dano, ordenacao e consulta de dados.

#### 15. O que mudaria com `UObject` na UE5?

Nao usaria `std::unique_ptr<UObject>` como dono comum. Usaria `UObject`, `UPROPERTY`, `TObjectPtr`, talvez `TArray<TObjectPtr<UItem>>`, e criaria objetos com `NewObject`.

### Checklist de dominio

#### 1. O que `const T&` evita?

Evita copia e impede alteracao do objeto por aquela referencia.

#### 2. O que e lvalue?

Expressao com identidade/endereco, geralmente nomeada, como uma variavel.

#### 3. O que e rvalue?

Valor temporario ou expressao sem identidade persistente, como resultado de `10 + 20` ou retorno temporario.

#### 4. O que `std::move` faz?

Converte uma expressao para permitir movimento. Ele nao move sozinho; habilita move constructor/assignment.

#### 5. Qual diferenca entre copy e move?

Copy duplica recursos. Move transfere recursos quando possivel.

#### 6. O que acontece com objeto depois de ser movido?

Continua valido, mas seu conteudo anterior nao deve ser assumido. Pode ser destruido ou receber novo valor.

#### 7. O que `std::vector` faz?

E um array dinamico contiguo que pode crescer.

#### 8. Por que `std::vector` pode invalidar ponteiros?

Porque ao crescer pode realocar os elementos para outro bloco de memoria.

#### 9. Quando usar `std::array`?

Quando o tamanho e fixo e conhecido em compile time.

#### 10. Quando usar `std::unordered_map`?

Quando precisa mapear chave para valor e nao precisa de ordem, buscando em media rapidamente por hash.

#### 11. Quando usar `std::optional`?

Quando uma funcao ou variavel pode ou nao ter valor.

#### 12. O que `std::unique_ptr` comunica?

Ownership exclusivo.

#### 13. Por que `std::unique_ptr` nao copia?

Porque copiar criaria dois donos para o mesmo objeto. Ele so move.

#### 14. Quando usar `std::shared_ptr`?

Quando ownership realmente precisa ser compartilhado.

#### 15. Para que serve `std::weak_ptr`?

Para observar objeto gerenciado por `shared_ptr` sem manter o objeto vivo e para evitar ciclos.

#### 16. O que e lambda?

Funcao anonima criada no local.

#### 17. Diferenca entre captura por valor e referencia?

Por valor copia a variavel para dentro da lambda. Por referencia acessa a variavel original.

#### 18. O que e template?

Codigo generico parametrizado por tipo, resolvido pelo compilador.

#### 19. Por que templates sao resolvidos em compile time?

Porque o compilador gera versoes concretas do template para os tipos usados.

#### 20. Por que smart pointers da STL nao substituem o GC da Unreal?

Porque `UObject`/`AActor` pertencem ao sistema da Unreal: reflection, GC, Outer, mundo e lifecycle proprios.

### Tarefa final da Aula 04 - Resposta central

```cpp
std::vector<std::unique_ptr<Item>> items;
```

Significa: o inventario possui uma colecao dinamica de itens polimorficos. Cada item pertence exclusivamente ao inventario (`unique_ptr`) e pode ser uma classe derivada de `Item`. O vector guarda os donos, e os objetos sao destruidos automaticamente quando o inventario morre.

---

## Aula 05 - Compilacao, headers e organizacao

### Perguntas obrigatorias

#### 1. Qual a diferenca entre `.h` e `.cpp`?

`.h` normalmente declara a interface. `.cpp` implementa as definicoes.

#### 2. O que e uma translation unit?

E um `.cpp` depois do preprocessamento, com todos os headers incluidos, compilado separadamente.

#### 3. O que `#include` faz conceitualmente?

Cola o conteudo de outro arquivo no ponto do include antes da compilacao.

#### 4. Para que serve `#pragma once`?

Evita que o mesmo header seja incluido mais de uma vez na mesma translation unit.

#### 5. O que o preprocessador faz?

Processa diretivas como `#include`, `#define`, `#if` antes da compilacao C++ normal.

#### 6. Por que macros podem ser perigosas?

Porque sao transformacoes textuais, nao respeitam tipos como funcoes normais e podem causar expansoes inesperadas.

#### 7. O que e um simbolo em linkagem?

Nome que o linker precisa resolver, como funcao, metodo ou variavel global.

#### 8. O que significa unresolved external symbol?

Algo foi declarado/chamado, mas o linker nao encontrou a definicao correspondente.

#### 9. O que significa multiple definition?

O mesmo simbolo foi definido mais de uma vez em translation units diferentes.

#### 10. Por que nao devemos incluir `.cpp`?

Porque `.cpp` deve entrar no build, nao ser colado por include. Incluir `.cpp` pode causar multiplas definicoes e build confuso.

#### 11. O que e forward declaration?

Declaracao antecipada de que um tipo existe.

```cpp
class AWeapon;
```

#### 12. Quando forward declaration nao basta?

Quando precisa do tipo completo: heranca, membro por valor, chamar metodos no header, saber tamanho ou acessar membros.

#### 13. Por que headers com muitos includes aumentam tempo de compilacao?

Porque todo `.cpp` que inclui esse header tambem processa todos os includes transitivos.

#### 14. Por que templates geralmente ficam em headers?

Porque o compilador precisa ver a implementacao para instanciar o template com tipos concretos.

#### 15. Por que `using namespace std;` em header e ruim?

Porque polui todos os arquivos que incluem o header e pode causar conflitos de nomes.

#### 16. Como `MyActor.h` e `MyActor.cpp` da UE5 seguem esse modelo?

`MyActor.h` declara a classe, propriedades e funcoes. `MyActor.cpp` implementa construtor, `BeginPlay`, `Tick` e outros metodos.

#### 17. Para que serve `MyActor.generated.h` em alto nivel?

Inclui codigo gerado pelo Unreal Header Tool para reflection, propriedades, funcoes e integracao com a engine.

#### 18. Por que `.generated.h` deve ser o ultimo include?

Porque o UHT espera essa ordem para gerar/injetar codigo corretamente; includes depois podem quebrar as expectativas do sistema.

#### 19. Quando incluiria um header no `.h`?

Quando o header precisa do tipo completo: heranca, membro por valor, macros/reflection que exigem tipo completo ou metodos inline que usam o tipo.

#### 20. Quando preferiria incluir no `.cpp`?

Quando o `.h` so precisa de ponteiro/referencia/forward declaration e a implementacao no `.cpp` usa metodos/detalhes do tipo.

### Checklist de dominio

#### 1. Header declara ou implementa?

Normalmente declara. Pode conter implementacoes pequenas, inline e templates.

#### 2. `.cpp` declara ou implementa?

Normalmente implementa as definicoes.

#### 3. O que e declaracao?

Promessa de existencia e assinatura.

#### 4. O que e definicao?

Implementacao/corpo real ou criacao de armazenamento.

#### 5. O que `#include` realmente faz?

Insere texto do arquivo incluido antes da compilacao.

#### 6. O que `#pragma once` evita?

Inclusao repetida do mesmo header na mesma translation unit.

#### 7. O que include guard evita?

O mesmo que `#pragma once`, usando macros `#ifndef/#define/#endif`.

#### 8. O que e preprocessador?

Etapa que processa diretivas antes do compilador C++.

#### 9. O que macro faz?

Define transformacoes/instrucoes de preprocessamento. Em Unreal, macros tambem alimentam o UHT.

#### 10. Por que macro nao e igual funcao?

Porque macro e substituicao/processamento antes da compilacao, sem as mesmas regras de tipo, escopo e chamada de funcao.

#### 11. O que e linkagem?

Etapa que junta arquivos objeto e resolve simbolos para formar executavel/modulo.

#### 12. O que e unresolved external?

Erro de linkagem por definicao ausente.

#### 13. O que e multiple definition?

Erro de linkagem por definicao duplicada.

#### 14. O que e forward declaration?

Declarar que um tipo existe sem incluir seu header completo.

#### 15. Quando preciso do tipo completo?

Para herdar, guardar por valor, acessar membros/metodos no header, calcular tamanho ou destruir em certos contextos.

#### 16. Como evitar dependencia circular?

Usando forward declarations nos headers e includes completos nos `.cpp`, alem de reduzir acoplamento.

#### 17. Por que headers ruins deixam build lento?

Porque espalham includes pesados e qualquer mudanca recompila muitos arquivos.

#### 18. Por que template fica no header?

Porque o compilador precisa da implementacao visivel para gerar codigo para cada tipo usado.

#### 19. Por que Unreal usa `.generated.h`?

Para incluir codigo gerado pelo UHT necessario para reflection e integracao com a engine.

#### 20. Por que macros da Unreal nao sao enfeite?

Porque controlam reflection, editor, Blueprint, GC, serializacao, networking e code generation.

### Tarefa final da Aula 05 - Respostas

#### 1. Quais headers cada `.cpp` inclui?

Cada `.cpp` deve incluir seu proprio header primeiro e depois os headers dos tipos usados na implementacao.

```cpp
#include "Player.h"
#include "Weapon.h"
#include "Enemy.h"
```

#### 2. Quais includes poderiam virar forward declaration?

No `.h`, tipos usados so como ponteiro/referencia, como `class Weapon;` e `class Enemy;`.

#### 3. Onde precisou de tipo completo?

No `.cpp` ao chamar metodos como `weapon->Fire()` ou `enemy.ReceiveDamage()`, e no `.h` quando guarda membro por valor ou herda.

#### 4. Qual erro apareceu ao esquecer implementacao?

Unresolved external symbol/undefined reference.

#### 5. Qual erro apareceu ao causar multiple definition?

Erro de multiple definition/already defined, geralmente no linker.

#### 6. Como resolveu dependencia circular?

Removendo includes mutuos dos headers, usando forward declarations e colocando includes completos nos `.cpp`.

#### 7. Como esse modelo aparece em uma classe C++ da UE5?

`MyActor.h` declara a classe com macros/propriedades/funcoes; `MyActor.cpp` implementa construtor, `BeginPlay`, `Tick` e logica.

---

## Aula 06 - C++ da Unreal Engine 5

### Perguntas obrigatorias finais

#### 1. O que C++ puro ajuda a entender na UE5?

Ajuda a entender tipos, ponteiros, referencias, lifetime, ownership, classes, heranca, `virtual`, `override`, headers, includes, compilacao e linkagem.

#### 2. Onde lifetime aparece em Unreal?

Em actors spawnados/destruidos, componentes, UObjects gerenciados pelo GC, timers, delegates, lambdas, `BeginPlay`, `EndPlay` e referencias entre objetos.

#### 3. Onde ownership aparece em Unreal?

No World possuindo actors, actors possuindo componentes, Outer de UObjects, GC rastreando referencias e propriedades marcadas com `UPROPERTY`.

#### 4. Por que ponteiros continuam importantes?

Porque Unreal usa muitos ponteiros para `UObject`, `AActor`, componentes e objetos opcionais. Eles podem ser nulos ou invalidos.

#### 5. Por que `UPROPERTY` nao e decoracao?

Porque integra a propriedade com reflection, editor, Blueprint, serializacao, GC e possivelmente networking.

#### 6. Por que `UFUNCTION` nao e decoracao?

Porque registra a funcao no sistema de reflection, podendo expor para Blueprint, RPC, eventos e outros sistemas da engine.

#### 7. Por que `UObject` nao deve ser tratado como C++ comum?

Porque participa de GC, reflection, Outer, serializacao e criacao pela engine. Use `NewObject`, nao `new/delete` manual.

#### 8. Por que `AActor` deve ser destruido com `Destroy`?

Porque actors pertencem ao mundo/engine. `Destroy` segue o lifecycle da Unreal; `delete` manual quebraria o modelo.

#### 9. Como componentes usam composicao?

Um actor tem componentes que adicionam comportamento/dados. Exemplo: character tem `UHealthComponent`, camera e mesh.

#### 10. Como heranca aparece em `ACharacter : public APawn`?

`ACharacter` e um tipo especializado de `APawn`, que por sua vez deriva de `AActor`. Isso e heranca "e um".

#### 11. O que ainda revisar antes de gameplay C++ serio?

Ponteiros/lifetime, `UPROPERTY`, `UObject` vs `AActor`, componentes, headers, `generated.h`, `SpawnActor`, `NewObject`, `Destroy`, validacao e tipos Unreal.

#### 12. Qual parte do roadmap foi mais dificil?

Resposta pessoal. Uma resposta comum: ponteiros/lifetime/ownership, porque e onde C++ e Unreal mais cobram entendimento real.

#### 13. Qual erro e mais perigoso: ponteiro invalido, GC nao rastreando referencia ou Tick excessivo?

Todos sao perigosos de formas diferentes. Ponteiro invalido e GC nao rastreando referencia podem causar crash/bugs difíceis. Tick excessivo tende a causar problema de performance. Para estabilidade, ponteiro invalido e GC errado costumam ser mais criticos.

#### 14. Como explicar `UCLASS` para alguem de C++ puro?

E uma macro que registra a classe no sistema da Unreal para reflection, editor, Blueprint, serializacao e outros sistemas.

#### 15. Como explicar `UPROPERTY` para alguem de C++ puro?

E uma macro que faz a Unreal conhecer aquela variavel, permitindo editor/Blueprint/serializacao/GC conforme especificadores.

### Checklist final antes de iniciar o jogo

#### 1. O que e `UCLASS`?

Macro que marca uma classe para o sistema de reflection da Unreal.

#### 2. O que e `GENERATED_BODY`?

Macro que injeta codigo gerado pelo Unreal Header Tool dentro da classe/struct/interface.

#### 3. O que e `UPROPERTY`?

Macro que registra propriedade para sistemas da Unreal, como editor, Blueprint, GC, serializacao e replicacao.

#### 4. O que e `UFUNCTION`?

Macro que registra funcao para reflection, Blueprint, eventos, RPCs e outros sistemas.

#### 5. O que e `UObject`?

Base do sistema de objetos da Unreal, com reflection, GC e serializacao.

#### 6. O que e `AActor`?

Objeto que pode existir no mundo, ter transform, componentes, BeginPlay, Tick e ser spawnado/destruido.

#### 7. O que e `UActorComponent`?

Componente que adiciona comportamento/dados a um actor.

#### 8. Quando usar `CreateDefaultSubobject`?

No construtor, para criar componentes default de um actor/componente.

#### 9. Quando usar `SpawnActor`?

Para criar um `AActor` no mundo.

#### 10. Quando usar `NewObject`?

Para criar um `UObject` que nao e actor.

#### 11. Por que nao usar `delete` em actor?

Porque o actor e gerenciado pelo World/engine. Use `Destroy`.

#### 12. Por que `UPROPERTY` importa para GC?

Porque torna referencias para `UObject` visiveis ao garbage collector.

#### 13. Diferenca entre `TObjectPtr` e `TWeakObjectPtr`?

`TObjectPtr` e referencia forte/rastreada para `UObject` em propriedades. `TWeakObjectPtr` observa sem manter vivo e pode invalidar.

#### 14. Diferenca entre `FString`, `FName` e `FText`?

`FString` manipula texto, `FName` e identificador eficiente, `FText` e texto para UI/localizacao.

#### 15. Quando usar `TArray`, `TMap` e `TSet`?

`TArray` para lista dinamica, `TMap` para chave/valor, `TSet` para conjunto de valores unicos.

#### 16. Diferenca entre construtor e `BeginPlay`?

Construtor configura defaults e subobjetos. `BeginPlay` roda quando o jogo inicia para aquele actor e e melhor para logica runtime.

#### 17. Por que `Tick` custa?

Porque roda todo frame. Muitos ticks desnecessarios acumulam custo de CPU.

#### 18. Como expor propriedade ao editor?

Com `UPROPERTY` e especificadores como `EditAnywhere`, `EditDefaultsOnly`, `VisibleAnywhere`.

```cpp
UPROPERTY(EditAnywhere, Category = "Health")
float MaxHealth = 100.0f;
```

#### 19. Como expor funcao ao Blueprint?

Com `UFUNCTION(BlueprintCallable)` ou `BlueprintPure`.

```cpp
UFUNCTION(BlueprintCallable)
void ApplyDamage(float Amount);
```

#### 20. Como componentes representam composicao?

Porque actors possuem componentes para adicionar capacidades, em vez de herdar uma classe para cada combinacao.

#### 21. Onde C++ puro ainda importa dentro da Unreal?

Em tipos, ponteiros, referencias, lifetime, `const`, OOP, headers, templates, lambdas, compilacao e entendimento de custo.

### Tarefa final da fase 6 - Resposta central

Em UE5, C++ continua tendo lifetime, ponteiros, referencias e ownership, mas objetos da engine tambem participam de reflection, GC e code generation. Por isso macros como `UCLASS`, `UPROPERTY` e `UFUNCTION` mudam como a engine enxerga seu codigo.

---

## Revisao final curta

Se voce consegue explicar os itens abaixo, o roadmap cumpriu o objetivo:

1. `#include` cola texto antes da compilacao.
2. `.h` declara e `.cpp` implementa.
3. Ponteiro pode ser nulo/invalido; referencia representa valor obrigatorio.
4. Objeto local morre ao sair do escopo.
5. `const&` le sem copiar e sem alterar.
6. Heranca modela "e um"; composicao modela "tem um".
7. `virtual` permite polimorfismo; `override` protege sobrescrita.
8. `std::unique_ptr` comunica ownership exclusivo em C++ puro.
9. Templates sao codigo generico resolvido pelo compilador.
10. Lambdas sao funcoes anonimas locais.
11. Unreal usa reflection e code generation via macros.
12. `UObject`/`AActor` nao sao gerenciados como objetos C++ comuns.
13. `UPROPERTY` importa para editor, Blueprint, serializacao e GC.
14. `CreateDefaultSubobject`, `NewObject` e `SpawnActor` criam coisas diferentes.
15. `Destroy`, nao `delete`, remove actors.
