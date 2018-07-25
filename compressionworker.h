#ifndef COMPRESSIONWORKER_H
#define COMPRESSIONWORKER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QDebug>

#include "lib7z_facade.h"
#include "init.h"

class CompressionWorker : public QObject
{
    Q_OBJECT
public:
    explicit CompressionWorker(QObject *parent = nullptr);
    CompressionWorker(QString fileName,QString pathName,QStringList fileChoosedList);
    ~CompressionWorker();

    void deleteDir(QString);

signals:
    void finished();
    void error(QString err);

public slots:
    void process();

private:
    QString m_FileName;
    QString m_PathName;
    QStringList m_FileChoosedList;
    Lib7z::File m_File7z;

};

#endif // COMPRESSIONWORKER_H
