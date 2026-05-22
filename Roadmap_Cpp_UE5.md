# Roadmap de C++ para chegar bem na UE5

## Contexto

Voce ja programa em Go e Node.js. Isso ajuda bastante em:

- logica de programacao
- decomposicao de problemas
- leitura de codigo
- organizacao de projetos

O que vai ser novo em C++ nao e a logica em si, e o modelo da linguagem:

- memoria manual e semiautomatica
- compilacao mais complexa
- sistema de tipos mais expressivo e mais perigoso
- orientacao a objetos com custo e detalhes reais
- separacao entre interface e implementacao com `.h` e `.cpp`
- controle fino de construcao, destruicao e ownership

Na UE5, alem do C++ padrao, existe um "dialeto de uso" com macros, reflection system, tipos proprios e um estilo de arquitetura especifico do motor. O ideal e aprender em duas camadas:

1. C++ moderno de verdade
2. C++ aplicado ao ecossistema da Unreal Engine

Este roadmap segue exatamente essa ordem.

---

## Objetivo realista

Seu objetivo nao deve ser "virar especialista em C++ antes de tocar na UE5". Isso atrasa demais.

O objetivo correto e:

- entender o modelo mental de C++
- aprender a ler e escrever C++ moderno sem medo
- saber onde o C++ da UE5 segue o padrao e onde ele se afasta
- chegar na engine com base teorica suficiente para entender o que esta acontecendo

---

## Estrategia geral de estudo

Divida seu estudo em 4 fases:

1. Fundamentos da linguagem
2. Modelo de memoria, objetos e custos
3. C++ moderno e boas praticas
4. Adaptacao para o ecossistema da UE5

Regra importante: em C++, entender a teoria muda diretamente a qualidade da pratica. Nao estude so "como faz", estude "o que o compilador e o runtime estao fazendo".

---

## Fase 1 - Fundamentos da linguagem

### Objetivo

Entender a sintaxe basica e, principalmente, o modelo estrutural da linguagem.

### O que estudar

#### 1. Estrutura minima de um programa C++

- funcao `main`
- includes
- namespaces
- diferenca entre declaracao e definicao
- compilacao e linkagem, em alto nivel

#### 2. Tipos basicos

- `int`, `float`, `double`, `char`, `bool`
- signed vs unsigned
- tamanho de tipos e por que isso importa
- conversoes implicitas e explicitas
- `const`
- `auto`

#### 3. Controle de fluxo

- `if`
- `switch`
- `for`
- `while`
- `range-based for`

#### 4. Funcoes

- passagem por valor
- passagem por referencia
- passagem por ponteiro
- sobrecarga de funcao
- argumentos default
- inline em conceito, sem obsessao

### Teoria que voce precisa dominar

- C++ nao e interpretado como Node.js
- o codigo passa por preprocessamento, compilacao e linkagem
- o tipo de uma variavel importa muito mais do que em JS
- copia de valor tem custo real
- referencia e ponteiro existem para representar acesso indireto e evitar copias

### Resultado esperado da fase

Ao terminar essa fase, voce deve conseguir ler um arquivo simples em C++ e entender:

- o que esta sendo declarado
- o que esta sendo definido
- como os dados circulam entre funcoes

---

## Fase 2 - Memoria, ponteiros e ciclo de vida

### Objetivo

Essa e a fase mais importante. Quem nao entende isso sofre em C++ e sofre mais ainda em UE5.

### O que estudar

#### 1. Stack vs Heap

- o que vai para stack
- o que geralmente vai para heap
- tempo de vida de objetos
- custo de alocacao

#### 2. Ponteiros

- o que e um ponteiro
- endereco de memoria
- `*` e `&`
- ponteiro nulo
- aritmetica de ponteiros em nivel conceitual
- por que ponteiros sao poderosos e perigosos

#### 3. Referencias

- diferenca entre referencia e ponteiro
- quando usar referencia
- por que referencia costuma ser a opcao mais segura quando nao existe ausencia de valor

#### 4. Ciclo de vida de objeto

- construtor
- destrutor
- ordem de destruicao
- escopo

#### 5. Copia e movimento

- copy constructor
- copy assignment
- move constructor
- move assignment
- custo de copiar objetos
- ideia de transferir ownership

### Teoria que voce precisa dominar

- RAII: Resource Acquisition Is Initialization
- ownership
- quem e responsavel por destruir um recurso
- dangling pointer
- memory leak
- use-after-free
- double free

### Por que isso importa para UE5

A Unreal abstrai varias coisas, mas nao elimina o fato de que:

- objetos tem ciclo de vida
- referencias invalidas quebram o jogo
- ownership importa
- performance importa em runtime

### Resultado esperado da fase

Voce deve conseguir explicar com clareza:

- por que um ponteiro pode ficar invalido
- por que um objeto local morre ao sair do escopo
- por que copiar um objeto pode ser caro
- por que RAII e uma ideia central em C++

---

## Fase 3 - Orientacao a objetos em C++

### Objetivo

Aprender OOP do jeito que C++ implementa, nao do jeito simplificado de linguagens gerenciadas.

### O que estudar

#### 1. Classes e structs

- diferenca entre `class` e `struct`
- membros publicos e privados
- encapsulamento

#### 2. Construtores e destrutores

- inicializacao correta
- initializer list
- invariantes de objeto

#### 3. Heranca

- heranca publica
- classes base e derivadas
- override
- virtual
- polimorfismo

#### 4. Classes abstratas e interfaces

- metodos virtuais puros
- design por contrato

### Teoria que voce precisa dominar

- despacho dinamico tem custo
- objeto precisa nascer valido
- heranca deve modelar relacao real, nao reutilizacao preguiçosa
- composicao muitas vezes e melhor que heranca

### Relacao com UE5

Na UE5 isso aparece o tempo todo:

- `AActor`
- `APawn`
- `ACharacter`
- `UObject`
- componentes
- overrides de metodos do engine

Sem entender heranca, virtual dispatch e ciclo de vida, o codigo da engine parece magico. Nao e. So e uma hierarquia grande com reflection e convencoes.

---

## Fase 4 - C++ moderno

### Objetivo

Sair do C++ "antigo e perigoso" e estudar o C++ moderno que voce realmente deve usar.

### O que estudar

#### 1. Referencias modernas

- lvalue e rvalue em nivel conceitual
- `T&`
- `const T&`
- `T&&`
- `std::move`

Nao precisa aprofundar isso no primeiro dia, mas precisa entender que move semantics existe para evitar copias desnecessarias.

#### 2. Smart pointers

- `std::unique_ptr`
- `std::shared_ptr`
- `std::weak_ptr`
- ownership exclusivo vs compartilhado

#### 3. STL

- `std::vector`
- `std::string`
- `std::array`
- `std::unordered_map`
- `std::optional`

#### 4. Templates

- o que sao
- generics em C++ vs generics de outras linguagens
- instanciacao em compile time
- custo de complexidade

#### 5. Lambdas

- captura por valor
- captura por referencia
- usos praticos

#### 6. Tratamento de erro

- excecoes em C++ em nivel conceitual
- codigos de erro
- precondicoes e invariantes

### Teoria que voce precisa dominar

- zero-cost abstractions
- diferenca entre custo aparente e custo real
- tipos da STL sao centrais para escrever C++ moderno
- template nao e "so generics"; ele participa fortemente do compile time

### Resultado esperado da fase

Voce deve conseguir ler codigo moderno e identificar:

- onde ha copia
- onde ha movimento
- quem possui o recurso
- que container faz sentido para cada caso

---

## Fase 5 - Compilacao, headers e organizacao de projeto

### Objetivo

Entender por que projetos C++ ficam grandes, lentos para compilar e sensiveis a dependencia circular.

### O que estudar

#### 1. Arquivos `.h` e `.cpp`

- declaracao no header
- implementacao no source
- include guards
- `#pragma once`

#### 2. Preprocessador

- `#include`
- `#define`
- custo conceitual das macros

#### 3. Linkagem

- simbolos
- multiplas definicoes
- undefined reference

#### 4. Forward declaration

- quando usar
- quando nao e suficiente

### Teoria que voce precisa dominar

- incluir demais piora tempo de compilacao
- header mal planejado espalha acoplamento
- macros existem antes da compilacao propriamente dita

### Relacao com UE5

A UE5 usa intensamente:

- headers
- macros
- code generation
- convencoes de include

Sem entender a estrutura de compilacao, os erros da Unreal vao parecer absurdos. Com base teorica, eles ficam apenas chatos.

---

## Fase 6 - O que muda no C++ da UE5

### Objetivo

Entender exatamente onde o "C++ da Unreal" difere do C++ padrao.

### O que estudar

#### 1. Reflection system

- `UCLASS`
- `USTRUCT`
- `UENUM`
- `UPROPERTY`
- `UFUNCTION`

Essas macros nao sao "enfeite". Elas integram seu codigo com:

- editor
- garbage collection da Unreal
- serializacao
- Blueprints
- networking

#### 2. Tipos da Unreal

- `FString`
- `FName`
- `FText`
- `TArray`
- `TMap`
- `TSet`
- `TObjectPtr`
- `TWeakObjectPtr`
- `TSharedPtr`

#### 3. Modelo de objetos do engine

- `UObject`
- `AActor`
- `UActorComponent`
- `GameMode`
- `PlayerController`
- `Pawn`
- `Character`

#### 4. Garbage collection da Unreal

Muito importante: a Unreal nao usa o mesmo modelo da STL para tudo.

Voce precisa entender:

- quais objetos sao rastreados pelo GC da engine
- por que `UPROPERTY` importa
- quando usar ponteiros da Unreal
- quando usar tipos padrao de C++

#### 5. Estilo de arquitetura da engine

- objetos criados e gerenciados pelo engine
- callbacks de ciclo de vida
- eventos
- gameplay framework

### Teoria que voce precisa dominar

- UE5 nao substitui C++, ela o estende
- macros da Unreal existem para integracao com o ecossistema
- nem todo ponteiro em Unreal tem o mesmo significado
- lifetime e ownership continuam sendo o centro do problema

---

## Ordem recomendada de estudo

### Etapa 1 - Base pura de C++

Estude nesta ordem:

1. tipos, variaveis e controle de fluxo
2. funcoes
3. referencias e ponteiros
4. stack, heap e ciclo de vida
5. classes, construtores e destrutores
6. heranca e virtual

### Etapa 2 - C++ moderno

1. `const` corretamente
2. `std::vector`, `std::string`, `std::optional`
3. smart pointers
4. copy vs move
5. lambdas
6. templates em nivel introdutorio

### Etapa 3 - Infraestrutura da linguagem

1. headers e source files
2. preprocessador
3. linkagem
4. forward declarations

### Etapa 4 - Entrada em UE5

1. macros da Unreal
2. tipos proprios da engine
3. gameplay framework
4. garbage collection e ownership no engine
5. interoperacao com Blueprints

---

## O que estudar teoricamente antes de abrir a UE5

Se quiser maximizar retorno, entre na UE5 ja entendendo bem:

- variaveis, tipos e `const`
- referencias vs ponteiros
- construtor e destrutor
- stack vs heap
- classes, heranca e virtual
- `std::vector` e `std::string`
- ownership
- RAII
- headers e compilacao

Se essa base estiver firme, a curva na Unreal cai muito.

---

## Comparacao mental com Go e Node.js

### Vindo de Go

Voce ja tem vantagem em:

- tipos estaticos
- structs
- ponteiros em nivel basico
- valor vs referencia como ideia pratica

Mas C++ adiciona:

- destrutores
- copy/move semantics
- heranca e polimorfismo classico
- muito mais controle e muito mais risco

### Vindo de Node.js

Voce ja tem vantagem em:

- modelagem de sistemas
- asincronismo como raciocinio geral
- produtividade de prototipagem

Mas vai estranhar:

- tipagem forte em compile time
- necessidade de declarar estrutura com mais rigor
- erros de compilacao mais detalhados e menos amigaveis
- ausencia de runtime gerenciado do jeito do JS

---

## Erros conceituais que voce deve evitar

### 1. Tentar aprender UE5 antes de entender ponteiros e lifetime

Isso gera decoracao de sintaxe sem compreensao.

### 2. Aprender C++ por material antigo demais

C++ moderno e muito melhor que o estilo legado. Aprenda primeiro o modelo atual.

### 3. Decorar sintaxe sem entender custo

Em C++, quase toda abstracao tem implicacoes de memoria, copia, destruicao ou compilacao.

### 4. Achar que Unreal "resolve tudo"

A engine ajuda, mas continua exigindo base seria de linguagem.

---

## Roadmap semanal sugerido

### Semana 1

- sintaxe basica
- tipos
- controle de fluxo
- funcoes
- `const`

### Semana 2

- referencias
- ponteiros
- stack vs heap
- construtores e destrutores
- RAII

### Semana 3

- classes
- encapsulamento
- heranca
- virtual
- polimorfismo

### Semana 4

- STL basica
- `std::vector`
- `std::string`
- `std::optional`
- copia vs movimento

### Semana 5

- smart pointers
- lambdas
- templates introdutorios
- headers
- preprocessador

### Semana 6

- macros da Unreal
- `UObject`
- `AActor`
- `UPROPERTY`
- gameplay framework

### Semana 7 em diante

- estudar C++ junto com pequenos exemplos em UE5
- comparar tipos da STL com tipos da Unreal
- entender ownership no contexto do engine

---

## Materias teoricas que merecem revisao continua

Volte varias vezes nestes temas:

- valor vs referencia
- ponteiro vs referencia
- ownership
- lifetime
- copy vs move
- polimorfismo
- compilacao e linkagem
- reflection da Unreal

Esses temas nao sao capitulos isolados. Eles formam o nucleo do entendimento.

---

## Como saber se voce esta pronto para estudar UE5 em C++

Voce esta pronto quando conseguir responder sem hesitar:

- o que diferencia ponteiro de referencia?
- quando um objeto e destruido?
- o que significa ownership?
- por que `const T&` e comum?
- por que um header mal feito causa problema?
- por que a Unreal usa macros como `UCLASS` e `UPROPERTY`?

Se voce responder isso com clareza, a base esta boa.

---

## Plano final recomendado

Siga esta sequencia:

1. Aprenda C++ moderno basico
2. Domine memoria, lifetime e ownership
3. Entenda OOP em C++ de forma real
4. Aprenda STL e move semantics
5. Entenda compilacao, headers e macros
6. So entao entre forte no C++ da UE5

---

## Fechamento

Para voce, que ja vem de Go e Node.js, o principal desafio nao e aprender a pensar como programador. Isso voce ja sabe.

O desafio e aprender como C++ representa objetos, memoria, custo e ciclo de vida. A teoria aqui nao e acessorio; ela e a parte central.

Se quiser continuar esse estudo de forma eficiente, o proximo passo ideal e montar um segundo arquivo com:

- ordem de leitura de topicos
- exercicios teoricos por fase
- comparacoes entre C++ padrao e C++ da UE5
- lista de conceitos obrigatorios antes de tocar em `UObject` e `AActor`
