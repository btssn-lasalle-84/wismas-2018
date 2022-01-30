#ifndef STATIONMETEO_H
#define STATIONMETEO_H

/**
  *
  * \file stationmeteo.h
  *
  * \brief Déclaration de la classe StationMeteo
  *
  * \author Petrella Olivier
  *
  * \version 1.1
  *
  */

#include <QVector>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

#include "structures.h"
#include "wismasprotocole.h"

#define SKI_ALPIN 1
#define SKI_DE_FOND 2
#define SNOW_SKITE 3

#define DIRECTION_VENT 1
#define VITESSE_VENT 2
#define TEMPERATURE_AIR 3
#define TEMPERATURE_NEIGE 4
#define HAUTEUR_NEIGE 5
#define HUMIDITE 6
#define PRESSION_AIR 7


//#define DEBUG

class BaseDeDonnees;

/**
 * @brief Gére les données météos d'une station d'un site
 *
 * @class StationMeteo
 */
class StationMeteo : public QObject
{
    Q_OBJECT

    private:
        BaseDeDonnees *bdd; /**< \brief Association vers la classe BaseDeDonnees */
        int idSite; /**< \brief  */
        int typeSite; /**< \brief  */
        QString nomSite;
        QString cheminVideo;
        QString couleurTextPanneau;
        WISMASProtocole* protocole; /**< \brief Association vers la classe WISMASProtocole */
        DONNEES_STATION donneeStation;
        QVector<DONNEES_STATION> donneesStation;
        QTime heureDebut;
        QTime heureFin;
        QTimer* timer_baseDeDonnees;
        QString directionVent;
        QString vitesseVent;
        QString temperatureAir;
        QString temperatureNeige;
        QString hauteurNeige;
        QString humidite;
        QString pressionAir;
        QString nomVideo;
        int heureCourante;

        bool preparerDonnees(DONNEES_STATION &donneesEnregistrement);
        void enregistrerDonnees();

    public:
        StationMeteo(QObject *parent = 0);
        StationMeteo(int idSite, int typeSite, QString nomSite, QString cheminVideo, QString couleurTextPanneau, QObject* parent = 0);
        ~StationMeteo();

        int getIdSite() const;
        int getTypeSite() const;
        QString getNomTypeSite() const;
        QString getCheminVideo() const;
        QString getNomVideo() const;
        QString getMesures(int typeMesure);
        QString getDirectionVent() const;
        QString getVitesseVent() const;
        QString getTemperatureAir();
        QString getTemperatureNeige();
        QString getHauteurNeige();
        QString getHumidite() const;
        QString getPressionAir() const;
        QString getConseilsFartage(int marque) const;
        QStringList getInformationComplementaire(int idSite);

        void setIdSite(int);
        void setTypeSite(int);
        void setDonneesTrame(DONNEES_STATION);
        void setNomVideo(QString);
        void setDirectionVent(QString);
        void setVitesseVent(QString);
        void setTemperatureAir(QString);
        void setTemperatureNeige(QString);
        void setHauteurNeige(QString);
        void setHumidite(QString);
        void setPressionAir(QString);


    signals:

    public slots:
        void traiterTrame(QString trame);
};

#endif // STATIONMETEO_H
