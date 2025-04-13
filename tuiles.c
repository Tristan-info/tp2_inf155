//
// Created by Rayan Darif on 2025-04-01.
//
#include "tuiles.h"

#include <assert.h>

#include "qdbmp.h"
#include <stdlib.h>


int get_nb_tuiles(BMP *original, int nbcol_tuile, int nblig_tuile) {
    if (original == NULL || nbcol_tuile <= 0 || nblig_tuile <= 0) return 0;

    UINT largeur = BMP_GetWidth(original);
    UINT hauteur = BMP_GetHeight(original);

    UINT nb_tuiles_col = largeur / nbcol_tuile;
    UINT nb_tuiles_lig = hauteur / nblig_tuile;

    return nb_tuiles_col * nb_tuiles_lig;
}


void init_tuile(int nbcol_tuile, int nblig_tuile, t_tuile *tuile) {
    if (tuile == NULL) return;

    tuile->nb_col = nbcol_tuile;
    tuile->nb_lig = nblig_tuile;

    tuile->id_enum = -1;
    tuile->offset_col = -1;
    tuile->offset_lig = -1;
}

int get_pos_kieme_tuile(BMP *original, int k, t_tuile *tuile) {
    if (original == NULL || tuile == NULL) return 0;

    UINT largeur_img = BMP_GetWidth(original);
    UINT hauteur_img = BMP_GetHeight(original);

    int largeur_tuile = tuile->nb_col;
    int hauteur_tuile = tuile->nb_lig;

    int nb_tuiles_col = largeur_img / largeur_tuile;
    int nb_tuiles_lig = hauteur_img / hauteur_tuile;

    // k invalide ?
    if (k < 0 || k >= get_nb_tuiles(original, tuile->nb_col, tuile->nb_lig)) {
        return 0;
    }

    int lig = k / nb_tuiles_col; // ligne de la tuile
    int col = k % nb_tuiles_col; // colonne de la tuile

    // On assigne les valeurs à la tuile
    tuile->id_enum = k;
    tuile->offset_col = col * largeur_tuile;
    tuile->offset_lig = lig * hauteur_tuile;

    return 1;
}


double calcul_integrale_seuil_lumin(t_spectre_gris *ptr_sp, double seuil) {
    // Valider le seuil : il doit être entre 0.0 et 1.0 inclus
    if (seuil < 0.0 || seuil > 1.0) {
        return -1;
    }

    double somme = 0.0;

    // Calcul de l'intégrale avec le seuil
    for (int i = seuil * (NB_FREQUENCES - 1); i < NB_FREQUENCES; i++) {
        // multiplication seuil par 255 peut retourner une valeur non int. (a verifier)
        double intensite = (double) i / (NB_FREQUENCES - 1);


        somme += ptr_sp->spectre[i] * intensite;
    }

    // Met à jour les champs associés à l'intégrale avec seuil
    ptr_sp->seuil_lumin = seuil;
    ptr_sp->integrale_lumin_seuil = somme;

    return somme;
}

t_spectre_gris *creer_spectre_tuile(BMP *original, const t_tuile *tuile) {
    if (original == NULL || tuile == NULL) return NULL;

    // Allouer dynamiquement le spectre
    t_spectre_gris *sp = (t_spectre_gris *) malloc(sizeof(t_spectre_gris));
    assert(sp != NULL);

    // Initialiser le spectre à 0
    for (int i = 0; i < NB_FREQUENCES; i++) {
        sp->spectre[i] = 0;
    }

    // Copier la tuile associée
    sp->tuile = *tuile;

    // Récupération des infos sur la tuile
    int debut_x = tuile->offset_col;
    int debut_y = tuile->offset_lig;
    int largeur = tuile->nb_col;
    int hauteur = tuile->nb_lig;

    // Parcours de tous les pixels de la tuile
    for (int y = debut_y; y < debut_y + hauteur; y++) {
        for (int x = debut_x; x < debut_x + largeur; x++) {
            UCHAR r, v, b;
            BMP_GetPixelRGB(original, x, y, &r, &v, &b);

            // Teinte de gris selon la formule
            double gris = 0.299 * r + 0.587 * v + 0.114 * b;
            int intensite = (int) gris;

            sp->spectre[intensite]++;
        }
    }

    // Calcul de l'intégrale complète (sans seuil)
    double somme = 0.0;
    for (int i = 0; i < NB_FREQUENCES; i++) {
        double intensite = (double) i / (NB_FREQUENCES - 1);
        somme += sp->spectre[i] * intensite;
    }

    sp->integrale_lumin_compl = somme;
    sp->seuil_lumin = -1.0; // pas encore utilsé
    sp->integrale_lumin_seuil = 0.0; // valeur de base

    return sp;
}


BMP *get_bitmap_tuile(BMP *original, const t_tuile *tuile) {
    int largeur = tuile->nb_col;
    int hauteur = tuile->nb_lig;

    int decalage_x = tuile->offset_col;
    int decalage_y = tuile->offset_lig;

    // Obtenir la profondeur (nombre de bits par pixel) de l'image originale
    UINT depth = BMP_GetDepth(original);

    // Créer un nouveau BMP vide avec les mêmes propriétés
    BMP *nouveau_bmp = BMP_Create(largeur, hauteur, depth);
    assert(nouveau_bmp != NULL);
    // Copier pixel par pixel depuis l'image originale
    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            UCHAR r, v, b;

            // Lire le pixel de l'image originale
            BMP_GetPixelRGB(original, decalage_x + x, decalage_y + y, &r, &v, &b);

            // Écrire le même pixel dans le nouveau BMP
            BMP_SetPixelRGB(nouveau_bmp, x, y, r, v, b);
        }
    }

    return nouveau_bmp;
}


BMP *get_bitmap_gris_tuile(BMP *original, const t_tuile *tuile, double seuil_lum) {
    if (original == NULL || tuile == NULL) return NULL;
    if (seuil_lum < 0.0 || seuil_lum > 1.0) return NULL;

    int largeur = tuile->nb_col;
    int hauteur = tuile->nb_lig;
    int decalage_x = tuile->offset_col;
    int decalage_y = tuile->offset_lig;

    UINT depth = BMP_GetDepth(original);
    BMP *bmp_gris = BMP_Create(largeur, hauteur, depth);
    assert(bmp_gris != NULL);
    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            UCHAR r, v, b;
            BMP_GetPixelRGB(original, decalage_x + x, decalage_y + y, &r, &v, &b);

            // Calcul de la teinte de gris
            double gris = 0.299 * r + 0.587 * v + 0.114 * b;
            double intensite = gris / 255.0;

            UCHAR valeur_finale;

            if (intensite >= seuil_lum) {
                // Conserver la valeur de gris
                valeur_finale = (UCHAR) gris;
            } else {
                // si trop sombre
                valeur_finale = 0;
            }

            // Appliquer même valeur sur R, V, B pour une image en gris
            BMP_SetPixelRGB(bmp_gris, x, y, valeur_finale, valeur_finale, valeur_finale);
        }
    }

    return bmp_gris;
}

//retourne K (ID)
int get_id_tuile(const t_tuile *tu) {
    if (tu == NULL) return -1;
    return tu->id_enum;
}

//copie les décalages
void get_offsets_tuile(const t_tuile *tu, int *offset_x, int *offset_y) {
    if (tu == NULL || offset_x == NULL || offset_y == NULL) return;

    *offset_x = tu->offset_col;
    *offset_y = tu->offset_lig;
}

double get_integrale_seuil0(const t_spectre_gris *ptr_sp) {
    if (ptr_sp == NULL) return -1.0;
    return ptr_sp->integrale_lumin_compl;
}

double get_seuil_variant(const t_spectre_gris *ptr_sp) {
    if (ptr_sp == NULL) return -1.0;
    return ptr_sp->seuil_lumin;
}

double get_integrale_seuil_variant(const t_spectre_gris *ptr_sp) {
    if (ptr_sp == NULL) return -1.0;
    return ptr_sp->integrale_lumin_seuil;
}

void get_tuile(const t_spectre_gris *ptr_sp, t_tuile *tuile) {
    if (ptr_sp == NULL || tuile == NULL) return;

    *tuile = ptr_sp->tuile;
}


//partie 2:

void calibrer_taille_tuile(BMP *original, int *nb_col, int *nb_lig) {
    UINT largeur = BMP_GetWidth(original);
    UINT hauteur = BMP_GetHeight(original);

    int meilleure_col = *nb_col;
    int reste_col = largeur % (*nb_col);

    while (reste_col <= meilleure_col) {
        meilleure_col++;
        reste_col = largeur % (meilleure_col);
    }

    int meilleure_lig = *nb_lig;
    int reste_lig = hauteur % (*nb_lig);

    while (reste_lig <= meilleure_lig) {
        meilleure_lig++;
        reste_lig = hauteur % (meilleure_lig);
    }

    *nb_col = meilleure_col;
    *nb_lig = meilleure_lig;
}

int tuiles_voisines(const t_tuile *tuile1, const t_tuile *tuile2) {
    if ((tuile1==NULL) || (tuile2==NULL)) return 0;

    int x1 = tuile1->offset_col;
    int y1 = tuile1->offset_lig;
    int x2 = tuile2->offset_col;
    int y2 = tuile2->offset_lig;

    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);

    return (dx <= tuile1->nb_col && dy <= tuile1->nb_lig && !(dx == 0 && dy == 0));
}

void copier_tuile_a_image(BMP *imag_res, BMP *imag, const t_tuile *tuile) {
    if ((imag == NULL) || (imag_res == NULL) || (tuile == NULL)) return;

    int largeur = tuile->nb_col;
    int hauteur = tuile->nb_lig;
    int decalage_x = tuile->offset_col;
    int decalage_y = tuile->offset_lig;

    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            UCHAR r, g, b;
            BMP_GetPixelRGB(imag, decalage_x + x, decalage_y + y, &r, &g, &b);
            BMP_SetPixelRGB(imag_res, decalage_x + x, decalage_y + y, r, g, b);
        }
    }
}








