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
#include <QtWidgets/QWidget>
#include <codeeditor.h>
#include <resultshower.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    ResultShower *AI_CodeShower;
    CodeEditor *CodeEditorUut;
    ResultShower *CompileResult;
    QPushButton *SaveCode;
    QPushButton *AIOptimize;
    QPushButton *TryToCompile;
    QPushButton *TryExecute;
    QPushButton *CompileAICode;
    QPushButton *NextStep;
    QPushButton *CutIn;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1419, 941);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        AI_CodeShower = new ResultShower(centralwidget);
        AI_CodeShower->setObjectName(QString::fromUtf8("AI_CodeShower"));
        AI_CodeShower->setGeometry(QRect(20, 490, 801, 361));
        AI_CodeShower->setReadOnly(true);
        CodeEditorUut = new CodeEditor(centralwidget);
        CodeEditorUut->setObjectName(QString::fromUtf8("CodeEditorUut"));
        CodeEditorUut->setGeometry(QRect(858, 1, 559, 500));
        CodeEditorUut->setMinimumSize(QSize(0, 500));
        CompileResult = new ResultShower(centralwidget);
        CompileResult->setObjectName(QString::fromUtf8("CompileResult"));
        CompileResult->setGeometry(QRect(858, 585, 541, 281));
        CompileResult->setReadOnly(true);
        SaveCode = new QPushButton(centralwidget);
        SaveCode->setObjectName(QString::fromUtf8("SaveCode"));
        SaveCode->setGeometry(QRect(858, 507, 80, 20));
        AIOptimize = new QPushButton(centralwidget);
        AIOptimize->setObjectName(QString::fromUtf8("AIOptimize"));
        AIOptimize->setGeometry(QRect(858, 559, 80, 20));
        TryToCompile = new QPushButton(centralwidget);
        TryToCompile->setObjectName(QString::fromUtf8("TryToCompile"));
        TryToCompile->setGeometry(QRect(858, 533, 80, 20));
        TryExecute = new QPushButton(centralwidget);
        TryExecute->setObjectName(QString::fromUtf8("TryExecute"));
        TryExecute->setGeometry(QRect(980, 540, 89, 24));
        CompileAICode = new QPushButton(centralwidget);
        CompileAICode->setObjectName(QString::fromUtf8("CompileAICode"));
        CompileAICode->setGeometry(QRect(980, 510, 89, 24));
        NextStep = new QPushButton(centralwidget);
        NextStep->setObjectName(QString::fromUtf8("NextStep"));
        NextStep->setGeometry(QRect(1100, 520, 89, 24));
        CutIn = new QPushButton(centralwidget);
        CutIn->setObjectName(QString::fromUtf8("CutIn"));
        CutIn->setGeometry(QRect(1100, 550, 89, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1419, 22));
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
        SaveCode->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\344\273\243\347\240\201", nullptr));
        AIOptimize->setText(QCoreApplication::translate("MainWindow", "AI\344\274\230\345\214\226", nullptr));
        TryToCompile->setText(QCoreApplication::translate("MainWindow", "\347\274\226\350\257\221\346\243\200\346\237\245", nullptr));
        TryExecute->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214\344\274\230\345\214\226\347\273\223\346\236\234", nullptr));
        CompileAICode->setText(QCoreApplication::translate("MainWindow", "\347\274\226\350\257\221AI\344\273\243\347\240\201", nullptr));
        NextStep->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\346\255\245", nullptr));
        CutIn->setText(QCoreApplication::translate("MainWindow", "\347\273\210\346\255\242\350\277\220\350\241\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
