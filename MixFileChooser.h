#pragma once

#ifndef MIXFILECHOOSER

#include "stdafx.h"

class MixFileChooser :
    public QFileDialog
{
    Q_OBJECT
public:
    MixFileChooser(QWidget* parent = 0);
};


#endif // !MIXFILECHOOSER


