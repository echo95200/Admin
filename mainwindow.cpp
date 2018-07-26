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
    connect(&m_FtpManager,SIGNAL(uploadFinished()),this,SLOT(uploadFinishedSlot()));
    connect(&m_FtpManager,SIGNAL(downloadFinished()),this,SLOT(downloadFinishedSlot()));
    connect(&m_FtpManager,SIGNAL(uploadProcess(qint64,qint64)),&m_WaitingDialog,SLOT(uploadProgressSlot(qint64,qint64)));

    //Signal and slot for checking the integrity of the file
    connect(this,SIGNAL(uploadSuccessdSignal()),this,SLOT(uploadSuccessdSlot()));
    connect(this,SIGNAL(uploadFailedSignal()),this,SLOT(uploadFailedSlot()));

    //Configuration of Waiting dialog
    m_WaitingDialog.setWindowTitle("Upload");
    connect(this,SIGNAL(compressionSignal()),&m_WaitingDialog,SLOT(compressionSlot()));
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
        //QString fileName = ui->lineEditFileName->text();
        m_FileName = ui->lineEditFileName->text();
        QDir *temp = new QDir;
        m_PathName = temp->absolutePath() + "/temp/";

        //Create a new thread to compress the file
        QThread *thread = new QThread();
        CompressionWorker *worker = new CompressionWorker(m_FileName,m_PathName,m_FileChoosedList);
        worker->moveToThread(thread);
        connect(worker,SIGNAL(error(QString)),this,SLOT(errorCompressionSlot()));
        connect(thread,SIGNAL(started()),worker,SLOT(process()));
        //Quit and delete
        connect(worker,SIGNAL(finished()),thread,SLOT(quit()));
        connect(worker,SIGNAL(finished()),worker,SLOT(deleteLater()));
        connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));

        //Connect the signal thread finished with the slot
        connect(worker,SIGNAL(finished()),this,SLOT(compressionFinishedSlot()));

        thread->start();
    }
}

//When the compression is finished, begin to use ftpManager
void MainWindow::compressionFinishedSlot()
{
    m_FileChoosedList.clear();
    ui->labelListFile->setText("The list of files :");
    ui->lineEditFileName->clear();
    ui->textEditChooseFile->clear();
    ui->statusBar->showMessage("Compress the files ok!",10000);

    //get the information of the package
    m_PackageName = m_FileName;
    QFile file(m_PathName + m_FileName);
    file.open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(file.readAll(),QCryptographicHash::Md5);
    m_PackageMD5 = ba.toHex().constData();
    m_PackageDate = QDateTime::currentDateTime().toTime_t();

    //Begin to upload the 7z file
    m_FtpManager.setHostPort("192.168.0.18",21);
    m_FtpManager.setUserInfo("ftpuser","echo");
    m_FtpManager.put(m_PathName + m_FileName,"/data/" + m_FileName);
    file.close();

}

void MainWindow::errorCompressionSlot()
{
    qDebug()<< "Compression error";
    return;
}

//When the upload is finished
//Download the file and check the integrity of the file
void MainWindow::uploadFinishedSlot()
{
    //Download the file from the Ftp server
    m_DownloadFileName = "download";
    m_FtpManager.get("/data/" + m_FileName, m_PathName + m_DownloadFileName);
}

//When the download is finished
//Check the integrity of the file
void MainWindow::downloadFinishedSlot()
{
    QFile file(m_PathName + m_DownloadFileName);
    file.open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(file.readAll(),QCryptographicHash::Md5);
    QString md5DownloadFile = ba.toHex().constData();
    file.close();

    if (md5DownloadFile == m_PackageMD5) {
        emit uploadSuccessdSignal();
    } else {
        emit uploadFailedSignal();
    }
}

//When we have checked the file
void MainWindow::uploadSuccessdSlot()
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

    //After the upload work
    this->show();
    m_WaitingDialog.close();
}

//When we have checked the file
void MainWindow::uploadFailedSlot()
{
    this->statusBar()->showMessage("Upload failed ",10000);

    //After the upload work
    this->show();
    m_WaitingDialog.close();
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
