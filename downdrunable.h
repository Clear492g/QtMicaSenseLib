#ifndef DOWNDRUNABLE_H
#define DOWNDRUNABLE_H

#include <QRunnable>
#include <QObject>
#include <QDateTime>
#include "Tools/myclient.h"

class DownDrunable :public QObject,  public QRunnable
{
    Q_OBJECT
public:
    DownDrunable(const QString& name, QString url, QString path, QString time, QString key);
    ~DownDrunable();
    void run();
    QString now();

private:
  QString name;
  QString path;
  MyClient * cli;
  QDateTime datetime;
  QString time;
  QString key;

signals:
  void Log(QString log);
  void DownloadFinsh(int index);
  void ToShowIMG(QString Path , int IDX);

};

#endif // DOWNDRUNABLE_H
