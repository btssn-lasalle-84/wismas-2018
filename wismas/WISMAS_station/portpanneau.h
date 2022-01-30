#ifndef PORTPANNEAU_H
#define PORTPANNEAU_H
/**
  *
  * \file portpanneau.h
  *
  * \brief Déclaration de la classe PortPanneau
  *
  * \author
  *
  * \version 1.1
  *
  */

#include <QObject>
#include "qextserialport.h"
#include <QString>
#include <QStringList>

//#define DEBUG

#define PORT_PANNEAU "/dev/ttyUSB1" /**< \brief Fichier périphérique par défaut */

// cf. http://www1.produktinfo.conrad.com/datenblaetter/575000-599999/590996-da-01-en-Communication_protocol_LED_Displ_Board.pdf

#define PANNEAU_LG_MAX_TRAME    128 // lg max 128
#define PANNEAU_LG_MAX          16 //longeur max 16
#define PANNEAU_LG_REPONSE      4 // au minimum 3 caractères pour ACK

// cf. code ascii
#define PANNEAU_NUL             0x00 // caractère NUL (c'est aussi le code du fin de chaîne)

// cf. protocole
#define PANNEAU_ACK             "ACK"  // accusé réception positif
#define PANNEAU_NACK            "NACK" // accusé réception négatif

// <PX> Indique à quelle page appartient ce message:
#define PANNEAU_PAGE_A 'A' //Page A
#define PANNEAU_PAGE_B 'B' //Page B
#define PANNEAU_PAGE_C 'C'
#define PANNEAU_PAGE_D 'D'
#define PANNEAU_PAGE_E 'E'
#define PANNEAU_PAGE_F 'F'
#define PANNEAU_PAGE_G 'G'
#define PANNEAU_PAGE_H 'H'
#define PANNEAU_PAGE_I 'I'

// Modes (panneau)

// <FX> Indique l'effet d'arrive du message  où X est:
// <FY> Indique l'effet de sortie du message où Y est:
#define PANNEAU_IMMEDIATE 'A' //A/a = Immediate (Image will be immediately appeared)
#define PANNEAU_XOPEN 'B' // B/b = Xopen (Image will be shown from center and extend to 4 side)
#define PANNEAU_CURTAIN_UP 'C' //C/c = Curtain UP (Image will be shown one line by one line from bottom to top).
#define PANNEAU_CURTAIN_DOWN 'D' //D/d = Curtain Down(Image will be shown one line by one line from Top to Bottom
#define PANNEAU_SCROLL_LEFT 'E' //E/e = Scroll Left (Image will be scrolled from Right to Left)
#define PANNEAU_SCROLL_RIGHT 'F' // F/f = Scroll Right (Image will be scrolled from Right to Left)
#define PANNEAU_VOPEN 'G' // G/g = Vopen (Image will be shown from center to top and Bottom one line by one line)
#define PANNEAU_VCLOSE 'H' // H/h = Vclose(Image will be shown from Top and Bottom to Center one line by one line.)
#define PANNEAU_SCROLL_UP 'I' // I/i = Scroll Up(Image will be scroll from Bottom to Top)
#define PANNEAU_SCROLL_DOWN 'J' // J/j = Scroll Down ((Image will be scrolled from Bottom to Top)
#define PANNEAU_HOLD 'K' // K/k = Hold (Previous Screen will be kept)
#define PANNEAU_SNOW 'L' // L/l = Snow ( Pixels will be dropped down from top and stack up to build the image)
#define PANNEAU_TWINKLE 'M' // M/m = Twinkle (a blank diagonal line will be scrolling on the image)
#define PANNEAU_BLOCK_MOVE 'N' //N/n = Block Move (8 pixel width display block will be moved from right to left one by one)
#define PANNEAU_RANDOM 'P' //P/p = Random (Random Pixel will be appeared to build the image)

// <MX> Indique la méthode d'affichage pendant la durée de l'attente et de l'effet lorsque X est:
#define PANNEAU_NORMAL 'A' // (41H) = Normal (Display stay steady while waiting)
#define PANNEAU_BLINK 'B' // (42H) = Blinking (Display Blinking while waiting)

// <AX> Indique la police des caractères suivants où X est:
//#define NORMAL 'A'
#define PANNEAU_SIZE_5X7  'A'// A = 5X7 (Normal size)
#define PANNEAU_SIZE_6X7  'B' // B = 6X7 (Bold size)
#define PANNEAU_BOLD SIZE_6X7 // B = 6X7 (Bold size)
//#define PANNEAU_SIZE_4x7 'C' // C = 4X7 (Narrow size)
//#define PANNEAU_SIZE_7x13 'D' //D = 7X13 (Large size) (for 16 pixel height or more LED display only.)
//#define PANNEAU_SIZE_5x7 'E'// E = 5X8 (Long Size, only for height more than 7 pixels)

#define PANNEAU_POSITION_CENTER 'C'

#define PANNEAU_NB_PAGES_MAX 8

#define DEMO

typedef unsigned char uint8_t;

/**
 * @brief Communique avec les stations météo des sites via un port série virtuel
 *
 * @class PortPanneau
 */
class PortPanneau : public QObject
{
    Q_OBJECT

private:
    QextSerialPort *port; /**< \brief relation vers la classe QextSerialPort */

    QString calculerChecksum(QString message);
    QString creerTrame(QStringList messages, int nb_page);
    void envoyerTrame(QString trame);

    // lit l'acquittement du panneau
    bool lireAcquittement(QString trame);

    // Configure les effets
    QString configure_page(uint8_t _mode);
    QString configure_taille_police(uint8_t _mode);
    QString configure_temps(int temps);
    QString configure_couleur(uint8_t _mode);
    QString configure_position(uint8_t _mode, QString message);

public:
    explicit PortPanneau(QObject *parent = 0);
    ~PortPanneau();

public slots:
    void afficherMessage(QString message);
    void changerPage(int nb_page);
    void lireTrame();
};

#endif // PORTPANNEAU_H
