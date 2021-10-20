#include "myclient.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


QtJson::JsonObject MyClient::Parser(std::string raw)
{
    bool ok;
    QtJson::JsonObject result = QtJson::parse(QString::fromStdString(raw), ok).toMap();

    if(!ok)
    {
        qDebug() << "serialize failed";
        throw  MyException("convert to json failed ", 800);
    }

    return result;
}

std::shared_ptr<httplib::Response> MyClient::Mget(QString url)
{
    std::shared_ptr<httplib::Response> res = this->Get(url.toStdString().c_str());
    if(!(res && res->status == 200)) {

        throw  MyException("Mget failed ,host is not reachable", 400);
    }

    return res;
}

std::shared_ptr<httplib::Response> MyClient::Mpost(QString url, QString data, QString format)
{

    auto res = this->Post(url.toStdString().c_str(), data.toStdString().c_str(), format.toStdString().c_str());
    if(!(res && res->status == 200)) {
        throw  MyException("Mpost failed ,host is not reachable", 400);
    }

    return res;
}

std::shared_ptr<httplib::Response> MyClient::Mdelete(QString url)
{
    auto res = this->Delete(url.toStdString().c_str());
    if(!(res && res->status == 200))
    {
        throw  MyException("Mdelete failed ,host is not reachable", 400);
    }

    return res;
}

std::shared_ptr<httplib::Response> MyClient::Mput(QString url)
{
    auto res = this->Delete(url.toStdString().c_str());

    if(!(res && res->status == 200)) {
        throw  MyException("Mput failed ,host is not reachable", 400);
    }

    return res;
}

void MyClient::StoreFile(std::string data, std::string path)
{
    outfile.open(path, std::ios::binary);
    if(!outfile.is_open())
    {
        throw  MyException("file path is unavialabble ", 900);
    }
    outfile << data;
    outfile.close();
}
