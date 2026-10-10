#include "MainWindow.h"
#include <QObject>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
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

    openFileAction->setText(tr("Open File"));
    saveAsFileAction->setText(tr("Save As"));
    saveFileAction->setText(tr("Save"));

    openFileAction->setStatusTip(tr("Open an existing document"));
    saveAsFileAction->setStatusTip(tr("Save the current document under a new name or location"));
    saveFileAction->setStatusTip(tr("Save changes to the current document"));

    openFileAction->setShortcut(QKeySequence::Open);
    saveAsFileAction->setShortcut(QKeySequence::SaveAs);
    saveFileAction->setShortcut(QKeySequence::Save);
   
    openFileAction->setShortcutContext(Qt::ApplicationShortcut);
    saveAsFileAction->setShortcutContext(Qt::ApplicationShortcut);
    saveFileAction->setShortcutContext(Qt::ApplicationShortcut);

    this->addAction(openFileAction);
    this->addAction(saveAsFileAction);
    this->addAction(saveFileAction);

    mdHorizontal = new QHBoxLayout();

    editor = new QTextEdit(this);
    editor->setPlaceholderText("Type here");

    mdView = new QTextBrowser(this);

    versionLabel = new QLabel("v" APP_VERSION, this);

    connect(editor, &QTextEdit::textChanged, this, &MainWindow::updateMarkdown);

    connect(openFileAction, &QAction::triggered, this, &MainWindow::handleOpenFile);
    connect(saveAsFileAction, &QAction::triggered, this, &MainWindow::handleSaveAsFile);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::handleSaveFile);

    fileMenu->addActions({openFileAction, saveAsFileAction, saveFileAction});
    menuBar->addMenu(fileMenu);

    mdHorizontal->addWidget(editor);
    mdHorizontal->addWidget(mdView);
 
    mainLayout->addWidget(header);
    mainLayout->addWidget(menuBar);
    mainLayout->addLayout(mdHorizontal);
    mainLayout->addWidget(versionLabel);

}

void MainWindow::updateMarkdown()
{
   mdView->setMarkdown(editor->toPlainText());
   unsavedFileChanges = true;
   MainWindow::updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
    QString displayPath = currentFilePath.isEmpty() ? "Untitled" : currentFilePath;

    if (unsavedFileChanges == true)
        displayPath += "*";

    setWindowTitle(QString("%1 | MDEZ++").arg(displayPath));
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
