#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void init_grade(int **grade, int tamanhoGrade){
    *grade = (int*)malloc(tamanhoGrade * sizeof(int));
    for(int i = 0; i < tamanhoGrade; i++){
        (*grade)[i] = 0;
    }
    (*grade)[(tamanhoGrade/2)] = 1;
}

void automatusCelulares(int *regra, int tamanhoGrade, int *grade){

    int atual, anterior, posterior;
    int *aux = (int *)malloc(tamanhoGrade * sizeof(int));

    for(int i = 0; i < 100; i++){

        anterior = (i == 0) ? tamanhoGrade - 1: i - 1;
        posterior = (i+1 == tamanhoGrade) ? 0 : i + 1;
        atual = i;

        if(grade[anterior] == 1 && grade[atual] == 1 && grade[posterior] == 1){
            aux[atual] = regra[0];
        }
        else if(grade[anterior] == 1 && grade[atual] == 1 && grade[posterior] == 0){
            aux[atual] = regra[1];
        }
        else if(grade[anterior] == 1 && grade[atual] == 0 && grade[posterior] == 1){
            aux[atual] = regra[2];
        }
        else if(grade[anterior] == 1 && grade[atual] == 0 && grade[posterior] == 0){
            aux[atual] = regra[3];
        }
        else if(grade[anterior] == 0 && grade[atual] == 1 && grade[posterior] == 1){
            aux[atual] = regra[4];
        }
        else if(grade[anterior] == 0 && grade[atual] == 1 && grade[posterior] == 0){
            aux[atual] = regra[5];
        }
        else if(grade[anterior] == 0 && grade[atual] == 0 && grade[posterior] == 1){
            aux[atual] = regra[6];
        }
        else if(grade[anterior] == 0 && grade[atual] == 0 && grade[posterior] == 0){
            aux[atual] = regra[7];
        }
    }
    for(int i = 0; i < tamanhoGrade; i++){
        grade[i] = aux[i];
    }

    free(aux);
}

void imprimir(int *grade, int tamanhoGrade){
    for(int i = 0; i < tamanhoGrade; i++){
        if(grade[i] == 1){
            printf("\033[32m%c\033[m", 223);
        }
        else if(grade[i] == 0){
            printf("%c", 255);
        }
    }
    printf("\n");
}

int main(){

    FILE *p;
    p = fopen("Entrada.txt", "r");

    int tamanhoGrade;
    int regra[8];
    int repeticoes;

    fscanf(p, "%d%*c", &tamanhoGrade);
    for(int i = 0; i < 8; i++){
        fscanf(p, "%d%*c", &regra[i]);
    }
    fscanf(p, "%d", &repeticoes);

    fclose(p);

    int *grade;
    init_grade(&grade, tamanhoGrade);
    for(int i = 0; i < repeticoes; i++){
        imprimir(grade, tamanhoGrade);
        automatusCelulares(regra, tamanhoGrade, grade);
        Sleep(100);
    }
    imprimir(grade, tamanhoGrade);

    free(grade);
}