#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Tab.h"
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTabWidget>
#include <QTextBrowser>
#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QSplitter;
class QLabel;
class QTextDocument;
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


class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

  void updateWindowTitle();
  void createTab();
  void closeTab(const int &index);
  Tab* currentTab() const;
  bool isTextFile(const QString &filePath);

  QTabWidget *tabsLayout;

  QLabel *wcLabel;
  QLabel *posLabel;

private slots:  
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
  QString replaceLineEndings(const QString &content);
#ifndef Q_OS_WASM
  void autoSaveHandler();
#endif
  void handleOpenFile();
  void handleSaveAsFile();
  void handleSaveFile(const QString &choice);
  void saveOneFile(Tab &tab);
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

class MdezDocBrowser : public QTextBrowser {
  Q_OBJECT
public:
  MdezDocBrowser(MainWindow *mainWindow, QWidget *parent = nullptr) : QTextBrowser(parent), m_mainWindow(mainWindow) {
    setOpenExternalLinks(true);
 
    manager = new QNetworkAccessManager(this);
  }

  void doSetSource(const QUrl &name, QTextDocument::ResourceType type = QTextDocument::UnknownResource) override {
    bool isAbsoluteFile = (name.isLocalFile() || name.scheme() == "file");
    bool isRelativeFile = name.isRelative() && !name.path().isEmpty();

    if (isAbsoluteFile || isRelativeFile) {
        Tab *previousTab = m_mainWindow->currentTab(); 

        m_mainWindow->createTab();

        Tab *tab = m_mainWindow->currentTab();
        int idx =  m_mainWindow->tabsLayout->count() - 1;

        QTextEdit *editor = tab->getEditor();

        if (!editor) {
            m_mainWindow->closeTab(idx);
            return;
        }

        if (name.isEmpty()) {
            m_mainWindow->closeTab(idx);
            return;
        }

        QString fullPath;

        if (isAbsoluteFile) {
            fullPath = name.toLocalFile();
            if (fullPath.isEmpty()) {
                fullPath = name.path();
            }
        } else if (isRelativeFile) {
            if (!previousTab || previousTab->getCurrentFilePath().isEmpty()) {
                QMessageBox::warning(
                    m_mainWindow,
                    tr("Open File by Link in Render Error"),
                    tr("Warning: File link is located in document without save location and file link is not absolute, and therefore can not be opened. "
                        "Please save document or change path of link to absolute path to open file link.")
                );
                return;
            }

            QDir dir = QFileInfo(previousTab->getCurrentFilePath()).absoluteDir();
            fullPath = dir.absoluteFilePath(name.path());
        }

        QFileInfo fileInfo(fullPath);

        if (!fileInfo.exists() || !fileInfo.isFile()) {
            m_mainWindow->closeTab(idx);

            bool isFlatpakPortal = fullPath.contains("/run/user/") && fullPath.contains("/doc/");
            bool isFlatpak = qEnvironmentVariableIsSet("FLATPAK_ID");

            
            QString errorMessage;
            if (isFlatpak && isFlatpakPortal) {
                errorMessage = tr("\n\nCould not access the file via the Flatpak document portal:\n%1\n\n"
                            "This usually happens because the application lacks permission to access this file or the file selection session expired. "
                            "Please try re-opening this file. "
                            "If that does not work, you can adjust permissions in your System Settings, use a utility like Flatseal, or install a non-Flatpak version of MDEZ++.").arg(fullPath);
            } else {
                errorMessage = tr("The file could not be found on the disk:\n%1\nPlease check if the path or file name is correct.").arg(fullPath);
            }


            QMessageBox::warning(
                m_mainWindow,
                tr("File Access Error"),
                errorMessage);
            return;
        }

        if (!m_mainWindow->isTextFile(fullPath)) {
            m_mainWindow->closeTab(idx);
            return;
        }

        QString cleanFullPath = fileInfo.canonicalFilePath();

        QFile file(cleanFullPath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_mainWindow->closeTab(idx);

            bool isPermissionError = (file.error() == QFile::PermissionsError);
            bool isFlatpak = qEnvironmentVariableIsSet("FLATPAK_ID");

            QString reason = file.errorString();

            if (isFlatpak && isPermissionError) {
                reason += tr("\n\nNote: Because this application is running inside of a Flatpak sandbox, "
                            "it may not have permission to access files outside its designated folders. "
                            "You can adjust permissions in your System Settings, use a utility like Flatseal, or install a non-Flatpak version of MDEZ++.");
            }

            QMessageBox::warning(
                m_mainWindow,
                tr("Open File by Link in Render Error"),
                tr("Could not open the file:\n%1\nReason: %2")
                    .arg(cleanFullPath)
                    .arg(reason)
            );
            return;
        }
        QTextStream in(&file);
        QString fileContents = in.readAll();

        tab->handleRenderVisibility(cleanFullPath);
        editor->setPlainText(fileContents);
        tab->handleEditorInput();

        return;
    }

    QTextBrowser::doSetSource(name, type);
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

        QTextBrowser::connect(reply, &QNetworkReply::finished, static_cast<QTextBrowser*>(this), [this, reply, name]() {
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
  MainWindow *m_mainWindow;
  QNetworkAccessManager *manager;
  QMap<QUrl, bool> activeDownloads;
};


#endif
