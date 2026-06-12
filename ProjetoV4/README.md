# Como o programa funciona

## 1. Leitura do arquivo de entrada (load_map_from_txt, get_map_size, get_map_cyclos_robot)

O programa começa lendo o arquivo de entrada.

As duas primeiras informações do arquivo são:

* número de ciclos da simulação
* quantidade de robôs

Depois vem o mapa, onde cada célula pode representar: 
* espaço livre
* parede

Esse mapa é carregado para cada robô.

Funções auxiliares são utilziadas:
* get_map_size: Pega a quantidade de linhas e colunas do mapa .txt
* get_map_cyclos_robots: Pega a quantidade de ciclos e de robos no .txt

---

## 2. Inicialização dos robôs (init_robots)

Para cada robô:

1. é criado o mapa interno do robô
2. é criada a matriz de feromônio
3. o mapa é carregado do arquivo
4. o robô é colocado em **uma posição aleatória válida**

---

## 3. Loop principal da simulação 

A simulação roda por um número definido de ciclos.

Em cada ciclo acontece:

1. o mapa de feromônio é exibido na tela
2. cada robô decide para onde se mover
3. os robôs depositam feromônio ao redor
4. ocorre a **evaporação do feromônio**

---

## 4. Movimento do robô (robot_move)

O robô analisa as **8 células vizinhas**.

Para cada vizinho:

* calcula a quantidade de feromônio
* atualiza o feromônio local
* calcula um peso para a escolha da próxima célula

Depois disso:

1. os pesos são normalizados
2. um número aleatório é gerado
3. a nova célula é escolhida probabilisticamente

A ideia é que o robô tenha **maior probabilidade de ir para lugares com menos feromônio**.

---

## 5. Atualização do mapa

Quando o robô se move:

* a posição antiga vira **visitada**
* a nova posição vira **posição do robô**

---

## 6. Evaporação do feromônio (pheromone_robot_evaporate)

Depois que todos os robôs se movem, ocorre a evaporação:

Cada célula perde uma porcentagem do feromônio atual.

Isso evita que o mapa fique saturado e permite novas explorações.

---

# Visualização (view_phremone_robot_map)

O mapa é exibido no terminal usando cores.

A intensidade da cor verde representa **quantidade de feromônio na célula**.

* verde escuro → pouco feromônio
* verde forte → muito feromônio
* vermelho → parede

A tela é atualizada a cada ciclo.

---

# Onde eu parei

O sistema atualmente já faz:

Leitura do mapa
Criação dos robôs
Movimento probabilístico
Depósito de feromônio
Evaporação
Visualização do mapa

-> TUDO DESCENTRALIZADO AGORA

---

