#include "mainwindow.h"
#include <QApplication>

#include "databaseinit.h"
#include "login.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseInit init;
    if (init.DatabaseExist()) {
        Login login;
        login.setWindowTitle("Admin Login");
        login.show();
        if (login.exec() == Login::Accepted) {
            MainWindow w;
            w.show();
            return a.exec();
        } else {
            return 0;
        }
    }
    return 0;
}
