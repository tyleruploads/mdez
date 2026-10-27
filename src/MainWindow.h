#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Tab.h"
#include <QEventLoop>
#include <QImage>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextBrowser>
#include <QWidget>

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
class QTabWidget;
class QTimer;

class MdezDocBrowser : public QTextBrowser {
  Q_OBJECT
public:
  MdezDocBrowser(QWidget *parent = nullptr) : QTextBrowser(parent) {
    setOpenExternalLinks(true);
    manager = new QNetworkAccessManager(this);
  }

  QVariant loadResource(int type, const QUrl &name) override {
    if (type == QTextDocument::ImageResource &&
        (name.scheme() == "http" || name.scheme() == "https")) {
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

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

  void updateWindowTitle();

  QLabel *wcLabel;
  QLabel *posLabel;

private slots:
  Tab* currentTab() const;
  void createTab();
  void closeTab(const int &index); 
  void refreshTabTitles();
  void updateFontPointSize(const int &editorSize, const int &renderSize);
  void initSettingVars();
  void showSettingsMenu();
  void formatHandler(const QString &format);
  void showFunFact();
  void showAboutDialog();
  void showFullMitLicense();
  void openLink(const QUrl &link);
  void openKeyboardCombos();  
  void alertUser(QWidget *parent, QMessageBox::Icon boxType,
                 const QString &title, const QString &text);
  bool isTextFile(const QString &filePath); 
  QString replaceLineEndings(const QString &content);
#ifndef Q_OS_WASM
  void autoSaveHandler();
#endif
  void handleOpenFile();
  void handleSaveAsFile();
  void handleSaveFile(const QString &choice);
  void saveOneFile(const Tab &tab);
  void printDocument();
  void exportHtml();

private:
  QVBoxLayout *mainLayout;
  QLabel *header;
  QMenuBar *menuBar;
  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *helpMenu;
  QMenu *formatMenu;
  QAction *settingsAction;

  QTabWidget *tabsLayout;

  QSettings *settings;
  QTimer *autoSaveTimer;

  QAction *openFileAction;
  QAction *saveAsFileAction;
  QAction *saveFileAction;
  QAction *saveAllFilesAction;
  QAction *printAction;
  QAction *exportHtmlAction;

  QAction *undoAction;
  QAction *redoAction;
  QAction *copyAction;
  QAction *pasteAction;
  QAction *cutAction;
  QAction *selectAllAction;

  QAction *aboutAction;
  QAction *mitLicenseAction;
  QAction *aboutQtAction;
  QAction *funFactAction;
  QAction *mdCheatSheetAction;
  QAction *keyboardCombosAction;
  QAction *reportIssueAction;
  QAction *checkUpdatesAction;

  QAction *boldFormat;
  QAction *italicFormat;
  QAction *h1Format;
  QAction *h2Format;
  QAction *h3Format;
  QAction *dividerFormat;
  QAction *blockquoteFormat;
  QAction *strikethroughFormat;
  QAction *linkFormat;
  QAction *imageFormat;
  QAction *codeFormat;
  QAction *inlineCodeFormat;

  QStatusBar *statusBar;
  QLabel *versionLabel; 
  QLabel *pathLabel;
  QWidget *statusSpacer1;
  QWidget *statusSpacer2;
  QWidget *statusSpacer3;

  QTextEdit *m_activeEditor = nullptr;

  bool unsavedFileChanges;
  bool isHtml;
  bool renderable;
  int lastFunFactIdx = -1;
  QString lineEndingChar;
#ifndef Q_OS_WASM
  bool autoSaveEnabled;
  int autoSaveInterval;
#endif
  bool useLineWrapping;
  bool showFunFacts;
};

#endif
