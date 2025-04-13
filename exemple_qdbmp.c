/*
	EXEMPLE D'UTILISATION DU MODULE QUICK & DIRTY BMP

	D'UNE SEULE IMAGE ORIGINALE LUE (BMP_ReadFile)
	ON VA CRÉER TROIS NOUVEAUX BITMAPS (BMP_Create) POUR :

	1- COPIER TEL QUEL LE QUART DE L'IMAGE ORIGINALE
	2- COPIER UNIQUEMENT LE VERT DE LA DEMIE GAUCHE DE L'IMAGE 
	3- SEUILLER À 0.8 LE ROUGE DES PIXELS DE L'IMAGE COMPLÈTE
*/

/*	
	CHOISIR VOTRE IMAGE :
	choisissez une image *.bmp 
	copiez-la avec les fichiers du projet ( .h et .c )
    mettre le nom complet de l'image lorsque demandé
	tout va fonctionner
*/

#define  _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

#include "qdbmp.h"    //le module de traitement de Bitmaps

#define SEUIL_ROUGE 0.8    //seuil de 80%

int main(void) {

	// pour conserver les valeurs RGB d'un pixel
	UCHAR red, green, blue;

	// image originale et ses valeurs
	char nom_image[80];
	BMP * original ;
	int nb_col, nb_lig, codage; 

	// la nouvelle image extraite de l'originale et ses valeurs de taille et d'offsets
	BMP * nouv_image;
	int col_depart, lig_depart, colonnes, lignes;

	// les iterateurs matriciels des pixels de l'image
	int  pos_x, pos_y;

	// demander le nom du fichier bitmap
	printf("Quel est le nom du fichier Bitmap? ");
	scanf("%s", nom_image);

	// ouvrir le fichier original et obtenir ses caractéristiques principales
	original = BMP_ReadFile(nom_image);
	assert(original);                    //arrêt du programme si == NULL

	nb_lig = BMP_GetHeight(original);
	nb_col = BMP_GetWidth(original);
	codage = BMP_GetDepth(original);     //nombre de bits par pixel

	// PREMIERE EXTRACTION (en vert uniquement)
	
	// les tailles de la sous-image vont dépendre de celles de l'original
	// on veut copier le QUART de l'image originale
	lig_depart = nb_lig / 4;
	lignes = nb_lig / 2;
	col_depart = nb_col / 4;
	colonnes = nb_col / 2;
	nouv_image = BMP_Create(colonnes, lignes, codage);

	// extraire le quart de l'image originale et le copier tel quel
	for (pos_x = 0; pos_x < colonnes; ++pos_x) {
		for (pos_y = 0; pos_y < lignes; ++pos_y) {
			// obtenir les couleurs du pixel à la position décalée
			BMP_GetPixelRGB(original, col_depart + pos_x, lig_depart + pos_y, 
				            &red, &green, &blue);

			// écrire ce pixel dans la nouvelle image (à partir de [0, 0])
			BMP_SetPixelRGB(nouv_image, pos_x, pos_y, red, green, blue);
		}
	}

	// sauvegarder l'image et libérer ensuite l'espace occupé par le BMP
	BMP_WriteFile(nouv_image, "nouvelle00.bmp");
	BMP_Free(nouv_image);

	// SECONDE EXTRACTION EN VERT

	// les tailles de la sous-image vont dépendre de celles de l'original
	// on veut extraire la moitié de GAUCHE de l'image
	lignes = nb_lig;
	colonnes = nb_col / 2;
	nouv_image = BMP_Create(colonnes, lignes, codage);

	// extraire la demie gauche de l'image originale et le copier en teinte verte unique 
	for (pos_x = 0; pos_x < colonnes; ++pos_x) {
		for (pos_y = 0; pos_y < lignes; ++pos_y) {
			// obtenir les couleurs du pixel à la position [pos_y, pos_x]
			BMP_GetPixelRGB(original, pos_x, pos_y, &red, &green, &blue);

			// on copie que la valeur du VERT (autres couleurs = 0)
			BMP_SetPixelRGB(nouv_image, pos_x, pos_y, 0, green, 0);
		}
	}

	// sauvegarder l'image et libérer ensuite l'espace occupé par le BMP
	BMP_WriteFile(nouv_image, "nouvelle01.bmp");
	BMP_Free(nouv_image);

	// LA TROISIEME IMAGE EXTRAITE VA SEUILLER LE ROUGE 
	// seul un rouge foncé à 80% et plus est conservé aux pixels

	// les tailles de la sous-image vont dépendre de celles de l'original (MËME taille)
	lignes = nb_lig;
	colonnes = nb_col;
	nouv_image = BMP_Create(colonnes, lignes, codage);

	// extraire l'image originale et le copier en seuillant le rouge de chaque pixel
	for (pos_x = 0; pos_x < colonnes; ++pos_x) {
		for (pos_y = 0; pos_y < lignes; ++pos_y) {
			// obtenir les couleurs du pixel à la position [pos_y, pos_x]
			BMP_GetPixelRGB(original, pos_x, pos_y, &red, &green, &blue);

			// déterminer si la valeur de rouge est suffisante (80% de 255 et plus)
			red = (red / 255.0) < SEUIL_ROUGE ? 0 : red;

			// on copie la valeur du ROUGE ou 0 si en bas du seuil (autres couleurs tel quel)
			BMP_SetPixelRGB(nouv_image, pos_x, pos_y, red, green, blue);
		}
	}

	// sauvegarder l'image et libérer ensuite l'espace occupé par le BMP
	BMP_WriteFile(nouv_image, "nouvelle02.bmp");
	BMP_Free(nouv_image);

	// le free fonctionnel du BMP original
	BMP_Free(original);

	// c'est fini
	printf("3 nouvelles images ont ete copiees dans le repertoire de travail..\n");
	system("pause");
	return 0;
}