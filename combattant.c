#include "combattant.h"
#include "utils.h"





Combattant* charger_combattants(unsigned int* nb_combattants) {
    FILE* fichier = fopen("fichiers_combattants.txt", "r");
    if (!fichier) {
        perror("Erreur ouverture fichier");
        exit(EXIT_FAILURE);
    }

    Combattant* liste = malloc(MAX_COMBATTANTS * sizeof(Combattant));
    if (!liste) {
        perror("Erreur allocation mémoire");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    *nb_combattants = 0;
    char ligne[MAX_LIGNE];

    while (fgets(ligne, sizeof(ligne), fichier) && *nb_combattants < MAX_COMBATTANTS) {
        Combattant* c = &liste[*nb_combattants];

        int lus = sscanf(ligne,
            "%49[^;];%d;%d;%d;%d;%d;%49[^;];%99[^;];%d;%d;%d;%9[^;];"
            "%49[^;];%99[^;];%d;%d;%d;%9[^;];"
            "%49[^;];%99[^;];%d;%d;%d;%9[^\n]",
            c->nom,
            &c->pv, &c->pv_max, &c->attaque, &c->defense, &c->agilite,
            c->spe_attaque.nom, c->spe_attaque.description, &c->spe_attaque.valeur,
            &c->spe_attaque.duree, &c->spe_attaque.rechargement, c->spe_attaque.type_cible,
            c->spe_defense.nom, c->spe_defense.description, &c->spe_defense.valeur,
            &c->spe_defense.duree, &c->spe_defense.rechargement, c->spe_defense.type_cible,
            c->spe_agilite.nom, c->spe_agilite.description, &c->spe_agilite.valeur,
            &c->spe_agilite.duree, &c->spe_agilite.rechargement, c->spe_agilite.type_cible
        );

        if (lus == 24) {
            // Réinitialisation des états
            c->cooldown_attaque = c->cooldown_defense = c->cooldown_agilite = 0;
            c->effets.attaque_boost = c->effets.defense_boost = c->effets.agilite_boost = 0;
            c->effets.tours_restants = 0;
            (*nb_combattants)++;
        } else {
            fprintf(stderr, "Ligne mal formatée (%d champs): %s", lus, ligne);
        }
    }



    fclose(fichier);
    return liste;
}

Equipe creer_equipe(Combattant* liste, int nb_combattants, int num_equipe) {
    Equipe e;
    int choix;
    char input[10];
    const int largeur = 70;
    int deja_choisis[3] = {-1, -1, -1}; // Pour stocker les indices déjà choisis

    printf("\n");
    afficher_separateur(largeur);
    printf("| CREATION EQUIPE %-51d |\n", num_equipe);
    afficher_separateur(largeur);
    
    printf("| Nom de l'equipe: %-49s |\n", "");
    printf("| ");
    if (fgets(e.Nom_equipe, sizeof(e.Nom_equipe), stdin)) {
        e.Nom_equipe[strcspn(e.Nom_equipe, "\n")] = '\0';
    } else {
        strcpy(e.Nom_equipe, "Equipe sans nom");
    }
    printf("\033[A\033[18C%s\n", e.Nom_equipe);

    afficher_liste_combattants(liste, nb_combattants);

    e.fighter_1 = malloc(sizeof(Combattant));
    e.fighter_2 = malloc(sizeof(Combattant));
    e.fighter_3 = malloc(sizeof(Combattant));

    for (int i = 0; i < 3; i++) {
        do {
            printf("\n");
            afficher_separateur(largeur);
            printf("| Selectionnez le combattant %d/3 (1-%d):%-30s |\n", 
                  i+1, nb_combattants, "");
            afficher_separateur(largeur);
            printf("| Choix: %-60s |\n", "");
            
            // Positionnement curseur
            printf("\033[A\033[8C");
            
            if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &choix) == 1) {
                if (choix >= 1 && choix <= nb_combattants) {
                    // Vérifier si déjà choisi
                    int deja_pris = 0;
                    for (int j = 0; j < i; j++) {
                        if (deja_choisis[j] == choix - 1) {
                            deja_pris = 1;
                            break;
                        }
                    }
                    
                    if (!deja_pris) {
                        deja_choisis[i] = choix - 1;
                        printf("| %-60s |\n", liste[choix-1].nom);
                        break;
                    } else {
                        printf("Ce combattant est deja dans l'equipe!\n");
                    }
                }
            }
            printf("Choix invalide!\n");
        } while (1);

        switch (i) {
            case 0: *e.fighter_1 = liste[choix - 1]; break;
            case 1: *e.fighter_2 = liste[choix - 1]; break;
            case 2: *e.fighter_3 = liste[choix - 1]; break;
        }

        printf("%s ajoute a l'equipe!\n", liste[choix - 1].nom);
    }

    return e;
}


void afficher_liste_combattants(Combattant* liste, int nb) {
    const int largeur = 80;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| LISTE DES COMBATTANTS (%d)%-55s |\n", nb, "");
    afficher_separateur(largeur);
    
    for (int i = 0; i < nb; i++) {
        printf("%d-",i);
       afficher_details_combattant(liste[i]);
    }
}



void liberer_equipe(Equipe e) {
    free(e.fighter_1);
    free(e.fighter_2);
    free(e.fighter_3);
}
void afficher_details_combattant(Combattant c) {
    const int largeur = 60;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-56s |\n", c.nom);
    afficher_separateur(largeur);
    printf("| PV: %-4d/%d | Att: %-3d | Def: %-3d | Agi: %-3d |\n",
           c.pv, c.pv_max, c.attaque, c.defense, c.agilite);
    afficher_separateur(largeur);
    
    // Attaque spéciale
    printf("| %-56s |\n", "Attaque spéciale:");
    printf("| %-56s |\n", c.spe_attaque.nom);
    printf("| %-56s |\n", c.spe_attaque.description);
    printf("| Valeur: %-4d | Durée: %-2d tours | Recharge: %-2d tours |\n",
           c.spe_attaque.valeur, c.spe_attaque.duree, c.spe_attaque.rechargement);
    afficher_separateur(largeur);
    
    // Défense spéciale
    printf("| %-56s |\n", "Défense spéciale:");
    printf("| %-56s |\n", c.spe_defense.nom);
    printf("| %-56s |\n", c.spe_defense.description);
    printf("| Valeur: %-4d | Durée: %-2d tours | Recharge: %-2d tours |\n",
           c.spe_defense.valeur, c.spe_defense.duree, c.spe_defense.rechargement);
    afficher_separateur(largeur);
    
    // Agilité spéciale
    printf("| %-56s |\n", "Agilité spéciale:");
    printf("| %-56s |\n", c.spe_agilite.nom);
    printf("| %-56s |\n", c.spe_agilite.description);
    printf("| Valeur: %-4d | Durée: %-2d tours | Recharge: %-2d tours |\n",
           c.spe_agilite.valeur, c.spe_agilite.duree, c.spe_agilite.rechargement);
    
    if (c.effets.tours_restants > 0) {
        afficher_separateur(largeur);
        printf("| Effets temporaires (%d tours restants):%-25s |\n", 
               c.effets.tours_restants, "");
        if (c.effets.attaque_boost != 0) 
            printf("| - Attaque +%-50d |\n", c.effets.attaque_boost);
        if (c.effets.defense_boost != 0) 
            printf("| - Defense +%-49d |\n", c.effets.defense_boost);
        if (c.effets.agilite_boost != 0) 
            printf("| - Agilité +%-49d |\n", c.effets.agilite_boost);
    }
    
    afficher_separateur(largeur);
}
void afficher_equipe(Equipe e) {
    const int largeur = 70;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| EQUIPE: %-59s |\n", e.Nom_equipe);
    afficher_separateur(largeur);
    
    printf("| 1. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_1->nom, e.fighter_1->pv, 
           e.fighter_1->attaque, e.fighter_1->defense, e.fighter_1->agilite);
    
    printf("| 2. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_2->nom, e.fighter_2->pv,
           e.fighter_2->attaque, e.fighter_2->defense, e.fighter_2->agilite);
    
    printf("| 3. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_3->nom, e.fighter_3->pv,
           e.fighter_3->attaque, e.fighter_3->defense, e.fighter_3->agilite);
    
    afficher_separateur(largeur);
}
Combattant* choisir_combattant_actif(Equipe* equipe) {
    printf("Combattants disponibles:\n");
    if (equipe->fighter_1->pv > 0) printf("1. %s\n", equipe->fighter_1->nom);
    if (equipe->fighter_2->pv > 0) printf("2. %s\n", equipe->fighter_2->nom);
    if (equipe->fighter_3->pv > 0) printf("3. %s\n", equipe->fighter_3->nom);
    
    int choix;
    char input[10];
    do {
        printf("Choisissez un combattant (1-3): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erreur de lecture.\n");
            continue;
        }
        if (sscanf(input, "%d", &choix) != 1) {
            printf("Entree invalide! Veuillez entrer un nombre.\n");
            continue;
        }
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

Combattant* choisir_cible(Equipe* equipe, const char* type) {
    printf("\nCibles %s disponibles:\n", type);
    if (equipe->fighter_1->pv > 0) printf("1. %s (PV: %d)\n", equipe->fighter_1->nom, equipe->fighter_1->pv);
    if (equipe->fighter_2->pv > 0) printf("2. %s (PV: %d)\n", equipe->fighter_2->nom, equipe->fighter_2->pv);
    if (equipe->fighter_3->pv > 0) printf("3. %s (PV: %d)\n", equipe->fighter_3->nom, equipe->fighter_3->pv);
    
    int choix;
    char input[10];
    do {
        printf("Choisissez une cible (1-3): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erreur de lecture.\n");
            continue;
        }
        if (sscanf(input, "%d", &choix) != 1) {
            printf("Entree invalide! Veuillez entrer un nombre.\n");
            continue;
        }
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
