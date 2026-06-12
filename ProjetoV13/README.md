# Simulação de robôs com feromônio e comunicação

Este projeto simula robôs explorando um mapa em grade. Cada robô possui seu próprio mapa interno, começa sem conhecer todo o ambiente e vai descobrindo novas células conforme se movimenta.

A movimentação usa uma lógica baseada em feromônio: os robôs tendem a escolher regiões com menos feromônio, aumentando a chance de explorar áreas ainda pouco visitadas.

Além disso, os robôs podem trocar informações quando estão próximos, deixando a simulação mais descentralizada.

---

## Arquivos principais

- `main.c`: controla a execução da simulação.
- `Project.h`: guarda as structs e constantes principais do projeto.
- `loadMap.c` / `loadMap.h`: fazem a leitura do arquivo de entrada.
- `Robot.c` / `Robot.h`: cuidam da criação, posicionamento e movimento dos robôs.
- `Communication.c` / `Communication.h`: controlam a troca de informações entre robôs.
- `MapView.c` / `MapView.h`: mostram o mapa no terminal.
- `entradas.txt`: arquivo com os dados da simulação e o mapa.

---

## Formato do arquivo de entrada

O arquivo `entradas.txt` começa com duas informações:

```txt
quantidade_de_ciclos
quantidade_de_robos
```

Depois dessas duas linhas, vem o mapa.

No mapa:

- `0` representa caminho livre.
- `2` representa parede ou obstáculo.

Exemplo simples:

```txt
4000
3
222222
200002
200002
222222
```

Nesse exemplo, a simulação roda por `4000` ciclos e usa `3` robôs.

---

## Como o programa funciona

### 1. Leitura do mapa

O programa começa lendo o arquivo `entradas.txt`.

Primeiro, ele pega:

- a quantidade de ciclos da simulação;
- a quantidade de robôs.

Depois, ele calcula o tamanho do mapa e carrega os valores em uma matriz chamada `realMap`.

As funções usadas nessa parte são:

- `get_map_cyclos_robots`: lê a quantidade de ciclos e robôs;
- `get_map_size`: descobre quantas linhas e colunas o mapa possui;
- `load_map_from_txt`: carrega o mapa real para a memória.

---

### 2. Inicialização dos robôs

Cada robô é criado pela função `init_robots`.

Nessa etapa, para cada robô:

- é criado um mapa interno próprio;
- é criada uma matriz de feromônio;
- todas as células começam como desconhecidas;
- o robô recebe um identificador;
- o robô é colocado em uma posição aleatória válida.

A posição inicial não pode ser parede e também não pode estar ocupada por outro robô.

Quando o robô é colocado no mapa, ele já descobre uma região ao redor da posição inicial, de acordo com o valor de `MOVE_RADIUS`.

---

### 3. Loop principal da simulação

A simulação roda dentro de um laço que se repete pela quantidade de ciclos definida no arquivo de entrada.

Em cada ciclo acontece basicamente isto:

1. o mapa é exibido no terminal;
2. cada robô escolhe para onde vai se mover;
3. o robô deposita feromônio nas células analisadas;
4. o robô atualiza sua informação no sistema de comunicação;
5. os robôs próximos podem ler informações uns dos outros;
6. o feromônio evapora um pouco.

Ou seja, cada robô explora, atualiza seu próprio mapa e pode compartilhar parte do que sabe com outros robôs.

---

## Movimento dos robôs

A movimentação é feita pela função `robot_move`.

O robô analisa uma região ao redor da sua posição atual. Como `MOVE_RADIUS` está definido como `2`, a área analisada pode chegar a uma região de `5 x 5`, excluindo a própria posição do robô.

Por isso, o máximo de posições analisadas é:

```txt
(2 * MOVE_RADIUS + 1)² - 1 = 24 posições
```

Para cada posição possível, o robô verifica:

- se a posição está dentro dos limites do mapa;
- se não é parede;
- se existe caminho livre até ela;
- qual é o valor de feromônio naquela célula.

Depois disso, o robô calcula pesos para escolher a próxima direção.

A ideia principal é:

- célula com mais feromônio → menor chance de escolha;
- célula com menos feromônio → maior chance de escolha.

Assim, o robô tende a procurar regiões menos exploradas.

Mesmo escolhendo uma célula dentro do raio, o robô não pula diretamente para ela. Ele dá apenas um passo na direção da célula escolhida.

---

## Feromônio

O feromônio é usado para marcar regiões já exploradas.

Durante o movimento, o robô deposita feromônio nas células analisadas e também na célula onde ele está.

Depois que todos os robôs se movimentam, a função `pheromone_robot_evaporate` reduz um pouco o feromônio de todas as células.

Isso evita que o mapa fique saturado e permite que regiões antigas possam voltar a ser consideradas interessantes depois de um tempo.

---

## Comunicação entre robôs

A comunicação é feita usando uma estrutura chamada `Pool`.

Cada robô escreve no `Pool` informações sobre sua posição, seu mapa conhecido e os valores de feromônio ao redor.

Essa parte acontece com a função:

```c
com_WriteOnPool(...)
```

Depois, cada robô tenta ler informações dos outros robôs usando:

```c
com_ReadPool(...)
```

Mas a leitura só acontece se o outro robô estiver dentro do raio de comunicação.

Esse raio é definido pela constante:

```c
DETECTION_COMUNICATION_RADIUS
```

Quando a comunicação acontece, o robô atualiza seu mapa interno e sua matriz de feromônio com informações recebidas de outro robô.

Isso faz com que os robôs não dependam de um mapa central pronto. Cada um vai construindo seu próprio conhecimento e compartilhando informações quando encontra outros robôs próximos.

---

## Visualização no terminal

A visualização é feita pelas funções de `MapView.c`.

No modo de visualização do mapa real do robô:

- azul representa a posição atual do robô;
- vermelho representa parede;
- verde representa célula conhecida, livre ou visitada;
- preto representa célula ainda desconhecida.

Também existe a função `view_pheromone_robot_map`, que mostra o mapa de feromônio.

Nesse modo, a intensidade do verde indica a quantidade de feromônio:

- verde mais escuro: pouco feromônio;
- verde mais forte: muito feromônio.

---

## Constantes importantes

As principais constantes ficam em `Project.h`.

| Constante | Significado |
|---|---|
| `CELL_UNKNOWN` | célula ainda desconhecida pelo robô |
| `CELL_FREE` | célula livre |
| `CELL_VISITED` | célula já visitada |
| `CELL_WALL` | parede ou obstáculo |
| `CELL_ROBOT` | posição atual do robô |
| `MOVE_RADIUS` | raio usado na escolha de movimento |
| `DETECTION_PHEROMONE_RADIUS` | raio usado para analisar/registrar feromônio |
| `DETECTION_COMUNICATION_RADIUS` | raio máximo para comunicação entre robôs |
| `MAX_NEIGHBORS` | quantidade máxima de células vizinhas analisadas |
| `QUEUE_MAX` | tamanho máximo da fila de memória usada na comunicação |

---

## Como compilar

Considerando que os arquivos estejam na mesma pasta, é possível compilar com GCC/MinGW usando:

```bash
gcc main.c Robot.c loadMap.c MapView.c Communication.c -o program.exe -lm
```

Depois, execute:

```bash
./program.exe
```

No Windows, também pode ser:

```bash
program.exe
```

Atenção: no código atual, o `main.c` procura o arquivo de entrada em:

```c
../entradas.txt
```

Então o arquivo `entradas.txt` precisa estar no lugar certo, ou esse caminho deve ser ajustado no código.

---

## O que o projeto já faz

Atualmente, o sistema já possui:

- leitura do mapa por arquivo `.txt`;
- criação de vários robôs;
- mapa interno individual para cada robô;
- movimentação probabilística baseada em feromônio;
- depósito e evaporação de feromônio;
- comunicação descentralizada entre robôs próximos;
- atualização do mapa interno com informações recebidas;
- visualização no terminal.

---

## Possíveis melhorias futuras

Algumas melhorias que ainda podem ser feitas:

- liberar a memória alocada com `free` no final do programa;
- tratar melhor erros de alocação de memória;
- melhorar a organização dos caminhos dos arquivos;
- permitir escolher qual mapa visualizar durante a execução;
- salvar os resultados da simulação em arquivo;
- calcular métricas de exploração, como porcentagem do mapa descoberto.
