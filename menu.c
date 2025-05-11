#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "menu.h"
#include "combattant.h"
#include "combat.h"
#include "utils.h"

/**
 * Fonction pour le mode Joueur contre Joueur (PvP)
 * Permet à deux joueurs de créer leurs équipes et de combattre
 */
void menu_pvp() {
    unsigned int nb_combattants;
    // Charge la liste des combattants disponibles
    Combattant* liste = charger_combattants(&nb_combattants);

    const int largeur = 60;  // Largeur des séparateurs
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "MODE PVP");
    afficher_separateur(largeur);
    
    // Création de l'équipe 1
    printf("| %-58s |\n", "Creation de l'equipe 1");
    afficher_separateur(largeur);
    Equipe equipe1 = creer_equipe(liste, nb_combattants, 1);
    
    // Création de l'équipe 2
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Creation de l'equipe 2");
    afficher_separateur(largeur);
    Equipe equipe2 = creer_equipe(liste, nb_combattants, 2);

    // Affichage des équipes créées
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Equipes pretes !");
    afficher_separateur(largeur);
    afficher_equipe(equipe1);
    afficher_equipe(equipe2);

    // Libération de la mémoire et lancement du combat
    free(liste);
    combat_pvp(equipe1, equipe2);
}

/**
 * Fonction pour le mode Joueur contre IA (PvE)
 * Permet au joueur de créer son équipe et d'affronter une IA
 */
void menu_pve() {
    unsigned int nb_combattants;
    // Charge la liste des combattants disponibles
    Combattant* liste = charger_combattants(&nb_combattants);
    const int largeur = 60;
    int difficulte;

    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "MODE PVE");
    afficher_separateur(largeur);
    
    // Menu de sélection de la difficulté
    printf("| %-58s |\n", "Choisissez la difficulte:");
    printf("| %-58s |\n", "1. Noob ");
    printf("| %-58s |\n", "2. Facile");
    printf("| %-58s |\n", "3. moyen");
    afficher_separateur(largeur);
    
    // Gestion de la saisie de la difficulté avec vérification
    char input[10];
    int result;
    do {
        printf("Entrez le choix de la difficulté = ");
        result = scanf("%d", &difficulte);
        while (getchar() != '\n');  // Vide le buffer d'entrée
        if (result != 1 || difficulte < 1 || difficulte > 3) {
            printf("Entrée invalide. Veuillez entrer 1,2 ou 3.\n");
        }
    } while (result != 1 || difficulte < 1 || difficulte > 3);
    
    // Création de l'équipe du joueur
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Creation de votre equipe");
    afficher_separateur(largeur);
    Equipe equipe_joueur = creer_equipe(liste, nb_combattants, 1);
    
    // Génération de l'équipe IA
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Generation de l'equipe IA");
    afficher_separateur(largeur);
    
    Equipe equipe_ia;
    strcpy(equipe_ia.Nom_equipe, "IA Enemy");
    // Allocation mémoire pour les combattants IA
    equipe_ia.fighter_1 = malloc(sizeof(Combattant));
    equipe_ia.fighter_2 = malloc(sizeof(Combattant));
    equipe_ia.fighter_3 = malloc(sizeof(Combattant));
    
    // Sélection aléatoire de 3 combattants distincts pour l'IA
    int indices[3] = {-1, -1, -1};
    for (int i = 0; i < 3; i++) {
        int index;
        do {
            index = rand() % nb_combattants;
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
        // Affectation du combattant à la position correspondante
        *(i == 0 ? equipe_ia.fighter_1 : i == 1 ? equipe_ia.fighter_2 : equipe_ia.fighter_3) = liste[index];
    }

    // Affichage de l'équipe IA
    printf("\n=== ÉQUIPE ENNEMIE ===\n");
    afficher_equipe(equipe_ia);
    
    // Lancement du combat PvE avec la difficulté choisie
    combat_pve(equipe_joueur, equipe_ia, difficulte);
    
    // Libération de la mémoire
    free(liste);
    free(equipe_joueur.fighter_1);
    free(equipe_joueur.fighter_2);
    free(equipe_joueur.fighter_3);
    free(equipe_ia.fighter_1);
    free(equipe_ia.fighter_2);
    free(equipe_ia.fighter_3);
}

/**
 * Menu principal du jeu
 * Permet de choisir entre les différents modes de jeu ou de quitter
 */
void menu_principal(){
    const int largeur = 40;
    int choix;
    char input[50];
    
    // Boucle principale du menu
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
        
        // Positionnement du curseur pour une meilleure expérience utilisateur
        printf("\033[A"); // Remonter d'une ligne
        printf("\033[9C"); // Avancer de 9 caractères
        
        // Récupération du choix de l'utilisateur
        if (fgets(input, sizeof(input), stdin)){
            sscanf(input, "%d", &choix);
        } else {
            choix = 0;
        }

        // Gestion du choix de l'utilisateur
        switch (choix) {
            case 1: 
                menu_pvp(); 
                break;
            case 2: 
                menu_pve(); 
                break;
            case 3: 
                printf("\nMerci d'avoir joué !\n");
                exit(0);
            default: 
                printf("Choix invalide!\n");
        }
    }while(1);  // Boucle infinie jusqu'à ce que l'utilisateur choisisse de quitter
}
