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
class QStatusBar;

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
    int countWords();
    void updatePosLabel();
    void handleOpenFile();
    void handleSaveAsFile();
    void handleSaveFile();

private:
    // Pointers for widgets managed by window
    QVBoxLayout *mainLayout;
    QLabel *header;
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

    QStatusBar *statusBar;
    QLabel *versionLabel;
    QLabel *wcLabel;
    QLabel *posLabel;
    QLabel *pathLabel;
    QWidget *statusSpacer1;
    QWidget *statusSpacer2;
    QWidget *statusSpacer3;


    bool unsavedFileChanges;
    QString currentFilePath;
};

#endif
