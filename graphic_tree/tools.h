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

class Tools
{
public:
    Tools();
    static Status logMessage(const QString &logFile,const QString& message);
    static Status runPython(bool waitForFinished = false);
};

#endif // TOOLS_H
