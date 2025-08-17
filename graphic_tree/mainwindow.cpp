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

    Tools tool(ui->CompileResult);
    tool.creatlog(logfile);
    files.push_back(logfile);

    //保存文件流程设计
    ui->SaveCode->setEnabled(true);
    connect(ui->SaveCode ,&QPushButton::clicked,[this,&tool](){
        ui->SaveCode->setEnabled(false);
        ui->SaveCode->setText("保存代码");
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("编译检查");
        ui->AIOptimize->setEnabled(false);
        ui->AIOptimize->setText("AI优化");

        ui->SaveCode->setText("正在保存...");
        tool.logMessage("正在保存...",ui->CompileResult);
        QApplication::processEvents(); //刷新界面

        Status res = ui->CodeEditorUut->SaveUserCode("code.cpp");
        files.push_back("code.cpp");

        if (res==Status::OK){
            ui->SaveCode->setText("保存代码");

            tool.logMessage(QString("代码保存成功"),ui->CompileResult);
            QApplication::processEvents(); //刷新界面
            ui->CodeEditorUut->setStyleSheet("background-color: green;");
            ui->SaveCode->setEnabled(true);//允许用户尝试编译
            ui->TryToCompile->setEnabled(true);//允许用户尝试重新保存
        }
        else{
            ui->CodeEditorUut->setStyleSheet("background-color: red;");
            ui->SaveCode->setText("保存失败");
            ui->CompileResult->ShowResults(QString(logfile));

            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存
        }
        ui->CompileResult->ShowResults();

        QApplication::processEvents(); //刷新界面


    });



    //编译检查流程设计
    ui->TryToCompile->setEnabled(true);
    connect (ui->TryToCompile,&QPushButton::clicked,[this,&tool](){
        ui->SaveCode->setEnabled(false);
        ui->SaveCode->setText("保存代码");
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("编译检查");
        ui->AIOptimize->setEnabled(false);
        ui->AIOptimize->setText("AI优化");

        ui->TryToCompile->setText("正在编译检查...");
        tool.logMessage("正在编译检查...",ui->CompileResult);
        QApplication::processEvents(); //刷新界面
        qDebug()<<"start compile";
        Status res = tool.compileExternalCode("code.cpp","code.exe");
        files.push_back("code.exe");
        qDebug()<<"compile finished";

        if (res==Status::OK){
            ui->TryToCompile->setText("编译检查");

            tool.logMessage(QString("编译检查成功"),ui->CompileResult);
            QApplication::processEvents(); //刷新界面
            ui->CodeEditorUut->setStyleSheet("background-color: green;");
            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存

            ui->AIOptimize->setEnabled(true);//允许用户尝试ai优化
        }
        else{
            ui->CodeEditorUut->setStyleSheet("background-color: red;");
            ui->TryToCompile->setText("编译检查失败");
            ui->CompileResult->ShowResults(logfile);

            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存

        }
        ui->CompileResult->ShowResults();

        QApplication::processEvents(); //刷新界面


    });
    ui->AIOptimize->setEnabled(false);
    connect (ui->AIOptimize ,&QPushButton::clicked ,[this,&tool](){
        ui->SaveCode->setEnabled(false);
        ui->SaveCode->setText("保存代码");
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("编译检查");
        ui->AIOptimize->setEnabled(false);
        ui->AIOptimize->setText("AI优化");

        ui->AIOptimize->setText("正在AI优化...");
        tool.logMessage("正在AI优化...",ui->CompileResult);
        QApplication::processEvents(); //刷新界面

        Status res = tool.calldeepseekapi("code.cpp","output.cpp");
        files.push_back("output.cpp");
        if (res==Status::OK){
            tool.logMessage(QString("AI优化成功"),ui->CompileResult);
            QApplication::processEvents(); //刷新界面
            ui->CodeEditorUut->setStyleSheet("background-color: green;");
            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存
        }
        else{
            ui->CodeEditorUut->setStyleSheet("background-color: red;");
            ui->AIOptimize->setText("AI优化失败");
            ui->CompileResult->ShowResults(QString(logfile));

            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存
        }
        ui->CompileResult->ShowResults();
        QApplication::processEvents(); //刷新界面

    });





//    connect(ui->TryToCompile, &QPushButton::clicked, [this]() {
//        // 立即禁用按钮

//        ui->TryToCompile->setEnabled(false);
//        ui->TryToCompile->setText("正在保存...");

//        QApplication::processEvents(); //刷新界面


//        Status res = ui->CodeEditorUut->SaveUserCode();
//        ui->TryToCompile->setEnabled(false);
//        if (res== Status::OK){
//            qDebug()<<"Code Saved Successed";
//            Tools::logMessage(logfile,QString("代码保存成功"));
//            Tools::logMessage(logfile,QString("正在尝试对源代码进行编译"));
//            ui->TryToCompile->setText("正在编译...");
//            ui->CompileResult->ShowResults(QString("compile.log"));
//            QApplication::processEvents(); //刷新界面
//            res = Tools::compileExternalCode("code.cpp");

//            if (res== Status::OK){
//                ui->CodeEditorUut->setStyleSheet("background-color: green;");
//                ui->TryToCompile->setText("正在ai优化...");
//                ui->CompileResult->ShowResults(QString("compile.log"));
//                res = Tools::calldeepseekapi(QString("code.cpp"));
//                if (res== Status::OK){
//                    ui->CodeEditorUut->setStyleSheet("background-color: green;");
//                    ui->AI_CodeShower->ShowResults(QString("output.cpp"));
//                    ui->TryToCompile->setText("尝试编译");
//                }
//                else{
//                    ui->CodeEditorUut->setStyleSheet("background-color: red;");
//                    ui->TryToCompile->setText("ai优化失败，尝试重新编译");
//                }
//                ui->CompileResult->ShowResults(QString("compile.log"));
//            }
//            else{
//                ui->CodeEditorUut->setStyleSheet("background-color: red;");
//                ui->TryToCompile->setText("编译失败，尝试重新编译");
//                ui->CompileResult->ShowResults(QString("compile.log"));
//            }
//        }
//        else{
//            ui->CodeEditorUut->setStyleSheet("background-color: red;");
//            ui->TryToCompile->setText("保存失败");
//            ui->CompileResult->ShowResults(QString("compile.log"));
//        }



//        // 操作完成后重新启用按钮
//        ui->TryToCompile->setEnabled(true);


//    });

}
MainWindow::~MainWindow()
{
    delete ui;
    delete thehighlighter;
    for (auto f:files){
        deleteFile(f);
    }
}

