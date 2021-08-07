#pragma once

#ifndef MIXFILECHOOSER

#include "stdafx.h"

#include <QObject>
#include <QFileDialog>
#include <QListView>
#include <QTreeView>
#include <QDialogButtonBox>

class MixFileChooser :
    public QFileDialog
{
    Q_OBJECT
public:
    MixFileChooser(QWidget* parent = 0);
};


#endif // !MIXFILECHOOSER


