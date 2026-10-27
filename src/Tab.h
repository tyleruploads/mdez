#ifndef TAB_H
#define TAB_H

#include <QWidget>

class MainWindow;
class QLabel;
class QSplitter;
class QTextEdit;
class QVBoxLayout;
class MdezDocBrowser;

class Tab : public QWidget {
    Q_OBJECT

public:
  explicit Tab(MainWindow* mainWindow, QWidget *parent = nullptr);
  ~Tab() override = default;

  void handleEditorInput();
  void handleRenderVisibility(const QString &filePath);
  void updateRenderFontSize(const int &size);

  QTextEdit* getEditor() const { return editor; }
  MdezDocBrowser* getMdView() const { return mdView; }
  QString getCurrentFilePath() const { return currentFilePath; }
  void setCurrentFilePath(const QString &filePath) { currentFilePath = filePath; }

private slots: 
  int countWords();
  void updatePosLabel(); 

private:
  MainWindow *m_mainWindow;

  QVBoxLayout *tabLayout;
  QLabel *renderingLabel;
  QSplitter *mdHorizontal;
  QTextEdit *editor;
  MdezDocBrowser *mdView;
 
  QString currentFilePath;
  bool unsavedFileChanges;
  bool isHtml;
  bool renderable;
  int renderSize = 12;
};

#endif
