#include "basededonnees.h"

#include <QDebug>
#include <QMessageBox>

BaseDeDonnees* BaseDeDonnees::baseDeDonnees = NULL;
int BaseDeDonnees::nbAcces = 0;

BaseDeDonnees::BaseDeDonnees()
{
    #ifdef DEBUG_BASEDEDONNEES
    qDebug() << "<BaseDeDonnees::BaseDeDonnees()>";
    #endif
    db = QSqlDatabase::addDatabase("QMYSQL");
}

BaseDeDonnees::~BaseDeDonnees()
{
    #ifdef DEBUG_BASEDEDONNEES
    qDebug() << "<BaseDeDonnees::~BaseDeDonnees()>";
    #endif
}

BaseDeDonnees* BaseDeDonnees::getInstance()
{
    if(baseDeDonnees == NULL)
        baseDeDonnees = new BaseDeDonnees();

    nbAcces++;
    #ifdef DEBUG_BASEDEDONNEES
    qDebug() << "<BaseDeDonnees::getInstance()> nbAcces = " << nbAcces;
    #endif

    return baseDeDonnees;
}

void BaseDeDonnees::detruireInstance()
{
    // instance ?
    if(baseDeDonnees != NULL)
    {
        nbAcces--;
        #ifdef DEBUG_BASEDEDONNEES
        qDebug() << "<BaseDeDonnees::detruireInstance()> nbAcces restants = " << nbAcces;
        #endif
        // dernier ?
        if(nbAcces == 0)
            delete baseDeDonnees;
    }
}

bool BaseDeDonnees::estConnecte()
{
    return db.isOpen();
}

bool BaseDeDonnees::connecter(QString nomBase /*= DATABASENAME*/)
{
    QMutexLocker verrou(&mutex);
    if(!db.isOpen())
    {
       db.setHostName(HOSTNAME);
       db.setUserName(USERNAME);
       db.setPassword(PASSWORD);
       db.setDatabaseName(nomBase);

       #ifdef DEBUG_BASEDEDONNEES
       qDebug() << "HostName : " << db.hostName();
       qDebug() << "UserName : " << db.userName();
       qDebug() << "DatabaseName : " << db.databaseName();
       #endif
       if(db.open())
       {
           #ifdef DEBUG_BASEDEDONNEES
           qDebug() << QString::fromUtf8("<BaseDeDonnees::connecter()> connexion réussie à %1").arg(db.hostName());
           #endif

           return true;
       }
       else
       {
           qDebug() << QString::fromUtf8("<BaseDeDonnees::connecter()> erreur : impossible de se connecter à la base de données !");

           QMessageBox::critical(0, QString::fromUtf8("Test MO"), QString::fromUtf8("Impossible de se connecter à la base de données !"));

           return false;
       }
    }
    else
        return true;
}

/* pour : UPDATE, INSERT et DELETE */
bool BaseDeDonnees::executer(QString requete)
{
    QMutexLocker verrou(&mutex);
    QSqlQuery r;
    bool retour;

    if(db.isOpen())
    {
        retour = r.exec(requete);
        #ifdef DEBUG_BASEDEDONNEES
        qDebug() << QString::fromUtf8("<BaseDeDonnees::executer()> retour %1 pour la requete : %2").arg(QString::number(retour)).arg(requete);
        #endif
        if(retour)
        {
            return true;
        }
        else
        {
            qDebug() << QString::fromUtf8("<BaseDeDonnees::executer()> erreur : %1 pour la requête %2").arg(r.lastError().text()).arg(requete);

            return false;
        }
    }
    else
        return false;

}

/* uniquement pour récupérer (SELECT) UN champ d'UN seul enregistrement
   Remarque : le champ est stocké dans un QString
 */
bool BaseDeDonnees::recuperer(QString requete, QString &donnees)
{
    QMutexLocker verrou(&mutex);
    QSqlQuery r;
    bool retour;

    if(db.isOpen())
    {
        retour = r.exec(requete);
        #ifdef DEBUG_BASEDEDONNEES
        qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QString)> retour %1 pour la requete : %2").arg(QString::number(retour)).arg(requete);
        #endif
        if(retour)
        {
            // on se positionne sur l'enregistrement
            r.first();

            // on vérifie l'état de l'enregistrement retourné
            if(!r.isValid())
            {
                #ifdef DEBUG_BASEDEDONNEES
                qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QString)> résultat non valide !");
                #endif
                return false;
            }

            // on récupère sous forme de QString la valeur du champ
            if(r.isNull(0))
            {
                #ifdef DEBUG_BASEDEDONNEES
                qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QString)> résultat vide !");
                #endif
                return false;
            }
            donnees = r.value(0).toString();
            #ifdef DEBUG_BASEDEDONNEES
            qDebug() << "<BaseDeDonnees::recuperer(QString, QString)> enregistrement -> " << donnees;
            #endif
            return true;
        }
        else
        {
            qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QString)> erreur : %1 pour la requête %2").arg(r.lastError().text()).arg(requete);

            return false;
        }
    }
    else
        return false;
}

/* uniquement pour récupérer (SELECT) plusieurs champs d'UN seul enregistrement
   Remarque : les différents champs sont stockés dans un QStringList
 */
bool BaseDeDonnees::recuperer(QString requete, QStringList &donnees)
{
    QMutexLocker verrou(&mutex);
    QSqlQuery r;
    bool retour;

    if(db.isOpen())
    {
        retour = r.exec(requete);
        #ifdef DEBUG_BASEDEDONNEES
        qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QStringList)> retour %1 pour la requete : %2").arg(QString::number(retour)).arg(requete);
        #endif
        if(retour)
        {
            // on se positionne sur l'enregistrement
            r.first();

            // on vérifie l'état de l'enregistrement retourné
            if(!r.isValid())
            {
                #ifdef DEBUG_BASEDEDONNEES
                qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QStringList)> résultat non valide !");
                #endif
                return false;
            }

            // on récupère sous forme de QString la valeur de tous les champs sélectionnés
            // et on les stocke dans une liste de QString
            for(int i=0;i<r.record().count();i++)
                if(!r.isNull(i))
                    donnees << r.value(i).toString();
            #ifdef DEBUG_BASEDEDONNEES
            qDebug() << "<BaseDeDonnees::recuperer(QString, QStringList)> enregistrement -> " << donnees;
            #endif
            return true;
        }
        else
        {
            qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QStringList)> erreur : %1 pour la requête %2").arg(r.lastError().text()).arg(requete);
            return false;
        }
    }
    else
        return false;
}

/* uniquement pour récupérer (SELECT) un seul champ de plusieurs enregistrements
   Remarque : le champ des différents enregistrements est stocké dans un QVector de QString
 */
bool BaseDeDonnees::recuperer(QString requete, QVector<QString> &donnees)
{
    QMutexLocker verrou(&mutex);
    QSqlQuery r;
    bool retour;
    QString data;

    if(db.isOpen())
    {
        retour = r.exec(requete);
        #ifdef DEBUG_BASEDEDONNEES
        qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QVector<QString>)> retour %1 pour la requete : %2").arg(QString::number(retour)).arg(requete);
        #endif
        if(retour)
        {
            // pour chaque enregistrement
            while ( r.next() )
            {
                // on récupère sous forme de QString la valeur du champs sélectionné
                data = r.value(0).toString();

                #ifdef DEBUG_BASEDEDONNEES
                //qDebug() << "<BaseDeDonnees::recuperer(QString, QVector<QString>)> enregistrement -> " << data;
                #endif

                // on stocke l'enregistrement dans le QVector
                donnees.push_back(data);
            }
            #ifdef DEBUG_BASEDEDONNEES
            qDebug() << "<BaseDeDonnees::recuperer(QString, QVector<QString>)> enregistrement -> " << donnees;
            #endif
            return true;
        }
        else
        {
            qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QVector<QString>)> erreur : %1 pour la requête %2").arg(r.lastError().text()).arg(requete);

            return false;
        }
    }
    else
        return false;
}

/* uniquement pour récupérer (SELECT) plusieurs champs de plusieurs enregistrements
   Remarque : les différents champs des différents enregistrements sont stockés dans un QVector de QStringList
 */
bool BaseDeDonnees::recuperer(QString requete, QVector<QStringList> &donnees)
{
    QMutexLocker verrou(&mutex);
    QSqlQuery r;
    bool retour;
    QStringList data;

    if(db.isOpen())
    {
        retour = r.exec(requete);
        #ifdef DEBUG_BASEDEDONNEES
        qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QVector<QStringList>)> retour %1 pour la requete : %2").arg(QString::number(retour)).arg(requete);
        #endif
        if(retour)
        {
            // pour chaque enregistrement
            while ( r.next() )
            {
                // on récupère sous forme de QString la valeur de tous les champs sélectionnés
                // et on les stocke dans une liste de QString
                for(int i=0;i<r.record().count();i++)
                    data << r.value(i).toString();

                #ifdef DEBUG_BASEDEDONNEES
                //qDebug() << "<BaseDeDonnees::recuperer(QString, QVector<QStringList>)> enregistrement -> " << data;
                /*for(int i=0;i<r.record().count();i++)
                    qDebug() << r.value(i).toString();*/
                #endif

                // on stocke l'enregistrement dans le QVector
                donnees.push_back(data);

                // on efface la liste de QString pour le prochain enregistrement
                data.clear();
            }
            #ifdef DEBUG_BASEDEDONNEES
            qDebug() << "<BaseDeDonnees::recuperer(QString, QVector<QStringList>)> enregistrement -> " << donnees;
            #endif
            return true;
        }
        else
        {
            qDebug() << QString::fromUtf8("<BaseDeDonnees::recuperer(QString, QVector<QStringList>)> erreur : %1 pour la requête %2").arg(r.lastError().text()).arg(requete);

            return false;
        }
    }
    else
        return false;
}
