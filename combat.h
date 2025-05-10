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
void combat_pvp(Equipe equipe1, Equipe equipe2);
void combat_pve_simple(Equipe equipe_joueur, Equipe equipe_pnj);
void attaque_de_base_pnj(Combattant* attaquant, Equipe* equipe_adverse);

