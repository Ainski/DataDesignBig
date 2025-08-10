#include "tools.h"
#include <QDebug>
Tools::Tools()
{

}
Status Tools::logMessage(const QString &logFile,const QString& message) {
    qCritical() << message;
    QFile log(logFile);
    if (log.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&log);
        out.setCodec("UTF-8");  // 设置编码为UTF-8
        out << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")
            << message << "\n";
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

Status Tools::compileExternalCode(const QString &filename) {
    QString sourceFile = filename;
    QString outputFile;
    QString compiler;
    QStringList arguments;

    // 创建/清空日志文件
    // 辅助函数：记录消息到日志和控制台
    // 检查操作系统类型
#ifdef Q_OS_WIN
    outputFile = "code.exe";
    compiler = "g++"; // 假设使用MinGW的g++
    arguments << sourceFile << "-o" << outputFile;
#elif defined(Q_OS_LINUX)
    outputFile = "code";
    compiler = "g++"; // Linux默认使用g++
    arguments << sourceFile << "-o" << outputFile;
#else
    qCritical() << "Unsupported operating system";
    return Status::COMPILE_ERROR;
#endif

    // 检查源文件是否存在
    if (!QFileInfo::exists(sourceFile)) {
        Tools::logMessage(logfile,"Source file not found: " + sourceFile);
        return Status::FILE_OPEN_WRONG;
    }

    // 执行编译命令
    QProcess compileProcess;
    compileProcess.setProcessChannelMode(QProcess::MergedChannels);

    // 启动编译进程
    compileProcess.start(compiler, arguments);



    if (!compileProcess.waitForStarted()) {
        Tools::logMessage(logfile,"Failed to start compiler process: " + compiler);
        return Status::COMPILE_ERROR;
    }

    // 实时捕获并记录编译输出
    QObject::connect(&compileProcess, &QProcess::readyReadStandardOutput, [&]() {
        QString output = compileProcess.readAllStandardOutput();
        QFile log(logfile);
        if (log.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&log);
            out << output;
            log.close();
        }
    });

    // 等待编译完成
    if (!compileProcess.waitForFinished(30000)) { // 增加超时到30秒
        if (compileProcess.state() == QProcess::Running) {
            compileProcess.kill();
            Tools::logMessage(logfile,"Compiler timed out after 30 seconds");
        } else {
            Tools::logMessage(logfile,"Compiler crashed unexpectedly");
        }
        return Status::COMPILE_ERROR;
    }

    // 检查编译结果
    if (compileProcess.exitStatus() != QProcess::NormalExit ||
        compileProcess.exitCode() != 0) {
        Tools::logMessage(logfile,"Compilation failed with exit code: " +
                   QString::number(compileProcess.exitCode()));
        return Status::COMPILE_ERROR;
    }

    // 验证输出文件
    if (!QFileInfo::exists(outputFile)) {
        Tools::logMessage(logfile,"Output file not created: " + outputFile);
        return Status::COMPILE_ERROR;
    }

    // 记录成功信息
    Tools::logMessage(logfile,"Successfully compiled: " + QDir::toNativeSeparators(outputFile));
    qDebug() << "Compilation log saved to:" << QDir::toNativeSeparators(logfile);
    return Status::OK;
}

Status Tools::calldeepseekapi(const QString& filename)
{
    // 1. 读取C++源代码文件
    qDebug()<< QSslSocket::sslLibraryBuildVersionString();

    QFile inputFile(filename);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<QString( "无法打开输出文件:")+filename;
        logMessage(logfile,QString( "无法打开输出文件:")+filename);
        return Status::FILE_OPEN_WRONG;
    }
    QString inputCode = inputFile.readAll();
    inputFile.close();

    // 2. 构造API提示
    QString prompt = "现在给你一个c++代码，请生成能够在任意时刻读取二叉树抽象结构的函数，示例如下，其中被注释掉的内容为希望生成的部分：\n\n";

    // 添加示例代码
    prompt += R"(
    #include <iostream>
    //#include <queue>
    //#include <vector>
    //#include <unordered_map>
    //#include <algorithm>
    //#include <fstream>
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
    //
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
    //    system("pause");
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
        //printTreeAsGraph(root);
        root = deleteNode(root, 1);  // 删除只有一个子节点的节点
        //printTreeAsGraph(root);
        root = deleteNode(root, 14); // 删除叶子节点
        //printTreeAsGraph(root);

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
        logMessage(logfile,error);
        return Status::NETWORK_ERROR;
    }

    // 解析JSON响应
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    if (jsonResponse.isNull() || !jsonResponse.isObject()) {
        qDebug()<<QString( "无效的API响应格式");
        logMessage(logfile,QString( "无效的API响应格式"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonObject responseObj = jsonResponse.object();
    if (!responseObj.contains("choices") || !responseObj["choices"].isArray()) {
        qDebug()<<QString( "API响应缺少choices字段");
        logMessage(logfile,QString( "API响应缺少choices字段"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonArray choices = responseObj["choices"].toArray();
    if (choices.isEmpty()) {
        qDebug()<<QString( "API响应中没有结果");
        logMessage(logfile,QString( "API响应中没有结果"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonObject firstChoice = choices.first().toObject();
    if (!firstChoice.contains("message") || !firstChoice["message"].isObject()) {
        qDebug()<<QString( "API响应缺少message字段");
        logMessage(logfile,QString( "API响应缺少message字段"));
        return Status::DEEPSEEK_ERROR;
    }

    QJsonObject messageObj = firstChoice["message"].toObject();
    if (!messageObj.contains("content") || !messageObj["content"].isString()) {
        qDebug()<<QString( "API响应缺少content字段");
        logMessage(logfile,QString( "API响应缺少content字段"));
        return Status::DEEPSEEK_ERROR;
    }

    QString content = messageObj["content"].toString();

    // 6. 提取C++代码块
    QRegularExpression regex(R"(```c\+\+([\s\S]*?)```)");
    QRegularExpressionMatch match = regex.match(content);

    if (!match.hasMatch()) {
        // 尝试不带语言标识的代码块
        regex.setPattern(R"(```([\s\S]*?)```)");
        match = regex.match(content);
    }

    if (!match.hasMatch()) {
        qDebug()<<QString( "无法从响应中提取代码块");
        logMessage(logfile,QString( "无法从响应中提取代码块"));
        return Status::DEEPSEEK_ERROR;
    }

    QString generatedCode = match.captured(1).trimmed();

    // 7. 保存生成的代码
    QFile outputFile("output.txt");
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug()<<QString( "无法打开输出文件: output.cpp");
        logMessage(logfile,QString( "无法打开输出文件: output.cpp"));
        return Status::FILE_OPEN_WRONG;
    }

    QTextStream out(&outputFile);
    out << generatedCode;
    outputFile.close();

    qDebug()<<QString( "代码生成成功，已保存到 output.cpp");
    logMessage(logfile,QString( "代码生成成功，已保存到 output.cpp"));
    return Status::OK;
}
