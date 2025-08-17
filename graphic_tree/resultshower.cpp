#include "resultshower.h"
#include<QDebug>
ResultShower::ResultShower(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setReadOnly(1);
}

void ResultShower::ShowResults(const QString filename)
{
    // 清空当前内容
    this->clear();

    // 检查文件是否存在
    if (!QFileInfo::exists(filename)) {
        this->appendPlainText(tr("Error: File not found - %1").arg(filename));
        QMessageBox::warning(this, tr("File Error"), tr("File not found: %1").arg(filename));
        return;
    }
    qDebug()<<"读取日志";
    QFile file(filename);

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = tr("Error opening file: %1\n%2").arg(filename).arg(file.errorString());
        this->appendPlainText(errorMsg);
        QMessageBox::critical(this, tr("File Error"), errorMsg);
        return;
    }

    // 读取文件内容
    QTextStream in(&file);
    in.setCodec("UTF-8");  // 设置编码为UTF-8

    // 一次性读取所有内容（适合小到中等文件）
    this->setPlainText(in.readAll());

    // 关闭文件
    file.close();
    qDebug()<<"关闭日志";


    // 显示内容


//    // 将光标移动到开头
//    QTextCursor cursor = this->textCursor();
//    cursor.movePosition(QTextCursor::Start);
//    this->setTextCursor(cursor);

//    // 添加状态信息
//    QFileInfo fileInfo(filename);
//    this->appendHtml(QString("<br><font color=\"gray\">%1: %2 bytes, last modified: %3</font>")
//                     .arg(tr("File loaded successfully"))
//                     .arg(fileInfo.size())
//                     .arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
}
