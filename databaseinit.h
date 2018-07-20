#ifndef DATABASEINIT_H
#define DATABASEINIT_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

class DatabaseInit
{
public:
    DatabaseInit();
    static bool DatabaseExist();
};

#endif // DATABASEINIT_H
