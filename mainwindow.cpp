#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Interface
    m_pStackedLayout = new QStackedLayout();
    m_pTableViewUser = new QTableView();
    QLabel *pAllUserPage = new QLabel(this);
    pAllUserPage->setText("This package is for all users");
    m_pStackedLayout->addWidget(pAllUserPage);
    m_pStackedLayout->addWidget(m_pTableViewUser);
    ui->verticalLayout->addLayout(m_pStackedLayout);
    ui->radioButtonAll->setChecked(true);
    m_pModelUser = new QSqlQueryModel(m_pTableViewUser);

    //Set the parametres for the compression
    Lib7z::init();
    m_File7z.path = "temp.7z";
    m_File7z.permissions = 0;
    m_File7z.compressedSize = 836;
    m_File7z.uncompressedSize = 5242880;
    m_File7z.isDirectory = false;
    m_File7z.archiveIndex = QPoint(0,0);
    m_File7z.mtime = QDateTime(QDate::fromJulianDay(2456413), QTime(12, 50, 42));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Choose the files for the compression
void MainWindow::on_pushButtonChooseFile_clicked()
{

    //Update the infos
    m_FileChoosedList.clear();
    ui->labelListFile->setText("The list of file :");
    ui->textEditChooseFile->clear();

    ChooseFileDialog fileDialog;
    if(fileDialog.exec() == QDialog::Accepted){
        m_FileChoosedList = fileDialog.selectedFiles();
        ui->labelListFile->setText("You have selected " + QString::number(m_FileChoosedList.size(),10) + " files!");
        for(int i=0;i<fileDialog.selectedFiles().size();i++){
            ui->textEditChooseFile->append(fileDialog.selectedFiles().at(i));
        }
    }
}

//Compress the file into 7z
//Upload the 7zFile into ftp server
void MainWindow::on_pushButtonUpload_clicked()
{
    if (m_FileChoosedList.size() == 0 || (ui->lineEditFileName->text().isEmpty())){
        this->statusBar()->showMessage("The file and the name can't be empty",10000);
    } else {
        QString fileName = ui->lineEditFileName->text();

        //Create a temporary file
        //Path is the current path + /temp
        QDir *temp = new QDir;
        QString preFileName = temp->absolutePath() + "/temp/";
        bool exist = temp->exists(preFileName);
        if (exist) {
            deleteDir(preFileName);
        }
        temp->mkdir(preFileName);

        //Begin to compress the file into .7z
        QFile file(preFileName + fileName);
        file.open(QIODevice::ReadWrite);
        Lib7z::createArchive(&file,m_FileChoosedList);
        if (Lib7z::isSupportedArchive(&file)) {
            m_FileChoosedList.clear();
            ui->labelListFile->setText("The list of files :");
            ui->lineEditFileName->clear();
            ui->textEditChooseFile->clear();
            ui->statusBar->showMessage("Compress the files ok!",10000);

            //get the information of the package
            m_PackageName = fileName;
            QByteArray ba = QCryptographicHash::hash(file.readAll(),QCryptographicHash::Md5);
            m_PackageMD5 = ba.toHex().constData();
            m_PackageDate = QDateTime::currentDateTime().toTime_t();

            //Begin to upload the 7z file
            m_FtpManager.setHostPort("192.168.0.18",21);
            m_FtpManager.setUserInfo("ftpuser","echo");
            m_FtpManager.put(preFileName + fileName,"/data/" + fileName);
            connect(&m_FtpManager,SIGNAL(uploadFinished(QNetworkReply*)),this,SLOT(uploadFinished(QNetworkReply*)));

            //Update the database

        }
        file.close();
    }
}

void MainWindow::deleteDir(QString nameFile)
{
    QDir dir(nameFile);
    if (dir.exists()) {
        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        QFileInfoList fileList = dir.entryInfoList();
        foreach (QFileInfo file, fileList) {
            if (file.isFile()) {
                file.dir().remove(file.fileName());
            } else {
                deleteDir(file.absoluteFilePath());
            }
        }
    }
    dir.rmpath(dir.absolutePath());
}

void MainWindow::uploadFinished(QNetworkReply *reply)
{
    //update the database for package
    m_DatabaseOperation.insertPackage(m_PackageName,m_PackageMD5,m_PackageDate);

    //update the database for authorization
    m_DatabaseOperation.updateAuthorization(m_PackageName);
    ui->statusBar->showMessage("Update the database ok!",10000);
}

void MainWindow::on_radioButtonAll_clicked()
{
    m_pStackedLayout->setCurrentIndex(0);
}

void MainWindow::on_radioButtonSpecial_clicked()
{
    m_pStackedLayout->setCurrentIndex(1);
    m_DatabaseOperation.listAllUsers(m_pModelUser);
    m_pTableViewUser->setModel(m_pModelUser);
    m_pTableViewUser->verticalHeader()->hide();
    m_pTableViewUser->setColumnWidth(1,150);
    m_pTableViewUser->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}
