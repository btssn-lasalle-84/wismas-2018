/**
* @file wismasprotocole.h
*
* @brief Déclaration de la classe WISMASProtocole
*
* @author PETRELLA Olivier
*
* @version 0.9
*
* @date 18 février 2018
*/

#ifndef WISMASPROTOCOLE_H
#define WISMASPROTOCOLE_H

#include <QString>

class StationMeteo;

/**
* @class WISMASProtocole
*
* @brief Gère la transmission entre les sites
*
* @author PETRELLA Olivier
*
* @version 1.1
*
* @date 18 février 2018
*/
class WISMASProtocole
{
    private:
        StationMeteo *stationMeteo; /**< \brief Association vers la classe StationMeteo */

    public:
        WISMASProtocole(StationMeteo *stationMeteo = 0);
        ~WISMASProtocole();

        bool verifierTrame(QString trame);
        bool decoderTrame(QString trame);
};

#endif // WISMASPROTOCOLE_H
