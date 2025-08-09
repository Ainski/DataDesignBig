#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QWidget>
#include <QSyntaxHighlighter>
#include <QPainter>
#include "status.h"
#include <QFile>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include "tools.h"
typedef enum{
    BROWSE,
    EDIT,
}EditorMode;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent=0);
    void setMode(EditorMode mode);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
public slots:
    Status SaveUserCode();


protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
};

class LineNumberArea: public QWidget
{
public:
    LineNumberArea(CodeEditor *editor): QWidget(editor){
        codeEditor = editor;
    }
    QSize sizeHint() const override{
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }
private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
