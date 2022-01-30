/**
  * @file IHMWismas.cpp
  *
  * @class IHMWismas
  *
  * @brief Interface administrateur pour gérer le système vidéo
  *
  * @author Pierre GRELET
  *
  * @version 1.1
  *
  * @date 18 février 2018
  *
  */

#include "IHMWismas.h"
#include "ui_IHMWismas.h"

/**
 * @brief Constructeur de la calsse IHMWismas
 * @details
 * @fn IHMWismas::IHMWismas
 * @param parent QObject Adresse objet Qt parent (ici 0 pour une fenêtre principale)
 */
IHMWismas::IHMWismas(QWidget *parent) : QWidget(parent), ui(new Ui::IHMWismas)
{
    ui->setupUi(this);

    fichierINI = QApplication::applicationDirPath() + "/" + "configuration-cameras.ini";

    // Les timers pour les déplacements périodiques
    timerInitial = new QTimer(this);
    timerFinal = new QTimer(this);
    manager = new QNetworkAccessManager(this);

    actionQuitter = new QAction(this);
    actionQuitter->setShortcut(QKeySequence(QKeySequence::Quit));
    addAction(actionQuitter);

    creerCameras();
    afficherFluxVideo();
    initialiserPosition();

    // Connecte les signaux aux slots
    connecter();

    // Affiche la fenêtre de paramétrage des caméras
    parametrer();

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << fichierINI;
    #endif
}

/**
  * @brief  Destructeur
  * @details
  */
IHMWismas::~IHMWismas()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    arreterAcquisitionVideo();

    for(int compteur = 0; compteur < cameras.count(); compteur++)
    {
        Camera* camera = cameras.at(compteur);
        delete camera;
    }

    Video::arreterEnregistrements();

    delete ui;
}

/**
  * @brief  Afficher la fenêtre de paramétrage des caméras
  * @details
  */
void IHMWismas::parametrer()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    ui->fenetre->setCurrentIndex(0);

    afficherParametresCamera();
}

/**
  * @brief  Afficher la fenêtre d'acquisition des vidéos
  * @details
  */
void IHMWismas::demarrer()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    ui->fenetre->setCurrentIndex(1);

    demarrerAcquisitionVideo();
}

/**
  * @brief  Gérer l'état des caméras au niveau de l'interface
  * @details    Activer/désactiver la caméra
  */
void IHMWismas::gererBoutonEtat()
{
    int numeroCamera = ui->listeConfigurationCameras->currentIndex();
    Parametres parametres = cameras.at(numeroCamera)->getParametres();
    QString identifiantCamera = "Camera" + QString::number(numeroCamera+1);

    QSettings configuration(fichierINI, QSettings::IniFormat);

    if(ui->etat->isChecked())
    {
        ui->etat->setText("Activé");
        configuration.setValue(identifiantCamera + "/etat", "1");
    }
    else
    {
        ui->etat->setText("Désactivé");
        configuration.setValue(identifiantCamera + "/etat", "0");;
    }
    rafraichirFluxVideo(identifiantCamera, parametres);
    cameras.at(numeroCamera)->lireParametres();
    afficherParametresCamera();
}

/**
  * @brief  Gérer les déplacements au niveau de l'interface
  * @details    Activer/désactiver boutons de déplacement
  */
void IHMWismas::gererTypeDeplacement() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    if(ui->typeDeplacementPanoramique->	isChecked())
        activerBoutonsCamera();
    else

        desactiverBoutonsCamera();
}

/**
  * @brief  Activer le type de déplacement fixe
  * @details
  */
void IHMWismas::activerBoutonsCamera() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << "Déplacer caméra : activé";
    #endif

    ui->deplacementGaucheParametrage->setEnabled(true);
    ui->deplacementDroiteParametrage->setEnabled(true);
    ui->deplacementHautParametrage->setEnabled(true);
    ui->deplacementBasParametrage->setEnabled(true);
    ui->enregistrerPositionInitiale->setEnabled(true);
    ui->enregistrerPositionFinale->setEnabled(false);
}

/**
  * @brief  Activer le type de déplacement panoramique
  * @details
  */
void IHMWismas::desactiverBoutonsCamera() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << "Déplacer caméra : désactivé";
    #endif

    ui->deplacementGaucheParametrage->setEnabled(true);
    ui->deplacementDroiteParametrage->setEnabled(true);
    ui->deplacementHautParametrage->setEnabled(true);
    ui->deplacementBasParametrage->setEnabled(true);
    ui->enregistrerPositionInitiale->setEnabled(false);
    ui->enregistrerPositionFinale->setEnabled(false);
}

/**
  * @brief  Déplacer la caméra à gauche
  * @details
  */
void IHMWismas::deplacerGauche() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();

    if(numeroCamera >=0 && numeroCamera < cameras.count())
    {
        Parametres parametres = cameras.at(numeroCamera)->getParametres();
        QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=4&onestep=1" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;
        manager->get(QNetworkRequest(QUrl(URL)));
    }
}

/**
  * @brief  Déplacer la caméra à droite
  * @details
  */
void IHMWismas::deplacerDroite() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();

    if(numeroCamera >=0 && numeroCamera < cameras.count())
    {
        Parametres parametres = cameras.at(numeroCamera)->getParametres();
        QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=6&onestep=1" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;
        manager->get(QNetworkRequest(QUrl(URL)));
    }
}

/**
  * @brief  Déplacer la caméra en haut
  * @details
  */
void IHMWismas::deplacerHaut() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();

    if(numeroCamera >=0 && numeroCamera < cameras.count())
    {
        Parametres parametres = cameras.at(numeroCamera)->getParametres();
        QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=0&onestep=1" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;
        manager->get(QNetworkRequest(QUrl(URL)));
    }
}

/**
  * @brief  Déplacer la caméra en bas
  * @details
  */
void IHMWismas::deplacerBas() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();

    if(numeroCamera >=0 && numeroCamera < cameras.count())
    {
        Parametres parametres = cameras.at(numeroCamera)->getParametres();
        QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=2&onestep=1" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;
        manager->get(QNetworkRequest(QUrl(URL)));
    }
}

/**
  * @brief  Sauvegarder les paramètres entrés via l'IHM sur le fichier de configuration .ini
  * @details
  */
void IHMWismas::sauvegarderParametres()
{
    int numeroCamera = ui->listeConfigurationCameras->currentIndex();
    QString identifiantCamera = "Camera" + QString::number(numeroCamera+1);
    Parametres parametres = cameras.at(numeroCamera)->getParametres();
    QSettings configuration(fichierINI, QSettings::IniFormat);

    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO << identifiantCamera;
    #endif

    configuration.setValue(identifiantCamera + "/adresse_IP", ui->champAdresseIP->text());
    configuration.setValue(identifiantCamera + "/numero_port", ui->champPort->text());
    configuration.setValue(identifiantCamera + "/duree", ui->champDuree->text());
    configuration.setValue(identifiantCamera + "/periode", ui->champPeriode->text());

    if(ui->typeDeplacementFixe->isChecked())
    {
        desactiverBoutonsCamera();

        configuration.setValue(identifiantCamera + "/type_deplacement", "Fixe");
        sauvegarderPositionInitiale();
        sauvegarderPositionFinale();
    }
    if(ui->typeDeplacementPanoramique->isChecked())
    {
        activerBoutonsCamera();
        configuration.setValue(identifiantCamera + "/type_deplacement", "Panoramique");
    }

    rafraichirFluxVideo(identifiantCamera, parametres);
    cameras.at(numeroCamera)->lireParametres();
}

/**
  * @brief  Afficher les paramètres d'une caméra
  * @details
  */
void IHMWismas::afficherParametresCamera() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();

    if(numeroCamera >=0 && numeroCamera < cameras.count())
    {
        Parametres parametres = cameras.at(numeroCamera)->getParametres();

        if(parametres.etat == "0")
        {
            ui->parametres->setDisabled(true);
            ui->acquisition->setDisabled(true);
            ui->visualisationParametrage->stop();
            ui->etat->setChecked(false);
            ui->etat->setText("Désactivé");
        }
        else
        {
            ui->parametres->setEnabled(true);
            ui->acquisition->setEnabled(true);
            ui->etat->setChecked(true);
            ui->etat->setText("Activé");
        }

        ui->champAdresseIP->setText(parametres.adresseIP);
        ui->champPort->setText(parametres.numeroPort);
        ui->champDuree->setText(parametres.duree);
        ui->champPeriode->setText(parametres.periode);

        if(parametres.typeDeplacement == "Panoramique")
        {
            ui->typeDeplacementPanoramique->setChecked(true);
            activerBoutonsCamera();
        }
        else if(parametres.typeDeplacement == "Fixe")
        {
            ui->typeDeplacementFixe->setChecked(true);
            desactiverBoutonsCamera();
        }
        else
        {
            ui->typeDeplacementFixe->setChecked(true);
            desactiverBoutonsCamera();
        }
    }
}

/**
  * @brief  Rafraîchir les paramètres caméra
  * @details    Une modification des paramètres qui est ensuite sauvegardée
  * @param  identifiantCamera QString le nom de la caméra
  * @param  parametres Parametres les paramètres de la caméra
  */
void IHMWismas::rafraichirFluxVideo(const QString identifiantCamera, Parametres &parametres)
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    QSettings configuration(fichierINI, QSettings::IniFormat);
    parametres.adresseIP = configuration.value(identifiantCamera + "/adresse_IP", "").toString();
    parametres.numeroPort = configuration.value(identifiantCamera + "/numero_port","").toString();

    QUrl URL("http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/mobile.htm");
    URL.setUserName(parametres.identifiant);
    URL.setPassword(parametres.motDePasse);

    ui->visualisationParametrage->load(URL);
    ui->visualisationParametrage->update();
}

/**
  * @brief  Afficher le flux vidéo de la caméra sélectionnée
  * @details
  */
void IHMWismas::afficherFluxVideo() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();
    Parametres parametres = cameras.at(numeroCamera)->getParametres();

    int resolution = parametres.resolution.toInt();

    switch(resolution)
    {
        case 480:
            ui->visualisationParametrage->setMaximumSize(720,480);
            break;
        case 720:
            ui->visualisationParametrage->setMaximumSize(1280, 720);
            break;
        default:
            ui->visualisationParametrage->setMaximumSize(720, 480);
            break;
    }
    ui->visualisationParametrage->load(creerUrlCamera());
}

/**
  * @brief  Paramétrer la position initiale de la caméra
  * @details    Pour un déplacement panoramique
  */
void IHMWismas::sauvegarderPositionInitiale() const
{
    //ENREGISTRER POSITION INITIALE
    //http://192.168.52.93:99/decoder_control.cgi?command=30&onestep=500&user=admin&pwd=
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();

    if(numeroCamera >=0 && numeroCamera < cameras.count())
    {
        Parametres parametres = cameras.at(numeroCamera)->getParametres();
        QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=30&onestep=500" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;
        manager->get(QNetworkRequest(QUrl(URL)));
    }
    ui->enregistrerPositionInitiale->setEnabled(false);
    ui->enregistrerPositionFinale->setEnabled(true);
}

/**
  * @brief  Paramétrer la position finale de la caméra
  * @details    Pour un déplacement panoramique
  */
void IHMWismas::sauvegarderPositionFinale() const
{
    //ENREGISTRER POSITION DETECTION
    //http://192.168.52.93:99/decoder_control.cgi?command=32&onestep=500&user=admin&pwd=
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();

    if(numeroCamera >=0 && numeroCamera < cameras.count())
    {
        Parametres parametres = cameras.at(numeroCamera)->getParametres();
        QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=32&onestep=500" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;
        manager->get(QNetworkRequest(QUrl(URL)));
    }   
    ui->enregistrerPositionInitiale->setEnabled(true);
    ui->enregistrerPositionFinale->setEnabled(false);

    initialiserPosition();
}

/**
  * @brief  Paramétrer la position finale de la caméra
  * @details    Pour un déplacement panoramique
  */
void IHMWismas::initialiserPosition() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    for(int compteur = 0; compteur < cameras.count(); compteur++)
    {
        QString URL = cameras.at(compteur)->seDeplacer();
        manager->get(QNetworkRequest(QUrl(URL)));
    }
}

/**
  * @brief  Lancer la vidéo
  * @details   La vidéo est lancée pour une durée prédéfinie
  */
void IHMWismas::demarrerAcquisitionVideo()
{    
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    ui->progessionEnregistrementCamera1->setValue(0);
    ui->progessionEnregistrementCamera2->setValue(0);
    ui->progessionEnregistrementCamera3->setValue(0);
    ui->parametrage->setEnabled(false);
    ui->demarrageAcquisitionVideo->setEnabled(false);
    ui->arretAcquisitionVideo->setEnabled(true);

    arreterTimers();

    timerFinal->singleShot(2500, this, SLOT(deplacerPositionFinale()));

    for(int compteur = 0; compteur < cameras.count(); compteur++)
    {
        Parametres parametres = cameras.at(compteur)->getParametres();

        if(parametres.etat == "1")
        {
            afficherMessage(QString::fromUtf8("Programmation enregistrement caméra %1 : toutes les %2  s").arg(parametres.nomSite).arg(parametres.periode));
            cameras.at(compteur)->enregistrer();
            demarrerTimers(compteur, parametres);
        }
    }
}

/**
  * @brief  Déplacer la prise vidéo position intiale
  * @details     Pour un déplacement panoramique
  */
void IHMWismas::deplacerPositionInitiale()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    for(int compteur = 0; compteur < cameras.count(); compteur++)
    {
        QString URL = cameras.at(compteur)->seDeplacer();
        manager->get(QNetworkRequest(QUrl(URL)));
    }

    timerFinal->singleShot(15000, this, SLOT(deplacerPositionFinale()));
}

/**
  * @brief  Déplacer la prise vidéo position finale
  * @details     Pour un déplacement panoramique
  */
void IHMWismas::deplacerPositionFinale()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    for(int compteur = 0; compteur < cameras.count(); compteur++)
    {
        Parametres parametres = cameras.at(compteur)->getParametres();
        QString URL = "http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/decoder_control.cgi?command=33&onestep=500" + "&user=" + parametres.identifiant + "&pwd=" + parametres.motDePasse;

        manager->get(QNetworkRequest(QUrl(URL)));
    }

    timerInitial->singleShot(10000, this, SLOT(deplacerPositionInitiale()));
}

/**
  * @brief  Arrêter la prise vidéo
  * @details   La vidéo est arrêtée par l'administrateur
  */
void IHMWismas::arreterAcquisitionVideo()
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    ui->parametrage->setEnabled(true);
    ui->demarrageAcquisitionVideo->setEnabled(true);
    ui->arretAcquisitionVideo->setEnabled(false);

    arreterTimers();

    emit fini(); // pour pévenir les caméras

    for(int compteur = 0; compteur < cameras.count(); compteur++)
    {
        Parametres parametres = cameras.at(compteur)->getParametres();

        if(parametres.etat == "0")
            afficherMessage(QString::fromUtf8("Fin de la programmation des enregistrements caméra %1").arg(parametres.nomSite));
    }
    initialiserPosition();
}

/**
  * @brief  Afficher des messages sur les enregistrements
  * @details
  * @param  message QString log sur les enregistrements vidéo
  */
void IHMWismas::afficherMessage(const QString message) const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    ui->messagesEnregistrement->append(message);
}

/**
  * @brief  Connecter les signaux aux slots
  * @details
  */
void IHMWismas::connecter() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui->parametrage, SIGNAL(clicked()), this, SLOT(parametrer()));
    connect(ui->acquisition, SIGNAL(clicked()), this, SLOT(demarrer()));

    connect(ui->listeConfigurationCameras, SIGNAL(currentIndexChanged(int)), this, SLOT(afficherFluxVideo()));
    connect(ui->listeConfigurationCameras, SIGNAL(currentIndexChanged(int)), this, SLOT(afficherParametresCamera()));
    connect(ui->sauvegarde, SIGNAL(clicked()), this, SLOT(sauvegarderParametres()));

    connect(ui->etat, SIGNAL(stateChanged(int)), this, SLOT(gererBoutonEtat()));

    connect(ui->typeDeplacementPanoramique, SIGNAL(clicked()), this, SLOT(gererTypeDeplacement()));
    connect(ui->typeDeplacementFixe, SIGNAL(clicked()), this, SLOT(gererTypeDeplacement()));
    connect(ui->deplacementGaucheParametrage, SIGNAL(clicked()), this, SLOT(deplacerGauche()));
    connect(ui->deplacementDroiteParametrage, SIGNAL(clicked()), this, SLOT(deplacerDroite()));
    connect(ui->deplacementHautParametrage, SIGNAL(clicked()), this, SLOT(deplacerHaut()));
    connect(ui->deplacementBasParametrage, SIGNAL(clicked()), this, SLOT(deplacerBas()));

    connect(ui->enregistrerPositionInitiale, SIGNAL(clicked()), this, SLOT(sauvegarderPositionInitiale()));
    connect(ui->enregistrerPositionFinale, SIGNAL(clicked()), this, SLOT(sauvegarderPositionFinale()));

    connect(ui->demarrageAcquisitionVideo, SIGNAL(clicked()), this, SLOT(demarrerAcquisitionVideo()));
    connect(ui->arretAcquisitionVideo, SIGNAL(clicked()), this, SLOT(arreterAcquisitionVideo()));
}

/**
  * @brief  Lire les paramètres du fichier de configuration .ini pour créer les caméras ainsi que leur timer
  * @details
  */
void IHMWismas::creerCameras()
{
    QSettings configuration(fichierINI, QSettings::IniFormat);
    int nbCameras = configuration.value("nb_cameras", "0").toInt();
    QString identifiant;
    QString nom;

    for(int compteur = 0; compteur < nbCameras; compteur++)
    {
        identifiant = configuration.value(QString("Camera%1").arg(compteur+1) + "/identifiant").toString();
        nom = configuration.value(QString("Camera%1").arg(compteur+1) + "/nom").toString();

        #ifdef DEBUG
        qDebug() << Q_FUNC_INFO << identifiant << " " << nom;
        #endif

        if(!identifiant.isEmpty())
        {
            Camera* camera = new Camera(compteur+1, identifiant);
            cameras.push_back(camera);

            QTimer* timer = new QTimer();
            timerCamera.push_back(timer);

            ui->listeConfigurationCameras->addItem(nom);

            connect(this, SIGNAL(fini()), camera, SLOT(arreter()));
            connect(camera, SIGNAL(afficherMessage(QString)), this, SLOT(afficherMessage(QString)));
            connect(actionQuitter, SIGNAL(triggered()), camera, SLOT(supprimer()));

            switch(compteur)
            {
            case 0:
                ui->labelEnregistrementCamera1->setText(QString::fromUtf8("Caméra %1 :").arg(nom));
                connect(timerCamera.at(compteur), SIGNAL(timeout()), camera, SLOT(enregistrer()));
                connect(camera, SIGNAL(progression(int)), ui->progessionEnregistrementCamera1, SLOT(setValue(int)));
                break;
            case 1:
                ui->labelEnregistrementCamera2->setText(QString::fromUtf8("Caméra %1 :").arg(nom));
                connect(timerCamera.at(compteur), SIGNAL(timeout()), camera, SLOT(enregistrer()));
                connect(camera, SIGNAL(progression(int)), ui->progessionEnregistrementCamera2, SLOT(setValue(int)));
                break;
            case 2:
                ui->labelEnregistrementCamera3->setText(QString::fromUtf8("Caméra %1 :").arg(nom));
                connect(timerCamera.at(compteur), SIGNAL(timeout()), camera, SLOT(enregistrer()));
                connect(camera, SIGNAL(progression(int)), ui->progessionEnregistrementCamera3, SLOT(setValue(int)));
                break;
            }
        }
    }
}


/**
  * @brief  Créer un url caméra pour récupérer son flux vidéo
  * @details
  */
QUrl IHMWismas::creerUrlCamera() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    int numeroCamera = ui->listeConfigurationCameras->currentIndex();
    Parametres parametres = cameras.at(numeroCamera)->getParametres();

    QUrl URL("http://" + parametres.adresseIP + ":" + parametres.numeroPort + "/mobile.htm");
    URL.setUserName(parametres.identifiant);
    URL.setPassword(parametres.motDePasse);

    return URL;
}

/**
  * @brief  Démarrer la période des caméras
  * @details    A une période 0, l'acquisition vidéo est à nouveau lancée
  * @param  numeroCamera int le numéro de la caméra
  * @param  parametres Parametres les paramètres de la caméra
  */
void IHMWismas::demarrerTimers(const int numeroCamera, const Parametres &parametres)
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    timerCamera.at(numeroCamera)->start(parametres.periode.toInt()*1000);
}

/**
  * @brief  Arrêter la période des caméras
  * @details    Arrêt du système d'acquisition vidéo
  */
void IHMWismas::arreterTimers() const
{
    #ifdef DEBUG
    qDebug() << Q_FUNC_INFO;
    #endif

    QSettings configuration(fichierINI, QSettings::IniFormat);

    int nbTimers = configuration.value("nb_cameras", "0").toInt();

    for(int compteur = 0; compteur < nbTimers; compteur++)
    {
        if(timerCamera.at(compteur)->isActive())
            timerCamera.at(compteur)->stop();
    }
    if(timerInitial->isActive())
        timerInitial->stop();
    if(timerFinal->isActive())
        timerFinal->stop();
}
