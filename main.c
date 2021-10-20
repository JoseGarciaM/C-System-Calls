//
//  main.c
// 
//
//  Created by Jose Carlos Garcia Mendizabal on 10/09/21.
//

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <uuid/uuid.h>

#define UUID_SIZE 37 // Tamaño del identificador de la urna
#define FILE_NAME_SIZE 1000 // Tamaño del nombre del archivo
#define MAX_URNS 10000 // Tamaño del nombre del archivo
#define RANGE_HELPER_SIZE 50 // Tamaño del rango en caracteres para alocar dinamicament
#define MAX_HISTOGRAM_ASTERISCS 10000 // Tamaño del nombre del archivo

typedef struct Urn {
    char *ID;
    int numberOfFiles;
    char *range;
    int rangeStart;
    int rangeEnd;
    char *histogram;
} Urn;

char *generateUUID(void); // Genera UUID
void traverse(Urn **Urns, char *directoryName, int urnSize, int *urnCounter, int *maxUrns); // Desciende por el arbol de archivos registrando los tamaños de todos los archivos que encuentre
void generateHistogram(Urn *Urns, int maxUrns); // Genera histograma dadas las caracteristicas de cada Urna
void printUrns(Urn *Urns, int *urnCounter); // imprimir un histograma de los tamaños de archivo, utilizando una anchura de urna especificada como parámetro
void freeUrns(Urn *Urns); // libera memoria usada por las urnas

int main(int argc, const char * argv[]) {
    char *directoryName;
    int urnSize;
    int urnCounter = 0;
    int maxUrns = MAX_URNS;
    directoryName = (char*)malloc(FILE_NAME_SIZE * sizeof(char));
    printf("===========================\n");
    printf(" (KYH) Know Your Histogram \n");
    printf("===========================\n\n");
    printf("Let's traverse!\n\n");
    printf("\nDirectory path: ");
    scanf(" %[^\n]", directoryName);
    do {
        printf("Urn size: ");
        scanf("%d", &urnSize);
    } while (urnSize <= 0);
    if (urnSize > 0) {
        Urn *Urns = (Urn*)malloc(MAX_URNS * sizeof(Urn));
        traverse(&Urns, directoryName, urnSize, &urnCounter, &maxUrns);
        generateHistogram(Urns, maxUrns);
        printUrns(Urns, &urnCounter);
        freeUrns(Urns);
    }
    free(directoryName);
    return 0;
}

char *generateUUID(void) {
    uuid_t binUUID;
    uuid_generate_random(binUUID);
    char *UUID = (char*)malloc(UUID_SIZE * sizeof(char));
    uuid_unparse(binUUID, UUID);
    return UUID;
}

void traverse(Urn **Urns, char *directoryName, int urnSize, int *urnCounter, int *maxUrns) {
    char *path = (char *) malloc(sizeof(char) * FILE_NAME_SIZE);
    struct dirent *directoryPtr;
    struct stat buffer;
    off_t size;
    int rangeStart;
    int rangeEnd;
    char *rangeStartString;
    char *rangeEndString;
    Urn *currentUrn;
    DIR *directory = opendir(directoryName);
    if (!directory) {
        stat(directoryName, &buffer);
        size = buffer.st_size;
        rangeStart = ceil(size / urnSize) * urnSize;
        rangeEnd = rangeStart + urnSize - 1;
        currentUrn = *Urns - 1;
        for (; currentUrn < *Urns + *urnCounter; currentUrn++) {
            if (currentUrn->range == NULL) {
                currentUrn->ID = generateUUID();
                currentUrn->numberOfFiles = 1;
                currentUrn->rangeStart = rangeStart;
                currentUrn->rangeEnd = rangeEnd;
                currentUrn->range = (char *) malloc(sizeof(char) * RANGE_HELPER_SIZE);
                rangeStartString = (char *) malloc(sizeof(char) * RANGE_HELPER_SIZE);
                rangeEndString = (char *) malloc(sizeof(char) * RANGE_HELPER_SIZE);
                sprintf(rangeStartString, "%d-", rangeStart);
                sprintf(rangeEndString, "%d", rangeEnd);
                strcpy(currentUrn->range, strcat(rangeStartString, rangeEndString));
                free(rangeStartString);
                free(rangeEndString);
                break;
            } else if (currentUrn->rangeEnd == rangeEnd) {
                (currentUrn->numberOfFiles)++;
                *maxUrns = fmax((double) *maxUrns, (double) currentUrn->numberOfFiles);
                break;
            }
        }
        (*urnCounter)++;
        free(path);
        return;
    }
    while ((directoryPtr = readdir(directory)) != NULL) {
        if (strcmp(directoryPtr->d_name, ".") != 0 && strcmp(directoryPtr->d_name, "..") != 0) {
            strcpy(path, directoryName);
            strcat(path, "/");
            strcat(path, directoryPtr->d_name);
            traverse(Urns, path, urnSize, urnCounter, maxUrns);
        }
    }
    closedir(directory);
    free(path);
}

void generateHistogram(Urn *Urns, int maxUrns) {
    Urn *currentUrn = Urns - 1;
    int asteriscsCounter = 0;
    char *histogram;
    for (; currentUrn->rangeEnd != 0; currentUrn++) {
        asteriscsCounter = currentUrn->numberOfFiles * MAX_HISTOGRAM_ASTERISCS / maxUrns;
        currentUrn->histogram = (char *) malloc(sizeof(char) * asteriscsCounter);
        strcpy(currentUrn->histogram, "*");
        histogram = currentUrn->histogram + 1;
        for (; histogram < currentUrn->histogram + asteriscsCounter; histogram++) {
            strcpy(histogram, "*");
        }
    }
    free(currentUrn);
}

void printUrns(Urn *Urns, int *urnCounter) {
    Urn *currentUrn = Urns - 1;
    printf("\n%20s%10s%35s\n", "Urn", "Number of files", "Histogram");
    for (int i = 0; currentUrn->rangeEnd != 0; i++) {
        printf("%20s%10d%35s\n", currentUrn->range, currentUrn->numberOfFiles, currentUrn->histogram);
        currentUrn++;
    }
    free(currentUrn);
}

void freeUrns(Urn *Urns) {
    Urn *currentUrn = Urns - 1;
    for (; currentUrn->rangeEnd != 0; currentUrn++) {
        currentUrn->numberOfFiles = 0;
        currentUrn->rangeEnd = 0;
        free(currentUrn->ID);
        free(currentUrn->range);
        free(currentUrn->histogram);
    }
}
