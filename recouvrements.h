//
// Created by Rayan Darif on 2025-04-10.
//

#ifndef RECOUVREMENTS_H
#define RECOUVREMENTS_H

#include "tuiles.h"
#include "qdbmp.h"

typedef struct {
    t_spectre_gris **tab_spectres; // Tableau dynamique des spectres (pointeurs)
    int taille_tab_spectres; // Taille du tableau
    int largeur_tuile; // Largeur d'une tuile
    int hauteur_tuile; // Hauteur d'une tuile
} t_recouvrement;

/** CONSTRUCTEUR **/
t_recouvrement *init_recouvrement(int nb_tuiles, int nbcol_tuile, int nblig_tuile);

/** DESTRUCTEUR **/
void detruire_recouvrement(t_recouvrement *rec);

/** AJOUT D'UN SPECTRE À LA POSITION id_num **/
int ajouter_spectre_rec(t_spectre_gris *ptr_sp, t_recouvrement *rec);

/** OBTENTION D'UN SPECTRE PAR SON NUMÉRO D'ID **/
t_spectre_gris *get_kieme_ptr_sp(const t_recouvrement *rec, int k);

/** CALCUL DES INTÉGRALES AVEC SEUIL POUR TOUS LES SPECTRES **/
void calcul_integrales_seuil(t_recouvrement *rec, double seuil);

/** TRI DES SPECTRES PAR LUMINOSITÉ (AVEC OU SANS SEUIL) **/
void trier_spectres(t_recouvrement *rec, double seuil);

/** RECONSTRUCTION D'UNE NOUVELLE IMAGE À PARTIR DES TUILES RETENUES **/
BMP *reconstruire_image(BMP *original, const t_recouvrement *rec,
                        double prop_garde, double prop_min,
                        char *fichier_log);


#endif //RECOUVREMENTS_H
