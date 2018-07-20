#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QFileInfo>

class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(QObject *parent = nullptr);
    void setHostPort(const QString &host,int port = 21);
    void setUserInfo(const QString &userName,const QString &password);
    void put(const QString &fileName,const QString &path);
    void get(const QString &path,const QString &fileName);

signals:
    void error(QNetworkReply::NetworkError);
    void uploadProcess(qint64 bytesSent, qint64 bytesTotal);
    void downloadProcess(qint64 bytesReceived,qint64 bytesTotal);

    void uploadFinished(QNetworkReply*);

private slots:
    void finished();

private:
    QUrl m_Url;
    QFile m_File;
    QNetworkAccessManager m_Manager;
};

#endif // FTPMANAGER_H
