/**
 * @file wismasihm.h
 *
 * @brief Déclaration de la classe WISMASIHM
 *
 * @author PETRELLA Olivier
 *
 * @version 0.9
 *
 * @date 18 février 2018
 */

#ifndef WISMASIHM_H
#define WISMASIHM_H

#include <QWidget>
#include <QTimer>
#include <QString>

#include "ui_wismasihm.h"
#include "structures.h"
#include "stationmeteo.h"

#include <Phonon/MediaObject>
#include <Phonon/VideoWidget>
#include <Phonon/VideoPlayer>

//#define DEBUG

class StationMeteo;
class PortXBee;
class PortPanneau;

namespace Ui
{
    class WISMASIHM;
}

/**
 *
 * @class WISMASIHM
 *
 * @brief Gère l'interface des sites de haute montagne
 *
 * @author PETRELLA Olivier
 *
 * @version 1.1
 *
 * @date 18 février 2018
 */
class WISMASIHM : public QWidget
{
    Q_OBJECT

    public:
        explicit WISMASIHM(QWidget *parent = 0);
        ~WISMASIHM();

        QString getMinute();
        QString getHeure();
        QString getDate();

    private:
        Ui::WISMASIHM* ui;
        StationMeteo* station; /**< \brief Association vers la classe StationMeteo */
        QVector<StationMeteo*> stations;
        PortXBee *portXBee; /**< \brief Association vers la classe PortXBee */
        PortPanneau *portPanneau; /**< \brief Association vers la classe PortPanneau */
        CONFIG_LOGICIEL config;
        CONFIG_ENREGISTREMENT enregistrement_config;
        PARAM_COMMUNICATION param_communication;
        PARAM_PANNEAU param_panneau;
        AFFICHAGE_PANNEAU affichage_panneau;
        AFFICHAGE_PANNEAU panneau;

        Phonon::MediaObject *media;

        QTimer* m_timer; /**< \brief Timer pour le rafraichissement de la fenetre principale */
        int idStationCourante;
        long m_valeur;
        long pagePanneau_compteur;
        int nb_page;

        void afficherConditionsMeteoStation();
        void afficherDonneesMeteoStation();
        void afficherInformationsMeteoStation();
        void demarrerAffichagePanneau();
        void afficherConseilsFartage();
        void afficherHorodatage();
        void changerStation();        
        void demarrerVideo();
        void recupererDerniereVideo();
        void chargerVideo();
        bool chargerFichierConfig();

    private slots:
        void rafraichir();        
        void rechargerVideo();
        void redemarrerVideo();
};

#endif // WISMASIHM_H
