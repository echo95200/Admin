#include "databaseinit.h"

DatabaseInit::DatabaseInit()
{

}

//Initialization the database
bool DatabaseInit::DatabaseExist()
{
    bool flag = false;
    QString databaseFilePath = "/home/echo/Database/autoUpdate.fdb";
    QFile dbFile(databaseFilePath);
    if (dbFile.exists()) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE");
        db.setDatabaseName(dbFile.fileName());
        db.setUserName("SYSDBA");
        db.setPassword("masterkey");
        if (db.open()) {
            flag = true;
        } else {
            QMessageBox::about(NULL,"ERROR","Database file cannot be opened!");
        }
    } else {
        QMessageBox::about(NULL,"Info","Database file does not exist!");
    }
    return flag;
}
