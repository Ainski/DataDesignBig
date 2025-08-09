#include "tools.h"
#include <QDebug>
Tools::Tools()
{

}
Status Tools::logMessage(const QString &logFile,const QString& message) {
    qCritical() << message;
    QFile log(logFile);
    if (!log.isOpen()) return Status::FILE_OPEN_WRONG;
    if (log.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&log);
        out << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")
            << message << "\n";
        log.close();
    }
    return Status::OK;
}


Status Tools:: runPython(bool waitForFinished) {
    // 确定Python解释器命令
    QString pythonCmd;

    #if defined(Q_OS_LINUX)
        pythonCmd = "python3";
    #elif defined(Q_OS_WIN)
        pythonCmd = "python";
    #else
        logMessage("compile.log", "错误: 不支持的操作系统");
        return Status::UNKNOWN_SYSTEM;
    #endif

    // 构建日志文件路径（与脚本同目录）
    QString logPath = "compile.log";
    QString appPath = QCoreApplication::applicationDirPath();
    QDir scriptDir(appPath);  // 上级目录
    // 向上移动一级到父目录
    if (!scriptDir.cdUp()) {
        QString errorMsg = "错误: 无法访问上级目录";
        logMessage("compile.log", errorMsg);
        return Status::FILE_OPEN_WRONG;
    }

    // 定位到main文件夹
    if (!scriptDir.cd("main")) {
        QString errorMsg = "错误: 找不到main文件夹";
        logMessage("compile.log", errorMsg);
        return Status::FILE_OPEN_WRONG;
    }
    // 记录开始时间
    QString startTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logMessage(logPath, QString("=").repeated(60));
    logMessage(logPath, QString("开始执行Python脚本 - %1").arg(startTime));
    logMessage(logPath, QString("=").repeated(60));

    // 检查Python解释器是否可用
    logMessage(logPath, QString("检查Python解释器: %1").arg(pythonCmd));
    QProcess checkProcess;
    checkProcess.start(pythonCmd, {"--version"});
    if (!checkProcess.waitForFinished(1000) || checkProcess.exitCode() != 0) {
        QString errorMsg = QString("错误: 找不到Python解释器 - %1").arg(pythonCmd);
        logMessage(logPath, errorMsg);
        return Status::CANT_FIND_PYTHON;
    }

    // 获取Python版本信息
    QString pythonVersion = checkProcess.readAllStandardOutput().trimmed();
    if (pythonVersion.isEmpty()) {
        pythonVersion = checkProcess.readAllStandardError().trimmed();
    }
    logMessage(logPath, QString("Python版本: %1").arg(pythonVersion));

    // 构建脚本路径
    QString scriptPath = scriptDir.filePath("main.py");
    logMessage(logPath, QString("脚本路径: %1").arg(scriptPath));

    // 检查脚本文件是否存在
    if (!QFile::exists(scriptPath)) {
        QString errorMsg = QString("错误: 找不到Python脚本 - %1").arg(scriptPath);
        logMessage(logPath, errorMsg);
        return Status::FILE_OPEN_WRONG;
    }
    logMessage(logPath, "脚本存在，准备执行...");

    // 创建并配置进程
    QProcess *process = new QProcess();  // 不指定父对象

    // 设置工作目录为脚本所在目录
    process->setWorkingDirectory(scriptDir.absolutePath());

    // 连接信号处理输出（记录到日志文件）
    QObject::connect(process, &QProcess::readyReadStandardOutput,
        [process, logPath]() {
            QString output = QString::fromLocal8Bit(process->readAllStandardOutput()).trimmed();
            if (!output.isEmpty()) {
                logMessage(logPath, "[输出] " + output);
            }
        });

    QObject::connect(process, &QProcess::readyReadStandardError,
        [process, logPath]() {
            QString error = QString::fromLocal8Bit(process->readAllStandardError()).trimmed();
            if (!error.isEmpty()) {
                logMessage(logPath, "[错误] " + error);
            }
        });

    // 处理进程结束（自动清理内存）
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [process, logPath, startTime](int exitCode, QProcess::ExitStatus exitStatus) {
            // 计算执行时间
            QDateTime endTime = QDateTime::currentDateTime();
            QDateTime start = QDateTime::fromString(startTime, "yyyy-MM-dd hh:mm:ss");
            qint64 seconds = start.secsTo(endTime);

            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                logMessage(logPath, QString("=").repeated(60));
                logMessage(logPath, QString("脚本执行成功! 耗时: %1秒").arg(seconds));
                logMessage(logPath, QString("=").repeated(60));
            } else {
                logMessage(logPath, QString("=").repeated(60));
                logMessage(logPath, QString("脚本执行失败! 退出码: %1, 耗时: %2秒").arg(exitCode).arg(seconds));
                logMessage(logPath, QString("=").repeated(60));
            }
            process->deleteLater();  // 自动删除对象
        });

    // 启动Python进程
    logMessage(logPath, QString("执行命令: %1 %2").arg(pythonCmd).arg(scriptPath));
    process->start(pythonCmd, {scriptPath});

    if (!process->waitForStarted(2000)) {
        logMessage(logPath, "错误: 无法启动Python进程");
        process->deleteLater();
        return Status::COMPILE_ERROR;
    }

    logMessage(logPath, "Python进程已启动");

    // 如果需要同步等待执行完成
    if (waitForFinished) {
        logMessage(logPath, "等待脚本执行完成...");
        process->waitForFinished(-1);  // 无限期等待
    }

    return Status::OK;
}
