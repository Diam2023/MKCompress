#pragma once

#include <QObject>
#include <QFileDialog>

class CFileDialog : public QFileDialog
{
    Q_OBJECT
    public:
        CFileDialog(QWidget* parent = 0);

};
