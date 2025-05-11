#include "combat.h"
#include "combattant.h"
#include "utils.h"
#include "menu.h"

// Fonction principale de gestion du tour de jeu pour une équipe
void jouer_tour(Equipe* equipe_actuelle, Equipe* equipe_adverse) {
    // Choix du combattant actif dans l'équipe actuelle
    Combattant* combattant_actuel = choisir_combattant_actif(equipe_actuelle);
    
    // Affichage du menu d'actions disponibles pour ce combattant
    printf("\nActions possibles pour %s:\n", combattant_actuel->nom);
    printf("1. Attaque de base\n");
    printf("2. Technique speciale - Attaque (%s)", combattant_actuel->spe_attaque.nom);
    if (combattant_actuel->cooldown_attaque > 0) printf(" (Recharge: %d)", combattant_actuel->cooldown_attaque);
    printf("\n");
    printf("3. Technique speciale - Defense (%s)", combattant_actuel->spe_defense.nom);
    if (combattant_actuel->cooldown_defense > 0) printf(" (Recharge: %d)", combattant_actuel->cooldown_defense);
    printf("\n");
    printf("4. Technique speciale - Agilite (%s)", combattant_actuel->spe_agilite.nom);
    if (combattant_actuel->cooldown_agilite > 0) printf(" (Recharge: %d)", combattant_actuel->cooldown_agilite);
    printf("\n");

    // Lecture de l'action choisie par l'utilisateur
    int choix_action;
    char input[10];
    do {
        printf("Choisissez une action (1-4): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erreur de lecture.\n");
            continue;
        }
        if (sscanf(input, "%d", &choix_action) != 1) {
            printf("Entree invalide! Veuillez entrer un nombre.\n");
            continue;
        }
    } while (choix_action < 1 || choix_action > 4);

    // Exécution de l'action choisie
    switch (choix_action) {
        case 1:
            attaque_de_base(combattant_actuel, equipe_adverse);
            break;
        case 2:
            if (combattant_actuel->cooldown_attaque > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_attaque);
                jouer_tour(equipe_actuelle, equipe_adverse); // Relance le tour
                return;
            }
            utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_attaque, equipe_adverse, equipe_actuelle);
            break;
        case 3:
            if (combattant_actuel->cooldown_defense > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_defense);
                jouer_tour(equipe_actuelle, equipe_adverse);
                return;
            }
            utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_defense, equipe_adverse, equipe_actuelle);
            break;
        case 4:
            if (combattant_actuel->cooldown_agilite > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_agilite);
                jouer_tour(equipe_actuelle, equipe_adverse);
                return;
            }
            utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_agilite, equipe_adverse, equipe_actuelle);
            break;
    }

    // Application des effets temporaires et diminution des cooldowns
    appliquer_effets(combattant_actuel);
    diminuer_cooldowns(combattant_actuel);
}

// Utilisation d’une technique spéciale (attaque, défense, agilité)
void utiliser_technique_speciale(Combattant* utilisateur, TechniqueSpeciale* tech, Equipe* equipe_adverse, Equipe* equipe_alliee) {
    if (tech == &utilisateur->spe_attaque) {
        // Cible ennemie pour attaque
        Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
        appliquer_technique(utilisateur, tech, cible);
    } else {
        // Cible alliée pour support
        Combattant* cible = choisir_cible(equipe_alliee, "alliee");
        appliquer_technique(utilisateur, tech, cible);
    }

    // Mise à jour du cooldown de la technique utilisée
    if (tech == &utilisateur->spe_attaque) {
        utilisateur->cooldown_attaque = tech->rechargement;
    } else if (tech == &utilisateur->spe_defense) {
        utilisateur->cooldown_defense = tech->rechargement;
    } else if (tech == &utilisateur->spe_agilite) {
        utilisateur->cooldown_agilite = tech->rechargement;
    }
}

// Appliquer l’effet d’une technique sur une cible
void appliquer_technique(Combattant* utilisateur, TechniqueSpeciale* tech, Combattant* cible) {
    printf("%s utilise %s sur %s!\n", utilisateur->nom, tech->nom, cible->nom);

    // Détection du type de technique
    int est_attaque = strstr(tech->nom, "(attaque)") != NULL;
    int est_defense = strstr(tech->nom, "(defense)") != NULL;
    int est_agilite = strstr(tech->nom, "(agilite)") != NULL;

    if (est_attaque) {
        // Technique offensive
        cible->pv = cible->pv - tech->valeur;
        if (cible->pv <= 0) printf("%s est K.O.!\n", cible->nom);
    }
    else if (est_defense) {
        // Technique défensive
        cible->defense += tech->valeur;
        cible->effets.defense_boost = tech->valeur;
        cible->effets.tours_restants = tech->duree;
        printf("Defense augmentee de %d pour %d tours!\n", tech->valeur, tech->duree);
    }
    else if (est_agilite) {
        // Technique d'agilité
        cible->agilite += tech->valeur;
        cible->effets.agilite_boost = tech->valeur;
        cible->effets.tours_restants = tech->duree;
        printf("Agilite augmentee de %d pour %d tours!\n", tech->valeur, tech->duree);
    }


}

// Attaque classique, avec esquive et calcul de réduction de dégâts
void attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse) {
    Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
    
    // Calcul de la chance d'esquive
    int chance_esquive = 10 + (cible->agilite - attaquant->agilite);
    if (chance_esquive < 5) chance_esquive = 5;
    if (chance_esquive > 90) chance_esquive = 90;

    if (rand() % 100 < chance_esquive) {
        printf("%s esquive l'attaque grace a son agilite (%d%% chance)!\n", cible->nom, chance_esquive);
        return;
    }

    // Calcul de la réduction des dégâts via la défense
    float reduction = (float)cible->defense / (cible->defense + 100);
    int degats = attaquant->attaque * (1 - reduction);
    if (degats < 1) degats = 1;

    cible->pv -= degats;
    printf("%s attaque %s et inflige %d degats! (Reduction: %.0f%%)\n", attaquant->nom, cible->nom, degats, reduction * 100);

    if (cible->pv <= 0) {
        cible->pv = 0;
        printf("%s est K.O.!\n", cible->nom);
    }
}

// Applique les effets temporaires (boost) et les retire s'ils expirent
void appliquer_effets(Combattant* c) {
    if (c->effets.tours_restants > 0) {
        c->effets.tours_restants--;
        if (c->effets.tours_restants == 0) {
            c->attaque -= c->effets.attaque_boost;
            c->defense -= c->effets.defense_boost;
            c->agilite -= c->effets.agilite_boost;
            c->effets.attaque_boost = 0;
            c->effets.defense_boost = 0;
            c->effets.agilite_boost = 0;
            printf("%s: Les effets temporaires ont expire.\n", c->nom);
        }
    }
}

// Réduction des cooldowns pour les techniques spéciales
void diminuer_cooldowns(Combattant* c) {
    if (c->cooldown_attaque > 0) c->cooldown_attaque--;
    if (c->cooldown_defense > 0) c->cooldown_defense--;
    if (c->cooldown_agilite > 0) c->cooldown_agilite--;
}

// Vérifie si tous les combattants d'une équipe sont K.O.
int verifier_equipe_ko(Equipe e) {
    return (e.fighter_1->pv <= 0 && e.fighter_2->pv <= 0 && e.fighter_3->pv <= 0);
}

void combat_pvp(Equipe equipe1, Equipe equipe2) {
    printf("\nLe combat commence!\n");
    srand(time(NULL)); // Initialisation du générateur aléatoire

    int tour = 1;
    int equipe1_ko = 0, equipe2_ko = 0;

    while (!equipe1_ko && !equipe2_ko) {
        printf("\n=== TOUR %d ===\n", tour);
        afficher_combat(equipe1, equipe2); // Affiche l’état des équipes

        // Vérifie si une des équipes est éliminée
        equipe1_ko = verifier_equipe_ko(equipe1);
        equipe2_ko = verifier_equipe_ko(equipe2);
        if (equipe1_ko || equipe2_ko) break;

        // Tour de l’équipe 1
        printf("\n--- Tour de l'equipe %s ---\n", equipe1.Nom_equipe);
        jouer_tour(&equipe1, &equipe2);
        equipe2_ko = verifier_equipe_ko(equipe2);
        if (equipe2_ko) break;

        afficher_combat(equipe1, equipe2);

        // Tour de l’équipe 2
        printf("\n--- Tour de l'equipe %s ---\n", equipe2.Nom_equipe);
        jouer_tour(&equipe2, &equipe1);
        tour++;
    }

    // Affichage du résultat
    if (equipe1_ko) {
        printf("\nL'equipe %s a gagne!\n", equipe2.Nom_equipe);
    } else {
        printf("\nL'equipe %s a gagne!\n", equipe1.Nom_equipe);
    }

    // Libération mémoire des équipes
    liberer_equipe(equipe1);
    liberer_equipe(equipe2);
}


void attaque_de_base_pnj(Combattant* attaquant, Equipe* equipe_adverse) {
    // Choisir une cible aléatoire valide dans l'équipe adverse
    Combattant* cible = NULL;
    do {
        switch(rand() % 3) {
            case 0: cible = equipe_adverse->fighter_1; break;
            case 1: cible = equipe_adverse->fighter_2; break;
            case 2: cible = equipe_adverse->fighter_3; break;
        }
    } while (cible == NULL || cible->pv <= 0);

    // Calcul de la chance d'esquive comme dans attaque_de_base
    int chance_esquive = 10 + (cible->agilite - attaquant->agilite);
    if (rand() % 100 < chance_esquive) {
        printf("%s esquive l'attaque de %s (Chance: %d%%)!\n", cible->nom, attaquant->nom, chance_esquive);
        return;
    }

    // Calcul des dégâts avec réduction
    float reduction = (float)cible->defense / (cible->defense + 100);
    int degats = attaquant->attaque * (1 - reduction);
    if (degats < 1) degats = 1;

    cible->pv -= degats;
    printf("%s attaque %s et inflige %d degats! (Reduction: %.0f%%)\n", attaquant->nom, cible->nom, degats, reduction*100);

    if (cible->pv <= 0) {
        cible->pv = 0;
        printf("%s est K.O.!\n", cible->nom);
    }
}

void ordi_hard(Equipe* equipe_ordi, Equipe* equipe_joueur) {
    Combattant* attaquant = choisir_combattant_aleatoire(equipe_ordi);
    if (!attaquant) return;

    // Trouver les cibles ennemie et alliée les plus faibles
    Combattant* cible_enemie = trouver_combattant_faible(equipe_joueur);
    Combattant* cible_allie = trouver_combattant_faible(equipe_ordi);
    if (!cible_enemie) return;

    int action_effectuee = 0;

    // Priorité 1 : Technique spéciale offensive
    if (attaquant->cooldown_attaque == 0) {
        int degats = attaquant->spe_attaque.valeur - (cible_enemie->defense / 2);
        if (degats < 1) degats = 1;
        cible_enemie->pv -= degats;
        if (cible_enemie->pv < 0) cible_enemie->pv = 0;

        attaquant->cooldown_attaque = attaquant->spe_attaque.rechargement;
        printf("%s utilise %s sur %s et inflige %d degats!\n",
               attaquant->nom, attaquant->spe_attaque.nom, cible_enemie->nom, degats);
        action_effectuee = 1;
    }
    // Priorité 2 : Technique spéciale défensive
    else if (attaquant->cooldown_defense == 0 && cible_allie) {
        appliquer_effets(cible_allie);
        attaquant->cooldown_defense = attaquant->spe_defense.rechargement;
        printf("%s utilise %s sur %s et boost sa defense!\n",
               attaquant->nom, attaquant->spe_defense.nom, cible_allie->nom);
        action_effectuee = 1;
    }
    // Priorité 3 : Technique spéciale d'agilité
    else if (attaquant->cooldown_agilite == 0 && cible_allie) {
        appliquer_effets(cible_allie);
        attaquant->cooldown_agilite = attaquant->spe_agilite.rechargement;
        printf("%s utilise %s sur %s et boost son agilite!\n",
               attaquant->nom, attaquant->spe_agilite.nom, cible_allie->nom);
        action_effectuee = 1;
    }

    // Si aucune action spéciale n'a été faite, attaque de base
    if (!action_effectuee) {
        attaque_de_base_pnj(attaquant, equipe_joueur);
        return;
    }

    // Appliquer les effets et diminuer les cooldowns
    appliquer_effets(attaquant);
    diminuer_cooldowns(attaquant);
}

void combat_pve(Equipe equipe_joueur, Equipe equipe_ia, int difficulte) {
    int tour = 1;
    const int MAX_TOURS = 100;

    printf("\n=== MODE PVE ===\n");
    printf("Difficulte: %s\n", difficulte == 1 ? "Noob" : "Facile");

    while (!verifier_equipe_ko(equipe_joueur) && !verifier_equipe_ko(equipe_ia) && tour <= MAX_TOURS) {
        printf("\n--- TOUR %d ---\n", tour++);
        afficher_combat(equipe_joueur, equipe_ia);

        // Tour du joueur
        if (!verifier_equipe_ko(equipe_ia)) {
            printf("\n[VOTRE TOUR]\n");
            jouer_tour(&equipe_joueur, &equipe_ia);
            if (verifier_equipe_ko(equipe_ia)) break;
        }

        // Tour de l'IA
        if (!verifier_equipe_ko(equipe_joueur)) {
            printf("\n[TOUR ENNEMIE]\n");

            Combattant* attaquant_ia = choisir_combattant_aleatoire(&equipe_ia);
            if (!attaquant_ia) continue;

            // Choix du comportement selon la difficulté
            if (difficulte == 1) { // IA aléatoire
                attaque_de_base_pnj(attaquant_ia, &equipe_joueur);
            } else if (difficulte == 3) { // IA « Hard »
                ordi_hard(&equipe_ia, &equipe_joueur);
            } else { // IA « Facile » - cible la plus faible
                attaque_de_base_pnj(attaquant_ia, &equipe_joueur);
            }

            if (verifier_equipe_ko(equipe_joueur)) break;
        }
    }

    // Résultat final
    if (verifier_equipe_ko(equipe_joueur)) {
        printf("L'ennemie a gagne !\n");
    } else if (verifier_equipe_ko(equipe_ia)) {
        printf("Vous avez gagne !\n");
    } else if (tour > MAX_TOURS) {
        printf("Egalite, nombre de tour maximum atteint !\n");
    }

    liberer_equipe(equipe_joueur);
    liberer_equipe(equipe_ia);
    menu_principal();
}



