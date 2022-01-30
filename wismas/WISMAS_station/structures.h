/**
 * @file Structures.h
 *
 * @brief Regroupement de structures de données
 *
 * @author PETRELLA Olivier
 *
 * @version 1.1
 *
 * @date 21 mars 2018
 */

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>

/**
 * @struct CONFIG_LOGICIEL
 * @brief fixe la configuration de départ de l'application.
 */
struct CONFIG_LOGICIEL
{
    int     nb_station; /*!< défini le nombre de station */
    long    periode; /*!< change l'affichage d'une station en seconde */
};

/**
 * \brief struct CONFIG_ENREGISTREMENT fixe la configuration des données à enregistrer
 *
 * \var int periode
 */
struct CONFIG_ENREGISTREMENT
{
    int     periode; // défini une période d'enregistrement en base de données
};

/**
 * @brief struct PARAM_COMMUNICATION fixe les paramètres du périphérique d'échange avec les stations.
 * @var QString port nom du port serie d'entrée du périphérique,
 * int baud_rate,
 * int data_bit,
 * int stop,
 * int parity
 */
struct PARAM_COMMUNICATION
{
    QString port; /*!< nom du port serie d'entrée du périphérique */
    int     baud_rate;
    int     data_bit;
    int     stop;
    int     parity;
};

/**
 * @struct PARAM_PANNEAU
 * @brief fixe les paramètres du périphérique panneau lumineux.
 * port QString,
 * int baud_rate,
 * int data_bit,
 * int stop,
 * int parity
 */
struct PARAM_PANNEAU
{
    QString port; /*!< nom du port serie d'entrée du panneau lumineux */
    int     baud_rate;
    int     data_bit;
    int     stop;
    int     parity;
};


/**
 * @struct AFFICHAGE_PANNEAU
 * @brief fixe la configuration de l'affichage des informations sur le panneau lumineux.
 * titre QString,
 * int periode,
 * bool temperature_air,
 * bool temperature_neige,
 * bool hauteur_neige,
 * bool humidite,
 * bool pression,
 * bool vitesse_vent,
 */
struct AFFICHAGE_PANNEAU
{
    QString titre;
    int     periode;
    QString taille_text;
    int     temps_text;
    QString effet_transition;
    bool    nom_station;
    bool    date_heure;
    bool    temperature_air;
    bool    temperature_neige;
    bool    hauteur_neige;
    bool    humidite;
    bool    pression;
    bool    vitesse_vent;
};

/**
 * @struct DONNEES_STATION
 * @brief fixe les données d'un station
 *
 *
 */
struct DONNEES_STATION
{
    QString dateDonnes;
    QString heureDonnes;
    QString tarifs;
    QString horaire;
    QString nbPisteOuverte;
    QString directionVent;
    QString vitesseVent;
    QString temperatureAir;
    QString temperatureNeige;
    QString hauteurNeige;
    QString humidite;
    QString pressionAir;
};

#endif // STRUCTURES_H
