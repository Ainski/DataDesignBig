#ifndef TOOLS_H
#define TOOLS_H

#include "status.h"
#include "resultshower.h"
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
#include <QPushButton>
#include <QTextCodec>

static const QString API_KEY = "sk-6df6f3dfb34a4d5bb35b6c454be9b300"; // 替换为实际API密钥
static const QString API_URL = "https://api.deepseek.com/v1/chat/completions";
class Tools
{
private:
    ResultShower* logShower;
public:
    Tools();
    Tools(ResultShower* logShower);
    Status logMessage(const QString& message,ResultShower* logShower=Q_NULLPTR,
                             const QString logFile=logfile);
    Status creatlog(const QString &logFile);
    Status compileExternalCode(const QString &filename,const QString&OutputFileName);
    Status calldeepseekapi(const QString& filename,const QString outfilename);

};
Status deleteFile(const QString &filePath);
Status ExecuteResult(const QString &filename,QPushButton* const &TryExecute,QPushButton* const &CutIn);
#endif // TOOLS_H
