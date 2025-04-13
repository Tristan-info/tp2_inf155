/**************************************************************
PAR : Hugues Saulnier. Révisé par Eric Thé
DATE: Hiver 2025

Le module d'outils demandé dans la première partie du TP2.

Le module offre les types-struct : t_tuile, t_spectre_gris

Le module offre les fonctions suivantes :
 - get_nb_tuiles()
 - init_tuile()
 - get_pos_kieme_tuile()
 - calcul_integrale_seuil_lumin()
 - creer_spectre_tuile()
 - get_bitmap_tuile()
 - get_bitmap_gris_tuile()
 - get_id_tuile()
 - get_offsets_tuile()
 - get_integrale_seuil0()
 - get_seuil_variant()
 - get_integrale_seuil_variant()
 - get_tuile()
 - afficher_spectre()

 - calibrer_taille_tuile()  //PARTIE 2
 - tuiles_voisines()		//PARTIE 2
 - copier_tuile_a_image()	//PARTIE 2

****************************************************************/

#ifndef __OUTILS_41028955_H__
#define __OUTILS_41028955_H__

//************************************************************** 

// merci Chai Braudo pour le beau petit module Quick n' Dirty BMP
#include "qdbmp.h"

#define  NB_FREQUENCES  256     //le nombre d'intensités et taille du spectre


//************************************************************** 
//**************************************************************
// DEUX TYPES ESSENTIELS dont voici les définitions

/*
  LA TUILE
  une tuile a une taille bien définie et occupe une position bien definie
  dans un bitmap original. Elle est représentée par 5 entiers
*/
typedef struct {
	// les decalages dans l'image originale
	int offset_col; // decalage-colonne
	int offset_lig; // decalage-ligne

	// sa taille véritable en pixels
	int nb_lig;
	int nb_col;

	// son numéro unique dans l'énumération des tuiles d'une image
	int id_enum;
} t_tuile;

/*
  LE SPECTRE
  c'est dans un spectre qu'on va conserver tout ce qui est pertinent à
  la mesure de la luminosité d'une tuile
*/
typedef struct {
	// la tuile dont est issu le spectre est conservé
	t_tuile tuile;

	// le tableau des fréquences de chaque teinte de gris dans la tuile
	UINT spectre[NB_FREQUENCES];

	// intégrale complète de la luminosité d'un spectre SANS seuil (= 0) obtenue
	// de la somme des produits ((i/255.0) * spectre[i]) pour i allant de 0 à 255
	double integrale_lumin_compl;

	// les deux derniers membres sont associés à une mesure de luminosité seuillée
	double seuil_lumin; // le seuil entre 0 et 1

	// intégrale avec seuil, idem à l'integrale précédente sauf que tous les i tels
	// que (i/255.0) < seuil_lumen ne seront PAS considérés dans la sommation
	double integrale_lumin_seuil;
} t_spectre_gris;


//************************************************************** 
// LES DÉCLARATIONS DES 7 FONCTIONS DE CALCULS DE LA PARTIE 1
//************************************************************** 

int get_nb_tuiles(BMP *original, int nbcol_tuile, int nblig_tuile);

void init_tuile(int nbcol_tuile, int nblig_tuile, t_tuile *tuile);

int get_pos_kieme_tuile(BMP *original, int k, t_tuile *tuile);

double calcul_integrale_seuil_lumin(t_spectre_gris *ptr_sp, double seuil);

t_spectre_gris *creer_spectre_tuile(BMP *original, const t_tuile *tuile);

BMP *get_bitmap_tuile(BMP *original, const t_tuile *tuile);

BMP *get_bitmap_gris_tuile(BMP *original, const t_tuile *tuile, double seuil_lum);

//Retourne le numéro unique d'énumération de la tuile "tu"
int get_id_tuile(const t_tuile *tu);

//Copie les deux décalages (col & lig) de la tuile "tu" dans les deux références reçues
void get_offsets_tuile(const t_tuile *tu, int *offset_x, int *offset_y);

//Retourne l'intégrale à seuil 0 (intégrale complète sans seuil)
double get_integrale_seuil0(const t_spectre_gris *ptr_sp);

//Retourne le seuil utilisé dans l'intégrale optionnelle (-1 par défaut)
double get_seuil_variant(const t_spectre_gris *ptr_sp);


//Retourne l'intégrale optionnelle calculée (sera 0 si non-calculée)
double get_integrale_seuil_variant(const t_spectre_gris *ptr_sp);


//Place dans la référence la copie de la tuile originale de ce spectre
void get_tuile(const t_spectre_gris *ptr_sp, t_tuile *tuile);


// partie 2:

//meilleur ajustement de la taille de la tuile dans l'image originale
void calibrer_taille_tuile(BMP *original, int *nb_col, int *nb_lig);

//indique si deux tuiles sont voisines
int tuiles_voisines(const t_tuile *tuile1, const t_tuile *tuile2);

//copier le contenu d'un bitmap tuile dans une image-résultat pixel par pixel selon taille et décalage
void copier_tuile_a_image(BMP *imag_res, BMP *imag, const t_tuile *tuile);

#endif BMP

