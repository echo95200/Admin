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
#include <QSqlTableModel>
#include <QModelIndexList>
#include <QModelIndex>
#include <QTableView>
#include <QTextEdit>
#include <QList>
#include <QThread>

#include "databaseoperation.h"
#include "lib7z_facade.h"
//Initialization of the QLib7Z
#include "init.h"
#include "choosefiledialog.h"
#include "ftpmanager.h"
#include "waitingdialog.h"
#include "compressionworker.h"

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

signals:

    void compressionSignal();

    void uploadSuccessdSignal();
    void uploadFailedSignal();

private slots:
    void on_pushButtonChooseFile_clicked();

    void on_pushButtonUpload_clicked();

    void on_radioButtonAll_clicked();

    void on_radioButtonSpecial_clicked();

    void on_pushButtonSerachUser_clicked();

    void on_pushButtonTest_clicked();

    //Slot for the compression error
    void errorCompressionSlot();

    //Slot for the end of the compression
    void compressionFinishedSlot();

    //Slot for the upload work
    void uploadFinishedSlot();
    void downloadFinishedSlot();
    void uploadSuccessdSlot();
    void uploadFailedSlot();

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
    QTableView* m_pTableViewCustomer;
    QSqlTableModel* m_pModelCustomer;
    QList<int> m_iListIdCustomer;

    WaitingDialog m_WaitingDialog;

    //File manager
    QString m_FileName;
    QString m_PathName;
    QString m_DownloadFileName;
};

#endif // MAINWINDOW_H
