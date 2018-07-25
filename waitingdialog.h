#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QWidget>

namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QWidget
{
    Q_OBJECT

public:
    explicit WaitingDialog(QWidget *parent = 0);
    ~WaitingDialog();

    void initialize();

private slots:

    void compressionSlot();
    void uploadProgressSlot(qint64,qint64);

    void uploadProgressSlot1();
    void uploadProgressSlot2();
    void uploadProgressSlot3();
    void uploadProgressSlot4();

private:
    Ui::WaitingDialog *ui;
};

#endif // WAITINGDIALOG_H
