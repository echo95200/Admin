#include "choosefiledialog.h"

ChooseFileDialog::ChooseFileDialog(QWidget *parent)
    : QFileDialog(parent)
{
    this->setOption(QFileDialog::DontUseNativeDialog,true);
    QListView *pListView = this->findChild<QListView*>("listView");
    if (pListView)
        pListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QTreeView *pTreeView = this->findChild<QTreeView*>();
    if (pTreeView)
        pTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QDialogButtonBox *pButton = this->findChild<QDialogButtonBox*>("buttonBox");

    disconnect(pButton,SIGNAL(accepted()),this,SLOT(accept()));
    connect(pButton,SIGNAL(accepted()),this,SLOT(onChoosed()));
}

ChooseFileDialog::~ChooseFileDialog()
{
    this->close();
}

void ChooseFileDialog::onChoosed()
{
    QDialog::accept();
}
