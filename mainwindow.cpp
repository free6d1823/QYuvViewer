#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createMenuAndToolbar();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
void MainWindow::createMenuAndToolbar()
{
    QMenu* fileMenu = ui->menuBar->addMenu(tr("&File"));
    QToolBar *fileToolBar = ui->mainToolBar;
    const QIcon openIcon = QIcon(":/images/open.png");
    QAction* openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setStatusTip(tr("Open file."));
    connect(openAct, SIGNAL(triggered()), SLOT(onFileOpen()));
    const QIcon saveIcon = QIcon(":/images/save.png");
    QAction* saveAct = new QAction(saveIcon, tr("&Save..."), this);
    openAct->setStatusTip(tr("Save file."));
    connect(saveAct, SIGNAL(triggered()), SLOT(onFileSave()));
    const QIcon closeIcon =  QIcon(":/images/exit.png");
    QAction* closeAct = new QAction(closeIcon, tr("E&xit"), this);
    closeAct->setStatusTip(tr("Exit this program."));
    connect(closeAct, SIGNAL(triggered()), SLOT(close()));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileMenu->addSeparator();

    QMenu* viewMenu = ui->menuBar->addMenu(tr("&View"));
    const QIcon zoominIcon =  QIcon(":/images/zoomin.png");
    QAction* zoominAct = new QAction(zoominIcon, tr("Zoom &In"), this);
    zoominAct->setStatusTip(tr("Magfying image."));
    connect(zoominAct, SIGNAL(triggered()), SLOT(onViewZoomin()));

    const QIcon zoomoutIcon = QIcon(":/images/zoomout.png");
    QAction* zoomoutAct = new QAction(zoomoutIcon, tr("Zoom &Out"), this);
    zoomoutAct->setStatusTip(tr("zoom in image."));
    connect(zoomoutAct, SIGNAL(triggered()), SLOT(onViewZoomout()));
    viewMenu->addAction(zoominAct);
    viewMenu->addAction(zoomoutAct);
    fileToolBar->addAction(zoominAct);
    fileToolBar->addAction(zoomoutAct);
    fileToolBar->addSeparator() ;

    QMenu* helpMenu = ui->menuBar->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon(":/images/about.png");
    QAction* aboutAct = new QAction(aboutIcon, tr("A&bout"), this);
    aboutAct->setStatusTip(tr("Copyrights."));
    connect(aboutAct, SIGNAL(triggered()), SLOT(onHelpAbout()));
     helpMenu->addAction(aboutAct);
fileToolBar->addAction(aboutAct);


}

void MainWindow::onFileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {

    }
}
void MainWindow::onFileSave()
{

}

void MainWindow::onViewZoomin()
{

}

void MainWindow::onViewZoomout()
{

}
void MainWindow::onHelpAbout()
{
    QMessageBox::about(this, tr("About Application"),
             tr("The <b>Application</b>  Copyrights 2018."));
}
