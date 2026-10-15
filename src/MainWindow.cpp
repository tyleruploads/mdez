#include "MainWindow.h"
#include <QObject>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QTextEdit>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QTextCursor>
#include <QStringList>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QStatusBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    unsavedFileChanges = false;

    mainLayout = new QVBoxLayout(this);

    header = new QLabel("MDEZ++", this);
    
    QFont headerFont("Arial", 16, QFont::Bold);
    header->setFont(headerFont);
    header->setAlignment(Qt::AlignmentFlag::AlignCenter);

    menuBar = new QMenuBar(this);

    fileMenu = new QMenu(this);
    fileMenu->setTitle("File");

    openFileAction = new QAction(this);
    saveAsFileAction = new QAction(this);
    saveFileAction = new QAction(this);
    printAction = new QAction(this);
    exportHtmlAction = new QAction(this);

    openFileAction->setText(tr("Open File"));
    saveAsFileAction->setText(tr("Save As"));
    saveFileAction->setText(tr("Save"));
    printAction->setText(tr("Print"));
    exportHtmlAction->setText(tr("Export as HTML"));

    openFileAction->setStatusTip(tr("Open an existing document"));
    saveAsFileAction->setStatusTip(tr("Save the current document under a new name or location"));
    saveFileAction->setStatusTip(tr("Save changes to the current document"));
    printAction->setStatusTip(tr("Print the current document"));
    exportHtmlAction->setStatusTip(tr("Export current document in the HTML format"));

    openFileAction->setShortcut(QKeySequence::Open);
    saveAsFileAction->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    saveFileAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    printAction->setShortcut(QKeySequence::Print);
   
    openFileAction->setShortcutContext(Qt::ApplicationShortcut);
    saveAsFileAction->setShortcutContext(Qt::ApplicationShortcut);
    saveFileAction->setShortcutContext(Qt::ApplicationShortcut);
    printAction->setShortcutContext(Qt::ApplicationShortcut);

    this->addAction(openFileAction);
    this->addAction(saveAsFileAction);
    this->addAction(saveFileAction);
    this->addAction(printAction);

    helpMenu = new QMenu(this);
    helpMenu->setTitle("Help");

    aboutAction = new QAction(this);
    mitLicenseAction = new QAction(this);
    aboutQtAction = new QAction(this);
    mdCheatSheetAction = new QAction(this);
    keyboardCombosAction = new QAction(this);
    reportIssueAction = new QAction(this);
    checkUpdatesAction = new QAction(this);

    aboutAction->setText(tr("About MDEZ++"));
    mitLicenseAction->setText(tr("MDEZ++ License"));
    mdCheatSheetAction->setText(tr("Markdown Cheat Sheet"));
    keyboardCombosAction->setText(tr("Keyboard Combinations"));
    reportIssueAction->setText(tr("Report Issue"));
    checkUpdatesAction->setText(tr("Check for Updates"));
    aboutQtAction->setText(tr("About Qt"));

    aboutAction->setStatusTip(tr("Show version, copyright, and licnesing information for MDEZ++"));
    mitLicenseAction->setStatusTip(tr("Display the full text of the MIT License for MDEZ++"));
    aboutQtAction->setStatusTip(tr("Show information about the Qt framework and its license"));
    mdCheatSheetAction->setStatusTip(tr("Open a quick reference cheat sheet for Markdown syntax"));
    keyboardCombosAction->setStatusTip(tr("View keyboard shortcuts for MDEZ++"));
    reportIssueAction->setStatusTip(tr("Open the page to make a GitHub Issue in the MDEZ++ Repository"));
    checkUpdatesAction->setStatusTip(tr("Open the the GitHub Releases page for MDEZ++ to check for updates"));

    mdHorizontal = new QHBoxLayout();

    editor = new QTextEdit(this);
    editor->setPlaceholderText("Type here");

    mdView = new QTextBrowser(this);
    mdView->setOpenExternalLinks(true); // Makes links open in browser

    statusBar = new QStatusBar(this);
    versionLabel = new QLabel(QString("v" APP_VERSION), this);
    wcLabel = new QLabel("Words: 0", this);
    posLabel = new QLabel("1,1", this);
    pathLabel = new QLabel("Untitled", this);

    statusSpacer1 = new QWidget(this);
    statusSpacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    statusSpacer2 = new QWidget(this);
    statusSpacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    statusSpacer3 = new QWidget(this);
    statusSpacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    connect(editor, &QTextEdit::textChanged, this, &MainWindow::updateMarkdown);

    connect(editor, &QTextEdit::cursorPositionChanged, this, &MainWindow::updatePosLabel);

    connect(openFileAction, &QAction::triggered, this, &MainWindow::handleOpenFile);
    connect(saveAsFileAction, &QAction::triggered, this, &MainWindow::handleSaveAsFile);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::handleSaveFile);
    connect(printAction, &QAction::triggered, this, &MainWindow::printDocument);
    connect(exportHtmlAction, &QAction::triggered, this, &MainWindow::exportHtml);

    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(mitLicenseAction, &QAction::triggered, this, &MainWindow::showFullMitLicense);
    connect(aboutQtAction, &QAction::triggered, this, [this]() {
        QMessageBox::aboutQt(this, tr("About Qt"));
    });
    connect(mdCheatSheetAction, &QAction::triggered, this, [this]() {
            this->openLink(QUrl("https://dev.to/thetylern/a-markdown-cheat-sheet-for-anywhere-32bp"));
    });
    connect(keyboardCombosAction, &QAction::triggered, this, &MainWindow::openKeyboardCombos);
    connect(reportIssueAction, &QAction::triggered, this, [this]() {
        this->openLink(QUrl("https://github.com/tyleruploads/mdez/issues/new"));
    });
    connect(checkUpdatesAction, &QAction::triggered, this, [this]() {
        this->openLink(QUrl("https://github.com/tyleruploads/mdez/releases/latest/"));
    });

    fileMenu->addActions({openFileAction, saveAsFileAction, saveFileAction, printAction, exportHtmlAction});
    helpMenu->addActions({aboutAction, mitLicenseAction, aboutQtAction, mdCheatSheetAction, keyboardCombosAction, reportIssueAction, checkUpdatesAction});

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);

    mdHorizontal->addWidget(editor);
    mdHorizontal->addWidget(mdView);

    statusBar->addWidget(versionLabel);
    statusBar->addWidget(statusSpacer1, 1); // Expands
    statusBar->addWidget(wcLabel);
    statusBar->addWidget(statusSpacer2, 1); // Expands
    statusBar->addWidget(posLabel);
    statusBar->addWidget(statusSpacer3, 1); // Expands
    statusBar->addWidget(pathLabel);
 
    mainLayout->addWidget(header);
    mainLayout->addWidget(menuBar);
    mainLayout->addLayout(mdHorizontal);
    mainLayout->addWidget(statusBar);

}

void MainWindow::updateMarkdown()
{
    QString mdText = editor->toPlainText();

    QTextDocument tempDoc;
    tempDoc.setMarkdown(mdText);
    QString rawHtml = tempDoc.toHtml();

    QRegularExpression qtBlockquoteRegex("<p style=\" margin-top:[^;]+; margin-bottom:[^;]+; margin-left:40px; margin-right:40px; [^\"]*\">(.*?)</p>");

    QString tableReplacement =
        "<table width=\"100%\" cellpadding=\"8\" style=\"margin: 6px 0px;\">"
        "<tr><td bgcolor=\"#f0f0f0\" style=\"border-left: 4px solid #007acc;\">"
        "<span style=\"color: #555555;\">\\1</span>"
        "</td></tr>"
        "</table>";

    rawHtml.replace(qtBlockquoteRegex, tableReplacement);

    mdView->setHtml(rawHtml);
    unsavedFileChanges = true;
    MainWindow::updateWindowTitle();

    int wordCount = countWords();
    QString wcLabelText = QString("Words: %1").arg(wordCount);
    wcLabel->setText(wcLabelText);
}

void MainWindow::updateWindowTitle()
{
    QString displayPath = currentFilePath.isEmpty() ? "Untitled" : currentFilePath;

    if (unsavedFileChanges == true)
        displayPath += "*";

    setWindowTitle(QString("%1 | MDEZ++").arg(displayPath));
    pathLabel->setText(displayPath);
}

void MainWindow::showAboutDialog()
{
    QString title = tr("About & License");
    QString text = tr(
        "<h3>MDEZ++ "
        APP_VERSION
        "</h3>"
        "<p>Copyright (c) 2026 Tyler N. <a href=\"https://github.com/tyleruploads\">https://github.com/tyleruploads</a>.</p>"

        "<h4>Application License</h4>"
        "<p>This application is open-source software licensed under the <b>MIT License</b>. "
        "For more details, use the 'MDEZ++ License' option in the Help menu or visit "
        "<a href=\"https://github.com/tyleruploads/mdez\">https://github.com/tyleruploads/mdez</a>.</p>"

        "<hr/>"

        "<h4>Third-Party Components</h4>"
        "<p>This application uses the <b>Qt Toolkit</b> framework under the terms of the "
        "GNU Lesser General Public License (LGPLv3).</p>"
        "<p>Qt is a registered trademark of The Qt Company Ltd. and/or its subsidiaries. The Qt framework is provided 'as is' without any express or implied warranty.</p>"
        "<p>For more details, use the 'About Qt' option in the Help menu or visit "
        "<a href=\"https://qt.io/licensing\">https://qt.io/licensing</a></p>"
    );

    QMessageBox::about(this, title, text);
}

void MainWindow::showFullMitLicense()
{
    QString title = tr("Mit License - MDEZ++");
    QString mitText = tr(R"(
Copyright (c) 2026 Tyler N. <https://github.com/tyleruploads>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
)");
    QMessageBox::about(this, title, mitText);
}

void MainWindow::openKeyboardCombos()
{
    QString text = tr(R"(
<table>
    <tr>
        <td>Action</td>
        <td>Shortcut</td>
        <td>Description</td>
    </tr>
    <tr>
        <td>Open File</td>
        <td>`Ctrl / Cmd + O`</td>
        <td>Open an existing document</td>
    </tr>
    <tr>
        <td>Save As</td>
        <td>`Ctrl / Cmd + Shift + S`</td>
        <td>Save the current document under a new name or location</td>
    </tr>
    <tr>
        <td>Save</td>
        <td>`Ctrl / Cmd + S`</td>
        <td>Save changes to the current document</td>
    </tr>
    <tr>
        <td>Print</td>
        <td>`Ctrl + P`</td>
        <td>Print the current document</td>
    </tr>
</table>
)");
    QMessageBox::information(this, "Keyboard Combinations", text);
}

void MainWindow::openLink(const QUrl &link)
{
    bool success = QDesktopServices::openUrl(link);

    if (!success) {
        QString text = tr("Failed to open link: %1").arg(link.toString());
        QMessageBox::critical(this, "Error", text);
    }
}

int MainWindow::countWords()
{
    QTextDocument doc;
    doc.setMarkdown(editor->toPlainText());

    QString text = doc.toPlainText();
    QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    return words.size();
}

void MainWindow::updatePosLabel() {
    QTextCursor cur = editor->textCursor();

    QString line = QString::number(cur.blockNumber() + 1);
    QString col = QString::number(cur.positionInBlock() + 1);

    posLabel->setText(QString("%1, %2").arg(line, col));
}


void MainWindow::handleOpenFile()
{
    QString fileOpenPath = QFileDialog::getOpenFileName(this,
        tr("Open Markdown File"), // Window title
        !currentFilePath.isEmpty() ? currentFilePath : "document",
        tr("Markdown Files (*.md);;All Files (*)"));

    if (fileOpenPath.isEmpty())
        return;

    QFile file(fileOpenPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File read only or not text file";
        return;
    }

    QTextStream in(&file);
    QString fileContents = in.readAll(); 

    editor->setText(fileContents);
    MainWindow::updateMarkdown();

    unsavedFileChanges = false;
    currentFilePath = fileOpenPath;
    MainWindow::updateWindowTitle();
}

void MainWindow::handleSaveAsFile()
{
    QString fileSavePath = QFileDialog::getSaveFileName(this,
        tr("Save Markdown File"),
        !currentFilePath.isEmpty() ? currentFilePath : "document",
        tr("Markdown files (*.md);;All Files (*)")
    );

    if (fileSavePath.isEmpty())
        return;

    QFile file(fileSavePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    unsavedFileChanges = false;
    currentFilePath = fileSavePath;
    MainWindow::updateWindowTitle();

    QTextStream out(&file);
    out << editor->toPlainText();
}

void MainWindow::handleSaveFile()
{
    if (currentFilePath.isEmpty()) {
        MainWindow::handleSaveAsFile();
        return;
    }

    QFile file(currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    unsavedFileChanges = false;
    MainWindow::updateWindowTitle();

    QTextStream out(&file);
    out << editor->toPlainText();

}

void MainWindow::printDocument()
{
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        mdView->print(&printer);
    }
}

void MainWindow::exportHtml()
{
    QTextDocument *doc = mdView->document();

    QString htmlSavePath = QFileDialog::getSaveFileName(this,
        tr("Save as HTML File"),
        "document.html",
        tr("HTML files (*.html);;All Files (*)")
    );

    if (htmlSavePath.isEmpty())
        return;

    QFile file(htmlSavePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextDocumentWriter writer(htmlSavePath);
    if (!writer.write(doc)) {
        QString text = tr("Failed to save document as HTML to: %1").arg(htmlSavePath);
        QMessageBox::critical(this, "Error", text);
    }
}
