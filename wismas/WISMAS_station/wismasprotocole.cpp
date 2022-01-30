#include "wismasprotocole.h"
#include "stationmeteo.h"

#include <QStringList>
#include <QDate>
#include <QTime>
#include <QDebug>

/**
  *
  * \file wismasprotocole.cpp
  *
  * \brief Définition de la classe WISMASProtocole
  *
  * \author Petrella Olivier
  *
  * \version 1.1
  *
  */


/**
 * @brief WISMASProtocole() constructeur par défaut
 */
WISMASProtocole::WISMASProtocole(StationMeteo *stationMeteo) : stationMeteo(stationMeteo)
{

}

/**
 * @brief WISMASProtocole() destructeur
 */
WISMASProtocole::~WISMASProtocole()
{

}

/**
 * @brief verifie la validite de la trame
 *
 * @fn WISMASProtocole::verifierTrame
 * @param trame QString une trame
 * @return bool true si la trame est valide sinon false
 */
bool WISMASProtocole::verifierTrame(QString trame)
{
    if(trame.length() == 0)
        return false;

    if(!trame.endsWith("/r"))
        return false;

    return true;
}

/**
 * @brief decode la trame et extrait les donnees pour la station méteo
 *
 * @param trame QString une trame
 * @return bool true si la trame est decodee sinon false
 */
bool WISMASProtocole::decoderTrame(QString trame)
{
    if(!verifierTrame(trame))
        return false;

    //qDebug() << Q_FUNC_INFO << trame;

    // Extraire les champs de la trame
    QStringList champs = trame.split(',');

    // Extraire les valeurs ID et TYPE des champs de la trame
    QStringList id = champs.at(0).split('.');
    QStringList type = champs.at(1).split('.');

    // Bonne station ?
    if(id.at(1).toInt() == stationMeteo->getIdSite())
    {
        // Bon type ?
        if(type.at(1).toInt() == stationMeteo->getTypeSite())
        {
            // Extraire les données des champs de la trame
            QString direction_vent = champs.at(2).split('.')[1];
            QString vitesse_vent = champs.at(3).split('.')[1];
            QString temperature_air = champs.at(4).split('.')[1];
            QString temperature_neige = champs.at(5).split('.')[1];
            QString hauteur_neige = champs.at(6).split('.')[1];
            QString humidite = champs.at(7).split('.')[1];
            QString pression_air = champs.at(8).split('.')[1].split('/')[0];
            pression_air.chop(0);

            DONNEES_STATION donneeStation;
            QDate date = QDate::currentDate();
            QTime heure = QTime::currentTime();

            donneeStation.dateDonnes = date.toString("dd/MM/yyyy");
            donneeStation.heureDonnes = heure.toString("hh:mm:ss");
            donneeStation.directionVent = direction_vent;
            donneeStation.vitesseVent = vitesse_vent;
            donneeStation.temperatureAir = temperature_air;
            donneeStation.temperatureNeige = temperature_neige;
            donneeStation.hauteurNeige = hauteur_neige;
            donneeStation.humidite = humidite;
            donneeStation.pressionAir = pression_air;

            stationMeteo->setDonneesTrame(donneeStation);

            return true;
        }
    }

    return false;
}
