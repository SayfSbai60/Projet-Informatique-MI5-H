#include "combat.h"
#include "combattant.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void jouer_tour(Equipe* equipe_actuelle, Equipe* equipe_adverse) {
    Combattant* combattant_actuel = choisir_combattant_actif(equipe_actuelle);
    
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
    
    switch (choix_action) {
        case 1: 
            attaque_de_base(combattant_actuel, equipe_adverse); 
            break;
        case 2: 
            if (combattant_actuel->cooldown_attaque > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_attaque);
                jouer_tour(equipe_actuelle, equipe_adverse);
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
    
    appliquer_effets(combattant_actuel);
    diminuer_cooldowns(combattant_actuel);
}

void utiliser_technique_speciale(Combattant* utilisateur, TechniqueSpeciale* tech, Equipe* equipe_adverse, Equipe* equipe_alliee) {
    if (tech == &utilisateur->spe_attaque) {
        Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
        appliquer_technique(utilisateur, tech, cible);
    }
    else {
        Combattant* cible = choisir_cible(equipe_alliee, "alliee");
        appliquer_technique(utilisateur, tech, cible);
    }
    
    // Mettre en rechargement
    if (tech == &utilisateur->spe_attaque) {
        utilisateur->cooldown_attaque = tech->rechargement;
    } else if (tech == &utilisateur->spe_defense) {
        utilisateur->cooldown_defense = tech->rechargement;
    } else if (tech == &utilisateur->spe_agilite) {
        utilisateur->cooldown_agilite = tech->rechargement;
    }
}

void appliquer_technique(Combattant* utilisateur, TechniqueSpeciale* tech, Combattant* cible) {
    if (strstr(tech->description, "Augmente") || strstr(tech->description, "Renforce") || 
        strstr(tech->description, "Protection") || strstr(tech->description, "Resistance") ||
        strstr(tech->description, "Acceleration") || strstr(tech->description, "Déplacement") ||
        strstr(tech->description, "Esquive") || strstr(tech->description, "Contre-attaque") ||
        strstr(tech->description, "Barrière") || strstr(tech->description, "Endurance")) {
        // Effet de boost positif
        if (strstr(tech->description, "attaque") || strstr(tech->description, "Attaque")) {
            cible->attaque += tech->valeur;
            cible->effets.attaque_boost = tech->valeur;
            printf("%s: Attaque augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        else if (strstr(tech->description, "defense") || strstr(tech->description, "Defense") ||
                 strstr(tech->description, "Protection") || strstr(tech->description, "Resistance") ||
                 strstr(tech->description, "Barrière") || strstr(tech->description, "Endurance")) {
            cible->defense += tech->valeur;
            cible->effets.defense_boost = tech->valeur;
            printf("%s: Defense augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        else if (strstr(tech->description, "agilite") || strstr(tech->description, "Agilite") ||
                 strstr(tech->description, "Acceleration") || strstr(tech->description, "Déplacement") ||
                 strstr(tech->description, "Esquive")) {
            cible->agilite += tech->valeur;
            cible->effets.agilite_boost = tech->valeur;
            printf("%s: Agilite augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        cible->effets.tours_restants = tech->duree;
    } 
    else if (strstr(tech->description, "Soin") || strstr(tech->description, "soin")) {
        // Effet de soin
        int nouveau_pv = cible->pv + tech->valeur;
        if (nouveau_pv > cible->pv_max) nouveau_pv = cible->pv_max;
        printf("%s soigne %s de %d PV! (%d -> %d)\n", 
               utilisateur->nom, cible->nom, tech->valeur, cible->pv, nouveau_pv);
        cible->pv = nouveau_pv;
    }
    else {
        // Dégâts normaux (uniquement pour les techniques d'attaque)
        if (strcmp(tech->type_cible, "ennemi") == 0) {
            cible->pv -= tech->valeur;
            printf("%s utilise %s sur %s et inflige %d degats!\n", 
                   utilisateur->nom, tech->nom, cible->nom, tech->valeur);
            if (cible->pv <= 0) {
                cible->pv = 0;
                printf("%s est K.O.!\n", cible->nom);
            }
        } else {
            printf("%s utilise %s sur %s pour un effet bénéfique!\n",
                   utilisateur->nom, tech->nom, cible->nom);
        }
    }
}

void attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse) {
    Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
    
    // Calcul de l'esquive basée sur l'agilité (10% de base + différence d'agilité)
    int chance_esquive = 10 + (cible->agilite - attaquant->agilite);
    if (chance_esquive < 5) chance_esquive = 5;  // Minimum 5% de chance
    if (chance_esquive > 40) chance_esquive = 40; // Maximum 40% de chance
    
    if (rand() % 100 < chance_esquive) {
        printf("%s esquive l'attaque grace a son agilite (%d%% chance)!\n", cible->nom, chance_esquive);
        return;
    }
    
    // Calcul des dégâts avec pourcentage de réduction
    float reduction = (float)cible->defense / (cible->defense + 100);
    int degats = attaquant->attaque * (1 - reduction);
    if (degats < 1) degats = 1;
    
    cible->pv -= degats;
    printf("%s attaque %s et inflige %d degats! (Reduction: %.0f%%)\n", 
           attaquant->nom, cible->nom, degats, reduction*100);
    
    if (cible->pv <= 0) {
        cible->pv = 0;
        printf("%s est K.O.!\n", cible->nom);
    }
}

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

void diminuer_cooldowns(Combattant* c) {
    if (c->cooldown_attaque > 0) c->cooldown_attaque--;
    if (c->cooldown_defense > 0) c->cooldown_defense--;
    if (c->cooldown_agilite > 0) c->cooldown_agilite--;
}

int verifier_equipe_ko(Equipe e) {
    return (e.fighter_1->pv <= 0 && e.fighter_2->pv <= 0 && e.fighter_3->pv <= 0);
}


void jouer_ia_autonome(Equipe* equipe_ia, Equipe* equipe_joueur, int difficulte){
    Combattant* actif = choisir_combattant_autonome(equipe_ia);
    if (!actif) return;

    printf("\nIA (%s) utilise %s:\n", 
           (difficulte == 1) ? "NOOB" : (difficulte == 2) ? "FACILE" : "MOYEN",
           actif->nom);

    // NOOB : seulement attaque de base aléatoire
    if (difficulte == 1) {
        ia_attaque_de_base(actif, equipe_joueur);  // <-- Utilisation de la version IA
        return;
    }

    // FACILE/MOYEN : logique optimisée
    TechniqueSpeciale* tech = NULL;
    Combattant* cible = NULL;

    // 1. Priorité aux attaques spéciales si disponibles
    if (actif->cooldown_attaque == 0) {
        tech = &actif->spe_attaque;
        cible = choisir_cible_autonome(equipe_joueur, "ennemi", difficulte);
    } 
    // 2. Soins/protections sur alliés faibles
    else if (actif->cooldown_defense == 0 && 
             (strstr(actif->spe_defense.description, "Soin") || 
              strstr(actif->spe_defense.description, "Protection"))) {
        tech = &actif->spe_defense;
        cible = choisir_cible_autonome(equipe_ia, "allie", difficulte);
    }
    // 3. Buffs d'agilité
    else if (actif->cooldown_agilite == 0) {
        tech = &actif->spe_agilite;
        cible = choisir_cible_autonome(equipe_ia, "allie", difficulte);
    }

    // Application de l'action choisie
    if (tech && cible) {
        ia_appliquer_technique(actif, tech, cible);  // <-- Utilisation de la version IA
        if (tech == &actif->spe_attaque) actif->cooldown_attaque = tech->rechargement;
        else if (tech == &actif->spe_defense) actif->cooldown_defense = tech->rechargement;
        else if (tech == &actif->spe_agilite) actif->cooldown_agilite = tech->rechargement;
    } else {
        // Fallback : attaque de base
        ia_attaque_de_base(actif, equipe_joueur);  // <-- Utilisation de la version IA
    }

    appliquer_effets(actif);
    diminuer_cooldowns(actif);
}

void ia_attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse) {
    Combattant* cible = choisir_cible_autonome(equipe_adverse, "ennemi", 2); // Mode FACILE par défaut
    int degats = attaquant->attaque - (cible->defense / 2);
    if (degats < 1) degats = 1;
    
    cible->pv -= degats;
    printf("%s attaque %s (%d PV restants)\n", attaquant->nom, cible->nom, cible->pv);
}

void ia_appliquer_technique(Combattant* utilisateur, TechniqueSpeciale* tech, Combattant* cible) {
    printf("%s utilise %s sur %s\n", utilisateur->nom, tech->nom, cible->nom);
    
    if (strstr(tech->description, "Soin") || strstr(tech->description, "soin")) {
        cible->pv = (cible->pv + tech->valeur > cible->pv_max) ? cible->pv_max : cible->pv + tech->valeur;
    } 
    else if (strcmp(tech->type_cible, "ennemi") == 0) {
        cible->pv -= tech->valeur;
        if (cible->pv < 0) cible->pv = 0;
    } 
    else { // Buffs
        if (strstr(tech->description, "attaque")) cible->attaque += tech->valeur;
        if (strstr(tech->description, "defense")) cible->defense += tech->valeur;
        if (strstr(tech->description, "agilite")) cible->agilite += tech->valeur;
    }
}

int choisir_difficulte() {
    const int largeur = 50;
    int choix;
    char input[10];
    
    do {
        printf("\n");
        afficher_separateur(largeur);
        printf("| %-48s |\n", "CHOIX DE LA DIFFICULTE");
        afficher_separateur(largeur);
        printf("| %-48s |\n", "1. Noob (facile)");
        printf("| %-48s |\n", "2. Facile");
        printf("| %-48s |\n", "3. Moyen");
        afficher_separateur(largeur);
        printf("| Choix: %-40s |\n", "");
        
        // Positionnement curseur
        printf("\033[A\033[9C");
        
        if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &choix) == 1) {
            if (choix >= 1 && choix <= 3) break;
        }
        printf("Choix invalide! (1-3)\n");
    } while (1);
    
    return choix;
}

void combat_pvp(Equipe equipe1, Equipe equipe2) {
    printf("\nLe combat commence!\n");
    srand(time(NULL));
    
    int tour = 1;
    int equipe1_ko = 0, equipe2_ko = 0;
    
    while (!equipe1_ko && !equipe2_ko) {
        printf("\n=== TOUR %d ===\n", tour);
        afficher_combat(equipe1, equipe2);
        
        equipe1_ko = verifier_equipe_ko(equipe1);
        equipe2_ko = verifier_equipe_ko(equipe2);
        if (equipe1_ko || equipe2_ko) break;
        
        printf("\n--- Tour de l'equipe %s ---\n", equipe1.Nom_equipe);
        jouer_tour(&equipe1, &equipe2);
        
        equipe2_ko = verifier_equipe_ko(equipe2);
        if (equipe2_ko) break;
        
        printf("\n--- Tour de l'equipe %s ---\n", equipe2.Nom_equipe);
        jouer_tour(&equipe2, &equipe1);
        
        tour++;
    }
    
    if (equipe1_ko) {
        printf("\nL'equipe %s a gagne!\n", equipe2.Nom_equipe);
    } else {
        printf("\nL'equipe %s a gagne!\n", equipe1.Nom_equipe);
    }
    
    liberer_equipe(equipe1);
    liberer_equipe(equipe2);
}

void combat_autonome(Equipe equipe_joueur, Equipe equipe_ia, int difficulte) {
    int tour = 1;
    while (!verifier_equipe_ko(equipe_joueur) && !verifier_equipe_ko(equipe_ia)) {
        printf("\n=== TOUR %d ===\n", tour++);
        afficher_combat(equipe_joueur, equipe_ia);

        // Tour joueur (manuel)
        printf("\n--- VOTRE TOUR ---\n");
        jouer_tour(&equipe_joueur, &equipe_ia);
        if (verifier_equipe_ko(equipe_ia)) break;

        // Tour IA (100% autonome)
        printf("\n--- TOUR IA ---\n");
        jouer_ia_autonome(&equipe_ia, &equipe_joueur, difficulte);
    }

    printf(verifier_equipe_ko(equipe_joueur) ? "\nL'IA a gagne!\n" : "\nVous avez gagne!\n");
    liberer_equipe(equipe_joueur);
    liberer_equipe(equipe_ia);
}
