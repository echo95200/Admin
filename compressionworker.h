#ifndef COMPRESSIONWORKER_H
#define COMPRESSIONWORKER_H

#include <QObject>

class CompressionWorker : public QObject
{
    Q_OBJECT
public:
    explicit CompressionWorker(QObject *parent = nullptr);
    CompressionWorker();
    ~CompressionWorker();

signals:
    void finished();
    void error(QString err);

public slots:
    void process();
};

#endif // COMPRESSIONWORKER_H
