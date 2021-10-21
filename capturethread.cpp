#include "capturethread.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

CaptureThread::CaptureThread(QString url, MicasenseConfig conf): conf(conf)
{
    IsCapturing = false;
    cli = new MyClient(url.toStdString(), 80);
    cli->set_timeout_sec(12);
    cli->set_keep_alive_max_count(10);
    cli->set_read_timeout(20, 20);
    for(int i = 0; i < 6; i++) sync_flags[i] = true;//下载完成标志
}

CaptureThread::~CaptureThread()
{
    emit Log(now() + "\t已停止捕获线程");
    delete cli;
}

void CaptureThread::SafeQuit()
{
    mutex.lock();
    this->quit();
    mutex.unlock();
}

QString CaptureThread::now()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    return current_date_time.toString("yyyyMMdd-HHmmss");
}

void CaptureThread::capture()//给命令曝光
{
    if(IsCapturing)
    {return;}
    IsCapturing=true;

    mutex.lock();
    QString CMD = "/capture?&block=true&cache_raw="+QString::number(conf.cache_raw)+"&store_capture" + conf.store_capture
                  + "&preview" + conf.preview + "&anti_sat" + conf.anti_sat;

    QtJson::JsonObject rejson;

    try
    {
        auto res = cli->Mget(CMD);
        rejson = cli->Parser(res->body);
    }
    catch(MyException error)
    {
        emit Log(error.what());
        mutex.unlock();
        return;
    }

    if(rejson["status"] != "complete")
    {
        throw  MyException("Capture not finished ", 800);
        return;
    }

    QList<QString> keys = rejson["raw_cache_path"].toMap().keys();
    datetime = QDateTime::currentDateTime();

    for(QString key : keys)
    {
        QString name = rejson["raw_cache_path"].toMap()[key].toString();

        sync_flags[key.toInt()] = false;//下载未完成

        emit this->AddDownloadTask(name, key, datetime.toString("yyyy_MM_dd_hh_mm_ss_zzz"));
    }
    mutex.unlock();
    IsCapturing=false;
}

void CaptureThread::TimerCaptureSlot()
{
    timer->stop();

    while(!(sync_flags[0] && sync_flags[1] && sync_flags[2] && sync_flags[3] && sync_flags[4]&& sync_flags[5]))
    {
        DelayBusy(100);
    }
    //阻塞 当本次任务中所有通道都下载结束退出阻塞  均为true  阻塞过程中停止定时器。
    capture();
    timer->start();

}

void CaptureThread::run()
{
    //qDebug() << "Run " <<"threadID : "<<QThread::currentThread();
    QtJson::setPrettySerialize(true);
    if(conf.mode == 1)
    {
        try
        {
            capture();
        }
        catch(MyException error)
        {
            emit Log(error.what());
        }
    }
    else if(conf.mode == 2)
    {
        timer = new QTimer();
        timer->stop();
        timer->setInterval(conf.interval);
        connect(timer, &QTimer::timeout, [ = ]
           {
              TimerCaptureSlot();
           });
        TimerCaptureSlot();//立即执行一次
        emit Log(now() + "\tMicaSense Start TimerCapture, invterval=" + QString::number(conf.interval));

        timer->start();
        this->exec();
    }
    else if(conf.mode == 3)//信号触发模式  你MicaSense开发人员是猪么？ 信号触发直接从HTTP给？
    {
        connect(this, &CaptureThread::QSignalEmit, [ = ]
           {

            try
            {
                while(!(sync_flags[0] && sync_flags[1] && sync_flags[2] && sync_flags[3] && sync_flags[4]&& sync_flags[5]))
                {
                    DelayBusy(100);
                }
                //阻塞 当本次任务中所有通道都下载结束退出阻塞  均为true  阻塞过程中停止定时器。
                capture();
            }
            catch(MyException error)
            {
                emit Log(error.what());
            }

           });
        emit Log(now() + "\tMicaSense Start QSignalMode, Wait For Signal..." );
        this->exec();
    }
}

void CaptureThread::QSignalEmited()
{
    emit QSignalEmit();
}

void CaptureThread::GetStatues()
{
    auto res = cli->Mget("/status");
    QtJson::JsonObject rejson = cli->Parser(res->body);
    QString sta = QtJson::serialize(rejson);
    emit SendStatues(sta);
}

void CaptureThread::DelayBusy(int msec)//阻塞延时
{
    QTime n=QTime::currentTime();
    QTime now;
    do{
        now=QTime::currentTime();
    }while (n.msecsTo(now)<=msec);
}
