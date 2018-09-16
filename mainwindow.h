#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFile(const QString &);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
     void onFileOpen();
     void onFileSaveAs();
     void onEditCopy();
     void onEditPaste();
     void onViewZoomin();
     void onViewZoomout();
     void onViewNormalSize();
     void onViewFitToWindow();
     void onHelpAbout();
private:
    void createMenuAndToolbar();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    Ui::MainWindow *ui;

    QImage mImage;
    QLabel *mImageLabel;
    QScrollArea *mScrollArea;
    double mZoomFactor;
    QAction *mSaveAsAct;
    QAction *mCopyAct;
    QAction *mZoomInAct;
    QAction *mZoomOutAct;
    QAction *mNormalSizeAct;
    QAction *mFitToWindowAct;
};

#endif // MAINWINDOW_H
