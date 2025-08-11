/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <codeeditor.h>
#include <resultshower.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    CodeEditor *CodeEditorUut;
    QPushButton *TryToCompile;
    ResultShower *CompileResult;
    ResultShower *AI_CodeShower;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1419, 941);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(857, 0, 561, 891));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        CodeEditorUut = new CodeEditor(layoutWidget);
        CodeEditorUut->setObjectName(QString::fromUtf8("CodeEditorUut"));
        CodeEditorUut->setMinimumSize(QSize(0, 500));

        verticalLayout->addWidget(CodeEditorUut);

        TryToCompile = new QPushButton(layoutWidget);
        TryToCompile->setObjectName(QString::fromUtf8("TryToCompile"));

        verticalLayout->addWidget(TryToCompile);

        CompileResult = new ResultShower(layoutWidget);
        CompileResult->setObjectName(QString::fromUtf8("CompileResult"));
        CompileResult->setReadOnly(true);

        verticalLayout->addWidget(CompileResult);

        AI_CodeShower = new ResultShower(centralwidget);
        AI_CodeShower->setObjectName(QString::fromUtf8("AI_CodeShower"));
        AI_CodeShower->setGeometry(QRect(50, 530, 801, 361));
        AI_CodeShower->setReadOnly(true);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1419, 20));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        TryToCompile->setText(QCoreApplication::translate("MainWindow", "\345\260\235\350\257\225\347\274\226\350\257\221", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
