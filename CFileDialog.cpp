#include "CFileDialog.h"
#include <QListView>
#include <QTreeView>
#include <QDialogButtonBox>

CFileDialog::CFileDialog(QWidget* parent)
    : QFileDialog(parent)
{
    this->setOption(QFileDialog::DontUseNativeDialog, true);

    //acept more than three file
    QListView* pListView = this->findChild<QListView*>();
    
    if (pListView) {
        pListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    QTreeView* pTreeView = this->findChild<QTreeView*>();
    
    if (pTreeView) {
        pTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    QDialogButtonBox* pButton = this->findChild<QDialogButtonBox*>();
    
    disconnect(pButton, SIGNAL(accepted()), this, SLOT(accept()));
    
    connect(
        pButton, QOverload<>::of(&QDialogButtonBox::accepted), [=]() {
            QDialog::accept();
        }
    );
}

