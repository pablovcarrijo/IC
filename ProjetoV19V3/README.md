Execute dentro de `Code` para compilar:

```powershell
.\raylib\w64devkit\bin\gcc.exe main.c Map\LoadMap.c Map\Render.c Robot\Robot.c Communication\Communication.c Tests\Tests.c -o program.exe -I.\raylib\raylib\src -L.\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm
```

O executavel procura `Config/entradas.txt` dentro da sua propria pasta, mesmo se
for iniciado a partir de outra pasta. Tambem e possivel passar outro arquivo como
primeiro argumento; nesse caso, caminhos relativos usam a pasta de onde o comando
foi executado.

`Tests/Tests.h` e `Tests/Tests.c` calculam as paredes conhecidas por cada robo
e a media aritmetica das porcentagens. O total inclui todas as celulas de parede
do mapa real, inclusive bordas. Cada posicao e contada uma vez por robo, incluindo
paredes recebidas por comunicacao. Os valores sao atualizados no inicio e apos
cada ciclo. Sem paredes ou sem robos, o painel mostra que a porcentagem nao se aplica.

O painel tambem mostra a cobertura coletiva: cada parede conhecida por pelo menos
um robo conta uma unica vez. Essa uniao complementa os mapas para calcular a metrica,
sem alterar os mapas individuais. A medicao ocorre no inicio, a cada 50 ciclos e
no ultimo ciclo, mesmo que ele nao seja multiplo do intervalo. Entre medicoes, o
painel mantem o ultimo resultado e informa o ciclo da medicao.
Para mudar o intervalo, altere `COLLECTIVE_MAPPING_INTERVAL` em `Code/Tests/Tests.h`
para um inteiro maior que zero e recompile. Um passo desta medicao equivale a um
ciclo da simulacao, em que todos os robos tem uma oportunidade de se movimentar.

Pressione `Y` para visualizar o mapa coletivo da ultima medicao salva em memoria.
As paredes conhecidas aparecem em vermelho e o restante permanece escuro (sem
informacao). O painel identifica o ciclo salvo. Essa copia e atualizada junto
com a porcentagem coletiva; descobertas posteriores so aparecem na proxima medicao.
`Y` novamente retorna ao modo anterior, e espaco sai do coletivo e alterna o modo
normal. A visualizacao coletiva tambem funciona apos o termino dos ciclos.

Pressione `A` para pausar ou continuar a simulacao. Durante a pausa, ciclos,
movimentacao, animacao, comunicacao, evaporacao e medicoes ficam parados; os
controles de visualizacao continuam funcionando. O painel mostra `PAUSADO`.
Ao continuar, a simulacao retoma do ponto em que parou. A tecla nao reinicia
uma simulacao que ja terminou.

Ao atingir o limite de ciclos, a simulacao para (incluindo a evaporacao), mas a
janela permanece aberta para consultar os resultados. Use as setas para selecionar
o robo, espaco para alternar os mapas e Esc ou o botao de fechar para sair.
