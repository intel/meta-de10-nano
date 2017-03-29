#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButton_Exit_clicked();

    void on_pushButton_Apply_clicked();


    void on_pushButton_Current_clicked();

private:
    Ui::Dialog *ui;

    bool GadgetEnable(bool bEnable);
    bool QueryCurrentSetting(bool *bEnable, const bool bShowError=true);

};

#endif // DIALOG_H
