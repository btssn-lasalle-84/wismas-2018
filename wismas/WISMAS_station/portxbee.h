#ifndef PORTXBEE_H
#define PORTXBEE_H

/**
  *
  * \file portxbee.h
  *
  * \brief Déclaration de la classe PortXBee
  *
  * \author
  *
  * \version 1.1
  *
  */

#include <QObject>
#include "qextserialport.h"
#include <QString>

#define PORT_XBEE "/dev/ttyUSB0" /**< \brief Fichier périphérique par défaut */

/**
 * @brief Communique avec les stations météo des sites via un port série virtuel
 *
 * @class PortXBee
 */
class PortXBee : public QObject
{
    Q_OBJECT

private:
    QextSerialPort *port; /**< \brief relation vers la classe QextSerialPort */

public:
    /**
     * @brief Constructeur
     *
     * @fn PortXBee
     * @param parent QObject
     */
    explicit PortXBee(QObject *parent = 0);
    /**
     * @brief Destructeur
     *
     * @fn ~PortXBee
     */
    ~PortXBee();

signals:
    void nouvelleTrame(QString trame);

public slots:
    /**
     * @brief Receptionne les trames en provenance des stations météos
     *
     * @fn recevoirTrame
     */
    void recevoirTrame();
};

#endif // PORTXBEE_H
