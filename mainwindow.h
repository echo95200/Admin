#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QMessageBox>
#include <QPoint>
#include <QDateTime>
#include <QTime>
#include <QDebug>
#include <QCryptographicHash>
#include <QStackedLayout>
#include <QLabel>
#include <QSqlQueryModel>
#include <QTableView>

#include "databaseoperation.h"
#include "lib7z_facade.h"
//Initialization of the QLib7Z
#include "init.h"
#include "choosefiledialog.h"
#include "ftpmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void deleteDir(QString);

private slots:
    void on_pushButtonChooseFile_clicked();

    void on_pushButtonUpload_clicked();

    void uploadFinished(QNetworkReply*);

    void on_radioButtonAll_clicked();

    void on_radioButtonSpecial_clicked();

private:
    Ui::MainWindow *ui;
    DatabaseOperation m_DatabaseOperation;
    QStringList m_FileChoosedList;
    Lib7z::File m_File7z;
    FtpManager m_FtpManager;

    QString m_PackageName;
    QString m_PackageMD5;
    qint64 m_PackageDate;

    //Layout
    QStackedLayout *m_pStackedLayout;
    QSqlQueryModel* m_pModelUser;
    QTableView* m_pTableViewUser;
};

#endif // MAINWINDOW_H