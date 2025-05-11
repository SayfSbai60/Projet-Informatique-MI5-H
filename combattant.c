#include "combattant.h"
#include "utils.h"

// Charge les combattants à partir d’un fichier texte
Combattant* charger_combattants(unsigned int* nb_combattants) {
    FILE* fichier = fopen("fichiers_combattants.txt", "r");
    if (!fichier) {
        perror("Erreur ouverture fichier");
        exit(EXIT_FAILURE);
    }

    // Allocation de mémoire pour la liste
    Combattant* liste = malloc(MAX_COMBATTANTS * sizeof(Combattant));
    if (!liste) {
        perror("Erreur allocation mémoire");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    *nb_combattants = 0;
    char ligne[MAX_LIGNE];

    // Lecture ligne par ligne du fichier
    while (fgets(ligne, sizeof(ligne), fichier) && *nb_combattants < MAX_COMBATTANTS) {
        Combattant* c = &liste[*nb_combattants];

        // Extraction des données du combattant et de ses 3 techniques
        int lus = sscanf(ligne,
            "%49[^;];%d;%d;%d;%d;%d;%49[^;];%99[^;];%d;%d;%d;%9[^;];"
            "%49[^;];%99[^;];%d;%d;%d;%9[^;];"
            "%49[^;];%99[^;];%d;%d;%d;%9[^\n]",
            c->nom, &c->pv, &c->pv_max, &c->attaque, &c->defense, &c->agilite,
            c->spe_attaque.nom, c->spe_attaque.description, &c->spe_attaque.valeur,
            &c->spe_attaque.duree, &c->spe_attaque.rechargement, c->spe_attaque.type_cible,
            c->spe_defense.nom, c->spe_defense.description, &c->spe_defense.valeur,
            &c->spe_defense.duree, &c->spe_defense.rechargement, c->spe_defense.type_cible,
            c->spe_agilite.nom, c->spe_agilite.description, &c->spe_agilite.valeur,
            &c->spe_agilite.duree, &c->spe_agilite.rechargement, c->spe_agilite.type_cible
        );

        if (lus == 24) {
            // Initialisation des états de combat
            c->cooldown_attaque = c->cooldown_defense = c->cooldown_agilite = 0;
            c->effets.attaque_boost = c->effets.defense_boost = c->effets.agilite_boost = 0;
            c->effets.tours_restants = 0;
            (*nb_combattants)++;
        } else {
            // Erreur de format
            fprintf(stderr, "Ligne mal formatée (%d champs): %s", lus, ligne);
        }
    }

    fclose(fichier);
    return liste;
}

/// Permet au joueur de créer une équipe de 3 combattants
Equipe creer_equipe(Combattant* liste, int nb_combattants, int num_equipe) {
    Equipe e;
    int choix;
    char input[10];
    int deja_choisis[3] = {-1, -1, -1};

    // Affichage du titre et demande du nom d’équipe
    printf("\nNom de l'equipe: ");
    if (fgets(e.Nom_equipe, sizeof(e.Nom_equipe), stdin)) {
        e.Nom_equipe[strcspn(e.Nom_equipe, "\n")] = '\0';
    } else {
        strcpy(e.Nom_equipe, "Equipe sans nom");
    }

    afficher_liste_combattants(liste, nb_combattants);

    // Allocation mémoire des combattants de l’équipe
    e.fighter_1 = malloc(sizeof(Combattant));
    e.fighter_2 = malloc(sizeof(Combattant));
    e.fighter_3 = malloc(sizeof(Combattant));

    // Choix des 3 combattants
    for (int i = 0; i < 3; i++) {
        do {
            printf("Selectionnez le combattant %d/3 (1-%d): ", i+1, nb_combattants);
            if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &choix) == 1) {
                if (choix >= 1 && choix <= nb_combattants) {
                    int deja_pris = 0;
                    for (int j = 0; j < i; j++) {
                        if (deja_choisis[j] == choix - 1) {
                            deja_pris = 1;
                            break;
                        }
                    }
                    if (!deja_pris) {
                        deja_choisis[i] = choix - 1;
                        break;
                    } else {
                        printf("Ce combattant est deja pris.\n");
                    }
                }
            }
            printf("Choix invalide.\n");
        } while (1);

        // Affectation du combattant sélectionné
        switch (i) {
            case 0: *e.fighter_1 = liste[choix - 1]; break;
            case 1: *e.fighter_2 = liste[choix - 1]; break;
            case 2: *e.fighter_3 = liste[choix - 1]; break;
        }
        printf("%s ajoute a l'equipe.\n", liste[choix - 1].nom);
    }

    return e;
}

// Retourne le combattant avec le moins de PV (et encore vivant)
Combattant* trouver_combattant_faible(Equipe* equipe) {
    Combattant* plus_faible = NULL;
    int min_pv; 

    // Vérifie chaque combattant vivant
    if (equipe->fighter_1->pv > 0 && equipe->fighter_1->pv < min_pv) {
        plus_faible = equipe->fighter_1;
        min_pv = equipe->fighter_1->pv;
    }
    if (equipe->fighter_2->pv > 0 && equipe->fighter_2->pv < min_pv) {
        plus_faible = equipe->fighter_2;
        min_pv = equipe->fighter_2->pv;
    }
    if (equipe->fighter_3->pv > 0 && equipe->fighter_3->pv < min_pv) {
        plus_faible = equipe->fighter_3;
        min_pv = equipe->fighter_3->pv;
    }

    return plus_faible;
}

// Choisit un combattant vivant au hasard dans l'équipe
Combattant* choisir_combattant_aleatoire(Equipe* equipe) {
    Combattant* disponibles[3];
    int nb_disponibles = 0;

    // On ne garde que ceux encore en vie
    if (equipe->fighter_1->pv > 0) disponibles[nb_disponibles++] = equipe->fighter_1;
    if (equipe->fighter_2->pv > 0) disponibles[nb_disponibles++] = equipe->fighter_2;
    if (equipe->fighter_3->pv > 0) disponibles[nb_disponibles++] = equipe->fighter_3;

    if (nb_disponibles == 0) return NULL;

    // Retourne un combattant au hasard parmi les vivants
    int choix = rand() % nb_disponibles;
    return disponibles[choix];
}

// Affiche la liste complète des combattants disponibles avec leurs détails
void afficher_liste_combattants(Combattant* liste, int nb) {
    const int largeur = 80;

    printf("\n");
    afficher_separateur(largeur);
    printf("| LISTE DES COMBATTANTS (%d)%-55s |\n", nb, "");
    afficher_separateur(largeur);

    // Affiche chaque combattant avec ses infos
    for (int i = 0; i < nb; i++) {
        printf("%d-", i + 1);
        afficher_details_combattant(liste[i]);
    }
}



// Libère la mémoire dynamique allouée à chaque combattant d’une équipe
void liberer_equipe(Equipe e) {
    free(e.fighter_1);
    free(e.fighter_2);
    free(e.fighter_3);
}

// Affiche toutes les statistiques et techniques spéciales d’un combattant
void afficher_details_combattant(Combattant c) {
    const int largeur = 60;
    
    afficher_separateur(largeur);
    printf("| %-56s |\n", c.nom);
    afficher_separateur(largeur);
    printf("| PV: %-4d/%d | Att: %-3d | Def: %-3d | Agi: %-3d |\n",
           c.pv, c.pv_max, c.attaque, c.defense, c.agilite);
    afficher_separateur(largeur);

    // Attaque spéciale
    printf("| Attaque speciale: %-42s \n", 
           c.spe_attaque.nom, c.cooldown_attaque);
    printf("| %-56s |\n", c.spe_attaque.description);
    printf("| Valeur: %-4d | Duree: %-2d tours |\n",
           c.spe_attaque.valeur, c.spe_attaque.duree);
    afficher_separateur(largeur);

    // Défense spéciale
    printf("| Defense speciale: %-42s \n",
           c.spe_defense.nom, c.cooldown_defense);
    printf("| %-56s |\n", c.spe_defense.description);
    printf("| Valeur: %-4d | Duree: %-2d tours |\n",
           c.spe_defense.valeur, c.spe_defense.duree);
    afficher_separateur(largeur);

    // Agilité spéciale
    printf("| Agilite speciale: %-42s \n",
           c.spe_agilite.nom, c.cooldown_agilite);
    printf("| %-56s |\n", c.spe_agilite.description);
    printf("| Valeur: %-4d | Duree: %-2d tours |\n",
           c.spe_agilite.valeur, c.spe_agilite.duree);
    afficher_separateur(largeur);
    }

// Affiche le nom et les stats de tous les membres d’une équipe
void afficher_equipe(Equipe e) {
    const int largeur = 70;

    afficher_separateur(largeur);
    printf("| EQUIPE: %-59s |\n", e.Nom_equipe);
    afficher_separateur(largeur);

    // Infos des 3 combattants
    printf("| 1. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_1->nom, e.fighter_1->pv, e.fighter_1->attaque,
           e.fighter_1->defense, e.fighter_1->agilite);

    printf("| 2. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_2->nom, e.fighter_2->pv, e.fighter_2->attaque,
           e.fighter_2->defense, e.fighter_2->agilite);

    printf("| 3. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_3->nom, e.fighter_3->pv, e.fighter_3->attaque,
           e.fighter_3->defense, e.fighter_3->agilite);

    afficher_separateur(largeur);
}
// Permet au joueur de choisir un combattant vivant à jouer
Combattant* choisir_combattant_actif(Equipe* equipe) {
    printf("Combattants disponibles:\n");
    if (equipe->fighter_1->pv > 0) printf("1. %s\n", equipe->fighter_1->nom);
    if (equipe->fighter_2->pv > 0) printf("2. %s\n", equipe->fighter_2->nom);
    if (equipe->fighter_3->pv > 0) printf("3. %s\n", equipe->fighter_3->nom);

    int choix;
    char input[10];
    do {
        printf("Choisissez un combattant (1-3): ");
        if (fgets(input, sizeof(input), stdin) == NULL) continue;
        if (sscanf(input, "%d", &choix) != 1) continue;
    } while (choix < 1 || choix > 3 ||
             (choix == 1 && equipe->fighter_1->pv <= 0) ||
             (choix == 2 && equipe->fighter_2->pv <= 0) ||
             (choix == 3 && equipe->fighter_3->pv <= 0));

    switch (choix) {
        case 1: return equipe->fighter_1;
        case 2: return equipe->fighter_2;
        case 3: return equipe->fighter_3;
        default: return NULL;
    }
}

// Permet de choisir une cible vivante dans une équipe (alliée ou ennemie)
Combattant* choisir_cible(Equipe* equipe, const char* type) {
    printf("\nCibles %s disponibles:\n", type);
    if (equipe->fighter_1->pv > 0) printf("1. %s (PV: %d)\n", equipe->fighter_1->nom, equipe->fighter_1->pv);
    if (equipe->fighter_2->pv > 0) printf("2. %s (PV: %d)\n", equipe->fighter_2->nom, equipe->fighter_2->pv);
    if (equipe->fighter_3->pv > 0) printf("3. %s (PV: %d)\n", equipe->fighter_3->nom, equipe->fighter_3->pv);

    int choix;
    char input[10];
    do {
        printf("Choisissez une cible (1-3): ");
        if (fgets(input, sizeof(input), stdin) == NULL) continue;
        if (sscanf(input, "%d", &choix) != 1) continue;
    } while (choix < 1 || choix > 3 ||
             (choix == 1 && equipe->fighter_1->pv <= 0) ||
             (choix == 2 && equipe->fighter_2->pv <= 0) ||
             (choix == 3 && equipe->fighter_3->pv <= 0));

    switch (choix) {
        case 1: return equipe->fighter_1;
        case 2: return equipe->fighter_2;
        case 3: return equipe->fighter_3;
        default: return NULL;
    }
}
