#include "MixFileChooser.h"

MixFileChooser::MixFileChooser(QWidget* parent)
{
    this->setOption(QFileDialog::DontUseNativeDialog, true);

    //acept more than three file
    QListView* listView = this->findChild<QListView*>();

    if (listView) {
        listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    QTreeView* treeView = this->findChild<QTreeView*>();

    if (treeView) {
        treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    QDialogButtonBox* button = this->findChild<QDialogButtonBox*>();

    disconnect(button, SIGNAL(accepted()), this, SLOT(accept()));

    connect(
        button, QOverload<>::of(&QDialogButtonBox::accepted), [=]() {
            QDialog::accept();
        }
    );
}
