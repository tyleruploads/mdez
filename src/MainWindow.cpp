#include "MainWindow.h"
#ifdef Q_OS_WASM
#include <emscripten.h>
#endif
#include <QObject>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QSplitter>
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
#ifndef Q_OS_WASM
#include <QPrinter>
#include <QPrintDialog>
#endif
#include <QMimeDatabase>
#include <QMimeType>
#include <QFile>
#include <QRandomGenerator>
#include <QPushButton>
#include <QTimer>
#include <QSettings>
#include <QCheckBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    settings = new QSettings();

    renderable = true;
    isHtml = false;
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

#ifndef Q_OS_WASM
    saveAsFileAction = new QAction(this);
    saveAsFileAction->setText(tr("Save As"));
    saveAsFileAction->setStatusTip(tr("Save the current document under a new name or location"));
    saveAsFileAction->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    saveAsFileAction->setShortcutContext(Qt::ApplicationShortcut);
    this->addAction(saveAsFileAction);
    connect(saveAsFileAction, &QAction::triggered, this, &MainWindow::handleSaveAsFile);
#endif

    saveFileAction = new QAction(this);
    printAction = new QAction(this);
    exportHtmlAction = new QAction(this);

    openFileAction->setText(tr("Open File")); 
    saveFileAction->setText(tr("Save"));
    printAction->setText(tr("Print"));
    exportHtmlAction->setText(tr("Export as HTML"));

    openFileAction->setStatusTip(tr("Open an existing document")); 
    saveFileAction->setStatusTip(tr("Save changes to the current document"));
    printAction->setStatusTip(tr("Print the current document"));
    exportHtmlAction->setStatusTip(tr("Export current document in the HTML format"));

    openFileAction->setShortcut(QKeySequence::Open); 
    saveFileAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    printAction->setShortcut(QKeySequence::Print);
   
    openFileAction->setShortcutContext(Qt::ApplicationShortcut); 
    saveFileAction->setShortcutContext(Qt::ApplicationShortcut);
    printAction->setShortcutContext(Qt::ApplicationShortcut);

    this->addAction(openFileAction); 
    this->addAction(saveFileAction);
    this->addAction(printAction);

    helpMenu = new QMenu(this);
    helpMenu->setTitle("Help");

    aboutAction = new QAction(this);
    mitLicenseAction = new QAction(this);
    aboutQtAction = new QAction(this);
    funFactAction = new QAction(this);
    mdCheatSheetAction = new QAction(this);
    keyboardCombosAction = new QAction(this);
    reportIssueAction = new QAction(this);
    checkUpdatesAction = new QAction(this);

    aboutAction->setText(tr("About MDEZ++"));
    mitLicenseAction->setText(tr("MDEZ++ License"));
    funFactAction->setText(tr("Fun Facts!"));
    mdCheatSheetAction->setText(tr("Markdown Cheat Sheet"));
    keyboardCombosAction->setText(tr("Keyboard Combinations"));
    reportIssueAction->setText(tr("Report Issue"));
    checkUpdatesAction->setText(tr("Check for Updates"));
    aboutQtAction->setText(tr("About Qt"));

    aboutAction->setStatusTip(tr("Show version, copyright, and licnesing information for MDEZ++"));
    mitLicenseAction->setStatusTip(tr("Display the full text of the MIT License for MDEZ++"));
    aboutQtAction->setStatusTip(tr("Show information about the Qt framework and its license"));
    funFactAction->setStatusTip(tr("Display fun facts about MDEZ++"));
    mdCheatSheetAction->setStatusTip(tr("Open a quick reference cheat sheet for Markdown syntax"));
    keyboardCombosAction->setStatusTip(tr("View keyboard shortcuts for MDEZ++"));
    reportIssueAction->setStatusTip(tr("Open the page to make a GitHub Issue in the MDEZ++ Repository"));
    checkUpdatesAction->setStatusTip(tr("Open the the GitHub Releases page for MDEZ++ to check for updates"));

    formatMenu = new QMenu(this);
    formatMenu->setTitle("Format");

    boldFormat = new QAction(this);
    italicFormat = new QAction(this);
    h1Format = new QAction(this);
    h2Format = new QAction(this);
    h3Format = new QAction(this);
    dividerFormat = new QAction(this);
    blockquoteFormat = new QAction(this);
    strikethroughFormat = new QAction(this);
    linkFormat = new QAction(this);
    imageFormat = new QAction(this);
    codeFormat = new QAction(this);
    inlineCodeFormat = new QAction(this);

    boldFormat->setText(tr("Bold"));
    italicFormat->setText(tr("Italic"));
    h1Format->setText(tr("Heading 1"));
    h2Format->setText(tr("Heading 2"));
    h3Format->setText(tr("Heading 3"));
    dividerFormat->setText(tr("Horizontal Divider"));
    blockquoteFormat->setText(tr("Block Quote"));
    strikethroughFormat->setText(tr("Strikethrough"));
    linkFormat->setText(tr("Link"));
    imageFormat->setText(tr("Image"));
    codeFormat->setText(tr("Code"));
    inlineCodeFormat->setText(tr("Inline Code"));

    boldFormat->setShortcut(QKeySequence(tr("Ctrl+B")));
    italicFormat->setShortcut(QKeySequence(tr("Ctrl+I")));
    h1Format->setShortcut(QKeySequence(tr("Ctrl+Alt+1")));
    h2Format->setShortcut(QKeySequence(tr("Ctrl+Alt+2")));
    h3Format->setShortcut(QKeySequence(tr("Ctrl+Alt+3")));
    dividerFormat->setShortcut(QKeySequence(tr("Ctrl+D")));
    blockquoteFormat->setShortcut(QKeySequence(tr("Ctrl+]")));
    strikethroughFormat->setShortcut(QKeySequence(tr("Alt+Shift+5")));
    linkFormat->setShortcut(QKeySequence(tr("Ctrl+K")));
    imageFormat->setShortcut(QKeySequence(tr("Ctrl+Shift+I")));
    codeFormat->setShortcut(QKeySequence(tr("Ctrl+\\")));
    inlineCodeFormat->setShortcut(QKeySequence(tr("Ctrl+E")));

    boldFormat->setShortcutContext(Qt::ApplicationShortcut);
    italicFormat->setShortcutContext(Qt::ApplicationShortcut);
    h1Format->setShortcutContext(Qt::ApplicationShortcut);
    h2Format->setShortcutContext(Qt::ApplicationShortcut);
    h3Format->setShortcutContext(Qt::ApplicationShortcut);
    dividerFormat->setShortcutContext(Qt::ApplicationShortcut);
    blockquoteFormat->setShortcutContext(Qt::ApplicationShortcut);
    strikethroughFormat->setShortcutContext(Qt::ApplicationShortcut);
    linkFormat->setShortcutContext(Qt::ApplicationShortcut);
    imageFormat->setShortcutContext(Qt::ApplicationShortcut);
    codeFormat->setShortcutContext(Qt::ApplicationShortcut);
    inlineCodeFormat->setShortcutContext(Qt::ApplicationShortcut);

    this->addAction(boldFormat);
    this->addAction(italicFormat);
    this->addAction(h1Format);
    this->addAction(h2Format);
    this->addAction(h3Format);
    this->addAction(dividerFormat);
    this->addAction(blockquoteFormat);
    this->addAction(strikethroughFormat);
    this->addAction(linkFormat);
    this->addAction(imageFormat);
    this->addAction(codeFormat);
    this->addAction(inlineCodeFormat);

    settingsAction = new QAction(this);
    settingsAction->setText(tr("Settings"));
    settingsAction->setStatusTip(tr("Configure settings in MDEZ++"));

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
}

QSplitter > QWidget {
    border: none;
}
)");
    mainLayout->setContentsMargins(0, 0, 0, 0);

    editor = new QTextEdit(this);
    editor->setAcceptRichText(false);
    editor->setPlaceholderText("Type here");

    mdView = new MdezDocBrowser(this);
    
    editor->setContentsMargins(0, 0, 0, 0);
    mdView->setContentsMargins(0, 0, 0, 0);

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

    connect(editor, &QTextEdit::textChanged, this, &MainWindow::handleEditorInput);

    connect(editor, &QTextEdit::cursorPositionChanged, this, &MainWindow::updatePosLabel);

    connect(openFileAction, &QAction::triggered, this, &MainWindow::handleOpenFile); 
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::handleSaveFile);
    connect(printAction, &QAction::triggered, this, &MainWindow::printDocument);
    connect(exportHtmlAction, &QAction::triggered, this, &MainWindow::exportHtml);

    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(mitLicenseAction, &QAction::triggered, this, &MainWindow::showFullMitLicense);
    connect(funFactAction, &QAction::triggered, this, &MainWindow::showFunFact);
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

    connect(boldFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("bold");
    });
    connect(italicFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("italic");
    });
    connect(h1Format, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("h1");
    });
    connect(h2Format, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("h2");
    });
    connect(h3Format, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("h3");
    });
    connect(dividerFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("horizontal divider");
    });
    connect(blockquoteFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("blockquote");
    });
    connect(strikethroughFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("strikethrough");
    });
    connect(linkFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("link");
    });
    connect(imageFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("image");
    });
    connect(codeFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("code");
    });
    connect(inlineCodeFormat, &QAction::triggered, this, [this]() {
        MainWindow::formatHandler("inline code");
    });

    connect(settingsAction, &QAction::triggered, this, &MainWindow::showSettingsMenu);

    fileMenu->addAction(openFileAction);
#ifndef Q_OS_WASM
    fileMenu->addAction(saveAsFileAction);
#endif
    fileMenu->addActions({saveFileAction, printAction, exportHtmlAction});
    helpMenu->addActions({aboutAction, mitLicenseAction, aboutQtAction, funFactAction, mdCheatSheetAction, keyboardCombosAction, reportIssueAction, checkUpdatesAction});
    formatMenu->addActions({boldFormat, italicFormat, h1Format, h2Format, h3Format, dividerFormat, blockquoteFormat, strikethroughFormat, linkFormat, imageFormat, codeFormat, inlineCodeFormat});

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);
    menuBar->addMenu(formatMenu);
    menuBar->addAction(settingsAction);

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
    mainLayout->addWidget(renderingLabel);
    mainLayout->addWidget(mdHorizontal, 1);
    mainLayout->addWidget(statusBar);

    MainWindow::initSettingVars();

    if (showFunFacts) QTimer::singleShot(1, this, &MainWindow::showFunFact);

#ifndef Q_OS_WASM
    autoSaveTimer = new QTimer(this);
    MainWindow::autoSaveHandler();
#endif
}

void MainWindow::handleEditorInput()
{
    QString editorText = editor->toPlainText();

    if (!isHtml) {
        QTextDocument tempDoc;
        tempDoc.setMarkdown(editorText);
        QString rawHtml = tempDoc.toHtml(); 

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
    } else {
        this->renderingLabel->setText("Viewing as HTML");

        mdView->setHtml(editorText);
    }

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

// Pass -1 for any value to skip its font point size change
void MainWindow::updateFontPointSize(const int &editorSize, const int &renderSize)
{
    if (editorSize != -1) {
        QFont font = editor->font();
        font.setPointSize(editorSize);
        editor->setFont(font);
    }

    if (renderSize != -1) {
        QFont docFont = mdView->font();
        docFont.setPointSize(renderSize);
        mdView->setFont(docFont);
    }
}

void MainWindow::initSettingVars()
{
    lineEndingChar = settings->value("editor/lineEndings", "\n").toString();
#ifndef Q_OS_WASM
    autoSaveInterval = settings->value("editor/autoSaveInterval", 60).toInt();
#endif
    useLineWrapping = settings->value("editor/enableLineWrapping", true).toBool();
    showFunFacts = settings->value("other/showFunFacts", true).toBool();

    MainWindow::updateFontPointSize(
        settings->value("editor/fontSize", 10).toInt(),
        settings->value("render/fontSize", 12).toInt()
    );

    this->editor->setLineWrapMode((useLineWrapping == true) ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

void MainWindow::showSettingsMenu()
{
    QDialog *settingsDialog = new QDialog(this);
    settingsDialog->setWindowTitle(tr("MDEZ++ Settings"));
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);

    QFormLayout *formLayout = new QFormLayout(settingsDialog);

#ifndef Q_OS_WASM
    QSpinBox *autoSaveSpin = new QSpinBox(settingsDialog);
    autoSaveSpin->setRange(10, 100);
    autoSaveSpin->setSingleStep(10);
#endif

    QSpinBox *editorFontSizeSpin = new QSpinBox(settingsDialog);
    editorFontSizeSpin->setRange(7, 36);
    editorFontSizeSpin->setSingleStep(1);

    QSpinBox *renderFontSizeSpin = new QSpinBox(settingsDialog);
    renderFontSizeSpin->setRange(12, 96);
    renderFontSizeSpin->setSingleStep(1);

    QComboBox *lineEndingCombo = new QComboBox(settingsDialog);
    QCheckBox *enableLineWrappingCheck = new QCheckBox(settingsDialog);

    lineEndingCombo->addItem(tr("UNIX / MacOS (LF)"), "\n");
    lineEndingCombo->addItem(tr("Windows (CRLF)"), "\r\n");

    QCheckBox *funFactsCheck = new QCheckBox(settingsDialog); 

#ifndef Q_OS_WASM
    formLayout->addRow("Seconds between autosave:", autoSaveSpin);
#endif
    formLayout->addRow("Font Size for Editor:", editorFontSizeSpin);
    formLayout->addRow("Relative Font Size for Render\n(based on Body Text)", renderFontSizeSpin);
    formLayout->addRow("End of Line Character:", lineEndingCombo);
    formLayout->addRow("Enable Line Wrapping in Editor:", enableLineWrappingCheck);
    formLayout->addRow("Show fun facts after application initialization:", funFactsCheck);

#ifndef Q_OS_WASM
    autoSaveSpin->setValue(settings->value("editor/autoSaveInterval", 30).toInt());
#endif
    editorFontSizeSpin->setValue(settings->value("editor/fontSize", 10).toInt());
    renderFontSizeSpin->setValue(settings->value("render/fontSize", 12).toInt());
    enableLineWrappingCheck->setChecked(settings->value("editor/enableLineWrapping", true).toBool());
    funFactsCheck->setChecked(settings->value("other/showFunFacts", true).toBool());

    QString savedEnding = settings->value("editor/lineEndings", "\n").toString();
    int endingIndex = lineEndingCombo->findData(savedEnding);
    if (endingIndex != -1) {
        lineEndingCombo->setCurrentIndex(endingIndex);
    }

#ifndef Q_OS_WASM
    connect(autoSaveSpin, &QSpinBox::valueChanged, [this](int value) {
        settings->setValue("editor/autoSaveInterval", value);
        settings->sync();
        autoSaveInterval = value;
        autoSaveHandler();
    });
#endif

    connect(editorFontSizeSpin, &QSpinBox::valueChanged, [this](int value) {
        settings->setValue("editor/fontSize", value);
        settings->sync();

        MainWindow::updateFontPointSize(value, -1);
    });

    connect(renderFontSizeSpin, &QSpinBox::valueChanged, [this](int value) {
        settings->setValue("render/fontSize", value);
        settings->sync();

        MainWindow::updateFontPointSize(-1, value);
    });

    connect(lineEndingCombo, &QComboBox::currentIndexChanged, [this, lineEndingCombo](int index) {
        QVariant data = lineEndingCombo->itemData(index);
        settings->setValue("editor/lineEndings", data);
        settings->sync();

        lineEndingChar = (index == 0) ? "\n" : "\r\n";
    });

    connect(enableLineWrappingCheck, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
        settings->setValue("editor/enableLineWrapping", state == Qt::Checked);
        settings->sync();
        useLineWrapping = (state == Qt::Checked);
        this->editor->setLineWrapMode((useLineWrapping == true) ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    });

    connect(funFactsCheck, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
        settings->setValue("other/showFunFacts", state == Qt::Checked);
        settings->sync();
        showFunFacts = (state == Qt::Checked);
    });

    settingsDialog->show();
}

void MainWindow::formatHandler(const QString &format)
{
    QTextCursor cursor = editor->textCursor();

    if (format == "bold" || format == "italic" || format == "strikethrough" || format == "inline code") {
        if (cursor.hasSelection()) {
            QString selectedText = cursor.selectedText();
            QString mdFragment = (format == "bold") ? "**" : (format == "italic" ? "*" : (format == "strikethrough" ? "~~" : "`"));
            cursor.insertText(mdFragment + selectedText + mdFragment);
        } else {
            cursor.insertText((format == "bold") ? "****" : (format == "italic" ? "**" : (format == "strikethrough" ? "~~~~" : "``")));
            cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, (format == "bold" || format == "strikethrough") ? 2 : 1);
            editor->setTextCursor(cursor);
        }
    } else if (format.startsWith('h') && format.length() == 2) {
        int headerNum = format[1].digitValue();

        if (cursor.hasSelection()) {
            QString selectedText = cursor.selectedText();
            QString mdFragment = QString("#").repeated(headerNum);
            cursor.insertText(mdFragment + " " + selectedText);
        } else {
            cursor.insertText("\n" + QString("#").repeated(headerNum) + " ");
            editor->setTextCursor(cursor);
        }
    } else if (format == "link") {
        if (cursor.hasSelection()) {
            QString text = cursor.selectedText();

            if (text.contains(".") && !text.contains(" ")) cursor.insertText("[](" + text + ")");
            else cursor.insertText("[" + text + "]()");
        } else {
            cursor.insertText("[]()");
            cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
            editor->setTextCursor(cursor);
        }
    } else if (format == "image") {
       if (cursor.hasSelection()) {
            QString text = cursor.selectedText();

            qDebug() << QUrl(text, QUrl::StrictMode).isValid();

            if (text.contains(".") && !text.contains(" ")) cursor.insertText("![](" + text + ")");
            else cursor.insertText("![" + text + "]()");
        } else {
            cursor.insertText("![]()");
            cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
            editor->setTextCursor(cursor);
        }
    } else if (format == "code") {
        if (cursor.hasSelection()) {
            QString selectedText = cursor.selectedText();
            cursor.insertText("```\n" + selectedText + "\n```");
        } else {
            cursor.insertText("\n```\n\n```");
            cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 1);
            editor->setTextCursor(cursor);
        }
    } else if (format == "horizontal divider") {
        cursor.insertText("\n---\n");
    } else if (format == "blockquote") {
        if (cursor.hasSelection()) {
            QString selectedText = cursor.selectedText();
            cursor.insertText("> " + selectedText);
        } else {
            cursor.insertText("\n> ");
            editor->setTextCursor(cursor);
        }
    }
}

void MainWindow::showFunFact()
{
    QFile file = QFile(":/funfacts.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QStringList facts = QString(file.readAll()).split('\n', Qt::SkipEmptyParts);
        file.close();

        if (facts.isEmpty())
            return;

        int randomIdx = QRandomGenerator::global()->bounded(facts.size());

        if (lastFunFactIdx != -1 && randomIdx == lastFunFactIdx) {
            while (randomIdx == lastFunFactIdx) {
               randomIdx = QRandomGenerator::global()->bounded(facts.size()); 
            }
        }

        lastFunFactIdx = randomIdx;

        QString funFact = facts.at(randomIdx).trimmed();

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Fun Fact!");
        msgBox.setText(funFact);
        msgBox.setStandardButtons(QMessageBox::Close);
        QPushButton *continueBtn = msgBox.addButton("Show me another!", QMessageBox::ActionRole);

        QObject::connect(continueBtn, &QPushButton::clicked, this, [&msgBox, this]() {
            msgBox.accept();
            
            QTimer::singleShot(0, this, [this]() {
                this->showFunFact();
            });
        });

        msgBox.setStyleSheet("QDialogButtonBox { qproperty-centerButtons: true; }");

        msgBox.exec();
    } else {
        return;
    }
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
> Select text and apply a formatting shortcut to apply the styling found in the Format menu directly to the selected text. If no text is selected, the Markdown syntax for the format will be applied and your cursor will be moved so that you can instantly continue typing.

| Action | Shortcut | Description |
| :---: | :---: | :---: |
| Open File | `Ctrl / Cmd + O` | Open an existing document |
| Save As | `Ctrl / Cmd + Shift + S` | Save the current document under a new name or location |
| Save | `Ctrl / Cmd + S` | Save changes to the current document |
| Print | `Ctrl + P` | Print the current document |
| Bold | `Ctrl+B` | Insert bold text |
| Italic | `Ctrl+I` | Insert italicized text |
| Heading 1 | `Ctrl+Alt+1` | Insert a level one heading |
| Heading 2 | `Ctrl+Alt+2` | Insert a level two heading |
| Heading 3 | `Ctrl+Alt+3` | Insert a level three heading |
| Horizontal Divider | `Ctrl+D` | Insert horizontal divider |
| Block Quote | `Ctrl+]` | Insert block quote |
| Strikethrough | `Alt+Shift+5` | Insert strikethrough text |
| Link | `Ctrl+K` | Insert link |
| Image | `Ctrl+Shift+I` | Insert image |
| Code | `Ctrl+\` | Insert code block |
| Inline Code | `Ctrl+E` | Insert inline code |

)");
    QMessageBox msgBox(QMessageBox::Information, "Keyboard Combinations", "", QMessageBox::Ok, this);

    msgBox.setText(text);
    msgBox.setTextFormat(Qt::MarkdownText);

    msgBox.exec();
}

void MainWindow::openLink(const QUrl &link)
{
    bool success = QDesktopServices::openUrl(link);

    if (!success) {
        QString text = tr("Failed to open link: %1").arg(link.toString());
        MainWindow::alertUser(this, QMessageBox::Critical, "Error", text);
    } else {
        this->statusBar->clearMessage();
        this->statusBar->showMessage(QString("Opening: %1").arg(link.toString()), 1500);
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

void MainWindow::alertUser(QWidget *parent, QMessageBox::Icon boxType, const QString &title, const QString &text)
{
    QMessageBox msgBox(boxType, title, text, QMessageBox::Ok, parent);
    msgBox.exec();
}

bool MainWindow::isTextFile(const QString &filePath)
{
    if (filePath.endsWith(".md", Qt::CaseInsensitive) ||
        filePath.endsWith(".markdown", Qt::CaseInsensitive) ||
        filePath.endsWith(".txt", Qt::CaseInsensitive) ||
        filePath.endsWith(".html", Qt::CaseInsensitive) ||
        filePath.endsWith(".htm", Qt::CaseInsensitive)) {
        return true;
    }

    QMimeDatabase db;

    QMimeType mime = db.mimeTypeForFile(filePath);

    if (mime.inherits("text/plain")) {
        return true;
    }

    alertUser(this, QMessageBox::Critical, "Error", "Provided file is not of plaintext MIME type.");
    return false;
}

void MainWindow::handleRenderVisibility(const QString &filePath)
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

QString MainWindow::replaceLineEndings(const QString &content)
{
    QString result = content;

    if (lineEndingChar == "\r\n") {
        result.replace(QRegularExpression("\r\n|\r"), "\n");
    } else
        result.replace("\r\n", "\n");

    return result;
}

#ifndef Q_OS_WASM
void MainWindow::autoSaveHandler()
{
    autoSaveTimer->stop();

    // Disconnect previous functions if called many times
    disconnect(autoSaveTimer, &QTimer::timeout, nullptr, nullptr);

    connect(autoSaveTimer, &QTimer::timeout, this, [this]() {
        if (autoSaveInterval == -1) return;
        if (!currentFilePath.isEmpty()) MainWindow::handleSaveFile();
        else {
            this->statusBar->clearMessage();
            this->statusBar->showMessage(QString("Save file to enable autosave!"), 3000);
        }
    });

    if (autoSaveInterval > 0)
        autoSaveTimer->start(autoSaveInterval * 1000);
}
#endif

void MainWindow::handleOpenFile()
{
#ifdef Q_OS_WASM
    QFileDialog::getOpenFileContent("All files (*)", [this](const QString &fileName, const QByteArray &fileContent) {
        if (fileName.isEmpty())
            return;

        if (!MainWindow::isTextFile(fileName)) {
            return;
        }

        QString fileContents = QString::fromUtf8(fileContent);

        MainWindow::handleRenderVisibility(fileName);
        editor->setPlainText(fileContents);
        MainWindow::handleEditorInput();

        this->statusBar->clearMessage();
        this->statusBar->showMessage(QString("Successfully Opened %1").arg(fileName), 3000);

        unsavedFileChanges = false;
        currentFilePath = fileName;
        MainWindow::updateWindowTitle();
    });
#else
    QString fileOpenPath = QFileDialog::getOpenFileName(this,
        tr("Open File"), // Window title
        !currentFilePath.isEmpty() ? currentFilePath : "document",
        tr("All Files (*)"));

    if (fileOpenPath.isEmpty())
        return;

    if (!MainWindow::isTextFile(fileOpenPath)) {
        return;
    }

    QFile file(fileOpenPath); 
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        alertUser(this, QMessageBox::Critical, "Error", "Failed to open file. File is read only or not of text type.");
        return;
    }

    QTextStream in(&file);
    QString fileContents = in.readAll(); 

    MainWindow::handleRenderVisibility(fileOpenPath);
    editor->setPlainText(fileContents);
    MainWindow::handleEditorInput();

    this->statusBar->clearMessage();
    this->statusBar->showMessage(QString("Successfully Opened %1").arg(fileOpenPath), 3000); 

    unsavedFileChanges = false;
    currentFilePath = fileOpenPath;
    MainWindow::updateWindowTitle();
#endif
}

void MainWindow::handleSaveAsFile()
{
    // This function should not be called from WASM

    QString fileSavePath = QFileDialog::getSaveFileName(this,
        tr("Save File"),
        !currentFilePath.isEmpty() ? currentFilePath : "document.md",
        tr("All Files (*)")
    );

    if (fileSavePath.isEmpty())
        return;

    QFile file(fileSavePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        alertUser(this, QMessageBox::Critical, "Error", "Failed to save the file. Please check if you have permissions for this location or if the disk is full.");
        return;
    }

    this->statusBar->clearMessage();
    this->statusBar->showMessage(QString("Successfully Saved as %1").arg(fileSavePath), 3000);

    unsavedFileChanges = false;
    currentFilePath = fileSavePath;
    MainWindow::updateWindowTitle();

    QTextStream out(&file);
    out << MainWindow::replaceLineEndings(editor->toPlainText());
}

void MainWindow::handleSaveFile()
{
#ifdef Q_OS_WASM
    QByteArray fileData = editor->toPlainText().toUtf8();
    QString hint = currentFilePath.isEmpty() ? "document.md" : currentFilePath;

    QFileDialog::saveFileContent(MainWindow::replaceLineEndings(fileData).toUtf8(), hint);

    this->statusBar->clearMessage();
    this->statusBar->showMessage(QString("Successfully Saved as %1").arg(hint), 3000);

    unsavedFileChanges = false;
    currentFilePath = hint;
    MainWindow::updateWindowTitle();
#else
    if (currentFilePath.isEmpty()) {
        MainWindow::handleSaveAsFile();
        return;
    }

    QFile file(currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        alertUser(this, QMessageBox::Critical, "Error", "Failed to save the file. Please check if you have permissions for this location or if the disk is full.");
        return;
    }

    this->statusBar->clearMessage();
    this->statusBar->showMessage(QString("Saved %1").arg(currentFilePath), 1000);

    unsavedFileChanges = false;
    MainWindow::updateWindowTitle();

    QTextStream out(&file);
    out << MainWindow::replaceLineEndings(editor->toPlainText());
#endif
}

void MainWindow::printDocument()
{
#ifdef Q_OS_WASM
    emscripten_run_script("window.print();");
#else
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        if (renderable) {
            mdView->print(&printer);
        } else {
            editor->print(&printer);
        }
    }
#endif
}

void MainWindow::exportHtml()
{
    QTextDocument *doc = mdView->document();

    QString htmlSavePath = QFileDialog::getSaveFileName(this,
        tr("Save HTML File"),
        "document.html",
        tr("HTML files (*.html);;All Files (*)")
    );

    if (htmlSavePath.isEmpty())
        return;

    QFile file(htmlSavePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        alertUser(this, QMessageBox::Critical, "Error", "Failed to open HTML file for writing. Please check write permissions for the destination file.");
        return;
    }

    QTextDocumentWriter writer(htmlSavePath);
    if (!writer.write(doc)) {
        alertUser(this, QMessageBox::Critical, "Error", "Failed to write the document to HTML. Please check write permissions for the destination file.");
    }
}
