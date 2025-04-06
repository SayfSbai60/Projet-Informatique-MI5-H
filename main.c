#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMBATTANTS 20
#define MAX_LIGNE 256

typedef struct {
    char nom[50];
    int pv;
    int attaque;
    int defense;
    int vitesse;
} Combattant;

void charger_combattants(Combattant *liste, int *nb_combattants) {
    FILE *fichier = fopen("combattants.txt", "r");
    if (!fichier) {
        printf("Erreur: Fichier 'combattants.txt' introuvable!\n");
        exit(1);
    }

    char ligne[MAX_LIGNE];
    *nb_combattants = 0;

    while (fgets(ligne, sizeof(ligne), fichier) && *nb_combattants < MAX_COMBATTANTS) {
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
}

void afficher_combattants(Combattant *liste, int nb) {
    printf("\n=== LISTE DES COMBATTANTS ===\n");
    for (int i = 0; i < nb; i++) {
        printf("%d. %s (PV: %d, Att: %d, Def: %d, Vit: %d)\n",
               i+1,
               liste[i].nom,
               liste[i].pv,
               liste[i].attaque,
               liste[i].defense,
               liste[i].vitesse);
    }
}

void PvP(){
    Combattant liste[MAX_COMBATTANTS];
    int nb_combattants = 0;
    
    charger_combattants(liste, &nb_combattants);
    afficher_combattants(liste, nb_combattants);
    
   
}

void menu_principal(){
    int choix;
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Nouvelle partie\n");
        printf("2. Quitter\n");
        printf("Choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                choix_game();
                break;
            case 2:
                printf("A bientot!\n");
                exit(1);
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 2);
}
void choix_game() {
    int choix;
    do{
        printf("\n===MENU JEU===\n");
        printf("1. PvP\n");
        printf("2. PvE\n");
        printf("3.Retour\n");
        printf("Choix : ");
        scanf("%d",&choix);

        switch(choix){
            case 1:
               PvP();
               break;
            case 2:
              exit(1);
            ///
            ///
              break;
            case 3:
              menu_principal();
              break;
            default:
            printf("Choix Invalide\n");

        }
    }while (choix<1 || choix>3);
}

int main() {
    menu_principal();
    return 0;
}