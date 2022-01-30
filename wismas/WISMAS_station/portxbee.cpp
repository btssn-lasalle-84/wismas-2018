#include "portxbee.h"
#include <unistd.h>

#include <QDebug>

/**
  *
  * \file portxbee.cpp
  *
  * \brief Définition de la classe PortXBee
  *
  * \author Petrella Olivier
  *
  * \version 1.1
  *
  */

/**
  *
  * \brief Constructeur de la classe PortXBee
  *
  * \param parent Adresse de l'objet parent Qt (sinon 0)
  *
  */
PortXBee::PortXBee(QObject *parent) : QObject(parent)
{
    port = new QextSerialPort(QLatin1String(PORT_XBEE), QextSerialPort::EventDriven, this);
    port->setBaudRate(BAUD9600);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->open(QIODevice::ReadWrite);
    if(port->isOpen())
    {
        connect(port, SIGNAL(readyRead()), this, SLOT(recevoirTrame()));
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Erreur : port" << QLatin1String(PORT_XBEE) << "non ouvert !";
    }
}

/**
 * @brief Destructeur
 *
 * @fn PortXBee::~PortXBee
 */
PortXBee::~PortXBee()
{
    if(port->isOpen())
    {
        port->close();
    }
}

/**
 * @brief
 *
 */
void PortXBee::recevoirTrame()
{
    QByteArray donneesRecues;

    if(port->isOpen())
    {
        while(port->bytesAvailable())
        {
            donneesRecues += port->readAll();
            usleep(10000);
        }

        QString trame(donneesRecues);

        //qDebug() << Q_FUNC_INFO << trame;

        emit nouvelleTrame(trame);
    }
    else
    {
        // Simulation
        //QString trame = "$ID.101,TS.1,DV.SE,VV.11,TA.20,TN.-15,H.30,HY.80,B.1010;";
        //emit nouvelleTrame(trame);
    }
}



