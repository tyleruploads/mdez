#include "MainWindow.h"
#include <QObject>
#include <QLabel>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);

    header = new QLabel("MDEZ++", this);
    
    QFont headerFont("Arial", 16, QFont::Bold);
    header->setFont(headerFont);
    header->setAlignment(Qt::AlignmentFlag::AlignCenter);

    mdHorizontal = new QHBoxLayout();

    editor = new QTextEdit(this);
    editor->setPlaceholderText("Type here");

    mdView = new QTextBrowser(this);

    versionLabel = new QLabel("v" APP_VERSION, this);

    connect(editor, &QTextEdit::textChanged, this, [this]() {
        mdView->setMarkdown(editor->toPlainText());
    });

    mdHorizontal->addWidget(editor);
    mdHorizontal->addWidget(mdView);
 
    mainLayout->addWidget(header);
    mainLayout->addLayout(mdHorizontal);
    mainLayout->addWidget(versionLabel);

}    
