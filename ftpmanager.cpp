#include "ftpmanager.h"

FtpManager::FtpManager(QObject *parent) : QObject(parent)
{
    m_Url.setScheme("ftp");
}

void FtpManager::setHostPort(const QString &host, int port)
{
    m_Url.setHost(host);
    m_Url.setPort(port);
}

void FtpManager::setUserInfo(const QString &userName, const QString &password)
{
    m_Url.setUserName(userName);
    m_Url.setPassword(password);
}

void FtpManager::put(const QString &fileName, const QString &path)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();

    m_Url.setPath(path);
    QNetworkReply *pReply = m_Manager.put(QNetworkRequest(m_Url),data);

    connect(pReply,SIGNAL(uploadProgress(qint64,qint64)),this,SIGNAL(uploadProcess(qint64,qint64)));
    connect(pReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SIGNAL(error(QNetworkReply::NetworkError)));
    connect(pReply,SIGNAL(finished()),this,SIGNAL(uploadFinished()));
}

void FtpManager::get(const QString &path, const QString &fileName)
{
    QFileInfo info;
    info.setFile(fileName);

    m_File.setFileName(fileName);
    m_File.open(QIODevice::WriteOnly | QIODevice::Append);
    m_Url.setPath(path);

    QNetworkReply *pReply = m_Manager.get(QNetworkRequest(m_Url));
    connect(pReply,SIGNAL(finished()),this,SLOT(downloadFinishedSlot()));
    //connect(pReply,SIGNAL(finished()),this,SIGNAL(downloadFinished()));
    connect(pReply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(downloadProcess(qint64,qint64)));
    connect(pReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SIGNAL(error(QNetworkReply::NetworkError)));
}

void FtpManager::downloadFinishedSlot()
{
    QNetworkReply *pReply = qobject_cast<QNetworkReply* >(sender());
    switch (pReply->error()) {
    case QNetworkReply::NoError : {
        m_File.write(pReply->readAll());
        m_File.flush();
    }
        break;
    default:
        break;
    }
    m_File.close();
    pReply->deleteLater();
    emit downloadFinished();
}
