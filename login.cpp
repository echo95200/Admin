#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    ui->pushButtonConnect->setFocus();
}

Login::~Login()
{
    delete ui;
}

//Check the login information
void Login::on_pushButtonConnect_clicked()
{
    QString userName = ui->lineEditAccount->text();
    QString password = ui->lineEditPassword->text();

    if (userName.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this,tr("Warning!"),tr("please insert completely !"),QMessageBox::Yes);
    } else {
        if (m_DatabaseOperation.checkLogin(userName,password)) {
            this->accept();
        } else {
            QMessageBox::warning(this,tr("Warning!"),tr("Login and password incorrect!"),QMessageBox::Yes);
            ui->lineEditPassword->clear();
            ui->lineEditPassword->setFocus();
        }
    }
}

void Login::on_pushButtonExit_clicked()
{
    this->close();
}
