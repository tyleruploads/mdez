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

    label = new QLabel("MDEZ++", this);
    
    QFont headerFont("Arial", 16, QFont::Bold);
    label->setFont(headerFont);

    label->setAlignment(Qt::AlignmentFlag::AlignCenter);

    mdHorizontal = new QHBoxLayout();

    editor = new QTextEdit(this);
    editor->setPlaceholderText("Type here");

    mdView = new QTextBrowser(this);

    connect(editor, &QTextEdit::textChanged, this, [this]() {
        mdView->setMarkdown(editor->toPlainText());
    });

    mdHorizontal->addWidget(editor);
    mdHorizontal->addWidget(mdView);
 
    mainLayout->addWidget(label);
    mainLayout->addLayout(mdHorizontal);

}    
