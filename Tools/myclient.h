#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QDebug>
#include "Tools/json.h"
#include "Tools/httplib.h"
#include "Tools/myexception.h"


class MyClient : public httplib::Client {
  public:

    using httplib::Client::Client;
    std::shared_ptr<httplib::Response> Mget(QString url) ;
    std::shared_ptr<httplib::Response> Mpost(QString url, QString data, QString format);
    std::shared_ptr<httplib::Response> Mput(QString url);
    std::shared_ptr<httplib::Response> Mdelete(QString url);
    QtJson::JsonObject Parser(std::string);
    void StoreFile(std::string data, std::string path);

  private:
    int port;
    int interval;
    QString url;
    std:: ofstream outfile;
};
#endif // CLIENT_H
