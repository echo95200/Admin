#include "databaseoperation.h"

DatabaseOperation::DatabaseOperation(QObject *parent) : QObject(parent)
{

}

//Check is the user name and the password is correct
bool DatabaseOperation::checkLogin(QString userName, QString password)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (!db.isOpen()) {
        qDebug() << "Cannot open the database!";
    } else {
        QString sql = "SELECT * FROM T_USER WHERE F_NAME = ? AND F_PASSWORD = ?";
        query.prepare(sql);
        query.bindValue(0,userName);
        query.bindValue(1,password);
        if (!query.exec()) {
            qDebug() << query.lastError();
        } else {
            if (query.next()) {
                db.close();
                return true;
            }
        }
    }
    db.close();
    return false;
}

bool DatabaseOperation::insertPackage(QString version, QString md5, qint64 date)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (!db.isOpen()) {
        QMessageBox::about(NULL,"ERROR","Database file cannot be opened!");
    } else {
        QString sql = "INSERT INTO T_PACKAGE VALUES (NULL,?,?,?)";
        query.prepare(sql);
        query.bindValue(0,version);
        query.bindValue(1,md5);
        query.bindValue(2,date);
        if (!query.exec()) {
            QMessageBox::about(NULL,"ERROR",query.lastError().text());
        } else {
            db.close();
            return true;
        }
    }
    db.close();
    return false;
}

bool DatabaseOperation::updateAuthorization(QString packageName)
{
    int id = -1;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (!db.isOpen()) {
        QMessageBox::about(NULL,"ERROR","Database file cannot be opened!");
    } else {

        //Get the ID of the package
        QString sql = "SELECT F_ID FROM T_PACKAGE WHERE F_VERSION = ?";
        query.prepare(sql);
        query.bindValue(0,packageName);
        if (!query.exec()) {
            QMessageBox::about(NULL,"ERROR",query.lastError().text());
        } else {
            if (query.next()) {
                id = query.value(0).toInt();
                //qDebug() <<"update authorization" << query.value(0).toInt();
            }
        }

        //Update the authorization of the users
        sql = "UPDATE T_AUTHORIZATION tau "
              "SET F_ID_PACKAGE = ? "
              "WHERE EXISTS "
              "(SELECT * FROM T_CUSTOMER tcus "
              "WHERE tau.F_ID_CUSTOMER = tcus.F_ID "
              "AND tcus.F_FLAG_BLACK = 'NO') ";
        query.prepare(sql);
        query.bindValue(0,id);
        if (!query.exec()) {
            QMessageBox::about(NULL,"ERROR",query.lastError().text());
        } else {
            db.close();
            return true;
        }
    }
    db.close();
    return false;
}

bool DatabaseOperation::updateAuthorizationWithCustomer(QString packageName, QList<int> listIdCustomer)
{
    int idPackage = -1;
     QSqlDatabase db = QSqlDatabase::database();
     QSqlQuery query(db);
     if (!db.isOpen()) {
         QMessageBox::about(NULL,"ERROR","Database file cannot be opened!");
     } else {

         //Get the ID of the package
         QString sql = "SELECT F_ID FROM T_PACKAGE WHERE F_VERSION = ?";
         query.prepare(sql);
         query.bindValue(0,packageName);
         if (!query.exec()) {
             QMessageBox::about(NULL,"ERROR",query.lastError().text());
         } else {
             if (query.next()) {
                 idPackage = query.value(0).toInt();
                 qDebug() << "special";
                 qDebug() << query.value(0).toInt();
             }
         }

         for (int i=0;i<listIdCustomer.count();i++) {
             //Update the authorization of the special customer
             sql = "UPDATE T_AUTHORIZATION tau "
                   "SET F_ID_PACKAGE = ? "
                   "WHERE EXISTS "
                   "(SELECT * FROM T_CUSTOMER tcus "
                   "WHERE tau.F_ID_CUSTOMER = tcus.F_ID "
                   "AND tcus.F_FLAG_BLACK = 'NO' "
                   "AND tau.F_ID_CUSTOMER = ?) ";
             query.prepare(sql);
             query.bindValue(0,idPackage);
             query.bindValue(1,listIdCustomer.at(i));
             if (!query.exec()) {
                 QMessageBox::about(NULL,"ERROR",query.lastError().text());
             }
         }
         db.close();
         return true;
     }
     db.close();
     return false;}

//Select all the customers
/*
void DatabaseOperation::listAllUsers(QSqlQueryModel *model)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::about(NULL,"ERROR","Database file cannot be opened!");
    } else {
        model->setQuery("SELECT F_ID,F_SYS_CLIENT_REF FROM T_CUSTOMER WHERE F_FLAG_BLACK = 'NO'");
        db.close();
    }
}
*/

//Show the customers with conditions
void DatabaseOperation::listSpecialUsers(QSqlTableModel *model, QString filter)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::about(NULL,"ERROR","Database file cannot be opened!");
    } else {
        model->setTable("T_CUSTOMER");
        model->setFilter(filter);
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();
        db.close();
    }
}
