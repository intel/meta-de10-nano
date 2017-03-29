#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QString strText;
    strText = "1. When USB gadget is enabled, the DE10-nano can work as an USB storage when it is connected to a Host PC. However, user's usb devices are required to plug into de10-nano after de10-nano linux boots. Otherwise, the usb devices will not be detected by the system.\r\n\r\n";
    strText += "2. When USB gadget is disabled, an external USB device can be detected upon connecting to DE10-Nano before or after Linux boots up on DE10-Nano.\r\n\r\n";
    strText += "Note: The system needs to reboot before the modified setting is applied to the system.";
    ui->label_hint->setText(strText);



    bool bEnable;
    if (QueryCurrentSetting(&bEnable, false)){
        if (bEnable)
            ui->radioButton_Enable->setChecked(true);
        else
            ui->radioButton_Disable->setChecked(true);
    }else{
        ui->radioButton_Enable->setChecked(true);
    }


}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_pushButton_Exit_clicked()
{
    qApp->quit();
}

void Dialog::on_pushButton_Apply_clicked()
{
    if (ui->radioButton_Enable->isChecked())
        GadgetEnable(true);
    else
        GadgetEnable(false);

}

bool Dialog::GadgetEnable(bool bEnable){
    bool bSuccess = false;
    int res;
    char szCommand[256], szInfo[256];

    sprintf(szCommand, "systemctl %s de10-nano-gadget-init.service", bEnable?"enable":"disable");
    res = system(szCommand);
    if (res == 0)
        bSuccess = true;

    if (!bSuccess){
        sprintf(szInfo, "Failed to configure USB gadget.\r\n(%s)", szCommand);
        QMessageBox(QMessageBox:: Warning, "Error", szInfo, QMessageBox::Ok).exec();
    }else{
        sprintf(szInfo, "Configure USB gadget successfully.\r\n");
        QMessageBox(QMessageBox::Information, "Information", szInfo, QMessageBox::Ok).exec();
    }


    return bSuccess;
}



void Dialog::on_pushButton_Current_clicked()
{
    // query current setting

    ui->radioButton_Enable->setChecked(true);
    bool bEnable;
    if (QueryCurrentSetting(&bEnable)){
        if (bEnable)
            ui->radioButton_Enable->setChecked(true);
        else
            ui->radioButton_Disable->setChecked(true);
    }

}

bool Dialog::QueryCurrentSetting(bool *bEnable, const bool bShowError){
    bool bSuccess = false;
    int res;
    char szFilename[] = "gadget.log";
    char szCommand[256], szInfo[256];

    remove(szFilename);
    //if (res == 0){
        sprintf(szCommand, "systemctl is-enabled de10-nano-gadget-init.service > %s", szFilename);
        res = system(szCommand);
        if (res == -1){
            printf("command return fail, code:%d", res);
        }else{
                FILE *file;
                int nReadSize;
                long int nSize;
                const char szEnable[] = "enabled";
                const char szDisable[] = "disabled";

                usleep( 200*1000 ); // delay to make sure the file is ready.

                file = fopen(szFilename, "r");
                if (file == 0){
                    printf("failed to open file %s\r\n", szFilename);
                }else{
                    fseek(file, 0L, SEEK_END);
                    nSize = ftell(file);
                    if(nSize < 0)
                       nSize = 0;
                    fseek(file, 0L, SEEK_SET);

                    //
                    char *szData;
                    szData = new char[nSize];
                    nReadSize = fread(szData, 1, nSize, file);

                    if (nReadSize == strlen(szEnable)+1){
                        if (strncmp(szEnable,szData, strlen(szEnable)) == 0){
                            *bEnable = true;
                            bSuccess = true;
                        }
                    }else  if (nReadSize == strlen(szDisable)+1){
                        if (strncmp(szDisable,szData, strlen(szDisable)) == 0){
                            *bEnable = false;
                            bSuccess = true;
                        }
                    }

                    delete []szData;

                    fclose(file);

                }

        }
        remove(szFilename);


    if (!bSuccess && bShowError){
        sprintf(szInfo, "Failed to query USB gadget setting.\r\n");
        QMessageBox(QMessageBox:: Warning, "Error", szInfo, QMessageBox::Ok).exec();
    }


    return bSuccess;
}
