/**
  * @file Parametrage.cpp
  *
  * @class Parametrage
  *
  * @brief Paramétrer le système vidéo
  *
  * @author Pierre GRELET
  *
  * @version 1.1
  *
  * @date 15 mars 2018
  *
  */

#include "Parametrage.h"

/**
  * @brief  Constructeur
  * @details
  */
Parametrage::Parametrage()
{
    fichierINI = QApplication::applicationDirPath() + "/" + "configuration-cameras.ini";

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << fichierINI;
    #endif
}

/**
  * @brief  Destructeur
  * @details
  */
Parametrage::~Parametrage()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif
}

/**
  * @brief  Mutateur de la structure parametres
  * @details
  * @param  parametres Parametres les paramètres de la caméra
  */
void Parametrage::setParametres(Parametres &parametres)
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    this->parametres = parametres;
}

/**
  * @brief  Accesseur de la structure parametres
  * @details    Les paramètres de la caméra
  * @return un type \e Parametres qui est la structure de Parametres
  */
Parametres Parametrage::getParametres() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    return parametres;
}

/**
  * @brief  Attribuer les paramètres à la caméra sélectionnée
  * @details
  * @param  identifiantCamera QString la caméra à paramétrer
  */
void Parametrage::lireParametres(QString identifiantCamera)
{
    QSettings configuration(fichierINI, QSettings::IniFormat);

    parametres.etat = configuration.value(identifiantCamera + "/etat", "1").toString();
    parametres.nomSite = configuration.value(identifiantCamera + "/nom", "").toString();
    parametres.adresseIP = configuration.value(identifiantCamera + "/adresse_IP", "").toString();
    parametres.numeroPort = configuration.value(identifiantCamera + "/numero_port","").toString();
    parametres.identifiant = configuration.value(identifiantCamera + "/identifiant", "admin").toString();
    parametres.motDePasse = configuration.value(identifiantCamera + "/mot_de_passe", "").toString();
    parametres.typeDeplacement = configuration.value(identifiantCamera + "/type_deplacement", "Fixe").toString();
    parametres.cheminVideo = configuration.value(identifiantCamera + "/chemin_video", "./videos").toString();
    parametres.duree = configuration.value(identifiantCamera + "/duree", "30").toString();
    parametres.periode = configuration.value(identifiantCamera +"/periode", "3600").toString();
    parametres.resolution = configuration.value(identifiantCamera + "/resolution", "480").toString();

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Etat : " + parametres.etat;
    qDebug() << "Camera courante : " + identifiantCamera;
    qDebug() << "Adresse IP : " + parametres.adresseIP;
    qDebug() << "Port : " + parametres.numeroPort;
    qDebug() << "Login : " + parametres.identifiant;
    qDebug() << "Mot de passe : " + parametres.motDePasse;
    qDebug() << "Chemin vidéo : " + parametres.cheminVideo;
    qDebug() << "Durée : " + parametres.duree;
    qDebug() << "Période : " + parametres.periode;
    qDebug() << "Résolution : " + parametres.resolution;
    qDebug() << "Type de déplacement : " + parametres.typeDeplacement;
    #endif
}
