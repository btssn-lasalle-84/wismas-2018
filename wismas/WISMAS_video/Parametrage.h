/**
  * @file Parametrage.h
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

#ifndef PARAMETRAGE_H
#define PARAMETRAGE_H

#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QString>
#include <QVector>
#include "Structures.h"

#define DEBUG

/**
 * @brief
 */
class Parametrage
{
    private:
        QString fichierINI;
        Parametres parametres;

    public:
        Parametrage();
        ~Parametrage();

        void setParametres(Parametres &parametres);
        Parametres getParametres() const;

        void lireParametres(QString identifiantCamera);
};

#endif // PARAMETRAGE_H
