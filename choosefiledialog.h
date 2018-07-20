#ifndef CHOOSEFILEDIALOG_H
#define CHOOSEFILEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include <QListView>
#include <QTreeView>
#include <QDialog>
#include <QDialogButtonBox>

class ChooseFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    ChooseFileDialog(QWidget *parent = 0);
    ~ChooseFileDialog();

private slots:
    void onChoosed();
};

#endif // CHOOSEFILEDIALOG_H
