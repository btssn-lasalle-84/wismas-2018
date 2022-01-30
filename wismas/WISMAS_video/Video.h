/**
  * @file Video.h
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

#ifndef VIDEO_H
#define VIDEO_H

#include <QtCore>
#include <QApplication>
#include <QDebug>

#define DEBUG

/**
 * @brief
 */
class Video : public QObject
{
    Q_OBJECT
    private:
        QProcess* processus;
        QTimer* timerProgression;
        QTimer* timerEnregistrement;
        bool enregistrement;
        int chargement;
        int numeroVideo;
        int duree;
        QString fichierINI;
        QString cheminVideo;

        int getNumeroVideo();
        void creerCheminVideo(int numeroCamera);
        void demarrerTimers(int duree);
        void arreterTimers();
        void verifierDossierEnregistrements(QDir& repertoire) const;
        void supprimerTousLesEnregistrements(QDir &repertoire) const;
        void supprimerNbEnregistrements(QDir &repertoire) const;

    public:
        Video();
        ~Video();
        void enregistrerVideo(const int numeroCamera, const QString nomSite, const QString adresseIP, const QString numeroPort, const QString identifiant, const QString motDePasse, const int duree);
        static void arreterEnregistrements();

    signals:
        void fini() const;
        void progression(int chargement) const;

    public slots:
        void arreter();
        void progresser();
        void supprimerEnregistrements() const;
};

#endif // VIDEO_H
