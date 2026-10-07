#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QTextEdit;
class QTextBrowser;
class QBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    // Pointers for widgets managed by window
    QVBoxLayout *mainLayout;
    QLabel *label;
    QTextEdit *editor;
    QTextBrowser *mdView;
    QHBoxLayout *mdHorizontal;
};

#endif
