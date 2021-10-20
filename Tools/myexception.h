#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <exception>
#include <QString>

class MyException: public std::exception {

  public:
    MyException(std::string msg, int code): code(code), msg(msg) {}

    virtual const char*  what() {
        return msg.c_str();
    }

    virtual int getErrorCode() {
        return code;
    }

    virtual QString getErrorMsg() {
        return QString::fromStdString(msg);
    }

  private:
    int code;
    std::string msg;
};

#endif // MYEXCEPTION_H
