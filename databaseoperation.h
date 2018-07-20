#ifndef DATABASEOPERATION_H
#define DATABASEOPERATION_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlQueryModel>

class DatabaseOperation : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseOperation(QObject *parent = nullptr);

    bool checkLogin(QString,QString);
    bool insertPackage(QString version,QString md5,qint64 date);

    //Update the authorization for all users without the black list
    bool updateAuthorization(QString packageName);

    //Show all the users
    void listAllUsers(QSqlQueryModel*);

signals:

public slots:
};

#endif // DATABASEOPERATION_H
