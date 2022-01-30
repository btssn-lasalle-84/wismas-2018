/**
  * @file Video.cpp
  *
  * @class Video
  *
  * @brief Traiter les vidéos capturer par le système vidéo
  *
  * @author Pierre GRELET
  *
  * @version 1.1
  *
  * @date 15 mars 2018
  *
  */

#include "Video.h"

/**
  * @brief  Constructeur
  * @details
  */
Video::Video()
{     
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    processus = new QProcess(this); // Crée l'objet processus
    timerProgression = new QTimer(this);
    timerEnregistrement = new QTimer(this);

    fichierINI = QApplication::applicationDirPath() + "/" + "configuration-cameras.ini";

    enregistrement = false;
    numeroVideo = 1;
    chargement = 0;
}

/**
  * @brief  Destructeur
  * @details
  */
Video::~Video()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif
}

/**
  * @brief  Accesseur de nbVideos
  * @details
  * @return int le nombre de vidéos
  */
int Video::getNumeroVideo()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << numeroVideo;
    #endif

    QSettings configuration(fichierINI, QSettings::IniFormat);
    int totalEnregistrement = configuration.value("nb_videos", "1").toInt();

    if(numeroVideo > totalEnregistrement)
        numeroVideo = 1;

    return numeroVideo;
}

/**
  * @brief  Créer le chemin où enregistrer les vidéos
  * @details
  */
void Video::creerCheminVideo(int numeroCamera)
{
    QSettings configuration(fichierINI, QSettings::IniFormat);
    QString identifiantCamera = "Camera" + QString::number(numeroCamera);

    cheminVideo = configuration.value(identifiantCamera + "/chemin_video", "./videos/").toString();

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << cheminVideo;
    #endif
}

/**
  * @brief  Enregistrer la vidéo capturée par une caméra
  * @details
  * @param numeroCamera int le numéro de la caméra
  * @param nomSite QString le nom du site
  * @param adresseIP QString l'adresse IP
  * @param numeroPort QString le numéro de port
  * @param identifiant QString le nom de l'administrateur
  * @param motDePasse QString le mot de passe administrateur
  * @param duree int le temps d'enregistrement
  */
void Video::enregistrerVideo(const int numeroCamera, const QString nomSite, const QString adresseIP, const QString numeroPort, const QString identifiant, const QString motDePasse, const int duree)
{
    QDir repertoire(cheminVideo);

    QString programme; // Le nom du programme
    QStringList arguments; // Arguments du programme

    QString nomFichier;
    QString extension;

    /*
        Commandes d'enregistrement video avec cvlc :

        Le flux video de la camera est en MJPEG

        - en mjpg : cvlc "http://192.168.52.221:99/videostream.cgi?user=admin&pwd=&resolution=32&rate=0" --sout file/ts:./video-test.mjpg;

        - en mp4 : cvlc "http://192.168.52.221:99/videostream.cgi?user=admin&pwd=&resolution=32&rate=0" --sout '#transcode{vcodec=h264} :file{mux=ts,dst=test.mpg}'
    */

    if(duree != 0)
    {
        // Enregistrement en cours ?
        if(enregistrement == true)
        {
            #ifdef DEBUG
            qDebug() << Q_FUNC_INFO << "Fin de l'enregistrement";
            #endif

            arreter();
        }

        creerCheminVideo(numeroCamera);

        extension = ".mp4";
        nomFichier = cheminVideo + QString::number(getNumeroVideo()) + "_" + nomSite + extension;

        verifierDossierEnregistrements(repertoire);

        // Format mp4
        programme = "./cvlc.sh";
        if(motDePasse.length() == 0)
        {
            arguments   << adresseIP
                        << numeroPort
                        << identifiant
                        << nomFichier
                        << QString::number(duree);
        }
        else
        {
            arguments   << adresseIP
                        << numeroPort
                        << identifiant
                        << motDePasse
                        << nomFichier
                        << QString::number(duree);
        }

        #ifdef DEBUG
        //qDebug() << Q_FUNC_INFO << programme << arguments;
        qDebug() << Q_FUNC_INFO << "FICHIER : " + nomFichier;
        #endif

        processus->start(programme, arguments);

        enregistrement = true;
        chargement = 0;
        numeroVideo++;

        this->duree = duree;
        demarrerTimers(duree);
    }
}

/**
  * @brief  Supprimer les vidéos enregistrées
  * @details Les enregistrements sont supprimés selon le niveau de supression indiqué dans le fichier de configuration
  * @param nomSite QString le nom du site
  */
void Video::supprimerEnregistrements() const
{
    QSettings configuration(fichierINI, QSettings::IniFormat);

    QDir repertoire(cheminVideo);

    QString typeSuppression = configuration.value("niveau_suppression", "Aucun").toString();

    /*
        Un nom de fichier vidéo enregistrée :

        - en mp4 : 1_Alpin.mp4
    */

    if(repertoire.exists())
    {
        if(typeSuppression == "Tous")   // Tous les fichiers seront supprimés
        {
            supprimerTousLesEnregistrements(repertoire);
        }
        else if(typeSuppression == "Nb_enregistrements")    // Un fichier (le getNombreVideos() le plus récent) de chaque caméra sauvegardé
        {
            supprimerNbEnregistrements(repertoire);
        }
        else if(typeSuppression == "Aucun") // Aucun fichiers supprimés
        {
            // Ne rien faire
        }
        else    // Valeur niveau_suppression invalide
        {
            // Ne rien faire
        }
    }
}

/**
  * @brief  Arrêter la capture d'image vidéo
  * @details
  */
void Video::arreter()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    if(enregistrement == true)
    {
        arreterTimers();
        processus->terminate();
        this->enregistrement = false;

        chargement = 0;
        emit progression(chargement);
    }
}

/**
  * @brief  Démarrer les timers
  * @details
  * @param duree int duree d'enregistrement
  */
void Video::demarrerTimers(int duree)
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    if(duree != 0)
    {
        timerProgression->start((duree*1000)/100);
        timerEnregistrement->start(duree*1000);
        connect(timerProgression, SIGNAL(timeout()), this, SLOT(progresser()));
        connect(timerEnregistrement, SIGNAL(timeout()), this, SLOT(arreter()));

        #ifdef DEBUG
        qDebug() << Q_FUNC_INFO << "timer progression" << ((duree*1000)/100);
        qDebug() << Q_FUNC_INFO << "timer enregistrement" << (duree*1000);
        #endif
    }
}

/**
  * @brief  Arrêter les timers
  * @details
  */
void Video::arreterTimers()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    if(timerProgression->isActive())
        timerProgression->stop();
    if(timerEnregistrement->isActive())
        timerEnregistrement->stop();

    disconnect(timerProgression, SIGNAL(timeout()), this, SLOT(progresser()));
    disconnect(timerEnregistrement, SIGNAL(timeout()), this, SLOT(arreter()));
}

/**
  * @brief  Actualiser la barre de progression de l'acquisition vidéo
  * @details
  */
void Video::progresser()
{
    if(chargement == duree*1000)
    {
        #ifdef DEBUG
        qDebug() << Q_FUNC_INFO << "Fin de l'enregistrement";
        #endif

        arreter();
    }
    else
    {
        chargement += ((duree*1000)/100); // en pourcentage
        emit progression(((double)chargement/((double)duree*1000.))*100.);

        #ifdef DEBUG
        //qDebug() << Q_FUNC_INFO << (((double)chargement/((double)duree*1000.))*100.) << chargement << (duree*1000);
        #endif
    }    
}

/**
  * @brief  Arrêter tous les enregistrements en cours d'exécution
  * @details Méthode statique
  */
void Video::arreterEnregistrements()
{
    QString programme = "killall"; // le nom du programme
    QStringList arguments; // arguments du programme

    arguments << "vlc";

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << programme << arguments;
    #endif

    QProcess *processus = new QProcess();
    processus->startDetached(programme, arguments);
    delete processus;
}

/**
  * @brief  Vérifier si les répertoires des enregistrements pour chaque site sont p
  * @details
  * @param repertoire QDir le répertoire où se trouve les enregistrements
  */
void Video::verifierDossierEnregistrements(QDir& repertoire) const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    if(!QDir(cheminVideo).exists())
    {
        repertoire.mkdir(cheminVideo);
    }
}

/**
  * @brief  Supprimer les vidéos enregistrées
  * @details Les enregistrements sont tous supprimés
  * @param repertoire QDir le répertoire où se trouve les enregistrements
  */
void Video::supprimerTousLesEnregistrements(QDir& repertoire) const
{
    QStringList filtre;
    QString extension = ".mp4";
    filtre << "*" + extension;

    foreach(QString enregistrement, repertoire.entryList(filtre))
    {
        repertoire.remove(enregistrement);

        #ifdef DEBUG
        for(int compteur = 0; compteur < enregistrement.length(); compteur++)
            qDebug() << Q_FUNC_INFO << repertoire.count() << enregistrement.at(compteur);
        #endif
    }
}

/**
  * @brief  Supprimer les vidéos enregistrées
  * @details Les enregistrements les plus anciens sont supprimés
  * @param repertoire QDir le répertoire où se trouve les enregistrements
  * @param nbEnregistrementsAConserver int le nombre d'enregistrements à conserver
  * @param nomSite QString le nom du site de la caméra
  */
void Video::supprimerNbEnregistrements(QDir& repertoire) const
{
    QStringList filtre;
    QStringList fichier;
    QString extension = ".mp4";
    filtre << "*" + extension;

    repertoire.setSorting(QDir::Time);

    fichier += repertoire.entryList(filtre);

    foreach(QString enregistrement, repertoire.entryList(QDir::NoDotAndDotDot | QDir::AllEntries))
    {
        if(enregistrement != fichier[0])
            repertoire.remove(enregistrement);

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << fichier[0];
    #endif
    }
}
