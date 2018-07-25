#include "waitingdialog.h"
#include "ui_waitingdialog.h"

WaitingDialog::WaitingDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaitingDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}

void WaitingDialog::uploadProgressSlot1()
{
    ui->progressBar->setValue(20);
}

void WaitingDialog::uploadProgressSlot2()
{
    ui->progressBar->setValue(40);
}

void WaitingDialog::uploadProgressSlot3()
{
    ui->progressBar->setValue(60);
}

void WaitingDialog::uploadProgressSlot4()
{
    ui->progressBar->setValue(80);
}

void WaitingDialog::initialize()
{
    ui->progressBar->setValue(0);
}

void WaitingDialog::compressionSlot()
{
    ui->label->setText("Compressing the file...");
    ui->progressBar->setMaximum(0);
    ui->progressBar->setMinimum(0);
    //ui->progressBar->setValue(50);
}

void WaitingDialog::uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal)
{
    ui->label->setText("Uploading...");
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesSent);
}
