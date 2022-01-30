/**
  * @file Camera.cpp
  *
  * @class Camera
  *
  * @brief Acquérir le système vidéo
  *
  * @author Pierre GRELET
  *
  * @version 1.1
  *
  * @date 15 mars 2018
  *
  */

#include "Camera.h"
#include "Structures.h"

/**
  * @brief  Constructeur
  * @details Constructeur par défaut
  */
Camera::Camera() : parametrage(NULL), video(NULL), numeroCamera(0), identifiant("")
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    video = new Video;
    connect(video, SIGNAL(progression(int)), this, SLOT(progresser(int)));
    connect(video, SIGNAL(fini()), this, SLOT(terminer()));

    parametrage = new Parametrage;
}

/**
  * @brief  Constructeur
  * @details
  * @param  numeroCamera int le numéro de la caméra
  * @param  identifiant QString le nom de la caméra
  */
Camera::Camera(int numeroCamera, QString identifiant) : parametrage(NULL), video(NULL), numeroCamera(numeroCamera), identifiant(identifiant)
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    video = new Video;
    connect(video, SIGNAL(progression(int)), this, SLOT(progresser(int)));
    connect(video, SIGNAL(fini()), this, SLOT(terminer()));

    parametrage = new Parametrage;
    lireParametres();
}

/**
  * @brief  Destructeur
  */
Camera::~Camera()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    delete parametrage;
    delete video;
}

/**
 * @brief Lire les paramètres de la caméra
 *
 * @fn Camera::lireParametres
 */
void Camera::lireParametres()
{
    parametrage->lireParametres("Camera" + QString::number(numeroCamera));
    parametres = parametrage->getParametres();
}

/**
  * @brief  Mutateur de la structure parametres
  * @details
  * @param  parametres Parametres les paramètres de la caméra
  */
void Camera::setParametres(Parametres parametres)
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    this->parametrage->setParametres(parametres);
}

/**
  * @brief  Accesseur de la structure parametres
  * @details    Les paramètres de la caméra
  * @return un type \e Parametres qui est la structure de Parametres
  */
Parametres Camera::getParametres() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    return parametrage->getParametres();
}

/**
  * @brief  Déplacer la caméra vers la position initiale
  * @details
  * @return un type \e QString qui est la requête
  */
QString Camera::seDeplacer() const
{
    //http://192.168.52.93:99/decoder_control.cgi?command=33&onestep=500&user=admin&pwd=
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=31&onestep=500" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;

    return URL;
}

/**
 * @brief Enregistrer les images vidéos sur une durée
 *
 * @fn Camera::enregistrer
 */
void Camera::enregistrer() const
{
    if(parametres.etat == "1")
    {
        emit afficherMessage(QString::fromUtf8("Démarrage enregistrement caméra %1 (durée %2 s)").arg(parametres.nomSite).arg(parametres.duree));
        video->enregistrerVideo(numeroCamera, parametres.nomSite, parametres.adresseIP, parametres.numeroPort, parametres.identifiant, parametres.motDePasse, parametres.duree.toInt());
    }

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << numeroCamera << parametres.nomSite << parametres.adresseIP << parametres.numeroPort;
    #endif
}

/**
  * @brief  Arrêter l'enregistrement
  * @details    L'administrateur a demandé d'arrêter l'acquisition
  */
void Camera::arreter() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    video->arreter();
}

/**
  * @brief  Envoyer la progression des enregistrements
  * @details
  * @param  pct int la valeur de progression d'enregistrement
  */
void Camera::progresser(int pct) const
{
    #ifdef DEBUG
    //qDebug() << Q_FUNC_INFO;
    #endif

    emit progression(pct);
}

/**
  * @brief  Réinitialiser les bars de progression de l'acquisition
  * @details
  */
void Camera::terminer() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    progression(0);
}

/**
 * @brief   Supprimer les enregistrements
 * @details Supprimer la vidéo la plus ancienne
 */
void Camera::supprimer() const
{
    video->supprimerEnregistrements();
}
