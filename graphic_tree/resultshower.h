#ifndef RESULTSHOWER_H
#define RESULTSHOWER_H

#include <QWidget>
#include <QString>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
const QString logfile = "compile.log";

class ResultShower : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ResultShower(QWidget *parent = nullptr);

    void ShowResults(const QString filename= logfile);

};

#endif // RESULTSHOWER_H
