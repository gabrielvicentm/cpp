# Aula 01 - Fundamentos de C++ para chegar bem na UE5

Este arquivo cobre a parte inicial do seu `Roadmap_Cpp_UE5.md`: a Fase 1, chamada "Fundamentos da linguagem".

O objetivo aqui nao e decorar sintaxe. O objetivo e voce construir o modelo mental correto para ler e escrever C++ sem tratar a linguagem como se fosse Go, JavaScript ou Blueprint.

Voce ja sabe programar. Entao a pergunta principal nao e "o que e um if?", mas sim:

- o que esse codigo vira quando compila?
- onde esse valor existe?
- esse valor esta sendo copiado?
- essa funcao altera o original ou so uma copia?
- o tipo escolhido tem custo, limite ou comportamento perigoso?

Essas perguntas vao acompanhar voce o tempo todo em C++ e depois na UE5.

---

## Como usar esta aula hoje

Estude nesta ordem:

1. Leia a explicacao de cada bloco.
2. Digite os exemplos manualmente.
3. Compile e rode.
4. Altere pequenas partes e veja o erro ou o novo comportamento.
5. Faca os exercicios sem olhar resposta pronta.
6. Escreva em voz alta o que o codigo esta fazendo.

Se voce apenas ler, vai parecer que entendeu. C++ cobra entendimento quando o compilador reclama ou quando uma copia invisivel aparece.

Tempo sugerido para hoje:

- 30 min: estrutura minima, compilacao e includes
- 45 min: tipos, `const`, conversoes e `auto`
- 45 min: controle de fluxo
- 90 min: funcoes, valor, referencia e ponteiro
- 60 min: exercicios integrados
- 30 min: revisao e anotacoes

Se der menos tempo, priorize funcoes, `const`, referencia e ponteiro. Isso vai aparecer em praticamente todo codigo da Unreal.

---

## Preparando o ambiente mental

Em Node.js, voce normalmente escreve um arquivo e executa com um runtime:

```js
console.log("Oi");
```

Em Go, voce ja tem compilacao, mas a linguagem esconde ou simplifica varias decisoes.

Em C++, o caminho conceitual e mais explicito:

```text
codigo fonte .cpp
        |
        v
preprocessador
        |
        v
compilador
        |
        v
arquivo objeto
        |
        v
linker
        |
        v
executavel
```

Voce nao precisa dominar todos os detalhes agora, mas precisa saber que:

- `#include` acontece antes da compilacao propriamente dita.
- O compilador verifica tipos e transforma codigo em objeto.
- O linker junta as partes e resolve referencias entre arquivos.
- Muitos erros de C++ nao aparecem como erro de "runtime"; eles aparecem antes, na compilacao ou na linkagem.

Na UE5, isso importa muito porque a engine usa muitos headers, macros, geracao de codigo e linkagem. Entender o basico agora evita que os erros da Unreal parecam magia.

---

## 1. Estrutura minima de um programa C++

Um programa C++ minimo:

```cpp
#include <iostream>

int main()
{
    std::cout << "Ola, C++\n";
    return 0;
}
```

Vamos destrinchar.

### `#include <iostream>`

`#include` pede ao preprocessador para incluir o conteudo de outro arquivo antes da compilacao.

`iostream` e parte da biblioteca padrao e fornece ferramentas de entrada e saida, como:

- `std::cout`: saida no console
- `std::cin`: entrada pelo teclado
- `std::cerr`: saida de erro

Em C++, incluir coisas demais pode aumentar tempo de compilacao. Por enquanto isso nao e problema, mas na UE5 isso vira um assunto serio.

### `int main()`

`main` e o ponto de entrada de um programa C++ comum.

```cpp
int main()
```

Isso significa:

- `int`: a funcao retorna um numero inteiro.
- `main`: nome especial da funcao de entrada.
- `()`: lista de parametros vazia.

O `return 0;` indica sucesso.

Na UE5 voce quase nunca vai escrever um `main`, porque a engine fornece o ponto de entrada. Mas entender `main` ajuda a entender programas C++ independentes e exemplos didaticos.

### `std::cout`

`std::cout` pertence ao namespace `std`.

```cpp
std::cout << "Ola\n";
```

Pense no `std::` como uma forma de dizer: "procure isso dentro do namespace da biblioteca padrao".

Voce pode ver exemplos com:

```cpp
using namespace std;
```

Evite criar esse habito. Em codigo pequeno funciona, mas em projetos grandes pode causar conflito de nomes. Na Unreal tambem e melhor ser claro sobre de onde vem cada coisa.

---

## 2. Declaracao vs definicao

Esse tema parece teorico, mas e central em C++.

Declarar e dizer que algo existe.

Definir e criar de fato aquilo, geralmente com corpo ou armazenamento.

### Exemplo com funcao

Declaracao:

```cpp
int Somar(int a, int b);
```

Isso diz ao compilador:

"Existe uma funcao chamada `Somar`, que recebe dois `int` e retorna `int`."

Definicao:

```cpp
int Somar(int a, int b)
{
    return a + b;
}
```

Agora voce deu o corpo da funcao.

Em projetos C++, voce frequentemente coloca declaracoes em arquivos `.h` e definicoes em arquivos `.cpp`.

Exemplo conceitual:

```text
Calculadora.h
    declara Somar

Calculadora.cpp
    define Somar
```

Na UE5 isso aparece o tempo todo:

```text
MyActor.h
    declara a classe e seus membros

MyActor.cpp
    implementa os metodos
```

### Exercicio rapido

Leia o codigo:

```cpp
#include <iostream>

int Multiplicar(int a, int b);

int main()
{
    std::cout << Multiplicar(3, 4) << "\n";
    return 0;
}

int Multiplicar(int a, int b)
{
    return a * b;
}
```

Responda:

1. Onde esta a declaracao?
2. Onde esta a definicao?
3. Por que `main` consegue chamar `Multiplicar` antes do corpo dela aparecer?

Resposta esperada:

1. `int Multiplicar(int a, int b);`
2. O bloco com `{ return a * b; }`
3. Porque a declaracao ja informou ao compilador que a funcao existe.

---

## 3. Tipos basicos

Em C++, tipo nao e detalhe. Tipo define:

- tamanho aproximado em memoria
- faixa de valores
- operacoes permitidas
- conversoes possiveis
- custo de copia
- como uma funcao recebe ou retorna dados

Tipos basicos importantes:

```cpp
int vida = 100;
float velocidade = 600.0f;
double precisao = 0.123456789;
char letra = 'A';
bool vivo = true;
```

### `int`

Usado para inteiros.

```cpp
int municao = 30;
int dano = -10;
```

Normalmente tem 32 bits em plataformas modernas, mas o padrao nao garante exatamente o mesmo tamanho em todo ambiente.

Quando precisar de tamanho fixo, C++ oferece tipos em `<cstdint>`:

```cpp
#include <cstdint>

int32_t vida = 100;
uint32_t moedas = 250;
```

Na Unreal voce vai ver tipos como:

```cpp
int32
uint8
float
bool
```

Ou seja: entender tamanho de tipo hoje ajuda a ler codigo da engine amanha.

### `float` e `double`

`float` usa menos memoria e tem menos precisao.

`double` usa mais memoria e tem mais precisao.

```cpp
float velocidade = 420.5f;
double calculoPreciso = 0.333333333333;
```

Repare no `f`:

```cpp
420.5f
```

Sem o `f`, um literal decimal costuma ser tratado como `double`.

Em jogos, `float` aparece muito porque e suficiente para muitas simulacoes e ocupa menos memoria. Na Unreal, `float` e extremamente comum.

### `char`

Representa um caractere simples:

```cpp
char tecla = 'W';
```

Use aspas simples para `char`:

```cpp
'A'
```

Use aspas duplas para texto:

```cpp
"A"
```

`'A'` e um caractere.

`"A"` e uma sequencia de caracteres.

Essa diferenca importa.

### `bool`

Representa verdadeiro ou falso:

```cpp
bool estaNoChao = true;
bool podePular = false;
```

Em C++, nomes booleanos ficam melhores quando parecem perguntas:

```cpp
bool isAlive = true;
bool hasAmmo = false;
bool canJump = true;
```

Na UE5 voce vai ver muito:

```cpp
bool bIsAlive;
bool bCanJump;
```

O prefixo `b` e uma convencao comum da Unreal para booleanos.

---

## 4. Signed vs unsigned

Tipos inteiros podem ser com sinal ou sem sinal.

Com sinal:

```cpp
int vida = -10;
```

Sem sinal:

```cpp
unsigned int quantidade = 10;
```

Um tipo `unsigned` nao representa valores negativos.

Isso parece bom para coisas que "nunca devem ser negativas", como moedas ou quantidade de itens. Mas cuidado: `unsigned` pode causar bugs estranhos em comparacoes e subtracoes.

Exemplo perigoso:

```cpp
#include <iostream>

int main()
{
    unsigned int moedas = 0;
    moedas = moedas - 1;

    std::cout << moedas << "\n";
    return 0;
}
```

Voce talvez espere `-1`, mas `unsigned` nao comporta negativo. O valor "daria a volta" para um numero enorme.

Regra pragmatica para agora:

- Use `int` para contagens simples nos estudos.
- Use `unsigned` quando voce realmente tiver um motivo.
- Na Unreal, respeite os tipos esperados pela API.

---

## 5. Conversoes implicitas e explicitas

C++ permite conversoes entre tipos. Algumas sao automaticas, outras voce deve tornar explicitas.

### Conversao implicita

```cpp
int vida = 100;
float vidaComoFloat = vida;
```

Aqui, `int` virou `float` automaticamente.

Pode ser ok.

Mas veja:

```cpp
float velocidade = 420.75f;
int velocidadeInteira = velocidade;
```

Agora voce perdeu a parte decimal.

`420.75f` vira `420`.

Isso e perda de informacao.

### Conversao explicita

Use `static_cast` quando quiser deixar claro que sabe que esta convertendo:

```cpp
float velocidade = 420.75f;
int velocidadeInteira = static_cast<int>(velocidade);
```

Isso nao muda o resultado, mas muda a intencao do codigo.

Voce esta dizendo:

"Sim, eu sei que estou convertendo `float` para `int`."

Em C++, codigo claro sobre conversoes evita bug silencioso.

### Exercicio

Preveja a saida:

```cpp
#include <iostream>

int main()
{
    int dano = 7;
    int armadura = 2;

    float resultado1 = dano / armadura;
    float resultado2 = static_cast<float>(dano) / armadura;

    std::cout << resultado1 << "\n";
    std::cout << resultado2 << "\n";

    return 0;
}
```

Resposta:

```text
3
3.5
```

Por que?

`dano / armadura` usa divisao entre inteiros. O resultado inteiro e `3`.

Quando voce converte `dano` para `float`, a divisao passa a ser de ponto flutuante.

---

## 6. `const`

`const` significa que algo nao deve ser modificado depois de inicializado.

```cpp
const int MaxHealth = 100;
```

Se voce tentar:

```cpp
MaxHealth = 200;
```

O compilador rejeita.

### Por que `const` importa tanto?

Porque C++ permite alterar muita coisa. `const` coloca uma trava semantica e ajuda o compilador a proteger sua intencao.

Compare:

```cpp
int danoBase = 10;
```

Com:

```cpp
const int danoBase = 10;
```

No segundo caso, fica claro que `danoBase` nao deve mudar.

### `const` com parametros

Este ponto e muito importante para funcoes.

```cpp
void ImprimirNome(const std::string& nome)
{
    std::cout << nome << "\n";
}
```

Ainda vamos estudar referencia com mais calma, mas leia assim:

- `std::string&` evita copiar a string.
- `const` garante que a funcao nao vai alterar a string original.

Esse padrao e muito comum:

```cpp
const TipoGrande& valor
```

Ele significa:

"Receba sem copiar, mas nao modifique."

Na UE5, voce vera muito `const` em parametros, metodos e retornos.

---

## 7. `auto`

`auto` pede ao compilador para deduzir o tipo.

```cpp
auto vida = 100;
auto velocidade = 600.0f;
auto nome = std::string("Heroi");
```

O compilador deduz:

- `vida`: `int`
- `velocidade`: `float`
- `nome`: `std::string`

### Quando `auto` ajuda

Quando o tipo e obvio ou muito verboso:

```cpp
auto danoFinal = danoBase * multiplicador;
```

Ou em iteradores e templates no futuro.

### Quando `auto` atrapalha

Quando esconde uma decisao importante:

```cpp
auto valor = 10u;
```

Aqui `valor` pode ser `unsigned int`. Se voce nao percebeu o `u`, pode criar comportamento inesperado.

Regra para agora:

- Use tipo explicito enquanto estiver aprendendo.
- Use `auto` quando o lado direito deixar o tipo muito claro.
- Nao use `auto` para fugir de entender o tipo.

---

## 8. Controle de fluxo

Controle de fluxo define quais partes do codigo executam e quantas vezes.

### `if`

```cpp
int vida = 40;

if (vida <= 0)
{
    std::cout << "Morto\n";
}
else if (vida < 50)
{
    std::cout << "Ferido\n";
}
else
{
    std::cout << "Bem\n";
}
```

Em C++, condicoes geralmente devem ser expressas como `bool` ou algo conversivel para `bool`.

Evite codigo ambiguo enquanto aprende:

```cpp
if (vida)
{
}
```

Prefira:

```cpp
if (vida > 0)
{
}
```

### `switch`

Bom para escolher entre valores discretos.

```cpp
int estado = 2;

switch (estado)
{
    case 0:
        std::cout << "Parado\n";
        break;
    case 1:
        std::cout << "Andando\n";
        break;
    case 2:
        std::cout << "Atacando\n";
        break;
    default:
        std::cout << "Estado desconhecido\n";
        break;
}
```

O `break` e importante. Sem ele, a execucao pode continuar para o proximo `case`.

Isso se chama fallthrough.

As vezes e intencional, mas para iniciante quase sempre e bug.

### `for`

```cpp
for (int i = 0; i < 5; ++i)
{
    std::cout << i << "\n";
}
```

Partes:

```text
int i = 0     inicializacao
i < 5         condicao
++i          incremento
```

Use `++i` por habito em C++. Para tipos basicos nao faz diferenca relevante aqui, mas para tipos mais complexos pode evitar copia temporaria em alguns contextos.

### `while`

```cpp
int stamina = 3;

while (stamina > 0)
{
    std::cout << "Correndo\n";
    --stamina;
}
```

Use quando voce nao sabe exatamente quantas iteracoes vao acontecer.

### Range-based for

Com containers, C++ moderno permite:

```cpp
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> danos = {10, 20, 15};

    for (int dano : danos)
    {
        std::cout << dano << "\n";
    }

    return 0;
}
```

Isso percorre cada elemento.

Mais tarde, voce vai usar muito:

```cpp
for (const auto& item : itens)
{
}
```

Leia assim:

"Para cada item em itens, acesse sem copiar e sem modificar."

Ainda vamos consolidar isso quando estudar referencias.

---

## 9. Funcoes

Funcoes separam comportamento, reduzem duplicacao e tornam codigo testavel.

Exemplo simples:

```cpp
int CalcularDanoFinal(int danoBase, int bonus)
{
    return danoBase + bonus;
}
```

Chamada:

```cpp
int dano = CalcularDanoFinal(10, 5);
```

### Assinatura de uma funcao

```cpp
int CalcularDanoFinal(int danoBase, int bonus)
```

Partes:

- `int`: tipo de retorno
- `CalcularDanoFinal`: nome
- `int danoBase`: primeiro parametro
- `int bonus`: segundo parametro

### Funcao sem retorno

```cpp
void ImprimirVida(int vida)
{
    std::cout << "Vida: " << vida << "\n";
}
```

`void` significa que a funcao nao retorna valor.

---

## 10. Passagem por valor

Por padrao, parametros sao copiados.

```cpp
#include <iostream>

void ReceberDano(int vida)
{
    vida -= 10;
    std::cout << "Dentro da funcao: " << vida << "\n";
}

int main()
{
    int vida = 100;

    ReceberDano(vida);

    std::cout << "Fora da funcao: " << vida << "\n";
    return 0;
}
```

Saida:

```text
Dentro da funcao: 90
Fora da funcao: 100
```

Por que?

Porque `ReceberDano` recebeu uma copia de `vida`.

Esse e um ponto central:

```cpp
void ReceberDano(int vida)
```

O parametro `vida` dentro da funcao e outro `int`.

Para tipos pequenos, como `int`, copiar e barato. Para objetos grandes, copiar pode ser caro.

### Quando usar passagem por valor

Use passagem por valor quando:

- o tipo e pequeno (`int`, `float`, `bool`, enums)
- voce quer uma copia independente
- a funcao precisa modificar a copia sem afetar o original

---

## 11. Passagem por referencia

Referencia permite que a funcao acesse o objeto original.

```cpp
#include <iostream>

void ReceberDano(int& vida)
{
    vida -= 10;
}

int main()
{
    int vida = 100;

    ReceberDano(vida);

    std::cout << vida << "\n";
    return 0;
}
```

Saida:

```text
90
```

O `&` no parametro:

```cpp
int& vida
```

significa "referencia para int".

Leia assim:

"Essa funcao recebe o `int` original, nao uma copia."

### Referencia com `const`

Se a funcao nao deve alterar o original, use `const`:

```cpp
#include <iostream>
#include <string>

void ImprimirNome(const std::string& nome)
{
    std::cout << nome << "\n";
}
```

Esse e um dos padroes mais importantes de C++:

```cpp
const Tipo& valor
```

Vantagens:

- evita copia
- protege contra modificacao
- comunica intencao

### Quando usar referencia

Use referencia quando:

- voce quer alterar o original: `Tipo&`
- voce quer evitar copia sem alterar: `const Tipo&`
- o valor sempre precisa existir

Se a ausencia de valor for possivel, ponteiro pode fazer mais sentido.

---

## 12. Passagem por ponteiro

Ponteiro guarda um endereco.

```cpp
int vida = 100;
int* ponteiroParaVida = &vida;
```

Partes:

- `int*`: ponteiro para `int`
- `&vida`: endereco de `vida`

Para acessar o valor apontado:

```cpp
*ponteiroParaVida = 90;
```

O `*` aqui e desreferenciacao: "acesse o valor no endereco".

Exemplo completo:

```cpp
#include <iostream>

void ReceberDano(int* vida)
{
    if (vida != nullptr)
    {
        *vida -= 10;
    }
}

int main()
{
    int vida = 100;

    ReceberDano(&vida);

    std::cout << vida << "\n";
    return 0;
}
```

Saida:

```text
90
```

### `nullptr`

Um ponteiro pode apontar para nada:

```cpp
int* alvo = nullptr;
```

Por isso, antes de usar um ponteiro, muitas vezes voce verifica:

```cpp
if (alvo != nullptr)
{
    // pode usar alvo
}
```

Referencia normalmente pressupoe que existe um objeto valido. Ponteiro pode representar "talvez exista".

### Referencia vs ponteiro

Use referencia quando:

- o valor e obrigatorio
- voce nao precisa representar ausencia
- voce quer sintaxe mais simples

Use ponteiro quando:

- o valor pode ser ausente
- voce precisa trocar para onde aponta
- a API exige ponteiro
- voce esta lidando com ownership ou objetos gerenciados por outro sistema

Na UE5, ponteiros aparecem muito:

```cpp
AActor* Target;
UCameraComponent* Camera;
```

Mas nem todo ponteiro significa a mesma coisa. Na Unreal, alguns objetos sao gerenciados pelo garbage collector da engine, e macros como `UPROPERTY` influenciam esse rastreamento. Isso fica para fases futuras, mas o alicerce e este: ponteiro e endereco, e endereco pode ser invalido.

---

## 13. Sobrecarga de funcao

C++ permite varias funcoes com o mesmo nome, desde que a lista de parametros seja diferente.

```cpp
int Somar(int a, int b)
{
    return a + b;
}

float Somar(float a, float b)
{
    return a + b;
}
```

Chamadas:

```cpp
Somar(2, 3);       // usa versao int
Somar(2.5f, 3.0f); // usa versao float
```

Isso e sobrecarga.

O compilador escolhe a funcao com base nos tipos dos argumentos.

Cuidado: conversoes implicitas podem tornar a escolha menos obvia.

---

## 14. Argumentos default

Voce pode dar valor padrao a parametros:

```cpp
int CalcularDano(int danoBase, int bonus = 0)
{
    return danoBase + bonus;
}
```

Chamadas:

```cpp
CalcularDano(10);    // bonus = 0
CalcularDano(10, 5); // bonus = 5
```

Use com moderacao. Argumentos default podem deixar APIs convenientes, mas tambem podem esconder comportamento importante.

Na Unreal, voce vera parametros default em algumas APIs e em codigo de gameplay.

---

## 15. `inline` em conceito

`inline` historicamente sugeria ao compilador inserir o corpo da funcao no lugar da chamada, evitando overhead de chamada.

Hoje, o compilador decide muita coisa sozinho.

Para agora, entenda:

- `inline` nao e uma garantia simples de performance.
- `inline` tambem tem relacao com definicoes em headers.
- Nao use `inline` como ferramenta magica.

Voce pode ignorar detalhes por enquanto. Quando estudar headers e linkagem, isso vai fazer mais sentido.

---

## 16. Comparacao com Go e Node.js

### Se voce pensar como Go

Go:

```go
func Add(a int, b int) int {
    return a + b
}
```

C++:

```cpp
int Add(int a, int b)
{
    return a + b;
}
```

Parecido, mas C++ deixa voce controlar mais detalhes:

- referencia
- ponteiro
- `const`
- sobrecarga
- construcao e destruicao de objetos
- copia e movimento

### Se voce pensar como Node.js

JavaScript:

```js
function add(a, b) {
  return a + b;
}
```

C++:

```cpp
int Add(int a, int b)
{
    return a + b;
}
```

Em JS, `a` e `b` podem ser varias coisas em runtime.

Em C++, `a` e `b` sao `int` em compile time.

Isso e uma troca:

- menos flexibilidade dinamica
- mais seguranca antes de rodar
- mais informacao para otimizacao
- erros mais cedo

---

## 17. Mini-projeto guiado: sistema simples de personagem

Crie um arquivo `main.cpp` com este codigo, compile e rode.

```cpp
#include <iostream>
#include <string>

void PrintStatus(const std::string& name, int health, int stamina)
{
    std::cout << "Name: " << name << "\n";
    std::cout << "Health: " << health << "\n";
    std::cout << "Stamina: " << stamina << "\n";
}

void ApplyDamage(int& health, int damage)
{
    health -= damage;

    if (health < 0)
    {
        health = 0;
    }
}

bool CanRun(int stamina)
{
    return stamina > 0;
}

void SpendStamina(int& stamina, int amount)
{
    stamina -= amount;

    if (stamina < 0)
    {
        stamina = 0;
    }
}

int main()
{
    std::string playerName = "Rookie";
    int health = 100;
    int stamina = 40;

    PrintStatus(playerName, health, stamina);

    ApplyDamage(health, 35);

    if (CanRun(stamina))
    {
        SpendStamina(stamina, 15);
    }

    std::cout << "\nAfter action:\n";
    PrintStatus(playerName, health, stamina);

    return 0;
}
```

Observe:

- `PrintStatus` recebe `name` como `const std::string&` para nao copiar e nao alterar.
- `ApplyDamage` recebe `health` como `int&` porque precisa alterar o original.
- `CanRun` recebe `stamina` por valor porque `int` e pequeno e a funcao so consulta.
- `SpendStamina` altera o original usando referencia.

Esse tipo de raciocinio vira natural em C++: cada parametro deve ter uma justificativa.

---

## 18. Exercicios

### Exercicio 1 - Tipos e saida

Crie variaveis para um personagem:

- nome
- vida
- stamina
- velocidade
- se esta vivo

Imprima tudo no console.

Regras:

- use `std::string` para nome
- use `int` para vida e stamina
- use `float` para velocidade
- use `bool` para vivo

Perguntas:

1. Por que velocidade faz mais sentido como `float`?
2. Por que vida pode comecar como `int`?
3. O que mudaria se vida tivesse decimais?

### Exercicio 2 - Conversao

Crie:

```cpp
int currentHealth = 37;
int maxHealth = 100;
```

Calcule a porcentagem de vida como `float`.

Resultado esperado:

```text
0.37
```

Cuidado com divisao inteira.

Dica:

```cpp
float ratio = static_cast<float>(currentHealth) / maxHealth;
```

### Exercicio 3 - Controle de fluxo

Crie uma funcao:

```cpp
void PrintHealthState(int health)
```

Ela deve imprimir:

- `Dead` se vida for 0 ou menor
- `Critical` se vida for menor que 25
- `Injured` se vida for menor que 75
- `Healthy` caso contrario

Teste com:

```cpp
PrintHealthState(100);
PrintHealthState(70);
PrintHealthState(10);
PrintHealthState(0);
```

### Exercicio 4 - Passagem por valor

Crie:

```cpp
void TryDamageByValue(int health)
```

Dentro dela, subtraia 10 e imprima.

No `main`, crie `int health = 100`, chame a funcao e imprima `health` depois.

Explique por escrito por que o valor original nao mudou.

### Exercicio 5 - Passagem por referencia

Crie:

```cpp
void DamageByReference(int& health, int damage)
```

Ela deve alterar a vida original.

Garanta que a vida nao fique abaixo de 0.

### Exercicio 6 - Ponteiro e `nullptr`

Crie:

```cpp
void HealIfValid(int* health, int amount)
```

Ela deve:

- verificar se `health != nullptr`
- se for valido, somar `amount`
- se for nulo, imprimir `"Invalid health pointer"`

Teste com:

```cpp
int health = 50;
HealIfValid(&health, 25);
HealIfValid(nullptr, 25);
```

Pergunta:

Por que uma referencia nao seria ideal nesse caso?

Resposta esperada:

Porque a funcao quer aceitar a possibilidade de "nao existe vida valida". Referencia representa melhor um valor obrigatorio; ponteiro pode representar ausencia com `nullptr`.

### Exercicio 7 - Sobrecarga

Crie duas funcoes:

```cpp
int Add(int a, int b)
float Add(float a, float b)
```

Teste com inteiros e floats.

Depois teste:

```cpp
Add(2, 3.5f);
```

Veja o que o compilador faz. Se houver aviso, leia com calma.

### Exercicio 8 - Mini gameplay loop

Simule 5 turnos.

Variaveis:

```cpp
std::string name = "Knight";
int health = 100;
int stamina = 50;
```

A cada turno:

- o personagem toma 12 de dano
- se tiver stamina maior que 0, gasta 8
- imprime status
- se a vida chegar a 0, interrompe o loop

Use pelo menos estas funcoes:

```cpp
void ApplyDamage(int& health, int damage);
void SpendStamina(int& stamina, int amount);
void PrintStatus(const std::string& name, int health, int stamina);
bool IsDead(int health);
```

Esse exercicio e o mais importante da aula. Ele junta tipos, funcoes, fluxo, referencia e `const`.

---

## 19. Checklist de dominio

Antes de seguir para memoria, stack, heap e lifecycle, voce deve conseguir responder:

- O que `#include` faz em alto nivel?
- O que e `main`?
- Qual a diferenca entre declaracao e definicao?
- Por que `int`, `float`, `double`, `char` e `bool` nao sao intercambiaveis?
- O que pode dar errado com `unsigned`?
- Quando usar `static_cast`?
- O que `const` comunica?
- Quando usar `auto` e quando evitar?
- Qual a diferenca entre passar por valor e por referencia?
- Quando um ponteiro pode ser melhor que uma referencia?
- O que e `nullptr`?
- O que e sobrecarga de funcao?
- O que argumentos default fazem?

Se voce travar em referencia e ponteiro, nao avance correndo. Essa e a primeira parede real de C++.

---

## 20. Quando vale ver video

Video ajuda quando o assunto tem movimento visual ou modelo mental dificil de imaginar. Para esta aula, vale procurar videos curtos sobre:

- "C++ compilation process preprocessing compiling linking"
- "C++ pointers and references explained visually"
- "C++ pass by value vs pass by reference"
- "C++ stack vs heap visual explanation"

Nao use video como substituto de escrever codigo. Use video quando voce sentir que esta decorando `*` e `&` sem enxergar memoria, endereco e objeto original.

Para hoje, o video mais util provavelmente e sobre ponteiros e referencias com desenho de memoria. Esse conceito vai voltar na UE5 quando voce lidar com `AActor*`, componentes, objetos nulos e referencias invalidas.

---

## 21. Erros comuns nesta fase

### Erro 1 - Achar que referencia e copia

```cpp
void Foo(int& x)
```

Isso nao recebe copia. Recebe acesso ao original.

### Erro 2 - Usar ponteiro sem verificar nulo

```cpp
void Foo(int* x)
{
    *x = 10;
}
```

Se `x` for `nullptr`, o programa quebra.

Versao mais segura:

```cpp
void Foo(int* x)
{
    if (x != nullptr)
    {
        *x = 10;
    }
}
```

### Erro 3 - Copiar objeto grande sem perceber

```cpp
void PrintName(std::string name)
```

Isso copia a string.

Melhor para apenas leitura:

```cpp
void PrintName(const std::string& name)
```

### Erro 4 - Achar que `auto` elimina a importancia do tipo

`auto` nao torna C++ dinamico. O tipo continua existindo, so foi deduzido pelo compilador.

### Erro 5 - Ignorar avisos do compilador

Warnings sobre conversao, sinal, perda de precisao e variavel nao usada sao aulas gratuitas. Leia.

---

## 22. Resumo mental da aula

C++ e uma linguagem compilada, estatica e muito explicita sobre tipos, custo e acesso a dados.

Nesta fase, voce deve fixar:

- programa C++ comum comeca em `main`
- `#include` traz declaracoes de outros arquivos
- namespaces evitam conflito de nomes
- tipos importam muito
- conversoes podem perder informacao
- `const` protege intencao
- `auto` deduz tipo, mas nao remove tipo
- passagem por valor copia
- passagem por referencia acessa o original
- ponteiro guarda endereco e pode ser nulo
- funcoes devem comunicar claramente se leem, copiam ou modificam dados

Se voce entender isso de verdade, a proxima fase do roadmap fica muito mais forte: memoria, ponteiros, stack, heap, ciclo de vida, RAII e ownership.

---

## 23. Tarefa final da aula

Crie um pequeno programa chamado `character_training.cpp`.

Ele deve ter:

- `std::string name`
- `int health`
- `int stamina`
- `float movementSpeed`
- `bool isAlive`

Funcoes obrigatorias:

```cpp
void PrintStatus(const std::string& name, int health, int stamina, float movementSpeed, bool isAlive);
void ApplyDamage(int& health, int damage);
void HealIfValid(int* health, int amount);
void SpendStamina(int& stamina, int amount);
bool IsAlive(int health);
float GetHealthRatio(int health, int maxHealth);
```

Regras:

- `ApplyDamage` nao deixa vida abaixo de 0.
- `HealIfValid` verifica `nullptr`.
- `SpendStamina` nao deixa stamina abaixo de 0.
- `IsAlive` retorna `health > 0`.
- `GetHealthRatio` deve retornar um `float` entre `0.0f` e `1.0f`.
- `PrintStatus` nao deve copiar o nome.

Depois de terminar, explique por escrito:

1. Quais parametros sao passados por valor e por que?
2. Quais parametros sao passados por referencia e por que?
3. Qual parametro usa ponteiro e por que?
4. Onde voce precisou usar `static_cast`?
5. Onde `const` protegeu seu codigo?

Se voce conseguir responder essas cinco perguntas com clareza, voce nao apenas escreveu C++; voce entendeu a primeira camada do modelo mental de C++.
