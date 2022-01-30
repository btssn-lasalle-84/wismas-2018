#include "portpanneau.h"
#include <unistd.h>

#include <QStringList>
#include <QDebug>

/**
  *
  * \file portpanneau.cpp
  *
  * \brief Définition de la classe PortPanneau
  *
  * \author Petrella Olivier
  *
  * \version 1.1
  *
  */

/**
  *
  * \brief Constructeur de la classe PortPanneau
  *
  * \param parent Adresse de l'objet parent Qt (sinon 0)
  *
  */
PortPanneau::PortPanneau(QObject *parent) : QObject(parent)
{
    port = new QextSerialPort(QLatin1String(PORT_PANNEAU), QextSerialPort::EventDriven, this);
    port->setBaudRate(BAUD9600);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->open(QIODevice::ReadWrite);
    if(port->isOpen())
    {
        connect(port, SIGNAL(readyRead()), this, SLOT(lireTrame()));
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Erreur : port" << QLatin1String(PORT_PANNEAU) << "non ouvert !";
    }
}

/**
 * @brief Destructeur
 *
 * @fn PortPanneau::~PortPanneau
 */
PortPanneau::~PortPanneau()
{
    if(port->isOpen())
    {
        port->close();
    }
}

/**
 * @brief
 *
 * @param trame QString la trame
 * @return QString le checksum calculé
 */
QString PortPanneau::calculerChecksum(QString trame)
{
    uint8_t checksum = 0;

    for(int i=0;i<trame.length();i++)
        checksum ^= trame.at(i).toAscii(); // ^= ou exclusif de chaque caractere de la trame

    QString Checksum = QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();

    #ifdef DEMO
        qDebug("checksum :\t0x%02X", checksum);
        qDebug() << Q_FUNC_INFO << Checksum;
    #endif

    return Checksum;
}

/**
 * @brief
 *
 * @param messages
 * @param nb_page
 * @return QString
 */
QString PortPanneau::creerTrame(QStringList messages, int nb_page)
{
    //Exemple de trame : "<ID01><L1><PA><FE><MA><WC><FE>message1F<E>"

    QString protocole = "<L1>";//QString("<L%1>").arg(nb_page+1);
    QString options   = "<FE><MA>";
    QString trame;

    //Conversion des caractères spéciaux
    if(messages[nb_page].contains("-"))
        messages[nb_page].replace("-", "<U0B> ");

    if(messages[nb_page].contains("°"))
        messages[nb_page].replace("°", "<U3A>");

    if(messages[nb_page].contains("é"))
        messages[nb_page].replace("é", "<U69>");

    if(messages[nb_page].contains("€"))
        messages[nb_page].replace("€", "<U00>");

    if(messages[nb_page].contains("à"))
        messages[nb_page].replace("à", "<U60>");

    char page = 0;

    switch(nb_page)
    {
        case 0:
            page = PANNEAU_PAGE_A;
            break;
        case 1:
            page = PANNEAU_PAGE_B;
            break;
        case 2:
            page = PANNEAU_PAGE_C;
            break;
        case 3:
            page = PANNEAU_PAGE_D;
            break;
        case 4:
            page = PANNEAU_PAGE_E;
            break;
        case 5:
            page = PANNEAU_PAGE_F;
            break;
        case 6:
            page = PANNEAU_PAGE_G;
            break;
        case 7:
            page = PANNEAU_PAGE_H;
            break;
        case 8:
            page = PANNEAU_PAGE_I;
            break;
    }


    trame  = protocole;

    trame += configure_page(page);

    trame += options;

    trame += configure_temps(10);

    trame += "<FE>";
    trame += configure_taille_police(PANNEAU_SIZE_5X7);
    trame += configure_couleur('A');
    //trame += configure_position(PANNEAU_POSITION_CENTER, messages[nb_page]);
    trame += messages[nb_page];
    trame += calculerChecksum(trame);
    trame += "<E>";
    trame = "<ID01>" + trame;

    #ifdef DEMO
        qDebug() << Q_FUNC_INFO << trame;
    #endif


    return trame;
}

/**
 * @brief
 *
 * @param trame QString la trame
 */
void PortPanneau::envoyerTrame(QString trame)
{
    port->write(trame.toLocal8Bit().constData());
}

/**
 * @brief
 *
 * @param nb_page
 */
void PortPanneau::changerPage(int nb_page)
{
    char page = 0;

    switch(nb_page)
    {
        case 0:
            page = PANNEAU_PAGE_A;
            break;
        case 1:
            page = PANNEAU_PAGE_B;
            break;
        case 2:
            page = PANNEAU_PAGE_C;
            break;
        case 3:
            page = PANNEAU_PAGE_D;
            break;
        case 4:
            page = PANNEAU_PAGE_E;
            break;
        case 5:
            page = PANNEAU_PAGE_F;
            break;
        case 6:
            page = PANNEAU_PAGE_G;
            break;
        case 7:
            page = PANNEAU_PAGE_H;
            break;
        case 8:
            page = PANNEAU_PAGE_I;
            break;
    }

    QString trame;
    //trame = "<SC>";
    trame = QString("<RP%1>").arg(page);
    trame += calculerChecksum(trame);
    trame += "<E>";
    trame = "<ID01>" + trame;
    this->envoyerTrame(trame);
    #ifdef DEMO
        qDebug() << Q_FUNC_INFO << trame << lireAcquittement(trame);
    #endif
}

/**
 * @brief
 *
 * @param message
 */
void PortPanneau::afficherMessage(QString message)
{
    QStringList messages = message.split(";");
    for(int i = 0; i < messages.count(); i++)
    {
        QString trame = creerTrame(messages, i);
        envoyerTrame(trame);
    }
}

/**
 * @brief
 *
 */
void PortPanneau::lireTrame()
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


    }
}

/**
 * @brief lit l'acquittement du panneau
 *
 * @param trame QString la trame
 * @return bool true si acquittement sinon false
 */
bool PortPanneau::lireAcquittement(QString trame)
{
    if(trame.contains(PANNEAU_NACK))
        return false;
    else if(trame.contains(PANNEAU_ACK))
        return true;
    else
        return false;
}

QString PortPanneau::configure_page(uint8_t _mode)
{
    QString mode;
    if(_mode >='A' && _mode <='I')
        mode = QString("<P%1>").arg(QChar(_mode));
    return mode;
}

// <AX> Indique la police des caractères suivants où X est:
// A = 5X7 (Normal size)
// B = 6X7 (Bold size)
// C = 4X7 (Narrow size)
// D = 7X13 (Large size) (for 16 pixel height or more LED display only.)
// E = 5X8 (Long Size, only for height more than 7 pixels)
QString PortPanneau::configure_taille_police(uint8_t _mode)
{
    QString mode;
    if(_mode >= 'A' && _mode <= 'E')
        mode = QString("<A%1>").arg(QChar(_mode));
    return mode;
}

// <CX> Indique la couleur des caractères suivants où X est:
// A = Dim Red B = Red
// C = Bright Red D = Dim Green
// E = Green F = Bright Green
// G = Dim Orange H = Orange
// I = Bright Orange J = Yellow
// K = Lime L = Inversed Red
// M = Inversed Green N = Inversed Orange
// P = Red on Dim Green Q = Green on Dim Red
// R = R/Y/G S = Rainbow

QString PortPanneau::configure_couleur(uint8_t _mode)
{
    QString mode;
    if(_mode >= 'A' && _mode <= 'S')
        mode = QString("<C%1>").arg(QChar(_mode));
    return mode;
 //
     //sprintf(protocole, "%s<C%c>", protocole, mode);
}

// <WX> Indique le temps d'attente
//B = 1 sec
//C = 2 sec
//D = 3 sec
//Z = 25 sec*/
QString PortPanneau::configure_temps(int temps)
{
    QString mode;
    if(temps >= 0 && temps <= 25)
        mode = QString("<W%1>").arg(QString('A' + temps));
    return mode;
}
