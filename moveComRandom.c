#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

typedef struct{
    int **gradeFront;
    double **gradeBack;
} Grade;

typedef struct{
    int i;
    int j;
} Posicao;

void init_grade(int linhas, int colunas, Grade *grade){
    
    (*grade).gradeFront = (int**)malloc(linhas * sizeof(int*));
    (*grade).gradeBack = (double**)malloc(linhas * sizeof(double*));
    
    for(int i = 0; i < linhas; i++){
        (*grade).gradeFront[i] = (int*)malloc(colunas * sizeof(int));
        (*grade).gradeBack[i] = (double*)malloc(colunas * sizeof(double));
    }

    for(int i = 0; i < linhas; i++){
        for(int j = 0; j < colunas; j++){
            (*grade).gradeFront[i][j] = 0;
            (*grade).gradeBack[i][j] = 0.0;
        }
    }

    (*grade).gradeFront[linhas/2][colunas/2] = 2;
}

void localizaPosicao(int linhas, int colunas, Grade grade, Posicao *pos){
    for(int i = 0; i < linhas; i++){
        for(int j = 0; j < colunas; j++){
            if(grade.gradeFront[i][j] == 2){
                pos->i = i;
                pos->j = j;
                break;
            }
        }
    }
}

void logica(int linhas, int colunas, Grade *grade, Posicao *pos){

    int vizinhos_i[8], vizinhos_j[8], vizinho_i, vizinho_j, n = 0;
    double vectPeso[8], sum = 0.0, contribuicao = 0.5, acumulado = 0.0, randomNumber;
    int novo_i, novo_j, i, j;

    for(i = -1; i <= 1; i++){
        for(j = -1; j <= 1; j++){
            if(j == 0 && i == 0) continue;

            vizinho_i = pos->i + i;
            vizinho_j = pos->j + j;

            if(vizinho_i >= linhas) vizinho_i = 0;
            if(vizinho_i < 0) vizinho_i = linhas - 1;
            if(vizinho_j >= colunas) vizinho_j = 0;
            if(vizinho_j < 0) vizinho_j = colunas - 1;
            
            (*grade).gradeBack[vizinho_i][vizinho_j] += ((1.0 - (*grade).gradeBack[vizinho_i][vizinho_j]) * contribuicao);

            vizinhos_i[n] = vizinho_i;
            vizinhos_j[n] = vizinho_j;
            vectPeso[n] = (*grade).gradeBack[vizinho_i][vizinho_j];
            sum += vectPeso[n++];

        }
    }

    for(i = 0; i < 8; i++){
        vectPeso[i] = vectPeso[i] / sum;
    }
    
    randomNumber = ((double)rand()) / RAND_MAX;
    for(i = 0; i < 8; i++){
        acumulado += vectPeso[i];
        if(acumulado >= randomNumber){
            break;
        }
    }
    i = (i < 8) ? i : i - 1;
    novo_i = vizinhos_i[i];
    novo_j = vizinhos_j[i];

    (*grade).gradeFront[pos->i][pos->j] = 1;
    (*grade).gradeFront[novo_i][novo_j] = 2;
    pos->i = novo_i;
    pos->j = novo_j;
}

void printGrade(int linhas, int colunas, Grade grade, Posicao pos){
    system("cls");
    for(int i = 0; i < linhas; i++){
        for(int j = 0; j < colunas; j++){
            int distancia = abs(pos.i - i) + abs(pos.j - j);

            double intensidadeVerde = (distancia / (double)(linhas + colunas));
            
            if(grade.gradeFront[i][j] == 2){
                printf(" ");
            }
            else if(intensidadeVerde > 0.0){
                int verde_intensidade = (int)(intensidadeVerde * 255);  
                printf("\033[48;2;0;%d;0m \033[m", verde_intensidade);  
            }
        }
        printf("\n");
    }
    Sleep(200);
}


void decrementa(int linhas, int colunas, Grade *grade){
    for(int i = 0; i < linhas; i++){
        for(int j = 0; j < colunas; j++){
           (*grade).gradeBack[i][j] -= 0.2;
        }
    }
}

int main(){

    Grade grade;
    Posicao pos;
    int linhas, colunas, ciclos;
    printf("Digite a quantidade de linhas: ");
    scanf("%d%*c", &linhas);
    printf("Digite a quantidade de colunas: ");
    scanf("%d%*c", &colunas);
    printf("Ciclos: ");
    scanf("%d%*c", &ciclos);

    init_grade(linhas, colunas, &grade);
    localizaPosicao(linhas, colunas, grade, &pos);
    for(int i = 0; i < ciclos; i++){
        printGrade(linhas, colunas, grade, pos);
        logica(linhas, colunas, &grade, &pos);
        decrementa(linhas, colunas, &grade);
    }

    return 0;
}