#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Interface
    m_pStackedLayout = new QStackedLayout();
    m_pTableViewCustomer = new QTableView();
    QLabel *pAllUserPage = new QLabel(this);
    pAllUserPage->setText("This package is for all users");
    m_pStackedLayout->addWidget(pAllUserPage);
    m_pStackedLayout->addWidget(m_pTableViewCustomer);
    ui->verticalLayout->addLayout(m_pStackedLayout);
    ui->radioButtonAll->setChecked(true);
    ui->lineEditSearchUser->hide();
    ui->pushButtonSerachUser->hide();
    m_pModelCustomer = new QSqlTableModel(m_pTableViewCustomer);
    ui->progressBar->hide();

    //Set the parametres for the compression
    Lib7z::init();
    m_File7z.path = "temp.7z";
    m_File7z.permissions = 0;
    m_File7z.compressedSize = 836;
    m_File7z.uncompressedSize = 5242880;
    m_File7z.isDirectory = false;
    m_File7z.archiveIndex = QPoint(0,0);
    m_File7z.mtime = QDateTime(QDate::fromJulianDay(2456413), QTime(12, 50, 42));

    //Configuration of Ftp manager
    connect(&m_FtpManager,SIGNAL(uploadFinished()),this,SLOT(uploadFinished()));
    //connect(&m_FtpManager,SIGNAL(uploadProcess(qint64,qint64)),this,SLOT(uploadProgress(qint64,qint64)));
    //connect(&m_FtpManager,SIGNAL(uploadProcess(qint64,qint64)),&m_WaitingDialog,SLOT(uploadProgressSlot(qint64,qint64)));

    //Configuration of Waiting dialog
    m_WaitingDialog.setWindowTitle("Upload");
    connect(this,SIGNAL(compressionSignal()),&m_WaitingDialog,SLOT(compressionSlot()));
//    connect(this,SIGNAL(uploadProgressSignal1()),&m_WaitingDialog,SLOT(uploadProgressSlot1()));
//    connect(this,SIGNAL(uploadProgressSignal2()),&m_WaitingDialog,SLOT(uploadProgressSlot2()));
//    connect(this,SIGNAL(uploadProgressSignal3()),&m_WaitingDialog,SLOT(uploadProgressSlot3()));
//    connect(this,SIGNAL(uploadProgressSignal4()),&m_WaitingDialog,SLOT(uploadProgressSlot4()));
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
    if (ui->radioButtonSpecial->isChecked()) {
        m_iListIdCustomer.clear();
        QModelIndexList listSelected = m_pTableViewCustomer->selectionModel()->selectedIndexes();
        foreach (QModelIndex index, listSelected) {
            int row = index.row();
            int id = m_pModelCustomer->data(m_pModelCustomer->index(row,0)).toInt();
            m_iListIdCustomer.append(id);
        }
        qDebug() << "ID : " << m_iListIdCustomer;
        qDebug() << "You have selected " << listSelected.count();
        if (listSelected.count() == 0) {
            this->statusBar()->showMessage("You have to select some customers",10000);
            return;
        }
    }

    if (m_FileChoosedList.size() == 0 || (ui->lineEditFileName->text().isEmpty())){
        this->statusBar()->showMessage("The file and the name can't be empty",10000);
    } else {

        this->hide();
        m_WaitingDialog.initialize();
        m_WaitingDialog.show();
        QTime t;
        t.start();
        while (t.elapsed() < 1000) {
            QCoreApplication::processEvents();
        }

        emit compressionSignal();
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
            emit uploadProgressSignal1();

            //Update the database

        }
        file.close();

        //After finish the upload file
        this->show();
        m_WaitingDialog.close();

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

void MainWindow::uploadFinished()
{
    //update the database for package
    m_DatabaseOperation.insertPackage(m_PackageName,m_PackageMD5,m_PackageDate);

    //update the database for authorization
    if (ui->radioButtonAll->isChecked()) {
        m_DatabaseOperation.updateAuthorization(m_PackageName);
        ui->statusBar->showMessage("Update the database ok!",10000);
    } else {
        //TODO
        m_DatabaseOperation.updateAuthorizationWithCustomer(m_PackageName,m_iListIdCustomer);
        ui->statusBar->showMessage("Update the database ok!",10000);
        ui->lineEditSearchUser->clear();
        m_pModelCustomer->clear();
    }

}

void MainWindow::on_radioButtonAll_clicked()
{
    m_pStackedLayout->setCurrentIndex(0);
    ui->lineEditSearchUser->hide();
    ui->pushButtonSerachUser->hide();
}

void MainWindow::on_radioButtonSpecial_clicked()
{
    m_pStackedLayout->setCurrentIndex(1);
    m_pTableViewCustomer->setModel(m_pModelCustomer);
    m_pTableViewCustomer->verticalHeader()->hide();
    m_pTableViewCustomer->setColumnWidth(1,150);
    m_pTableViewCustomer->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    ui->lineEditSearchUser->show();
    ui->pushButtonSerachUser->show();
}

void MainWindow::on_pushButtonSerachUser_clicked()
{
    QString strListUser = ui->lineEditSearchUser->text();
    QStringList listUser = strListUser.split(";");
    QString modelFilter;
    QString subFilter;
    for (int i=0;i < listUser.count();i++) {
        if (i != 0) {
            modelFilter += " or ";
        }
        subFilter = QString().sprintf("F_SYS_CLIENT_REF LIKE '%%%s%%' AND F_FLAG_BLACK = 'NO'",QString(listUser.at(i)).toUtf8().data());
        modelFilter += subFilter;
    }
    qDebug() << modelFilter;
    m_DatabaseOperation.listSpecialUsers(m_pModelCustomer,modelFilter);
    m_pTableViewCustomer->setModel(m_pModelCustomer);
    m_pTableViewCustomer->setColumnWidth(1,150);
    m_pTableViewCustomer->setColumnHidden(2,true);
}

void MainWindow::on_pushButtonTest_clicked()
{
    this->close();
}
