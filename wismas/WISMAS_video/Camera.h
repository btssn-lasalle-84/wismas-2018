/**
  * @file Camera.h
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

#ifndef CAMERA_H
#define CAMERA_H

#include <QString>
#include <QUrl>
#include "Parametrage.h"
#include "Video.h"

#define DEBUG

/**
 * @brief
 */
class Camera : public QObject
{
    Q_OBJECT
    private:
        Parametrage* parametrage;
        Video* video;
        Parametres parametres;
        int numeroCamera;
        QString identifiant;

    public:
        Camera();
        Camera(int numeroCamera, QString identifiant);
        ~Camera();

        void setIdentifiant(QString identifiant) ;
        void setNumero(int numeroCamera);
        void setParametres(Parametres parametres);
        Parametres getParametres() const;
        void lireParametres();

        QString seDeplacer() const;

    signals:
        void afficherMessage(QString message) const;
        void progression(int chargement) const;

    public slots :
        void enregistrer() const;
        void arreter() const;
        void progresser(int pct) const;
        void terminer() const;
        void supprimer() const;
};

#endif // CAMERA_H
