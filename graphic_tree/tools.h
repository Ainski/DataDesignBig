#ifndef TOOLS_H
#define TOOLS_H

#include "status.h"
#include <QFile>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QCoreApplication>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QEventLoop>
#include <QTimer>
#include <QSslSocket>

class Tools
{
public:
    Tools();
    static Status logMessage(const QString &logfile,const QString& message);
    static Status creatlog(const QString &logFile);
    static Status compileExternalCode(const QString &filename);
    static Status calldeepseekapi(const QString& filename);
};
static const QString logfile = "compile.log";
static const QString API_KEY = "sk-6df6f3dfb34a4d5bb35b6c454be9b300"; // 替换为实际API密钥
static const QString API_URL = "https://api.deepseek.com/v1/chat/completions";

#endif // TOOLS_H
