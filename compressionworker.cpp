#include "compressionworker.h"

CompressionWorker::CompressionWorker(QObject *parent) : QObject(parent)
{

}

CompressionWorker::CompressionWorker(QString fileName, QString pathName, QStringList fileChoosedList)
{
    m_FileName = fileName;
    m_PathName = pathName;
    m_FileChoosedList = fileChoosedList;

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

CompressionWorker::~CompressionWorker()
{
    m_FileName.clear();
    m_PathName.clear();
    m_FileChoosedList.clear();
}

void CompressionWorker::process()
{
    //Create a temporary file currentPath + /temp
    QDir *temp = new QDir;
//    QString preFileName = temp->absolutePath() + "/temp/";
    bool exist = temp->exists(m_PathName);
    if (exist) {
        deleteDir(m_PathName);
    }
    temp->mkdir(m_PathName);

     //Begin to compress the file into .7z
    QFile file(m_PathName + m_FileName);
    file.open(QIODevice::ReadWrite);
    Lib7z::createArchive(&file,m_FileChoosedList);
    if (Lib7z::isSupportedArchive(&file))
        emit finished();
    file.close();
}

void CompressionWorker::deleteDir(QString nameFile)
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
