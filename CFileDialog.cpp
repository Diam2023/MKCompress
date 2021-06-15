#include "CFileDialog.h"
#include <QListView>
#include <QTreeView>
#include <QDialogButtonBox>

CFileDialog::CFileDialog(QWidget* parent)
    : QFileDialog(parent)
{
    this->setOption(QFileDialog::DontUseNativeDialog, true);

    //acept more than three file
    QListView* pListView = this->findChild<QListView*>("listView");
    if (pListView)
        pListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QTreeView* pTreeView = this->findChild<QTreeView*>();
    if (pTreeView)
        pTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QDialogButtonBox* pButton = this->findChild<QDialogButtonBox*>("buttonBox");

    disconnect(pButton, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pButton, SIGNAL(accepted()), this, SLOT(onChiose()));
}

CFileDialog::~CFileDialog()
{

}

void CFileDialog::onChiose()
{
    QDialog::accept();
}
