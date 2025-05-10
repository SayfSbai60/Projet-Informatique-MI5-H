#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "menu.h"
#include "combattant.h"
#include "combat.h"
#include "utils.h"



void menu_pvp() {
    unsigned int nb_combattants;
    Combattant* liste = charger_combattants(&nb_combattants);

    const int largeur = 60;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "MODE PVP");
    afficher_separateur(largeur);
    
    printf("| %-58s |\n", "Creation de l'equipe 1");
    afficher_separateur(largeur);
    Equipe equipe1 = creer_equipe(liste, nb_combattants, 1);
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Creation de l'equipe 2");
    afficher_separateur(largeur);
    Equipe equipe2 = creer_equipe(liste, nb_combattants, 2);

    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Equipes pretes !");
    afficher_separateur(largeur);
    afficher_equipe(equipe1);
    afficher_equipe(equipe2);

    free(liste);
    combat_pvp(equipe1, equipe2);
}

void menu_pve() {
    unsigned int nb_combattants;
    Combattant* liste = charger_combattants(&nb_combattants);
    const int largeur = 60;

    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "MODE PVE");
    afficher_separateur(largeur);
    
    
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Creation de votre equipe");
    afficher_separateur(largeur);
    Equipe equipe_joueur = creer_equipe(liste, nb_combattants, 1);
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Generation de l'equipe IA");
    afficher_separateur(largeur);
    
    // Génération équipe IA
    Equipe equipe_ia;
    strcpy(equipe_ia.Nom_equipe, "IA Enemy");
    equipe_ia.fighter_1 = malloc(sizeof(Combattant));
    equipe_ia.fighter_2 = malloc(sizeof(Combattant));
    equipe_ia.fighter_3 = malloc(sizeof(Combattant));
    
    // Choix aléatoire sans doublons
    int indices[3] = {-1, -1, -1};
    for (int i = 0; i < 3; i++) {
        int index;
        do {
            index = rand() % nb_combattants;
            // Vérifier que ce combattant n'est pas déjà choisi
            int deja_pris = 0;
            for (int j = 0; j < i; j++) {
                if (indices[j] == index) {
                    deja_pris = 1;
                    break;
                }
            }
            if (!deja_pris) break;
        } while (1);
        
        indices[i] = index;
        *(i == 0 ? equipe_ia.fighter_1 : i == 1 ? equipe_ia.fighter_2 : equipe_ia.fighter_3) = liste[index];
    }

    printf("\n=== ÉQUIPE ENNEMIE ===\n");
    afficher_equipe(equipe_ia);
    
    combat_pve_simple(equipe_joueur, equipe_ia);
    free(liste);
}

void menu_principal(){
    const int largeur = 40;
    int choix;
    char input[50];
    
    do{
        printf("\n");
        afficher_separateur(largeur);
        printf("| %-38s |\n", "ONE PIECE FIGHT");
        afficher_separateur(largeur);
        printf("| %-38s |\n", "1. Mode PvP (Joueur vs Joueur)");
        printf("| %-38s |\n", "2. Mode PvE (Joueur vs Bot)");
        printf("| %-38s |\n", "3. Quitter");
        afficher_separateur(largeur);
        printf("| Choix: %-30s |\n", "");
        
        // Positionner le curseur pour le choix
        printf("\033[A"); // Remonter d'une ligne
        printf("\033[9C"); // Avancer de 9 caractères
        
        if (fgets(input, sizeof(input), stdin)){
            sscanf(input, "%d", &choix);
        } else {
            choix = 0;
        }

        switch (choix) {
            case 1: menu_pvp(); break;
            case 2: menu_pve(); break;
            case 3: 
                printf("\nMerci d'avoir joué !\n");
                exit(0);
            default: 
                printf("Choix invalide!\n");
        }
    }while(1);
}
