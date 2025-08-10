#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->CodeEditorUut->setMode(EditorMode::EDIT);
    //代码编辑器
    thehighlighter = new CodeHighLighter;
    thehighlighter->setDocument(ui->CodeEditorUut->document());

    Tools::creatlog(logfile);


    connect(ui->TryToCompile, &QPushButton::clicked, [this]() {
        // 立即禁用按钮
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("正在编译...");
        // 执行耗时操作（如保存代码）
        Status res = ui->CodeEditorUut->SaveUserCode();
        if (res== Status::OK){
            ui->CodeEditorUut->setStyleSheet("background-color: green;");
            ui->TryToCompile->setText("正在ai优化...");
            ui->CompileResult->ShowResults(QString("compile.log"));
            res = Tools::calldeepseekapi(QString("code.cpp"));
            if (res== Status::OK){
                ui->CodeEditorUut->setStyleSheet("background-color: green;");
                ui->TryToCompile->setText("尝试编译");
            }
            else{
                ui->CodeEditorUut->setStyleSheet("background-color: red;");
                ui->TryToCompile->setText("ai优化失败，尝试重新编译");
            }
            ui->CompileResult->ShowResults(QString("compile.log"));
        }
        else{
            ui->CodeEditorUut->setStyleSheet("background-color: red;");
            ui->TryToCompile->setText("编译失败，尝试重新编译");
            ui->CompileResult->ShowResults(QString("compile.log"));
        }



        // 操作完成后重新启用按钮
        ui->TryToCompile->setEnabled(true);


    });

}
MainWindow::~MainWindow()
{
    delete ui;
    delete thehighlighter;
}

