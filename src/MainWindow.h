#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QTextBrowser>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QImage>

class QVBoxLayout;
class QHBoxLayout;
class QSplitter;
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
class QSettings;
class QTimer;

class MdezDocBrowser : public QTextBrowser {
    Q_OBJECT
public:
    MdezDocBrowser(QWidget *parent = nullptr) : QTextBrowser(parent) {
        setOpenExternalLinks(true);
        manager = new QNetworkAccessManager(this);
    }

    QVariant loadResource(int type, const QUrl &name) override {
        if (type == QTextDocument::ImageResource && (name.scheme() == "http" || name.scheme() == "https")) {
            QVariant cachedResource = QTextBrowser::loadResource(type, name);

            if (cachedResource.isValid()) {
                return cachedResource;
            }

            if (!activeDownloads.contains(name)) {
                activeDownloads.insert(name, true);

                QNetworkRequest request(name);
                QNetworkReply *reply = manager->get(request);

                connect(reply, &QNetworkReply::finished, this, [this, reply, name]() {
                    activeDownloads.remove(name);

                    if (reply->error() == QNetworkReply::NoError) {
                        QByteArray data = reply->readAll();
                        QImage img = QImage::fromData(data);
                        if (!img.isNull()) {
                            int targetWidth = 500;

                            if (img.width() > targetWidth)
                                img = img.scaledToWidth(targetWidth, Qt::SmoothTransformation);

                            document()->addResource(QTextDocument::ImageResource, name, img);

                            viewport()->update();
                            document()->markContentsDirty(0, document()->characterCount());

                        }
                    }
                    reply->deleteLater();
                });
            }
            return QVariant();
        }
        return QTextBrowser::loadResource(type, name);
    }

private:
    QNetworkAccessManager *manager;
    QMap<QUrl, bool> activeDownloads;
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void handleEditorInput();
    void updateWindowTitle();
    void initSettingVars();
    void showSettingsMenu();
    void showFunFact();
    void showAboutDialog();
    void showFullMitLicense();
    void openLink(const QUrl &link);
    void openKeyboardCombos();
    int countWords();
    void updatePosLabel();
    void alertUser(QWidget* parent, QMessageBox::Icon boxType, const QString &title, const QString &text);
    bool isTextFile(const QString &filePath);
    void handleRenderVisibility(const QString &filePath);
    QString replaceLineEndings(const QString &content);
    void autoSaveHandler();
    void handleOpenFile();
    void handleSaveAsFile();
    void handleSaveFile();
    void printDocument();
    void exportHtml();

private:
    // Pointers for widgets managed by window
    QVBoxLayout *mainLayout;
    QLabel *header;
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *settingsAction;
    QLabel *renderingLabel;
    QTextEdit *editor;
    MdezDocBrowser *mdView;
    QSplitter *mdHorizontal;

    QSettings *settings;
    QTimer *autoSaveTimer;

    QAction *openFileAction;
    QAction *saveAsFileAction;
    QAction *saveFileAction;
    QAction *printAction;
    QAction *exportHtmlAction;

    QAction *aboutAction;
    QAction *mitLicenseAction;
    QAction *aboutQtAction;
    QAction *funFactAction;
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
    bool isHtml;
    bool renderable;
    QString currentFilePath;
    int lastFunFactIdx = -1;
    QString lineEndingChar;
    int autoSaveInterval;
    bool useLineWrapping;
    bool showFunFacts;
};

#endif
