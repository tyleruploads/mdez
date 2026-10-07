#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    QWidget window;
    window.resize(800, 800);
    window.setWindowTitle("MDEZ++");

    QVBoxLayout layout(&window);

    QLabel label("MDEZ++", &window);
    QFont headerFont("Arial", 16, QFont::Bold);
    label.setFont(headerFont);
    label.setAlignment(Qt::AlignmentFlag::AlignCenter);

    QHBoxLayout mdHorizontal(&window);

    QTextEdit editor(&window);
    editor.setPlaceholderText("Type here");

    QTextBrowser mdView(&window);

    QObject::connect(&editor, &QTextEdit::textChanged, [&editor, &mdView]() {
        mdView.setMarkdown(editor.toPlainText());
    });

    mdHorizontal.addWidget(&editor);
    mdHorizontal.addWidget(&mdView);
 
    layout.addWidget(&label);
    layout.addLayout(&mdHorizontal);

    window.show();

    return app.exec();
}
