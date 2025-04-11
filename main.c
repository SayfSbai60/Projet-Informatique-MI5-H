#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LIGNE 256

typedef struct {
    char nom[50];
    int pv;
    int attaque;
    int defense;
    int vitesse;
} Combattant;

Combattant *charger_combattants(unsigned int *nb_combattants) {
    FILE *fichier = fopen("combattants.txt", "r");
    if (!fichier) {
        printf("Erreur: Fichier 'combattants.txt' introuvable\n");
        exit(1);
    }

    Combattant *liste = NULL;
    *nb_combattants = 0;
    char ligne[MAX_LIGNE];

    while (fgets(ligne, sizeof(ligne), fichier)) {
        
        Combattant *temp = realloc(liste, (*nb_combattants + 1) * sizeof(Combattant));
        if (!temp) {
            printf("Erreur mémoire!\n");
            free(liste);
            fclose(fichier);
            exit(1);
        }
        liste = temp;

        sscanf(
            ligne, "%[^;];%d;%d;%d;%d",
            liste[*nb_combattants].nom,
            &liste[*nb_combattants].pv,
            &liste[*nb_combattants].attaque,
            &liste[*nb_combattants].defense,
            &liste[*nb_combattants].vitesse
        );
        (*nb_combattants)++;
    }

    fclose(fichier);
    return liste;
}

void afficher_combattants(Combattant *liste, int nb) {
    printf("\n=== LISTE DES COMBATTANTS (%d) ===\n", nb);
    for (int i = 0; i < nb; i++) {
        printf("%d. %s (PV: %d, Att: %d, Def: %d, Vit: %d)\n",
               i + 1,
               liste[i].nom,
               liste[i].pv,
               liste[i].attaque,
               liste[i].defense,
               liste[i].vitesse);
    }
}

void PvP() {
    int nb_combattants = 0;
    Combattant *liste = charger_combattants(&nb_combattants);
    afficher_combattants(liste, nb_combattants);
    
    
    free(liste);
}

void choix_game() {
    int choix;
    do {
        printf("\n=== MENU JEU ===\n");
        printf("1. PvP\n");
        printf("2. PvE\n");
        printf("3. Retour\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                PvP();
                break;
            case 2:
                printf("Mode PvE non implémenté.\n");
                break;
            case 3:
                return; 
            default:
                printf("Choix invalide.\n");
        }
    } while (choix < 1 || choix > 3);
}

void menu_principal() {
    int choix;
    do {
        printf("\n=== MENU PRINCIPALE ===\n");
        printf("1. Nouvelle partie\n");
        printf("2. Quitter\n");
        printf("Choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                choix_game();
                break;
            case 2:
                printf("Au revoir !\n");
                exit(0);
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 2);
}

int main() {
    menu_principal();
    return 0;
}
