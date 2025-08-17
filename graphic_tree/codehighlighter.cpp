#include "codehighlighter.h"
#include <QtDebug>

CodeHighLighter::CodeHighLighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // 类名和命名空间 - 深紫色
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    // 匹配类名 (class/struct关键字后的标识符)
    rule.pattern = QRegExp("\\b(class|struct|enum)\\s+\\w+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
    // 匹配命名空间
    rule.pattern = QRegExp("\\bnamespace\\s+\\w+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    // 字符串 - 深红色
    quotationFormat.setForeground(Qt::darkRed);
    // 双引号字符串 (支持转义字符)
    rule.pattern = QRegExp("\"[^\"]*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    // 单引号字符
    rule.pattern = QRegExp("'[^']*'");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // 函数 - 斜体蓝色
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    // 匹配函数名 (后跟左括号)
    rule.pattern = QRegExp("\\b\\w+(?=\\s*\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // 关键字 - 深蓝色
    QStringList keywords = {
        "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
        "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "class", "compl", "concept", "const", "consteval", "constexpr", "const_cast",
        "continue", "co_await", "co_return", "co_yield", "decltype", "default", "delete",
        "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern",
        "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
        "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator",
        "or", "or_eq", "private", "protected", "public", "register", "reinterpret_cast",
        "requires", "return", "short", "signed", "sizeof", "static", "static_assert",
        "static_cast", "struct", "switch", "template", "this", "thread_local", "throw",
        "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
        "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
    };

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    for(const QString &keyword : keywords) {
        rule.pattern = QRegExp("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // 预处理指令 - 紫红色
    preprocessorFormat.setForeground(QColor(150, 0, 150)); // 深紫色
    preprocessorFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("#\\s*\\w+\\b");
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);

    // 单行注释 - 绿色
    singleLineCommentFormat.setForeground(Qt::gray);
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // 多行注释 /* ... */
    multiLineCommentFormat.setForeground(Qt::gray);
    multiLineCommentFormat.setFontItalic(true);
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void CodeHighLighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);


    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

