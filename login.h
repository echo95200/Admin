#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QMessageBox>
#include "databaseoperation.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonExit_clicked();

private:
    Ui::Login *ui;
    DatabaseOperation m_DatabaseOperation;
};

#endif // LOGIN_H
