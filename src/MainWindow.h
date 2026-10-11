#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QTextEdit;
class QTextBrowser;
class QBoxLayout;
class QMenuBar;
class QMenu;
class QAction;
class QString;
class QUrl;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void updateMarkdown();
    void updateWindowTitle();
    void showAboutDialog();
    void showFullMitLicense();
    void openLink(const QUrl &link);
    void openKeyboardCombos();
    void handleOpenFile();
    void handleSaveAsFile();
    void handleSaveFile();

private:
    // Pointers for widgets managed by window
    QVBoxLayout *mainLayout;
    QLabel *header;
    QLabel *versionLabel;
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *helpMenu;
    QTextEdit *editor;
    QTextBrowser *mdView;
    QHBoxLayout *mdHorizontal;

    QAction *openFileAction;
    QAction *saveAsFileAction;
    QAction *saveFileAction;

    QAction *aboutAction;
    QAction *mitLicenseAction;
    QAction *aboutQtAction;
    QAction *mdCheatSheetAction;
    QAction *keyboardCombosAction;
    QAction *reportIssueAction;
    QAction *checkUpdatesAction;

    bool unsavedFileChanges;
    QString currentFilePath;
};

#endif
