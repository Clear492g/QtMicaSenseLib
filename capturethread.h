#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QDateTime>
#include "Tools/myclient.h"


typedef struct MicasenseConfig
{
    int interval = 3000;
    int cache_raw = 31;
    int cache_jpeg = 0;
    QString detect_panel = "false";
    QString preview = "false";
    QString anti_sat = "false";
    QString store_capture = "false";
    QString use_post_capture_state = "true";
    int mode;//mode:1为单帧采集 2为定时采集 3为QSignal触发模式
} MicasenseConfig;


class CaptureThread: public QThread
{
    Q_OBJECT
public:
    CaptureThread(QString url, MicasenseConfig conf);//mode:1为单帧采集 2为定时采集
    ~CaptureThread() override;

protected:
  void run() override;

private:
  bool sync_flags[6];//下载完成标志 第六个是Altium热红外波段
  MicasenseConfig conf;
  MyClient * cli;
  QTimer *timer;
  QMutex mutex;
  QString now();
  QDateTime datetime;

  void capture();
  void ReformatSD();

public slots:
  void SafeQuit();
  void UpdateFlags(int index) {sync_flags[index] = true;}
  void GetStatues();
  void TimerCaptureSlot();
  void DelayBusy(int msec);
  void QSignalEmited();

signals:
  void Log(QString log);
  void SendStatues(QString data);
  void AddDownloadTask(QString name, QString key, QString time);
  void QSignalEmit();
};

#endif // CAPTURETHREAD_H
