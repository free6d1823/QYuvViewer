#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createMenuAndToolbar();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
     void onFileOpen();
     void onFileSave();
     void onViewZoomin();
     void onViewZoomout();
     void onHelpAbout();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
