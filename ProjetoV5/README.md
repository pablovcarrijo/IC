# 1. Leitura do arquivo de entrada (load_map_from_txt, get_map_size, get_map_cyclos_robot)

O programa começa lendo o arquivo de entrada.
As duas primeiras informações do arquivo são:
* número de ciclos da simulação
* quantidade de robôs

Depois vem o mapa, onde cada célula pode representar:
* espaço livre
* parede

Esse mapa é carregado para cada robô.
Funções auxiliares são utilizadas:
* get_map_size: Pega a quantidade de linhas e colunas do mapa .txt
* get_map_cyclos_robots: Pega a quantidade de ciclos e de robôs no .txt

# 2. Inicialização dos robôs (init_robots)

Para cada robô:
* é criado o mapa interno do robô
* é criada a matriz de feromônio
* é criada uma fila de memória do robô
* o mapa é carregado do arquivo
* o robô é colocado em uma posição aleatória válida
* A fila de memória armazena as células por onde o robô já passou, juntamente com o valor de feromônio presente naquele momento.

Essa memória possui tamanho limitado, funcionando como uma fila circular, onde as posições mais antigas são removidas conforme novas posições são adicionadas.

# 3. Loop principal da simulação

A simulação roda por um número definido de ciclos.
Em cada ciclo acontece:
* o mapa de feromônio é exibido na tela
* cada robô decide para onde se mover
* os robôs depositam feromônio ao redor
* a posição visitada é armazenada na fila de memória do robô
* ocorre a evaporação do feromônio

# 4. Movimento do robô (robot_move)

O robô analisa as células vizinhas dentro de um raio de detecção configurável.

Antes o robô observava apenas as 8 células vizinhas (raio = 1).
Agora o sistema permite definir um raio maior, possibilitando que o robô analise áreas maiores do ambiente.

Exemplo:

* Raio 1:

    x x x
    x R x
    x x x

* Raio 2:

    x x x x x
    x x x x x
    x x R x x
    x x x x x
    x x x x x

Para cada célula detectada:
* verifica se a célula não é uma parede
* calcula a quantidade de feromônio presente
* atualiza o valor de feromônio ao redor do robô
* calcula um peso para a escolha da próxima célula
Depois disso:
* os pesos são invertidos (preferindo células com menos feromônio)
* os pesos são normalizados
* um número aleatório é gerado
* a nova célula é escolhida probabilisticamente
Isso permite que o robô tenha maior probabilidade de explorar áreas menos visitadas.

# 5. Atualização do mapa

Quando o robô se move:
* a posição antiga vira visitada
* a nova posição vira posição do robô
Além disso, a nova posição é armazenada na fila de memória do robô, registrando:
* posição da célula
* valor de feromônio naquele momento
Essa memória representa o histórico recente de movimentação do robô.

# 6. Memória de trajetória do robô (CellQueue)

Cada robô possui uma fila circular de memória que armazena as últimas células visitadas.
Cada elemento da fila contém:
* posição da célula
* valor de feromônio na célula no momento da visita
Essa memória representa apenas o caminho percorrido pelo robô, não incluindo o depósito de feromônio ao redor.
Quando a fila atinge sua capacidade máxima:
* a célula mais antiga é removida
* a nova célula visitada é adicionada
Isso permite manter apenas um histórico recente de movimentação.

# 7. Evaporação do feromônio (pheromone_robot_evaporate)

Depois que todos os robôs se movem, ocorre a evaporação:
* Cada célula perde uma porcentagem do feromônio atual.

# 8. Visualização (view_phremone_robot_map)

O mapa é exibido no terminal utilizando cores.
* A intensidade da cor verde representa a quantidade de feromônio na célula.
* verde escuro → pouco feromônio
* verde forte → muito feromônio
* vermelho → parede

# Onde eu parei

O sistema atualmente já faz:

Leitura do mapa
Criação dos robôs
Movimento probabilístico
Depósito de feromônio
Evaporação
Visualização do mapa
Fila de memória da trajetória do robô
Sistema de raio de detecção variável