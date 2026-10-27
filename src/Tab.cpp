#include "MainWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QTextEdit>
#include <QTextDocument>
#include <QRegularExpression>
#include <QMimeDatabase>
#include <QTextCursor>
#include <QScrollBar>

Tab::Tab(MainWindow* mainWindow, QWidget *parent)
    : QWidget(parent), m_mainWindow(mainWindow), isHtml(false), renderable(true), unsavedFileChanges(false)
{
    tabLayout = new QVBoxLayout(this);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(0);

    renderingLabel = new QLabel("", this);

    mdHorizontal = new QSplitter();
    mdHorizontal->setStyleSheet(R"(
QSplitter {
    margin: 0px;
    padding: 0px;
    border: none;
}

QSplitter::handle {
    margin: 0px;
    padding: 0px;
    background-color: black;
}

QSplitter > QWidget {
    border: none;
}
)"); 

    editor = new QTextEdit(this);
    editor->setAcceptRichText(false);
    editor->setPlaceholderText("Type here");

    mdView = new MdezDocBrowser(this);
    
    editor->setContentsMargins(0, 0, 0, 0);
    mdView->setContentsMargins(0, 0, 0, 0);
 
    mdHorizontal->addWidget(editor);
    mdHorizontal->addWidget(mdView);

    tabLayout->addWidget(renderingLabel);
    tabLayout->addWidget(mdHorizontal, 1);

    connect(editor, &QTextEdit::textChanged, this, &Tab::handleEditorInput);
    connect(editor, &QTextEdit::cursorPositionChanged, this, &Tab::updatePosLabel);
}

void Tab::handleEditorInput()
{
    int scrollPos = mdView->verticalScrollBar()->value();

    QString editorText = editor->toPlainText();

    if (!isHtml) {
        QTextDocument tempDoc;
        tempDoc.setMarkdown(editorText);
        QString rawHtml = tempDoc.toHtml();

        int h1Size = renderSize * 2.0;
        int h2Size = renderSize * 1.6;
        int h3Size = renderSize * 1.4;
        int h4Size = renderSize * 1.25;
        int h5Size = renderSize * 1.1;
        int h6Size = renderSize;

        QString css = QString(R"""(
    p, body { font-size: %1pt; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }
    h1 { font-size: %2pt !important; font-weight: bold; }
    h2 { font-size: %3pt !important; font-weight: bold; }
    h3 { font-size: %4pt !important; font-weight: bold; }
    h4 { font-size: %5pt !important; font-weight: bold; }
    h5 { font-size: %6pt !important; font-weight: bold; }
    h6 { font-size: %7pt !important; font-weight: bold; }
)""")
            .arg(renderSize).arg(h1Size).arg(h2Size).arg(h3Size).arg(h4Size).arg(h5Size).arg(h6Size);

        mdView->document()->setDefaultStyleSheet(css);

        int headPos = rawHtml.indexOf("<head>, Qt::CaseInsensitive");
        if (headPos != -1) rawHtml.insert(headPos+6, css);
        else rawHtml.prepend("<head>" + css + "</head>"); 

        QRegularExpression qtBlockquoteRegex("<p style=\" margin-top:[^;]+; margin-bottom:[^;]+; margin-left:40px; margin-right:40px; [^\"]*\">(.*?)</p>");

        QString tableReplacement =
        "<table width=\"100%\" cellpadding=\"8\" style=\"margin: 6px 0px;\">"
        "<tr><td bgcolor=\"#f0f0f0\" style=\"border-left: 4px solid #007acc;\">"
        "<span style=\"color: #555555;\">\\1</span>"
        "</td></tr>"
        "</table>";

        rawHtml.replace(qtBlockquoteRegex, tableReplacement);

        QRegularExpression qtCodeRegex(R"(((?:<pre[^>]*>(?:<span[^>]*>)?[\s\S]*?(?:</span>)?</pre>\s*)+))");
        QRegularExpressionMatchIterator i = qtCodeRegex.globalMatch(rawHtml);

        QList<QRegularExpressionMatch> matches;
        while (i.hasNext()) matches.append(i.next());

        for (int idx = matches.size() - 1; idx >= 0; --idx) {
            const QRegularExpressionMatch &match = matches.at(idx);
            QString fullBlock = match.captured(1);

            QRegularExpression tagCleaner(R"(</?(?:pre|span|br)[^>]*>)");
            QString cleanText = fullBlock;
            cleanText.remove(tagCleaner);

            cleanText.replace("\n", "<br />");


            QString codeReplacement =
            "<table border=\"0\" cellspacing=\"0\" cellpadding=\"10\" bgcolor=\"#f4f4f5\" style=\"border: 1px solid #e4e4e7; border-radius: 4px; margin: 10px 0;\">"
            "<tr><td>"
            "<pre style=\"margin: 0; font-family: 'Hack', monospace; color: #27272a; font-size: 13px; line-height: 1.5; width: 100%; white-space: pre-wrap;\">"
            + cleanText +
            "</pre>"
            "</td></tr>"
            "</table>";

            rawHtml.replace(match.capturedStart(0), match.capturedLength(0), codeReplacement);

        }

        mdView->setHtml(rawHtml);

        QFont baseFont = mdView->font();
        baseFont.setPointSize(renderSize);
        mdView->document()->setDefaultFont(baseFont);
    } else {
        this->renderingLabel->setText("Viewing as HTML");

        mdView->setHtml(editorText);
    }

    mdView->verticalScrollBar()->setValue(scrollPos);

    unsavedFileChanges = true;
    m_mainWindow->updateWindowTitle();

    int wordCount = countWords();
    QString wcLabelText = QString("Words: %1").arg(wordCount);
    m_mainWindow->wcLabel->setText(wcLabelText);
}

int Tab::countWords()
{
    QTextDocument doc;
    doc.setMarkdown(editor->toPlainText());

    QString text = doc.toPlainText();
    QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    return words.size();
}

void Tab::updatePosLabel() {
    QTextCursor cur = editor->textCursor();

    QString line = QString::number(cur.blockNumber() + 1);
    QString col = QString::number(cur.positionInBlock() + 1);

    m_mainWindow->posLabel->setText(QString("%1, %2").arg(line, col));
}

void Tab::handleRenderVisibility(const QString &filePath)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath);
    QString mimeName = mime.name();

    if (filePath.endsWith(".md", Qt::CaseInsensitive) ||
        filePath.endsWith(".markdown", Qt::CaseInsensitive)) {
        mimeName = "text/markdown";
    } else if (filePath.endsWith(".html", Qt::CaseInsensitive) ||
               filePath.endsWith(".htm", Qt::CaseInsensitive)) {
        mimeName = "text/html";
    }

    if (mimeName == "text/markdown") {
        renderable = true;
        isHtml = false;
        this->mdView->setVisible(true);
        this->renderingLabel->setText("");
    }
    else if (mimeName == "text/html") {
        renderable = true;
        isHtml = true;
        this->mdView->setVisible(true);
        this->renderingLabel->setText("Viewing as HTML");
    }
    // Code files
    else if (mime.inherits("text/plain")) {
        renderable = false;
        isHtml = false;
        this->mdView->setVisible(false);
        this->renderingLabel->setText("Viewing as plain text (Markdown rendering disabled for this file type)");
    }
    else {
        renderable = false;
        isHtml = false;
        this->mdView->setVisible(false);
        this->renderingLabel->setText("Viewing as plain text (Markdown rendering disabled for this file type)");
    }
}

void Tab::updateRenderFontSize(const int &size)
{
    if (size != -1 && mdView) {
        renderSize = size;
        handleEditorInput();
    }
}
