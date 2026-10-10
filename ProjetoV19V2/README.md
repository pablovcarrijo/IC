Compile dentro da pasta `Code`:

```powershell
.\raylib\w64devkit\bin\gcc.exe main.c Map\LoadMap.c Map\Render.c Robot\Robot.c Communication\Communication.c Tests\Tests.c -o program.exe -I.\raylib\raylib\src -L.\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm
```

Execute dentro da pasta `Code`:

```powershell
.\program.exe
```

A janela abre e acompanha os experimentos em sequencia. Quando termina um,
grava sua linha no TXT e inicia o proximo automaticamente. Depois do ultimo,
a janela permanece aberta para consultar os mapas.

A `main` mostra o fluxo da aplicacao: carregar o mapa, abrir a janela, repetir
os experimentos, movimentar/comunicar os robos, desenhar e liberar a memoria.
Os calculos das porcentagens e a funcao que grava cada resultado ficam em
`Tests/Tests.c`, com suas declaracoes em `Tests/Tests.h`.

Configure em `Tests/Tests.h` e recompile:

```c
#define COLLECTIVE_MAPPING_INTERVAL 50
#define EXPERIMENT_COUNT 105
#define EXPERIMENT_INPUT_FILE "Config/entradas.txt"
#define EXPERIMENT_RESULTS_FILE "resultados_experimentos.txt"
```

O limite de ciclos e a quantidade de robos continuam nas duas primeiras linhas
de `Config/entradas.txt`. Entrada e resultados sao relativos a pasta de onde
voce executa o programa. Por isso, execute em `Code`. A aplicacao nao muda a
pasta de trabalho. Para usar outra entrada, passe o caminho como primeiro argumento.

Exemplo de uma linha (valores ilustrativos):

```text
experimento=1;semente=1234;ciclo_0=12.500000%;ciclo_50=60.000000%;ciclo_100=95.000000%;ciclo_125=100.000000%;ciclo_100pct=117
```

A cobertura coletiva conta cada parede conhecida por pelo menos um robo uma vez.
As amostras incluem o inicio, os intervalos configurados e o ultimo ciclo.
O primeiro ciclo com 100% e verificado em TODO ciclo pela contagem de paredes.
Se nao completar, registra `nao_atingiu`. Se nao houver paredes, registra
porcentagens `N/A` e `sem_paredes` na conclusao.

Os robos, os feromonios, os mapas e a comunicacao sao reinicializados a cada
experimento. Cada repeticao usa uma semente diferente e libera sua memoria.
O TXT recebe novas linhas sem apagar as anteriores. A numeracao comeca em 1
a cada execucao. Fechar durante um experimento cancela as repeticoes restantes;
a linha incompleta nao e gravada, e as linhas ja concluidas permanecem no arquivo.

Controles:
- `A`: pausar/continuar.
- `Y`: mostrar o ultimo mapa coletivo salvo/voltar.
- Espaco: alternar os mapas individuais.
- Setas: selecionar robo.
- Esc ou fechar a janela: sair.

A copia coletiva e atualizada a cada intervalo e no ultimo ciclo. O painel
identifica o ciclo salvo. Na visualizacao coletiva, vermelho representa paredes
conhecidas e escuro representa posicoes sem informacao. Consultar o coletivo
nao muda o conhecimento dos robos nem pausa a simulacao.
