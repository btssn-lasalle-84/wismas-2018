/**
  * @file IHMWismas.h
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

#ifndef IHMWISMAS_H
#define IHMWISMAS_H

#include <QtGui>
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QDebug>
#include <QVector>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include "Camera.h"

#define DEBUG

namespace Ui
{
    class IHMWismas;
}

/**
 * @brief La fenêtre principale de l'application WISMAS_video
 *
 * @class IHMWismas
 */
class IHMWismas : public QWidget
{
    Q_OBJECT

    private:
        Ui::IHMWismas* ui; /**< \brief relation vers la classe IHM */
        QString fichierINI; /**< \brief le fichier de parametrage */
        QVector<QTimer*> timerCamera; /**< \brief les timers de chaque caméra */
        QTimer* timerInitial;
        QTimer* timerFinal;
        QVector<Camera*> cameras; /**< \brief les caméras à gérer */
        QNetworkAccessManager* manager; /**< \brief pour émettre des demandes de deplacement de la camera */
        QAction* actionQuitter;

        void connecter() const;
        void creerCameras();
        QUrl creerUrlCamera() const;
        void demarrerTimers(const int numeroCamera, const Parametres &parametres);
        void arreterTimers() const;

    public:
        explicit IHMWismas(QWidget* parent = 0);
        ~IHMWismas();

    signals:
        void fini();

    private slots:

    public slots:
        void parametrer();
        void demarrer();

        void gererBoutonEtat();
        void gererTypeDeplacement() const;
        void activerBoutonsCamera() const;
        void desactiverBoutonsCamera() const;
        void deplacerGauche() const;
        void deplacerDroite() const;
        void deplacerHaut() const ;
        void deplacerBas() const;

        void sauvegarderParametres();
        void afficherParametresCamera() const;
        void rafraichirFluxVideo(const QString identifiantCamera, Parametres &parametres);
        void afficherFluxVideo() const;
        void sauvegarderPositionInitiale() const;
        void sauvegarderPositionFinale() const;
        void initialiserPosition() const;

        void demarrerAcquisitionVideo();
        void deplacerPositionInitiale();
        void deplacerPositionFinale();
        void arreterAcquisitionVideo();
        void afficherMessage(const QString message) const;
};

#endif // FENETREACCUEIL_H
