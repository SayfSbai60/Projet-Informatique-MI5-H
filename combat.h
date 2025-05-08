#ifndef COMBAT_H
#define COMBAT_H

#include "combattant.h"

void jouer_tour(Equipe* equipe_actuelle, Equipe* equipe_adverse);
void utiliser_technique_speciale(Combattant* utilisateur, TechniqueSpeciale* tech, Equipe* equipe_adverse, Equipe* equipe_alliee);
void appliquer_technique(Combattant* utilisateur, TechniqueSpeciale* tech, Combattant* cible);
void attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse);
void appliquer_effets(Combattant* c);
void diminuer_cooldowns(Combattant* c);
int verifier_equipe_ko(Equipe e);

// PvP / PvE
void ia_attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse);
void ia_appliquer_technique(Combattant* utilisateur, TechniqueSpeciale* tech, Combattant* cible);
void jouer_ia_autonome(Equipe* equipe_ia, Equipe* equipe_joueur, int difficulte);
void combat_pvp(Equipe equipe1, Equipe equipe2);
void combat_autonome(Equipe equipe_joueur, Equipe equipe_ia, int difficulte);


int choisir_difficulte();

#endif