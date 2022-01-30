#ifndef BASEDEDONNEES_H
#define BASEDEDONNEES_H

#include <QObject>
#include <QtSql/QtSql>
#include <QSqlDatabase>
#include <QMutex>

//#define DEBUG_BASEDEDONNEES

#define HOSTNAME        "localhost"
#define USERNAME        "root"
#define PASSWORD        "password"
#define DATABASENAME    "WISMAS_2018"

class BaseDeDonnees : public QObject
{
    Q_OBJECT
    public:
        static BaseDeDonnees* getInstance();
        static void detruireInstance();

        bool estConnecte();
        bool connecter(QString nomBase = DATABASENAME);
        /* uniquement pour : UPDATE, INSERT et DELETE */
        bool executer(QString requete);
        /* uniquement pour : SELECT */
        bool recuperer(QString requete, QString &donnees); // 1 -> 1
        bool recuperer(QString requete, QStringList &donnees); // 1 -> 1..*
        bool recuperer(QString requete, QVector<QString> &donnees); // 1..* -> 1
        bool recuperer(QString requete, QVector<QStringList> &donnees); // 1..* -> 1..*

    private:
        BaseDeDonnees();
        ~BaseDeDonnees();
        static BaseDeDonnees*  baseDeDonnees;
        static int             nbAcces;
        QSqlDatabase           db;
        QMutex                 mutex;
};

#endif // BASEDEDONNEES_H
