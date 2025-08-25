#include "tools.h"
#include <QDebug>
Tools::Tools()
{
    logShower=Q_NULLPTR;
}
Tools::Tools(ResultShower *logShower)
{
    this->logShower = logShower;
}

Status Tools::logMessage(const QString& message,ResultShower* logShower,
                         const QString logFile) {
    qDebug() << message;
    QFile log(logFile);
    if (logShower==Q_NULLPTR) logShower=this->logShower;
    if (log.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&log);
        out.setCodec("UTF-8");  // 设置编码为UTF-8
        out << endl<<QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")
            << message;
        log.close();
    }
    else{
        qDebug()<<logfile<<"can't open ";
        return Status::FILE_OPEN_WRONG;
    }

    return Status::OK;
}


Status Tools::creatlog(const QString &logFile)
{
    QFile log(logFile);
    if (!log.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to create log file:" << logFile;
        return Status::FILE_OPEN_WRONG;
    }
    log.close();
    return Status::OK;
}

Status Tools::compileExternalCode(const QString &filename,const QString&OutputFileName) {
    qDebug() << "compile" << filename;
    QString sourceFile = filename;
    QString outputFile;
    QString compiler;
    QStringList arguments;

#ifdef Q_OS_WIN
    outputFile = OutputFileName;
    compiler = "g++";
    arguments << sourceFile << "-o" << outputFile;
#elif defined(Q_OS_LINUX)
    outputFile = "code";
    compiler = "g++";
    arguments << sourceFile << "-o" << outputFile;
#else
    qCritical() << "Unsupported operating system";
    return Status::COMPILE_ERROR;
#endif
    // 创建日志文件（提前打开避免频繁开关）
    QFile logFile(logfile);
    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Cannot open log file:" << logfile;
        return Status::FILE_OPEN_WRONG;
    }
    QTextStream logStream(&logFile);

    if (!QFileInfo::exists(sourceFile)) {
        logStream <<QString('\n')<< QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")<<
                     ("Source file not found: " + sourceFile);
        return Status::FILE_OPEN_WRONG;
    }

    QProcess compileProcess;
    compileProcess.setProcessChannelMode(QProcess::MergedChannels);



    // 关键优化：添加事件循环处理UI事件
    QEventLoop eventLoop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    // 连接超时和完成信号
    QObject::connect(&compileProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     &eventLoop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
        if (compileProcess.state() == QProcess::Running) {
            compileProcess.kill();
            logStream << QString('\n')<< QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")<<
                         "Compiler timed out after 30 seconds\n";
            eventLoop.quit();
        }
    });

    // 实时捕获输出
    QObject::connect(&compileProcess, &QProcess::readyReadStandardOutput, [&]() {
        logStream << compileProcess.readAllStandardOutput();
        logStream.flush(); // 确保及时写入
    });

    // 启动编译进程
    compileProcess.start(compiler, arguments);
    if (!compileProcess.waitForStarted()) {
        logStream << QString('\n')<< QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")<<
                     "Failed to start compiler: " << compiler;
        logFile.close();
        return Status::COMPILE_ERROR;
    }

    // 设置30秒超时
    timeoutTimer.start(30000);
    eventLoop.exec();

    // 处理编译结果
    const bool timedOut = !timeoutTimer.isActive();
    timeoutTimer.stop();

    if (timedOut) {
        logFile.close();
        return Status::COMPILE_ERROR;
    }

    if (compileProcess.exitStatus() != QProcess::NormalExit ||
        compileProcess.exitCode() != 0) {
        logStream << QString('\n')<< QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")<<
                     "Compilation failed. Exit code: "
                  << compileProcess.exitCode();
        logFile.close();
        return Status::COMPILE_ERROR;
    }

    if (!QFileInfo::exists(outputFile)) {
        logStream << QString('\n')<< QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")<<
                     "Output file not created: " << outputFile;
        logFile.close();
        return Status::COMPILE_ERROR;
    }

    logStream << QString('\n')
              << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")
              << "Compilation successful: " << outputFile;
    logFile.close();
    return Status::OK;
}
Status Tools::calldeepseekapi(const QString& filename,const QString outfilename)
{
    qDebug()<<"calldeepseekapi";
    // 1. 读取C++源代码文件
    QFile inputFile(filename);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<QString( "无法打开输出文件:")+filename;
        logMessage(QString( "无法打开输出文件:")+filename);
        return Status::FILE_OPEN_WRONG;
    }
    QString inputCode = inputFile.readAll();
    inputFile.close();

    // 2. 构造API提示
    QString prompt = R"(现在给你一个c++代码，请生成能够在任意时刻读取二叉树抽象结构的函数，要求每次二叉树发生变化的时候，能够输出所有的内容到"map.txt"，map.txt会参照如下的形式描述
'''
10 0 0
1 2
1 3
2 3
2 4
3 4
3 5
4 5
4 6
5 6
5 7
6 7
6 8
7 8
7 9
8 9
8 10
9 10
'''
其中，第一行的10表示共有10个节点，0表示为无向图（1表示有向图），第二个0表示边不可重复，后续每一行表示一条边。此外每次输出txt之后，要求输出map.txt之后，输出一行"please input c to continue",随后等待用户输入c,再接着运行。示例如下，其中被注释掉的内容为希望生成的部分：\n\n
    )";

    // 添加示例代码
    prompt += R"(
#include <iostream>
//#include <queue>
//#include <vector>
//#include <unordered_map>
//#include <algorithm>
//#include <fstream>
//#include <conio.h>
using namespace std;

// 二叉排序树节点结构
struct BSTNode {
    int data;
    BSTNode* left;
    BSTNode* right;
    BSTNode(int val) : data(val), left(nullptr), right(nullptr) {}
};

// 插入节点函数
BSTNode* insertNode(BSTNode* root, int value) {
    if (root == nullptr) {
        return new BSTNode(value);
    }
    if (value < root->data) {
        root->left = insertNode(root->left, value);
    }
    else if (value > root->data) {
        root->right = insertNode(root->right, value);
    }
    return root;
}

// 查找节点函数
bool searchNode(BSTNode* root, int key) {
    if (root == nullptr) return false;
    if (root->data == key) return true;
    return key < root->data ? searchNode(root->left, key) : searchNode(root->right, key);
}

// 找到子树最小值节点
BSTNode* minValueNode(BSTNode* node) {
    BSTNode* current = node;
    while (current && current->left != nullptr)
        current = current->left;
    return current;
}

// 删除节点函数
BSTNode* deleteNode(BSTNode* root, int key) {
    if (root == nullptr) return root;

    if (key < root->data) {
        root->left = deleteNode(root->left, key);
    }
    else if (key > root->data) {
        root->right = deleteNode(root->right, key);
    }
    else {
        // 节点只有一个子节点或没有子节点
        if (root->left == nullptr) {
            BSTNode* temp = root->right;
            delete root;
            return temp;
        }
        else if (root->right == nullptr) {
            BSTNode* temp = root->left;
            delete root;
            return temp;
        }

        // 节点有两个子节点：找到右子树的最小值节点
        BSTNode* temp = minValueNode(root->right);
        root->data = temp->data;
        root->right = deleteNode(root->right, temp->data);
    }
    return root;
}

// 中序遍历打印（验证BST结构）
void inorderTraversal(BSTNode* root) {
    if (root != nullptr) {
        inorderTraversal(root->left);
        cout << root->data << " ";
        inorderTraversal(root->right);
    }
}
//// 辅助函数：BFS遍历树并收集节点信息和边
//void collectTreeInfo(BSTNode* root, int& nodeCount, vector<pair<int, int>>& edges) {
//    if (!root) return;
//
//    queue<BSTNode*> q;
//    q.push(root);
//
//    while (!q.empty()) {
//        BSTNode* current = q.front();
//        q.pop();
//        nodeCount++;
//
//        if (current->left) {
//            edges.push_back({ current->data, current->left->data });
//            q.push(current->left);
//        }
//        if (current->right) {
//            edges.push_back({ current->data, current->right->data });
//            q.push(current->right);
//        }
//    }
//}

//// 将二叉排序树输出为指定图格式
//void printTreeAsGraph(BSTNode* root) {
//    int nodeCount = 0;
//    vector<pair<int, int>> edges;
//
//    // 收集树的信息
//    collectTreeInfo(root, nodeCount, edges);
//    ofstream file("map.txt");
//    if (!file.is_open()) {
//        cerr << "无法打开文件" << endl;
//        exit(-1);
//    }
//    // 输出图格式
//    file << nodeCount << " 0 0" << endl;
//    for (auto& edge : edges) {
//        file << edge.first << " " << edge.second << endl;
//    }
//    file.close();
//    cout<<"please input c to continue"<<endl;
//    while (_getch() != 'c')
//        ;
//
//}
// 主函数
int main() {
    BSTNode* root = nullptr;
    int values[] = { 1, 4, 2, 8, 5, 7, 11, 14, 99, 33 };
    int n = sizeof(values) / sizeof(values[0]);

    // 插入所有值
    for (int i = 0; i < n; i++) {
        root = insertNode(root, values[i]);
        //printTreeAsGraph(root);
    }

    cout << "中序遍历结果: ";
    inorderTraversal(root);
    cout << endl;

    // 测试查找功能
    cout << "查找 7: " << (searchNode(root, 7) ? "存在" : "不存在") << endl;
    cout << "查找 99: " << (searchNode(root, 99) ? "存在" : "不存在") << endl;
    cout << "查找 100: " << (searchNode(root, 100) ? "存在" : "不存在") << endl;

    // 测试删除功能
    root = deleteNode(root, 8);  // 删除有两个子节点的节点
    root = deleteNode(root, 1);  // 删除只有一个子节点的节点
    root = deleteNode(root, 14); // 删除叶子节点

    cout << "删除 8, 1, 14 后的中序遍历: ";
    inorderTraversal(root);
    cout << endl;

    return 0;
}
    )";

    prompt += "需要处理的代码如下：\n\n";
    prompt += inputCode;

    //qDebug()<<prompt;
    // 3. 准备API请求
    QUrl apiUrl(API_URL);
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + API_KEY).toUtf8());

    QJsonObject requestBody;
    requestBody["model"] = "deepseek-chat";
    requestBody["temperature"] = 0.3;
    requestBody["max_tokens"] = 2000;

    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;
    messages.append(message);

    requestBody["messages"] = messages;


    // 4. 发送API请求（同步方式）
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.post(request, QJsonDocument(requestBody).toJson());

    // 等待请求完成（同步）
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // 5. 处理API响应
    if (reply->error() != QNetworkReply::NoError) {
        QString error = "API调用失败: " + reply->errorString();
        reply->deleteLater();
        qDebug()<<error;
        logMessage(error);
        return Status::NETWORK_ERROR;
    }

    // 解析JSON响应
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
    logMessage(QString ("deepseek 返回结果: ")+reply->readAll());
    reply->deleteLater();


    if (jsonResponse.isNull() || !jsonResponse.isObject()) {
        qDebug()<<QString( "无效的API响应格式");
        logMessage(QString( "无效的API响应格式"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonObject responseObj = jsonResponse.object();
    if (!responseObj.contains("choices") || !responseObj["choices"].isArray()) {
        qDebug()<<QString( "API响应缺少choices字段");
        logMessage(QString( "API响应缺少choices字段"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonArray choices = responseObj["choices"].toArray();
    if (choices.isEmpty()) {
        qDebug()<<QString( "API响应中没有结果");
        logMessage(QString( "API响应中没有结果"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonObject firstChoice = choices.first().toObject();
    if (!firstChoice.contains("message") || !firstChoice["message"].isObject()) {
        qDebug()<<QString( "API响应缺少message字段");
        logMessage(QString( "API响应缺少message字段"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonObject messageObj = firstChoice["message"].toObject();
    if (!messageObj.contains("content") || !messageObj["content"].isString()) {
        qDebug()<<QString( "API响应缺少content字段");
        logMessage(QString( "API响应缺少content字段"));
        return Status::DEEPSEEK_ERROR;
    }

    QString content = messageObj["content"].toString();

    // 6. 提取C++代码块
    QRegularExpression regex(R"(```cpp([\s\S]*?)```)");
    QRegularExpressionMatch match = regex.match(content);

    if (!match.hasMatch()) {
        // 尝试不带语言标识的代码块
        regex.setPattern(R"(```([\s\S]*?)```)");
        match = regex.match(content);
    }

    if (!match.hasMatch()) {
        logMessage(QString( "无法从响应中提取代码块"));
        return Status::DEEPSEEK_ERROR;
    }

    QString generatedCode = match.captured(1).trimmed();

    // 7. 保存生成的代码
    QFile outputFile(outfilename);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {

        logMessage(QString( "无法打开输出文件: ")+outfilename);
        return Status::FILE_OPEN_WRONG;
    }

    QTextStream out(&outputFile);
    out.setCodec("UTF-8");  // 设置编码为UTF-8
    out << generatedCode;
    outputFile.close();

    logMessage(QString( "代码生成成功，已保存到 "+outfilename));
    return Status::OK;
}
Status deleteFile(const QString &filePath) {
    if (QFile::exists(filePath)) {
        if(QFile::remove(filePath)){
            qDebug()<<"The file "<<filePath<<"has been deleted";
            return Status::OK;
        }
        return Status::FILE_DELETE_ERROR;
    }
    return (Status::FILE_DELETE_ERROR); // 文件不存在
}
#include <QTextCodec>

Status ExecuteResult(const QString &filename, QPushButton* const &TryExecute, QPushButton* const &CutIn)
{
    Tools tool;
    QProcess *process = new QProcess();
    process->setProgram(filename);

    // 设置初始按钮状态
    TryExecute->setEnabled(false);
    CutIn->setEnabled(true); // 允许中断

    // 使用标志位跟踪中断状态，避免重复中断
    bool isInterrupted = false;

    // 创建GBK编码解码器
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    if (!gbkCodec) {
        tool.logMessage("Warning: GBK codec not available, using system default");
        gbkCodec = QTextCodec::codecForLocale();
    }

    // 连接process的输出信号
    QObject::connect(process, &QProcess::readyReadStandardOutput, [process, TryExecute, &tool, gbkCodec]() {
        QByteArray rawOutput = process->readAllStandardOutput();

        // 将GBK编码转换为QString
        QString output = gbkCodec->toUnicode(rawOutput);

        if (output.contains("please input c to continue")) {
            // 当检测到继续提示时启用按钮
            TryExecute->setEnabled(true);
            tool.logMessage("Process waiting for input 'c' to continue");
        }

        // 总是记录输出，即使不包含特定消息
        if (!output.trimmed().isEmpty()) {
            tool.logMessage("Process output: " + output);
        }
    });

    // 连接标准错误输出
    QObject::connect(process, &QProcess::readyReadStandardError, [process, &tool, gbkCodec]() {
        QByteArray rawError = process->readAllStandardError();

        // 将GBK编码转换为QString
        QString error = gbkCodec->toUnicode(rawError);

        if (!error.trimmed().isEmpty()) {
            tool.logMessage("Process error: " + error);
        }
    });

    // 连接进程状态变化信号
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [&TryExecute, &CutIn, &tool, &isInterrupted](int exitCode, QProcess::ExitStatus exitStatus) {
            Q_UNUSED(exitCode);
            // 进程结束时禁用所有按钮
            TryExecute->setEnabled(false);
            CutIn->setEnabled(false);

            if (exitStatus == QProcess::NormalExit) {
                tool.logMessage("Process completed successfully");
            } else {
                if (isInterrupted) {
                    tool.logMessage("Process was interrupted by user");
                } else {
                    tool.logMessage("Process exited abnormally");
                }
            }

            isInterrupted = false; // 重置中断标志
        });

    // 连接TryExecute按钮点击信号
    QObject::connect(TryExecute, &QPushButton::clicked, [process, TryExecute, &tool, gbkCodec]() {
        // 读取map.txt文件
        QFile file("map.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            file.close();

            // 输出文件内容到屏幕
            qDebug() << content;
            tool.logMessage("Map content: " + content);

            // 向进程发送"c"继续执行 - 注意这里需要转换为GBK编码
            QByteArray command = gbkCodec->fromUnicode("c\n");
            process->write(command);
            process->waitForBytesWritten();

            // 禁用按钮直到下次出现提示
            TryExecute->setEnabled(false);
        } else {
            QMessageBox::warning(nullptr, "Error", "Could not open map.txt");
            tool.logMessage("Error: Could not open map.txt");
        }
    });

    // 连接CutIn按钮点击信号 - 中断进程
    QObject::connect(CutIn, &QPushButton::clicked, [process, &tool, &CutIn, &isInterrupted]() {
        if (process->state() != QProcess::NotRunning && !isInterrupted) {
            isInterrupted = true; // 设置中断标志
            tool.logMessage("Process interrupted by user");

            // 立即禁用中断按钮，防止重复点击
            CutIn->setEnabled(false);

            // 先尝试正常终止
            process->terminate();

            // 设置超时，如果进程在3秒内没有正常终止，则强制终止
            QTimer::singleShot(3000, [process, &tool]() {
                if (process->state() != QProcess::NotRunning) {
                    tool.logMessage("Process not responding, forcing termination");
                    process->kill();
                }
            });
        }
    });

    // 启动进程
    process->start();
    if (!process->waitForStarted()) {
        tool.logMessage("Error: Failed to start process");
        delete process;
        return Status::PROCESS_ERROR;
    }

    tool.logMessage("Process started: " + filename);

    // 设置超时机制，防止进程死循环
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    QEventLoop loop;

    // 连接进程结束信号
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        &loop, &QEventLoop::quit);

    // 连接超时信号
    QObject::connect(&timeoutTimer, &QTimer::timeout, [process, &tool, &loop, &isInterrupted]() {
        if (process->state() != QProcess::NotRunning && !isInterrupted) {
            tool.logMessage("Process timeout - terminating");
            isInterrupted = true;
            process->terminate();

            // 再给一点时间让进程正常结束
            QTimer::singleShot(2000, [process, &tool, &loop]() {
                if (process->state() != QProcess::NotRunning) {
                    tool.logMessage("Process did not terminate - killing");
                    process->kill();
                }
                loop.quit();
            });
        } else {
            loop.quit();
        }
    });

    // 设置超时时间（例如2分钟）
    timeoutTimer.start(2 * 60 * 1000);
    loop.exec();

    // 停止超时计时器
    timeoutTimer.stop();

    // 检查执行结果
    Status result;
    if (process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0) {
        result = Status::OK;
        tool.logMessage("Process completed successfully with exit code 0");
    } else {
        result = Status::PROCESS_ERROR;
        QString exitMsg = isInterrupted ?
            "Process was interrupted by user" :
            "Process failed with exit code: " + QString::number(process->exitCode());
        tool.logMessage(exitMsg);
    }

    // 清理资源
    process->deleteLater();
    return result;
}
