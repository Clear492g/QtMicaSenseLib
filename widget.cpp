#include "widget.h"
#include "ui_widget.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QApplication::setStyle(QStyleFactory::create("fusion"));
    this->setWindowTitle("MicaSense RedEdgeMX/Altium RunDemo");

    ReadIni();

    ui->textBrowserMicaSenseLog->document()->setMaximumBlockCount(100);
    ui->textBrowserMicaSenseLog->setFont(QFont("宋体", 10));

    MicaSensePool = new QThreadPool;
    MicaSensePool->setMaxThreadCount(10);

   ui->Button_MiCaSenseCaptureSignal->setDisabled(true);
}

Widget::~Widget()
{
    delete MicaSensePool;
    delete ui;
}

void Widget::MicaSenseRunTask(QString name, QString key, QString time)//MicaSense启动线程下载
{
    DownDrunable *r = new DownDrunable(name, ui->lineEdit_MicasenseIP->text(), ui->label_Path->text(), time, key);
    connect(r, &DownDrunable::Log, this, &Widget::MicaSenseLog);
    connect(r, &DownDrunable::DownloadFinsh, MicasenseCaptureThread, &CaptureThread::UpdateFlags);
    connect(r, &DownDrunable::ToShowIMG, this, &Widget::MicaSenseLiveView);
    MicaSensePool->start(r);
}

void Widget::MicaSenseLiveView(QString Path , int IDX)//多光谱实时预览
{
    QImage Image(Path);
    QImage Image2 = Image.scaled(ui->label_MicaSenseLive1->width(), ui->label_MicaSenseLive1->height(),
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 饱满填充

    if(IDX==1)  ui->label_MicaSenseLive1->setPixmap(QPixmap::fromImage(Image2));
    else if(IDX==2)  ui->label_MicaSenseLive2->setPixmap(QPixmap::fromImage(Image2));
    else if(IDX==3)  ui->label_MicaSenseLive3->setPixmap(QPixmap::fromImage(Image2));
    else if(IDX==4)  ui->label_MicaSenseLive4->setPixmap(QPixmap::fromImage(Image2));
    else if(IDX==5)  ui->label_MicaSenseLive5->setPixmap(QPixmap::fromImage(Image2));
    else if(IDX==6)  ui->label_MicaSenseLive6->setPixmap(QPixmap::fromImage(Image2));
}

void Widget::MicaSenseRunTimer()//多光谱启动（定时模式下）
{
    MicasenseConfig MicasenseConf;


    MicasenseConf.store_capture = ui->comboBox_SavetoSD->currentText();
    MicasenseConf.preview = ui->comboBox_LiveViewOrNo->currentText();
    MicasenseConf.anti_sat = ui->comboBox_KBH->currentText();
    MicasenseConf.interval =  ui->lineEdit_MicasenseTimer->text().toInt();
    MicasenseConf.mode = 2;

   MicasenseCaptureThread = new CaptureThread(ui->lineEdit_MicasenseIP->text(), MicasenseConf);

   connect(this, &Widget::MicaSenseApplyQuit, MicasenseCaptureThread, &CaptureThread::SafeQuit);
   connect(MicasenseCaptureThread, &CaptureThread::finished, MicasenseCaptureThread, &CaptureThread::deleteLater);
   connect(MicasenseCaptureThread, &CaptureThread::Log, this, &Widget::MicaSenseLog);
   connect(MicasenseCaptureThread, &CaptureThread::AddDownloadTask, this, &Widget::MicaSenseRunTask);
   MicasenseCaptureThread->start();
   ui->Button_MiaSenseStartOrNo->setText("Stop");
}

void Widget::MicaSenseRunSignal()//多光谱启动（信号触发模式下）
{
    MicasenseConfig MicasenseConf;

    MicasenseConf.store_capture = ui->comboBox_SavetoSD->currentText();
    MicasenseConf.preview = ui->comboBox_LiveViewOrNo->currentText();
    MicasenseConf.anti_sat = ui->comboBox_KBH->currentText();
    MicasenseConf.interval =  ui->lineEdit_MicasenseTimer->text().toInt();
    MicasenseConf.mode = 3;

   MicasenseCaptureThread = new CaptureThread(ui->lineEdit_MicasenseIP->text(), MicasenseConf);

   connect(this, &Widget::MicaSenseApplyQuit, MicasenseCaptureThread, &CaptureThread::SafeQuit);
   connect(MicasenseCaptureThread, &CaptureThread::finished, MicasenseCaptureThread, &CaptureThread::deleteLater);
   connect(MicasenseCaptureThread, &CaptureThread::Log, this, &Widget::MicaSenseLog);
   connect(MicasenseCaptureThread, &CaptureThread::AddDownloadTask, this, &Widget::MicaSenseRunTask);

   connect(this, &Widget::MicaSenseCaptureSignal, MicasenseCaptureThread, &CaptureThread::QSignalEmited);/**/

   MicasenseCaptureThread->start();
   ui->Button_MiaSenseStartOrNo->setText("Stop");

   //然后发送MicaSenseCaptureSignal  emit MicaSenseCaptureSignal();
}

void Widget::MicaSenseStop()//多光谱停止
{
    emit MicaSenseApplyQuit();
    disconnect(this, &Widget::MicaSenseApplyQuit, MicasenseCaptureThread, &CaptureThread::SafeQuit);
    disconnect(MicasenseCaptureThread, &CaptureThread::AddDownloadTask, this, &Widget::MicaSenseRunTimer);
    disconnect(MicasenseCaptureThread, &CaptureThread::AddDownloadTask, this, &Widget::MicaSenseRunSignal);
    ui->Button_MiaSenseStartOrNo->setText("Start");
}

void Widget::MicaSenseLog(QString log)//多光谱日志输出
{
    ui->textBrowserMicaSenseLog->append(log);
}

void Widget::on_Button_ClearMicaSenseLog_clicked()//多光谱日志输出清空
{
    ui->textBrowserMicaSenseLog->clear();
}

void Widget::on_Button_MicaSenseWebSetting_clicked()//Web设置
{
    QDesktopServices::openUrl(QUrl( "http://" + ui->lineEdit_MicasenseIP->text()  ));
}

void Widget::on_Button_MicasenseTimer_clicked()//MicaSense存储周期修改
{
    QSettings *configIni = new QSettings(QCoreApplication::applicationDirPath() + "/ClearAllInOne.ini", QSettings::IniFormat);
    configIni->setValue("/MicaSense/StoreTime", ui->lineEdit_MicasenseTimer->text());
    delete  configIni;
}

void Widget::on_Button_MiaSenseStartOrNo_clicked()//MicaSense启停按钮
{
    if(ui->comboBox_CaptureMode->currentText()=="Single")
    {
        MicasenseConfig MicasenseConf;

        MicasenseConf.store_capture = ui->comboBox_SavetoSD->currentText();
        MicasenseConf.preview = ui->comboBox_LiveViewOrNo->currentText();
        MicasenseConf.anti_sat = ui->comboBox_KBH->currentText();
        MicasenseConf.interval =  ui->lineEdit_MicasenseTimer->text().toInt();
        MicasenseConf.mode = 1;

       MicasenseCaptureThread = new CaptureThread(ui->lineEdit_MicasenseIP->text(), MicasenseConf);

       connect(this, &Widget::MicaSenseApplyQuit, MicasenseCaptureThread, &CaptureThread::SafeQuit);
       connect(MicasenseCaptureThread, &CaptureThread::finished, MicasenseCaptureThread, &CaptureThread::deleteLater);
       connect(MicasenseCaptureThread, &CaptureThread::Log, this, &Widget::MicaSenseLog);
       connect(MicasenseCaptureThread, &CaptureThread::AddDownloadTask, this, &Widget::MicaSenseRunTask);
       MicasenseCaptureThread->start();

    }
    else if (ui->comboBox_CaptureMode->currentText()=="Timer")
    {
        if(ui->Button_MiaSenseStartOrNo->text() == "Start")
            MicaSenseRunTimer();
        else
            MicaSenseStop();
    }
    else if (ui->comboBox_CaptureMode->currentText()=="Signal")
    {
        if(ui->Button_MiaSenseStartOrNo->text() == "Start")
        {
            ui->Button_MiCaSenseCaptureSignal->setDisabled(false);
            MicaSenseRunSignal();
        }
        else
        {
            ui->Button_MiCaSenseCaptureSignal->setDisabled(true);
            MicaSenseStop();
        }
    }
}


/********************************************************************************************************************************/


void Widget::ReadIni()//读配置文件
{
    QSettings *configIni = new QSettings(QCoreApplication::applicationDirPath() + "/ClearAllInOne.ini", QSettings::IniFormat);
    ui->label_Path->setText(configIni->value("/All/StorePath").toString());
    ui->lineEdit_MicasenseIP->setText(configIni->value("/MicaSense/IP").toString());
    ui->lineEdit_MicasenseTimer->setText(configIni->value("/MicaSense/StoreTime").toString());    
    ui->comboBox_CaptureMode->setCurrentText(configIni->value("/MicaSense/CaptureMode").toString());
    ui->comboBox_CameraModel->setCurrentText(configIni->value("/MicaSense/CameraModel").toString());

    delete  configIni;

}

void Widget::on_Button_PathEdit_clicked()//修改路径
{
    QString file_path = QFileDialog::getExistingDirectory(this, "请选择文件路径...", "./");
    if(!file_path.isEmpty())
    {
        ui->label_Path->setText(file_path);
        QSettings *configIni = new QSettings(QCoreApplication::applicationDirPath() + "/ClearAllInOne.ini", QSettings::IniFormat);
        configIni->setValue("/All/StorePath", file_path);
        delete  configIni;
    }

}

void Widget::on_Button_PathOpen_clicked()//打开当前路径
{
    QDesktopServices::openUrl(QUrl(ui->label_Path->text(),QUrl::TolerantMode));
}

void Widget::on_Button_MicaSenseIPSet_clicked()//修改IP
{
    QSettings *configIni = new QSettings(QCoreApplication::applicationDirPath() + "/ClearAllInOne.ini", QSettings::IniFormat);
    configIni->setValue("/MicaSense/IP", ui->lineEdit_MicasenseIP->text());
    delete  configIni;
}

void Widget::on_Button_MiCaSenseCaptureSignal_clicked()//发射触发信号
{
    emit MicaSenseCaptureSignal();
}

void Widget::on_comboBox_CameraModel_currentTextChanged(const QString &arg1)//修改相机型号
{
    QSettings *configIni = new QSettings(QCoreApplication::applicationDirPath() + "/ClearAllInOne.ini", QSettings::IniFormat);
    configIni->setValue("/MicaSense/CameraModel", arg1);
    delete  configIni;
}

void Widget::on_comboBox_CaptureMode_currentTextChanged(const QString &arg1)//修改拍照模式
{
    QSettings *configIni = new QSettings(QCoreApplication::applicationDirPath() + "/ClearAllInOne.ini", QSettings::IniFormat);
    configIni->setValue("/MicaSense/CaptureMode", arg1);
    delete  configIni;

}
