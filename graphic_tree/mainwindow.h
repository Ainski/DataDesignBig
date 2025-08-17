#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "codeeditor.h"
#include "codehighlighter.h"
#include "status.h"
#include "resultshower.h"
#include "tools.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    QVector<QString> files;
    CodeHighLighter *thehighlighter;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
