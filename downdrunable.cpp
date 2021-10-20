#include "downdrunable.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

DownDrunable::DownDrunable
    (const QString& name, QString url,  QString path, QString time, QString key)
    : name(name), path(path), time(time), key(key)
{
    this->setAutoDelete(true);
    cli = new MyClient(url.toStdString(), 80);
    cli->set_timeout_sec(10);
    cli->set_read_timeout(20, 20);
    cli->set_keep_alive_max_count(10);
}

DownDrunable::~DownDrunable()
{
    emit Log(now() + "\tdowload " + time + "_" + key + ".tif");
}

QString DownDrunable::now()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    return current_date_time.toString("yyyy_MM_dd_HH_mm_ss_zzz");
}

void DownDrunable::run()
{
    QString ClearPath = path + "/" + time + "_" +key + ".tif";
    try
    {
        auto res = cli->Mget(name.toStdString().c_str());
        cli->StoreFile(res->body, ClearPath.toStdString());
    }
    catch(MyException error)
    {
        emit Log(now() + "\t" + error.what());
    }
    emit DownloadFinsh(key.toInt());
    emit ToShowIMG( ClearPath,key.toInt());
}
