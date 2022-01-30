/**
  * @file main.cpp
  *
  * @brief Lancer l'application administrateur
  *
  * @author Pierre GRELET
  *
  * @version 1.1
  *
  * @date 18 février 2018
  *
  */

#include <QApplication>
#include "IHMWismas.h"
#include "Camera.h"

/**
 * @brief Programme principal WISMAS_video
 *
 * @file WISMAS_video/main.cpp
 */

/**
 * @brief Main du système d'acquisition vidéo
 *
 * @fn main(int argc, char *argv[])
 * @param argc
 * @param argv[]
 * @return int
 */
int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QApplication a(argc, argv);
    IHMWismas w;

    w.showFullScreen();

    return a.exec();
}
