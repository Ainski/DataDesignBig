#include "codeeditor.h"
#include <QDebug>

CodeEditor::CodeEditor(QWidget *parent): QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    //事件绑定
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    setMode(EditorMode::EDIT);
    setTabStopDistance(32);
    setFont(QFont(QString::fromUtf8("Source Code Pro"), 12));
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    QFontMetrics metrics(QFont(QString::fromUtf8("Source Code Pro"), 12, QFont::Weight::Bold));
    int space = 10 + metrics.horizontalAdvance(QChar('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor("whitesmoke");
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.setFont(QFont(QString::fromUtf8("Source Code Pro"), 12, QFont::Weight::Bold));
            painter.drawText(-5, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::setMode(EditorMode mode)
{
    if(mode == EditorMode::BROWSE) //预览
    {
        this->setReadOnly(true);
        this->setStyleSheet("background: #f0f0f0;");
        highlightCurrentLine();
    }
    else if(mode == EditorMode::EDIT) //编辑
    {
        this->setReadOnly(false);
        this->setStyleSheet("background: #fcfcfc;");
        highlightCurrentLine();
    }
}




Status compileExternalCode(const QString &filename) {
    QString sourceFile = filename;
    QString outputFile;
    QString compiler;
    QStringList arguments;
    QString logFile = "compile.log";

    // 创建/清空日志文件
    QFile log(logFile);
    if (!log.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to create log file:" << logFile;
        return Status::FILE_OPEN_WRONG;
    }
    log.close();
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
        Tools::logMessage(logFile,"Source file not found: " + sourceFile);
        return Status::FILE_OPEN_WRONG;
    }

    // 执行编译命令
    QProcess compileProcess;
    compileProcess.setProcessChannelMode(QProcess::MergedChannels);

    // 启动编译进程
    compileProcess.start(compiler, arguments);



    if (!compileProcess.waitForStarted()) {
        Tools::logMessage(logFile,"Failed to start compiler process: " + compiler);
        return Status::COMPILE_ERROR;
    }

    // 实时捕获并记录编译输出
    QObject::connect(&compileProcess, &QProcess::readyReadStandardOutput, [&]() {
        QString output = compileProcess.readAllStandardOutput();
        QFile log(logFile);
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
            Tools::logMessage(logFile,"Compiler timed out after 30 seconds");
        } else {
            Tools::logMessage(logFile,"Compiler crashed unexpectedly");
        }
        return Status::COMPILE_ERROR;
    }

    // 检查编译结果
    if (compileProcess.exitStatus() != QProcess::NormalExit ||
        compileProcess.exitCode() != 0) {
        Tools::logMessage(logFile,"Compilation failed with exit code: " +
                   QString::number(compileProcess.exitCode()));
        return Status::COMPILE_ERROR;
    }

    // 验证输出文件
    if (!QFileInfo::exists(outputFile)) {
        Tools::logMessage(logFile,"Output file not created: " + outputFile);
        return Status::COMPILE_ERROR;
    }

    // 记录成功信息
    Tools::logMessage(logFile,"Successfully compiled: " + QDir::toNativeSeparators(outputFile));
    qDebug() << "Compilation log saved to:" << QDir::toNativeSeparators(logFile);
    return Status::OK;
}
Status CodeEditor::SaveUserCode()
{
    QFile file("code.cpp");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<"Failed to open the file out.txt";
        return Status::FILE_OPEN_WRONG;
    }
    QTextStream out(&file);
    QTextDocument *doc = document();
    int count=doc->blockCount();
    for (int i=0;i<count;i++){
        out<< doc->findBlockByNumber(i).text()<<endl;
    }
    file.close();
    qDebug()<<"Code Saved Successed";

    //尝试编译
    return compileExternalCode("code.cpp");
}
