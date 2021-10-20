#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSettings>
#include <QThreadPool>
#include <QDesktopServices>
#include "capturethread.h"
#include "downdrunable.h"

#include <QFileDialog>
#include <QStyleFactory>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    QThreadPool *MicaSensePool;
    CaptureThread * MicasenseCaptureThread;

public slots:
    void MicaSenseRunTask(QString name, QString key, QString time);//MicaSense启动线程下载
    void MicaSenseLiveView(QString Path , int IDX);//多光谱实时预览
    void MicaSenseRunTimer();//多光谱启动 定时器模式
    void MicaSenseRunSignal();//多光谱启动 信号触发模式
    void MicaSenseStop();//多光谱停止
    void MicaSenseLog(QString log);//多光谱日志输出
    void ReadIni();//读取配置文件


signals:
    void MicaSenseApplyQuit();//MicaSense
    void MicaSenseCaptureSignal();//信号触发模式中的触发信号

private slots:
    void on_Button_ClearMicaSenseLog_clicked();//清除日志
    void on_Button_MicaSenseWebSetting_clicked();//打开Web后台
    void on_Button_MicasenseTimer_clicked();//修改定时时长
    void on_Button_MiaSenseStartOrNo_clicked();//启停按钮
    void on_Button_PathEdit_clicked();//修改存储路径
    void on_Button_PathOpen_clicked();//打开当前路径
    void on_Button_MicaSenseIPSet_clicked();//修改IP地址

    void on_Button_MiCaSenseCaptureSignal_clicked();

    void on_comboBox_CameraModel_currentTextChanged(const QString &arg1);

    void on_comboBox_CaptureMode_currentTextChanged(const QString &arg1);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
