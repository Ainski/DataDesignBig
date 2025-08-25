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

    Tools tool;
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
        ui->CompileAICode->setEnabled(false);
        ui->CompileAICode->setText("编译AI代码");
        ui->TryExecute->setEnabled(false);
        ui->TryExecute->setText("执行优化结果");

        ui->SaveCode->setText("正在保存...");
        tool.logMessage("正在保存...",ui->CompileResult);
        ui->CodeEditorUut->setStyleSheet("background-color: white;");

        QApplication::processEvents(); //刷新界面

        Status res = ui->CodeEditorUut->SaveUserCode(UserInput);


        if (res==Status::OK){
            ui->SaveCode->setText("保存代码");
            files.push_back(UserInput);
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
    ui->TryToCompile->setEnabled(false);
    connect (ui->TryToCompile,&QPushButton::clicked,[this,&tool](){
        ui->SaveCode->setEnabled(false);
        ui->SaveCode->setText("保存代码");
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("编译检查");
        ui->AIOptimize->setEnabled(false);
        ui->AIOptimize->setText("AI优化");
        ui->CompileAICode->setEnabled(false);
        ui->CompileAICode->setText("编译AI代码");
        ui->TryExecute->setEnabled(false);
        ui->TryExecute->setText("执行优化结果");


        ui->TryToCompile->setText("正在编译检查...");
        tool.logMessage("正在编译检查...",ui->CompileResult);
        QApplication::processEvents(); //刷新界面
        qDebug()<<"start compile";
        ui->CodeEditorUut->setStyleSheet("background-color: white;");

        Status res = tool.compileExternalCode(UserInput,UserExe);

        qDebug()<<"compile finished";

        if (res==Status::OK){
            files.push_back(UserExe);
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
    // ai 优化过程
    ui->AIOptimize->setEnabled(false);
    connect (ui->AIOptimize ,&QPushButton::clicked ,[this,&tool](){
        ui->SaveCode->setEnabled(false);
        ui->SaveCode->setText("保存代码");
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("编译检查");
        ui->AIOptimize->setEnabled(false);
        ui->AIOptimize->setText("AI优化");
        ui->CompileAICode->setEnabled(false);
        ui->CompileAICode->setText("编译AI代码");
        ui->TryExecute->setEnabled(false);
        ui->TryExecute->setText("执行优化结果");

        ui->AIOptimize->setText("正在AI优化...");

        tool.logMessage("正在AI优化...",ui->CompileResult);
        ui->CodeEditorUut->setStyleSheet("background-color: white;");

        QApplication::processEvents(); //刷新界面

        Status res = tool.calldeepseekapi(UserInput,AIoutput);

        if (res==Status::OK){
            files.push_back(AIoutput);
            tool.logMessage(QString("AI优化成功"),ui->CompileResult);
            QApplication::processEvents(); //刷新界面
            ui->CodeEditorUut->setStyleSheet("background-color: green;");
            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存
            ui->CompileAICode->setEnabled(true);
            ui->AIOptimize->setText("AI优化");
            ui->CodeEditorUut->ShowResults(AIoutput);

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
    // 编译ai代码
    ui->CompileAICode->setEnabled(false);
    connect (ui->CompileAICode,&QPushButton::clicked,[this,&tool](){
        ui->SaveCode->setEnabled(false);
        ui->SaveCode->setText("保存代码");
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("编译检查");
        ui->AIOptimize->setEnabled(false);
        ui->AIOptimize->setText("AI优化");
        ui->CompileAICode->setEnabled(false);
        ui->CompileAICode->setText("编译AI代码");
        ui->TryExecute->setEnabled(false);
        ui->TryExecute->setText("执行优化结果");

        ui->CompileAICode->setText("正在编译检查...");
        tool.logMessage("正在编译检查...",ui->CompileResult);
        QApplication::processEvents(); //刷新界面
        qDebug()<<"start compile";
        ui->CodeEditorUut->setStyleSheet("background-color: white;");

        Status res = tool.compileExternalCode(AIoutput,AIExe);
        qDebug()<<"compile finished";

        if (res==Status::OK){
            files.push_back(AIExe);

            ui->CompileAICode->setText("编译检查");

            tool.logMessage(QString("编译检查成功"),ui->CompileResult);
            QApplication::processEvents(); //刷新界面
            ui->CodeEditorUut->setStyleSheet("background-color: green;");
            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存

            ui->AIOptimize->setEnabled(true);//允许用户尝试ai优化
            ui->TryExecute->setEnabled(true);//允许用户执行ai所产生代码
        }
        else{
            ui->CodeEditorUut->setStyleSheet("background-color: red;");
            ui->CompileAICode->setText("编译检查失败");

            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存
            ui->AIOptimize->setEnabled(true);//允许用户尝试ai优化


        }
        ui->CompileResult->ShowResults();

        QApplication::processEvents(); //刷新界面


    });

    ui->TryExecute->setEnabled(true);
    ui->CutIn->setEnabled(false);
    ui->NextStep->setEnabled(false);
    connect(ui->TryExecute,&QPushButton::clicked,[this,&tool](){
        ui->SaveCode->setEnabled(false);
        ui->SaveCode->setText("保存代码");
        ui->TryToCompile->setEnabled(false);
        ui->TryToCompile->setText("编译检查");
        ui->AIOptimize->setEnabled(false);
        ui->AIOptimize->setText("AI优化");
        ui->CompileAICode->setEnabled(false);
        ui->CompileAICode->setText("编译AI代码");
        ui->TryExecute->setEnabled(false);
        ui->TryExecute->setText("执行优化结果");

        tool.logMessage("开始执行优化结果");
        qDebug()<<"开始执行优化结果";
        ui->TryExecute->setText("执行中...");
        ui->CodeEditorUut->setStyleSheet("background-color: white;");



        if (ExecuteResult(AIExe,ui->NextStep,ui->CutIn)==Status::OK){
            files.push_back("map.txt");

            ui->TryExecute->setText("执行优化结果");

            tool.logMessage(QString("执行优化结果结束"));
            QApplication::processEvents(); //刷新界面
            ui->CodeEditorUut->setStyleSheet("background-color: green;");
            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存

            ui->AIOptimize->setEnabled(true);//允许用户尝试ai优化
            ui->TryExecute->setEnabled(true);//允许用户执行ai所产生代码
        }
        else {
            ui->CodeEditorUut->setStyleSheet("background-color: red;");
            ui->TryExecute->setText("执行失败");
            ui->SaveCode->setEnabled(true);//允许用户尝试重新保存
            ui->AIOptimize->setEnabled(true);//允许用户尝试ai优化
            ui->CompileAICode->setEnabled(true);

        }
        ui->CompileResult->ShowResults();
        ui->CutIn->setEnabled(false);
        ui->NextStep->setEnabled(false);

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
    for (auto f : files){
        deleteFile(f);
    }
}

