#pragma once

#include <QtWidgets/QMainWindow>
#include <qtranslator.h>
#include "ui_MKCompress.h"

class MKCompress : public QMainWindow
{
    Q_OBJECT

public:
    MKCompress(QWidget *parent = Q_NULLPTR);
private:
    Ui::MKCompressClass ui;
};
