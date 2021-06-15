#ifndef CFILEDIALOG_H
#define CFILEDIALOG_H

#include <QObject>
#include <QFileDialog>

class CFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    CFileDialog(QWidget* parent = 0);
    ~CFileDialog();

public slots:
    void onChiose();
};

#endif