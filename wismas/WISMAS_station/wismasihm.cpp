/**
 *
 * \file WISMASIHM.cpp
 *
 * \brief Définition de la classe WISMASIHM
 *
 * \author Petrella Olivier
 *
 * \version 1.1
 *
 */
#include "wismasihm.h"

#include <QDateTime>
#include <QSettings>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QStringList>
#include <QList>
#include <QDebug>

#include "portxbee.h"
#include "portpanneau.h"

/**
 * \brief WISMASIHM::WISMASIHM
 *
 * \param QWidget parent, Ui ui, QTimer m_timer, int idStationCourante, int m_valeur, int pagePanneau_compteur(0), int nb_page.
 */
WISMASIHM::WISMASIHM(QWidget *parent) : QWidget(parent), ui(new Ui::WISMASIHM), m_timer(NULL), idStationCourante(0), m_valeur(0), pagePanneau_compteur(0), nb_page(0)
{
    ui->setupUi(this);

    QAction *actionQuitter = new QAction("&Quitter", this);
    actionQuitter->setShortcut(QKeySequence(QKeySequence::Quit)); // Ctrl+Q
    addAction(actionQuitter);
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    // Lecture de la configuration
    if(!this->chargerFichierConfig())
    {
        #ifdef DEBUG
            qDebug() << Q_FUNC_INFO << "Erreur : Lecture du fichier a échoué !";
        #endif
    }

    // Timer pour le rafraichissement de la fenetre principale
    m_timer = new QTimer(this);
    m_timer->start(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(rafraichir()));

    // Gestion des vidéos
    media = new Phonon::MediaObject(this);
    Phonon::createPath(media, ui->vwidget);

    // gestion des stations installées sur site
    if(stations.count() != 0)
    {
        // on prend le premier site pour l'affichage
        #ifdef DEBUG
            qDebug() << Q_FUNC_INFO << "Site : " << idStationCourante << stations.at((idStationCourante%stations.count()))->getIdSite() << stations.at((idStationCourante%stations.count()))->getNomTypeSite();
        #endif
        ui->nom_site->setText(stations.at(idStationCourante)->getNomTypeSite());

        connect(media, SIGNAL(aboutToFinish()), this, SLOT(rechargerVideo()));
        connect(media, SIGNAL(finished()), this, SLOT(redemarrerVideo()));
        demarrerVideo();

        portXBee = new PortXBee(this);
        for(int i = 0; i < stations.count(); i++)
        {
            connect(portXBee, SIGNAL(nouvelleTrame(QString)), stations.at(i), SLOT(traiterTrame(QString)));
        }

        portPanneau = new PortPanneau(this);

        portPanneau->afficherMessage(stations.at((idStationCourante%stations.count()))->getNomTypeSite() + ";" + QDateTime::currentDateTime().toString("dd/MM/yy") + ";" + QDateTime::currentDateTime().toString("HH:mm") + ";" + station->getInformationComplementaire(stations.at((idStationCourante%stations.count()))->getIdSite())[0] + "; Horaire " + station->getInformationComplementaire(stations.at((idStationCourante%stations.count()))->getIdSite())[1] + "; Piste " + station->getInformationComplementaire(stations.at((idStationCourante%stations.count()))->getIdSite())[2] + ";" + stations.at((idStationCourante%stations.count()))->getTemperatureAir() + ";" + "Vent " + stations.at((idStationCourante%stations.count()))->getVitesseVent() + ";" + "Neige " + stations.at((idStationCourante%stations.count()))->getHauteurNeige());
    }
    else
    {
        #ifdef DEBUG
            qDebug() << Q_FUNC_INFO << "Erreur Aucune station !";
        #endif
    }

}

/**
 * \brief WISMASIHM::~WISMASIHM
 */
WISMASIHM::~WISMASIHM()
{
    qDebug() << Q_FUNC_INFO;
    delete ui;
}

/**
 * \brief WISMASIHM::getMinute
 *
 * \return QString des minutes
 */
QString WISMASIHM::getMinute()
{
    return QDateTime::currentDateTime().toString("mm");
}

/**
 * \brief WISMASIHM::getHeure
 *
 * \return QString de l'heure
 */
QString WISMASIHM::getHeure()
{
    return QDateTime::currentDateTime().toString("HH");
}

/**
 * \brief WISMASIHM::demarrerAffichagePanneau affiche sur le panneau lumineux la synthèse des données météos d'un site
 */
void WISMASIHM::demarrerAffichagePanneau()
{
        portPanneau->afficherMessage(stations.at((idStationCourante%stations.count()))->getNomTypeSite() + ";" + QDateTime::currentDateTime().toString("dd/MM/yy") + ";" + QDateTime::currentDateTime().toString("HH:mm") + ";" + station->getInformationComplementaire(stations.at((idStationCourante%stations.count()))->getIdSite())[0] + "; Horaire " + station->getInformationComplementaire(stations.at((idStationCourante%stations.count()))->getIdSite())[1] + "; Piste " + station->getInformationComplementaire(stations.at((idStationCourante%stations.count()))->getIdSite())[2] + ";" + stations.at((idStationCourante%stations.count()))->getTemperatureAir() + ";" + "Vent " + stations.at((idStationCourante%stations.count()))->getVitesseVent() + ";" + "Neige " + stations.at((idStationCourante%stations.count()))->getHauteurNeige());
}


/**
 * \brief WISMASIHM::afficherConditionsMeteoStation affiche sur l’écran de diffusion (écran TV) l’ensemble des données météos d'un site et les conseils de fartage
 */
void WISMASIHM::afficherConditionsMeteoStation()
{
    afficherDonneesMeteoStation();

    afficherConseilsFartage();
}

/**
 * \brief WISMASIHM::afficherHorodatage met à jour l'affichage de la date et l'heure
 */
void WISMASIHM::afficherHorodatage()
{
    ui->date_heure->setText("   " + QDateTime::currentDateTime().toString("dd/MM/yy HH:mm"));
}

/**
 * \brief WISMASIHM::changerStation change de station
 */
void WISMASIHM::changerStation()
{

    ++idStationCourante;

    #ifdef DEBUG
        qDebug() << Q_FUNC_INFO << "Site : " << idStationCourante << stations.at((idStationCourante%stations.count()))->getIdSite() << stations.at((idStationCourante%stations.count()))->getNomTypeSite();
    #endif

    idStationCourante = (idStationCourante%stations.count());

}

/**
 * \brief WISMASIHM::afficherDonneesMeteoStation assure l'affichage des mesures de la station méteo d'un site.
 *
 * \return void
 */
void WISMASIHM::afficherDonneesMeteoStation()
{
    ui->nom_site->setText(stations.at((idStationCourante))->getNomTypeSite());

    ui->label_directionVent->setText(stations.at((idStationCourante))->getMesures(DIRECTION_VENT));
    ui->label_vitesseVent->setText(stations.at((idStationCourante))->getMesures(VITESSE_VENT));
    ui->label_temperature->setText(stations.at((idStationCourante))->getMesures(TEMPERATURE_AIR));
    ui->label_temperatureNeige->setText(stations.at((idStationCourante))->getMesures(TEMPERATURE_NEIGE));
    ui->label_hauteurNeige->setText(stations.at((idStationCourante))->getMesures(HAUTEUR_NEIGE));
    ui->label_humidite->setText(stations.at((idStationCourante))->getMesures(HUMIDITE));
    ui->label_pression->setText(stations.at((idStationCourante))->getMesures(PRESSION_AIR));
}

/**
 * \brief WISMASIHM::afficherConseilsFartage affiche les conseils de fartage
 *
 * \return void
 */
void WISMASIHM::afficherConseilsFartage()
{
    ui->label_SWIX->setText(stations.at((idStationCourante))->getConseilsFartage(1));
    ui->label_TOKO->setText(stations.at((idStationCourante))->getConseilsFartage(2));
    ui->label_VOLA->setText(stations.at((idStationCourante))->getConseilsFartage(3));
    ui->label_BRIK->setText(stations.at((idStationCourante))->getConseilsFartage(4));
    ui->label_DRAG->setText(stations.at((idStationCourante))->getConseilsFartage(5));
    ui->label_XCSO->setText(stations.at((idStationCourante))->getConseilsFartage(6));
}


/**
 * \brief SLOT WISMASIHM::rafraichir rafraichir l'IHM
 */
void WISMASIHM::rafraichir()
{
    m_valeur++;
    afficherHorodatage();

    if((m_valeur % config.periode) == 0)
    {
        changerStation();
        demarrerAffichagePanneau();
        demarrerVideo();
    }

    // écran TV
    afficherConditionsMeteoStation();


    // panneau lumineux
    afficherInformationsMeteoStation();
}

/**
 * \brief SLOT WISMASIHM::afficherInformationsMeteoStation
 */
void WISMASIHM::afficherInformationsMeteoStation()
{
    pagePanneau_compteur++;

    if((this->pagePanneau_compteur % 10) == 0)
    {
        portPanneau->changerPage(nb_page);
        nb_page++;

        if(nb_page == PANNEAU_NB_PAGES_MAX)
        {
            nb_page = 0;
        }
    }
}

/**
 * \brief démarre une vidéo
 *
 * \fn WISMASIHM::demarrerVideo
 */
void WISMASIHM::demarrerVideo()
{
    recupererDerniereVideo();
    media->stop();
    media->clear();
    chargerVideo();
    media->play();
}

void WISMASIHM::recupererDerniereVideo()
{
    //reperer les repertoires des stations
    QString chemin = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + stations.at(idStationCourante)->getCheminVideo()).toString();
    QStringList fichier;
    QStringList filtre;
    filtre << ".mp4";

    QDirIterator it(chemin.split("file://")[1], QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QDir dir(it.next());

        dir.setFilter(QDir::Files);
        dir.setSorting(QDir::Time);

        fichier += dir.entryList();
    }

    if(!fichier.isEmpty())
    {
        stations.at(idStationCourante)->setNomVideo(fichier[0]);
    }
}

/**
 * @brief charge la vidéo enregistrée sur le serveur
 *
 * @fn WISMASIHM::chargerVideo
 *
 * @todo charger la dernière video.
 */
void WISMASIHM::chargerVideo()
{
    if(QFile(qApp->applicationDirPath() + stations.at(idStationCourante)->getCheminVideo() + stations.at(idStationCourante)->getNomVideo()).exists())
    {
        media->setCurrentSource(qApp->applicationDirPath() + stations.at(idStationCourante)->getCheminVideo() + stations.at(idStationCourante)->getNomVideo());
    }
    else
    {
        media->setCurrentSource(qApp->applicationDirPath() + "/videos/defaut.mp4");
    }
}

/**
 * @brief SLOT permettant de recharger la vidéo pour une lecture en boucle
 *
 * @fn WISMASIHM::rechargerVideo
 */
void WISMASIHM::rechargerVideo()
{
    if(QFile(qApp->applicationDirPath() + stations.at(idStationCourante)->getCheminVideo() + stations.at(idStationCourante)->getNomVideo()).exists())
    {
        media->enqueue(qApp->applicationDirPath() + stations.at(idStationCourante)->getCheminVideo() + stations.at(idStationCourante)->getNomVideo());
    }
    else
    {
        media->setCurrentSource(qApp->applicationDirPath() + "/videos/defaut.mp4");
    }
}

/**
 * @brief SLOT permettant de relire la vidéo
 *
 * @fn WISMASIHM::redemarrerVideo
 */
void WISMASIHM::redemarrerVideo()
{
    media->play();
}

/**
 * @brief WISMASIHM::chargerFichierConfig
 *
 * @return bool
 */
bool WISMASIHM::chargerFichierConfig()
{
    // Le nom du fichier INI : chemin-executable/nom-executable.ini
    QString fichierINI =  qApp->applicationDirPath() + "/" + qApp->applicationName() + ".ini";

    #ifdef DEBUG
        qDebug() << Q_FUNC_INFO << "Nom fichier INI : " + fichierINI;
    #endif

    if(QFile::exists(fichierINI))
    {
        QSettings parametres(fichierINI, QSettings::IniFormat);

        // Chargement des informations logiciel
        this->config.nb_station                       = parametres.value("Configuration/nb_station", "0").toInt();
        this->config.periode                          = parametres.value("Configuration/periode", "90").toInt();

        // Chargement des informations d'enregistrement en base de données
        this->enregistrement_config.periode          = parametres.value("Enregistrement/periode", "60").toInt();

        // Chargement de la configuration du recepteur station
        this->param_communication.port                = parametres.value("Station_configuration/port", "0").toString();
        this->param_communication.baud_rate           = parametres.value("Station_configuration/baud_rate", "0").toInt();
        this->param_communication.data_bit            = parametres.value("Station_configuration/data_bit", "0").toInt();
        this->param_communication.stop                = parametres.value("Station_configuration/stop", "0").toInt();
        this->param_communication.parity              = parametres.value("Station_configuration/parity", "0").toInt();

        // Chargement de la configuration du panneau lumineux
        this->param_panneau.port                      = parametres.value("Panneau_configuration/port", "0").toString();
        this->param_panneau.baud_rate                 = parametres.value("Panneau_configuration/baud_rate", "0").toInt();
        this->param_panneau.data_bit                  = parametres.value("Panneau_configuration/data_bit", "0").toInt();
        this->param_panneau.stop                      = parametres.value("Panneau_configuration/stop", "0").toInt();
        this->param_panneau.parity                    = parametres.value("Panneau_configuration/parity", "0").toInt();

        // Chargement des paramètres d'affichage du panneau lumineux
        this->affichage_panneau.titre                 = parametres.value("Panneau_affichage/titre", "0").toString();
        this->affichage_panneau.periode               = parametres.value("Panneau_affichage/periode", "0").toInt();
        this->affichage_panneau.taille_text           = parametres.value("Panneau_affichage/taille_text", "0").toString();
        this->affichage_panneau.temps_text            = parametres.value("Panneau_affichage/temps_text", "0").toInt();
        this->affichage_panneau.effet_transition      = parametres.value("Panneau_affichage/effet_transition", "0").toString();
        this->affichage_panneau.nom_station           = parametres.value("Panneau_affichage/nom_station", "0").toBool();
        this->affichage_panneau.date_heure            = parametres.value("Panneau_affichage/date_heure", "0").toBool();
        this->affichage_panneau.temperature_air       = parametres.value("Panneau_affichage/temperature_air", "0").toBool();
        this->affichage_panneau.temperature_neige     = parametres.value("Panneau_affichage/temperature_neige", "0").toBool();
        this->affichage_panneau.hauteur_neige         = parametres.value("Panneau_affichage/hauteur_neige", "0").toBool();
        this->affichage_panneau.humidite              = parametres.value("Panneau_affichage/humidite", "0").toBool();
        this->affichage_panneau.pression              = parametres.value("Panneau_affichage/pression", "0").toBool();
        this->affichage_panneau.vitesse_vent          = parametres.value("Panneau_affichage/vitesse_vent", "0").toBool();

        // Chargement des paramètres des stations

        for(int i=0; i < this->config.nb_station; i++)
        {
            int idSite                  = parametres.value(QString("Station%1/id").arg(i+1), "").toInt();
            int typeSite                = parametres.value(QString("Station%1/type").arg(i+1), "").toInt();
            QString nomSite             = parametres.value(QString("Station%1/nom").arg(i+1), "").toString();
            QString cheminVideo         = parametres.value(QString("Station%1/chemin_video").arg(i+1), "").toString();
            QString couleurTextPanneau  = parametres.value(QString("Station%1/couleur_text_panneau").arg(i+1), "").toString();

            if(!nomSite.isEmpty())
            {
                station = new StationMeteo(idSite, typeSite, nomSite, cheminVideo, couleurTextPanneau, this);
                stations.push_back(station);
            }
        }
    }
    else
    {
        #ifdef DEBUG
            qDebug() << "Erreur Fichier " + fichierINI + " absent !";
        #endif
        return false;
    }
    return true;
}
