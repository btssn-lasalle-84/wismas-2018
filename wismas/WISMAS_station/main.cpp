#include <QApplication>
#include <QTextCodec>
#include <QDebug>
#include "wismasihm.h"

/**
 * @brief Programme principal WISMAS_station
 *
 * @details Crée et affiche la fenêtre principale de l'application WISMAS_station
 *
 * @file WISMAS_station/main.cpp
 *
 * @version 1.1
 */

/**
 * @brief Programme principal : Crée et affiche la fenêtre principale de l'application WISMAS_station
 *
 * @fn main(int argc, char *argv[])
 * @param argc
 * @param argv[]
 * @return int
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    a.setApplicationName("WISMAS_station");

    WISMASIHM w;

    //w.show();
    w.showFullScreen();

    return a.exec();
}
