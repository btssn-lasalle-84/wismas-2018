/**
  *
  * \file stationmeteo.cpp
  *
  * \brief Définition de la classe StationMeteo
  *
  * \author Petrella Olivier
  *
  * \version 1.1
  *
  */

#include "stationmeteo.h"
#include "basededonnees.h"
#include <qmath.h>

/**
 * @brief Constructeur
 * @details Constructeur par défaut
 * @param parent QObject L'adresse d'un objet Qt parent
 */
StationMeteo::StationMeteo(QObject *parent) : QObject(parent), idSite(0),
    typeSite(0), directionVent("--"), vitesseVent("--"), temperatureAir("--"),
    temperatureNeige("--"), hauteurNeige("--"), humidite("--"), pressionAir("--"), nomVideo("")
{
    bdd = BaseDeDonnees::getInstance();
    bdd->connecter();

    this->protocole = new WISMASProtocole(this);

    heureDebut = QTime::currentTime();
    //heureFin = QTime::currentTime().addSecs(3600);
    heureFin = QTime::currentTime().addSecs(60);

    heureCourante = QDateTime::currentDateTime().toString("HH").toInt();
}

/**
 * @brief StationMeteo::StationMeteo
 * @details Constructeur par défaut
 */
StationMeteo::StationMeteo(int idSite, int typeSite, QString nomSite, QString cheminVideo, QString couleurTextPanneau, QObject* parent) : QObject(parent), idSite(idSite),
    typeSite(typeSite), nomSite(nomSite), cheminVideo(cheminVideo), couleurTextPanneau(couleurTextPanneau), directionVent("--"), vitesseVent("--"), temperatureAir("--"),
    temperatureNeige("--"), hauteurNeige("--"), humidite("--"), pressionAir("--")
{
    bdd = BaseDeDonnees::getInstance();
    bdd->connecter();

    QStringList infosSite;
    QString requete;
    bool retour;

    // Récupère les information du site
    requete = "SELECT * FROM sites WHERE idSite = '" + QString::number(idSite) + "'";
    retour = bdd->recuperer(requete, infosSite);
    if(retour)
    {
        #ifdef DEBUG
            qDebug() << QString::fromUtf8("Site : ") << infosSite.at(0) << infosSite.at(1) << infosSite.at(2) << infosSite.at(3);
        #endif
    }
    else
    {
        #ifdef DEBUG
            qDebug() << Q_FUNC_INFO << requete;
        #endif
    }

    // La station est active
    requete = "UPDATE sites SET etat = '1' WHERE idSite = '" + QString::number(idSite) + "'";
    retour = bdd->executer(requete);
    if(!retour)
    {
        #ifdef DEBUG
            qDebug() << Q_FUNC_INFO << requete;
        #endif
    }

    this->protocole = new WISMASProtocole(this);

    heureDebut = QTime::currentTime();
    //heureFin = QTime::currentTime().addSecs(3600);
    heureFin = QTime::currentTime().addSecs(60);

    heureCourante = QDateTime::currentDateTime().toString("HH").toInt();
}

/**
 * @brief StationMeteo::~StationMeteo
 */
StationMeteo::~StationMeteo()
{
    // La station est inactive
    QString requete = "UPDATE sites SET etat = '0' WHERE idSite = '" + QString::number(idSite) + "'";
    bool retour = bdd->executer(requete);
    if(!retour)
    {
        #ifdef DEBUG
            qDebug() << Q_FUNC_INFO << requete;
        #endif
    }

    BaseDeDonnees::detruireInstance();
}

/**
 * @brief StationMeteo::getIdSite
 * @details
 * @return int id l'identifiant du site sur lequel la station méteo est installée.
 */
int StationMeteo::getIdSite() const
{
    return idSite;
}

/**
 * @brief StationMeteo::getTypeSite
 * @details
 * @return int id l'identifiant du site.
 */
int StationMeteo::getTypeSite() const
{
    return typeSite;
}

/**
 * @brief StationMeteo::getCheminVideo
 * @details
 * @return QString cheminVideo.
 */
QString StationMeteo::getCheminVideo() const
{
    return cheminVideo;
}

QString StationMeteo::getNomVideo() const
{
    return this->nomVideo;
}

/**
 * @brief StationMeteo::getNomTypeSite
 * @details
 * @return QString nom le nom du type de site.
 */
QString StationMeteo::getNomTypeSite() const
{
    switch(typeSite)
    {
        case SKI_ALPIN:
            return "SKI ALPIN";
            break;
        case SKI_DE_FOND:
            return "SKI DE FOND";
            break;
        case SNOW_SKITE:
            return "SNOWKITE";
            break;
        default:
            qDebug() << Q_FUNC_INFO << " [INFO] Type de site inconnu";
            return "";
            break;
    }
}

/**
 * @brief StationMeteo::getMesures
 * @return QString
 */
QString StationMeteo::getMesures(int typeMesure)
{
    switch(typeMesure)
    {
        case DIRECTION_VENT:
            return getDirectionVent();
            break;

        case VITESSE_VENT:
            return getVitesseVent();
            break;

        case TEMPERATURE_AIR:
            return getTemperatureAir();
            break;

        case TEMPERATURE_NEIGE:
            return getTemperatureNeige();
            break;

        case HAUTEUR_NEIGE:
            return getHauteurNeige();
            break;

        case HUMIDITE:
            return getHumidite();
            break;

        case PRESSION_AIR:
            return getPressionAir();
            break;
    }
}

/**
 * @brief StationMeteo::getDirectionVent
 * @return QString
 */
QString StationMeteo::getDirectionVent() const
{
    if(this->directionVent == "500")
    {
        return "--";
    }
    else
    {
        return this->directionVent;
    }
}

/**
 * @brief StationMeteo::getVitesseVent
 * @return QString
 */
QString StationMeteo::getVitesseVent() const
{
    if(this->vitesseVent == "500")
    {
        return "-- km/h";
    }
    else
    {
        return this->vitesseVent + "km/h";
    }
}

/**
 * @brief StationMeteo::getTemperatureAir
 * @return QString
 */
QString StationMeteo::getTemperatureAir()
{
    if(this->temperatureAir == "500")
    {
        return "-- °C";
    }
    else
    {
        return this->temperatureAir + "°C";
    }
}

/**
 * @brief StationMeteo::getTemperatureNeige
 * @return QString
 */
QString StationMeteo::getTemperatureNeige()
{
    if(this->temperatureNeige == "500")
    {
        return "-- °C";
    }
    else
    {
        return this->temperatureNeige + "°C";
    }
}

/**
 * @brief StationMeteo::getHauteurNeige
 * @return QString
 */
QString StationMeteo::getHauteurNeige()
{
    if(this->hauteurNeige == "500")
    {
        return "-- cm";
    }
    else
    {
        return this->hauteurNeige + "cm";
    }
}

/**
 * @brief StationMeteo::getHumidite
 * @return QString
 */
QString StationMeteo::getHumidite() const
{
    if(this->humidite == "500")
    {
        return "-- %";
    }
    else
    {
        return this->humidite +" %";
    }
}

/**
 * @brief StationMeteo::getPressionAir
 * @return QString
 */
QString StationMeteo::getPressionAir() const
{
    if(this->pressionAir == "500")
    {
        return "-- hPa";
    }
    else
    {
        return this->pressionAir + " hPa";
    }

}

/**
 * @brief StationMeteo::getConseilsFartage
 * @param marque int marque de farte
 * @return QString la couleur du farte
 */
QString StationMeteo::getConseilsFartage(int marque) const
{
    if(donneesStation.count() == 0)
        return "--";

    switch(marque)
    {
        case 1:
            if(this->temperatureNeige.toInt() >= 0 && this->temperatureNeige.toInt() <= 10)   { return "jaune"; }
            else if(abs(this->temperatureNeige.toInt()) >= 4 && abs(this->temperatureNeige.toInt()) <= abs(-4))   { return "rouge"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-2) && abs(this->temperatureNeige.toInt()) <= abs(-8))  { return "violet";}
            else if(abs(this->temperatureNeige.toInt()) >= abs(-6) && abs(this->temperatureNeige.toInt()) <= abs(-12)) { return "bleu";  }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-10) && abs(this->temperatureNeige.toInt()) <= abs(-32)){ return "vert";  }
            else
            {
                #ifdef DEBUG
                    qDebug() << Q_FUNC_INFO << "Temperature de neige non prise en charge pour la marque SWIX";
                #endif
                return "--";
            }
            break;

        case 2:
            if(this->temperatureNeige.toInt() >= 0 && this->temperatureNeige.toInt() <= 6)    { return "jaune"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-4) && abs(this->temperatureNeige.toInt()) <= abs(-12)) { return "rouge"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-10) && abs(this->temperatureNeige.toInt()) <= abs(-30)){ return "bleu";  }
            else
            {
                #ifdef DEBUG
                    qDebug() << Q_FUNC_INFO << "Temperature de neige non prise en charge pour la marque TOKO";
                #endif
                return "--";
            }
            break;
        case 3:
            if(this->temperatureNeige.toInt() >= 14 && abs(this->temperatureNeige.toInt()) <= abs(-2))  { return "jaune"; }
            else if(this->temperatureNeige.toInt() >= 0 && abs(this->temperatureNeige.toInt()) <= abs(-5))   { return "rouge"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-2) && abs(this->temperatureNeige.toInt()) <= abs(-10)) { return "violet";}
            else if(abs(this->temperatureNeige.toInt()) >= abs(-7) && abs(this->temperatureNeige.toInt()) <= abs(-15)) { return "bleu";  }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-10) && abs(this->temperatureNeige.toInt()) <= abs(-25)){ return "vert";  }
            else
            {
                #ifdef DEBUG
                    qDebug() << Q_FUNC_INFO << "Temperature de neige non prise en charge pour la marque VOLA";
                #endif
                return "--";
            }
            break;
        case 4:
            if(this->temperatureNeige.toInt() >= 0 && abs(this->temperatureNeige.toInt()) <= abs(-4))   { return "jaune"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-4) && abs(this->temperatureNeige.toInt()) <= abs(-8))  { return "rouge"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-6) && abs(this->temperatureNeige.toInt()) <= abs(-12)) { return "violet";}
            else if(abs(this->temperatureNeige.toInt()) >= abs(-10) && abs(this->temperatureNeige.toInt()) <= abs(-18)){ return "bleu";  }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-10) && abs(
                        this->temperatureNeige.toInt()) <= abs(-30)){ return "vert";  }
            else
            {
                #ifdef DEBUG
                    qDebug() << Q_FUNC_INFO << "Temperature de neige non prise en charge pour la marque BRIKO / MAPLUS";
                #endif
                return "--";
            }
            break;
        case 5:
            if(this->temperatureNeige.toInt() >= 10 && this->temperatureNeige.toInt() <= 0)  { return "jaune"; }
            else if(this->temperatureNeige.toInt() >= 2 && abs(this->temperatureNeige.toInt()) <= abs(-8))  { return "rouge"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-5) && abs(this->temperatureNeige.toInt()) <= abs(-15)){ return "bleu";  }
            else
            {
                #ifdef DEBUG
                    qDebug() << Q_FUNC_INFO << "Temperature de neige non prise en charge pour la marque DRAGON SKI";
                #endif
                return "--";
            }
            break;
        case 6:
            if(this->temperatureNeige.toInt() >= 10 && this->temperatureNeige.toInt() <= 0)  { return "jaune"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-3) && abs(this->temperatureNeige.toInt()) <= abs(-7)) { return "rouge"; }
            else if(abs(this->temperatureNeige.toInt()) >= abs(-5) && abs(this->temperatureNeige.toInt()) <= abs(-15)){ return "bleu";  }
            else
            {
                #ifdef DEBUG
                    qDebug() << Q_FUNC_INFO << "Temperature de neige non prise en charge pour la marque XC SOLUTION";
                #endif
                return "--";
            }
            break;

        default:
            #ifdef DEBUG
                qDebug() << Q_FUNC_INFO << "Temperature de neige non prise en charge";
            #endif
            return "--";
            break;
    }
}

/**
 * @brief StationMeteo::setNomVideo
 * @param nomVideo QString
 */
void StationMeteo::setNomVideo(QString nomVideo)
{
    this->nomVideo = nomVideo;
}

/**
 * @brief StationMeteo::setId
 * @param id int
 */
void StationMeteo::setIdSite(int id)
{
    this->idSite = id;
}

/**
 * @brief StationMeteo::setTypeSite
 * @param typeSite int
 */
void StationMeteo::setTypeSite(int typeSite)
{
    this->typeSite = typeSite;
}

void StationMeteo::setDonneesTrame(DONNEES_STATION donnees)
{
    this->donneeStation = donnees;

    this->setDirectionVent(donneeStation.directionVent);
    this->setVitesseVent(donneeStation.vitesseVent);
    this->setTemperatureAir(donneeStation.temperatureAir);
    this->setTemperatureNeige(donneeStation.temperatureNeige);
    this->setHauteurNeige(donneeStation.hauteurNeige);
    this->setHumidite(donneeStation.humidite);
    this->setPressionAir(donneeStation.pressionAir);

    this->donneesStation.push_back(this->donneeStation);
}

/**
 * @brief StationMeteo::setDirectionVent
 * @param directionVent int
 */
void StationMeteo::setDirectionVent(QString directionVent)
{
    this->directionVent = directionVent;
}

/**
 * @brief StationMeteo::setVitesseVent
 * @param vitesseVent QString vitesse du vent
 */
void StationMeteo::setVitesseVent(QString vitesseVent)
{
    this->vitesseVent = vitesseVent;
}

/**
 * @brief StationMeteo::setTemperatureAir
 * @param temperatureAir int
 * @return void
 */
void StationMeteo::setTemperatureAir(QString temperatureAir)
{
    this->temperatureAir = temperatureAir;
}

/**
 * @brief StationMeteo::setTemperatureNeige
 * @param temperatureNeige int
 */
void StationMeteo::setTemperatureNeige(QString temperatureNeige)
{
    this->temperatureNeige = temperatureNeige;
}

/**
 * @brief StationMeteo::setHauteurNeige
 * @param hauteur_neige int
 */
void StationMeteo::setHauteurNeige(QString hauteur_neige)
{
    this->hauteurNeige = hauteur_neige;
}

/**
 * @brief StationMeteo::setHumidite
 * @param humidite int
 */
void StationMeteo::setHumidite(QString humidite)
{
    this->humidite = humidite;
}

/**
 * @brief StationMeteo::setPressionAir
 * @param pression int
 */
void StationMeteo::setPressionAir(QString pression)
{
    this->pressionAir = pression;
}


QStringList StationMeteo::getInformationComplementaire(int idSite)
{
    if(bdd->estConnecte())
    {
        bool retour;
        QString requete;
        QStringList donnees;

        requete = "SELECT tarifs, horaire, nbPisteOuverte FROM informations WHERE idSite = " + QString::number(idSite);

        retour = bdd->recuperer(requete, donnees);

        if(retour)
        {
            return donnees;
        }
    }
}

/**
 * @brief preparerDonnees() prepare les données pour l'enregistrement
 *
 * @return bool
 */
bool StationMeteo::preparerDonnees(DONNEES_STATION &donneesEnregistrement)
{

    QTime heureCourante = QTime::currentTime();

    // une heure d'acquisition ?
    if(heureCourante > heureFin)
    {
        if(donneesStation.count() == 0)
            return false;

        // calcule la moyenne horaire des données stockées
        double vitesseVent      = 0.;
        double temperatureAir   = 0.;
        double temperatureNeige = 0.;

        int hauteurNeige = 0;
        int humidite     = 0;
        int pressionAir  = 0;

        QString directionVent = "";


        for(int i=0; i < donneesStation.count(); i++)
        {
            vitesseVent      += donneesStation.at(i).vitesseVent.toDouble();
            temperatureAir   += donneesStation.at(i).temperatureAir.toDouble();
            temperatureNeige += donneesStation.at(i).temperatureNeige.toDouble();

            hauteurNeige += donneesStation.at(i).hauteurNeige.toInt();
            humidite     += donneesStation.at(i).humidite.toInt();
            pressionAir  += donneesStation.at(i).pressionAir.toInt();
        }

        donneesEnregistrement.dateDonnes = donneeStation.dateDonnes;
        donneesEnregistrement.heureDonnes = QString::number(heureDebut.hour()); // juste l'heure H
        donneesEnregistrement.vitesseVent = QString("%1").arg(qRound(vitesseVent/donneesStation.count()), 0, 'f', 0);
        donneesEnregistrement.temperatureAir = QString("%1").arg(qRound(temperatureAir/donneesStation.count()), 0, 'f', 0);
        donneesEnregistrement.temperatureNeige = QString("%1").arg(qRound(temperatureNeige/donneesStation.count()), 0, 'f', 0);
        donneesEnregistrement.hauteurNeige = QString("%1").arg(qRound(hauteurNeige/donneesStation.count()), 0, 'f', 0);
        donneesEnregistrement.humidite = QString("%1").arg(qRound(humidite/donneesStation.count()), 0, 'f', 0);
        donneesEnregistrement.pressionAir = QString("%1").arg(qRound(pressionAir/donneesStation.count()), 0, 'f', 0);
        donneesEnregistrement.directionVent = directionVent;

        // initialise pour la prochaine acquisition
        donneesStation.clear();
        heureDebut = QTime::currentTime();
        //heureFin = heureDebut.addSecs(3600);
        heureFin = heureDebut.addSecs(60);

        return true;
    }
    else
        return false;
}

/**
 * @brief enregistrerDonnees() enregistre les données dans la base de données.
 *
 * @return void
 */
void StationMeteo::enregistrerDonnees()
{
    DONNEES_STATION donneesEnregistrement;

    if(!preparerDonnees(donneesEnregistrement))
        return;

    if(bdd->estConnecte())
    {
        bool retour;
        QString requete;

        QDate dateMesure = QDate::fromString(donneesEnregistrement.dateDonnes, "dd/MM/yyyy");

        requete = "INSERT INTO mesures(idSite,dateMesure,heure,temperatureAir,temperatureNeige,hauteurNeige,humidite,pressionAir,vitesseVent,directionVent) VALUES('" + QString::number(this->getIdSite()) + "','" + dateMesure.toString("yyyy-MM-dd") + "','" + donneesEnregistrement.heureDonnes + "','" + donneesEnregistrement.temperatureAir + "','" + donneesEnregistrement.temperatureNeige + "','" + donneesEnregistrement.hauteurNeige + "','" + donneesEnregistrement.humidite + "','" + donneesEnregistrement.pressionAir + "','" + donneesEnregistrement.vitesseVent + "','" + donneesEnregistrement.directionVent + "')";

        retour = bdd->executer(requete);
        qDebug() << Q_FUNC_INFO << requete << retour;
        if(!retour)
        {
            qDebug() << Q_FUNC_INFO << "Erreur " << requete;
        }
    }
}

/**
 * @brief
 * @fn StationMeteo::traiterTrame
 * @param trame QString trame reçue
 */
void StationMeteo::traiterTrame(QString trame)
{
    QStringList trames = trame.split('$');

    for(int i = 1; i < trames.count(); i++)
    {
        if(this->protocole->decoderTrame(trames.at(i)))
        {
            int heure = QDateTime::currentDateTime().toString("HH").toInt();
            if(this->heureCourante + 1 == heure)
            {
               enregistrerDonnees();
               this->heureCourante = QDateTime::currentDateTime().toString("HH").toInt();
            }
        }
    }
}

