/**
  * @file Structures.h
  *
  * @brief La structure Parametres
  *
  * @author Pierre GRELET
  *
  * @version 1.1
  *
  * @date 15 mars 2018
  *
  */

#ifndef STRUCTURES_H
#define STRUCTURES_H

/**
 * @struct Parametres
 * @brief  paramètres d'une caméra
 */
struct Parametres
{
    QString etat;
    QString nomSite;
    QString adresseIP;
    QString numeroPort;
    QString identifiant;
    QString motDePasse;
    QString typeDeplacement;
    QString cheminVideo;
    QString duree;
    QString periode;
    QString resolution;
};

#endif // STRUCTURES_H
