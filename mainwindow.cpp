#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QScrollArea>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QImageReader>
#include <QImageWriter>
#include <QStandardPaths>
#include <QByteArray>
#include <QList>
#include <QClipboard>
#include <QMimeData>
#include <QScrollBar>

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)


// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)
//////
/// \brief YuyvToRgb32 YUV420 to RGBA 32
/// \param pYuv
/// \param width
/// \param stride
/// \param height
/// \param pRgb     output RGB32 buffer
/// \param uFirst   true if pYuv is YUYV, false if YVYU
///
static void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst)
{
    //YVYU - format
    int nBps = width*4;
    unsigned char* pY1 = pYuv;

    unsigned char* pV;
    unsigned char* pU;

    if (uFirst) {
        pU = pY1+1; pV = pU+2;
    } else {
        pV = pY1+1; pU = pV+2;
    }


    unsigned char* pLine1 = pRgb;

    unsigned char y1,u,v;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j+=2)
        {
            y1 = pY1[2*j];
            u = pU[2*j];
            v = pV[2*j];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y1 = pY1[2*j+2];
            pLine1[j*4+4] = YUV2B(y1, u, v);//b
            pLine1[j*4+5] = YUV2G(y1, u, v);//g
            pLine1[j*4+6] = YUV2R(y1, u, v);//r
            pLine1[j*4+7] = 0xff;
        }
        pY1 += stride;
        pV += stride;
        pU += stride;
        pLine1 += nBps;

    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow),
    mImageLabel(new QLabel),
    mScrollArea(new QScrollArea),
    mZoomFactor(1)
{
    ui->setupUi(this);
    mImageLabel->setBackgroundRole(QPalette::Base);
    mImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mImageLabel->setScaledContents(true);

    mScrollArea->setBackgroundRole(QPalette::Dark);
    mScrollArea->setWidget(mImageLabel);
    mScrollArea->setVisible(false);
    setCentralWidget(mScrollArea);

    createMenuAndToolbar();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

}

MainWindow::~MainWindow()
{
    delete ui;
}
bool MainWindow::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoDetectImageFormat(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(mImage.width()).arg(mImage.height()).arg(mImage.depth());
    statusBar()->showMessage(message);
    return true;
}
bool MainWindow::loadFileYuv(const QString & filename, bool isPlanMode)
{

    QFile fp(filename);
     if(!fp.open(QIODevice::ReadOnly))
             return false;
     //check filename and dimension

    char value[32];
    const char* pname = filename.toUtf8().data();
    const char* p1 = strchr(pname, '_');
    const char* p2 = strchr(pname, '.');
    int length = p2-p1-1;
    if (p1 == NULL || p2 == NULL || length <3 || length>= (int) sizeof(value)-1){
        fp.close();

        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("File name must be in the format of abc_widthxheight.yuv!"));
        return false;
    }

    memcpy(value, p1+1, length);
    value[length] = 0;
    p1 = strchr(value, 'x');
    if (!p1) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("File name must be in the form of abc_widthxheight.yuv!"));
        return false;
    }
    *(char*) p1 = 0;

    int width = atoi(value);
    int height = atoi(p1+1);

     unsigned char* m_pRgb32 = (unsigned char*) malloc(width*4*height);
     if (!m_pRgb32) return false;
     void* pYuv = malloc(width*2*height);
     if (!pYuv) {
         free(m_pRgb32);
         return false;
     }
     QImage* newImage = NULL;
     if ( fp.read((char* )pYuv, width*2*height) >0){
         YuyvToRgb32((unsigned char*)pYuv, width, width*2, height,
                                 m_pRgb32, true);
         free(pYuv);
         newImage = new QImage(m_pRgb32,
                     width, height, QImage::Format_RGBA8888);

     }
     fp.close();
     if(newImage) {
         setImage(*newImage);
         setWindowFilePath(filename);
         const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
             .arg(QDir::toNativeSeparators(filename)).arg(mImage.width()).arg(mImage.height()).arg(mImage.depth());
         statusBar()->showMessage(message);

     }
     else {
              QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                       tr("Failed to read file!"));
          }
     return (newImage!=NULL);
}

void MainWindow::setImage(const QImage &newImage)
{
    mImage = newImage;
    mImageLabel->setPixmap(QPixmap::fromImage(mImage));
    mZoomFactor = 1.0;

    mScrollArea->setVisible(true);
    mFitToWindowAct->setEnabled(true);
    updateActions();

    if (!mFitToWindowAct->isChecked())
        mImageLabel->adjustSize();
}
bool MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(mImage)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }
    QStringList filters;
    filters << "YUYV packet mode (*.yuv)"
            << "Image files (*.png *.xpm *.jpg)";
    dialog.setNameFilters(filters);;
   if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}
void MainWindow::onFileOpen()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Imahe"), QDir::currentPath(),
          tr("YUYV packet mode (*.yuv);;"
              "Image files (*.png *.xpm *.jpg)") );

    if (!filename.isNull()){
        if(filename.contains(".yuv"))
            loadFileYuv(filename);
        else if(filename.contains(".yuyv"))
            loadFileYuv(filename, false);
        else
            loadFile(filename);
    }

}

void MainWindow::onFileSaveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}

}
void MainWindow::onEditCopy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(mImage);
#endif // !QT_NO_CLIPBOARD
}
#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void MainWindow::onEditPaste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        ui->statusBar->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        ui->statusBar->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void MainWindow::onViewZoomin()
{
    scaleImage(1.25);
}

void MainWindow::onViewZoomout()
{
    scaleImage(0.8);
}

void MainWindow::onViewNormalSize()
{
    mImageLabel->adjustSize();
    mZoomFactor = 1.0;
}

void MainWindow::onViewFitToWindow()
{
    bool fitToWindow = mFitToWindowAct->isChecked();
    mScrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        onViewNormalSize();
    updateActions();
}

void MainWindow::onHelpAbout()
{
    QMessageBox::about(this, tr("About Application"),
             tr("The <b>Application</b>  Copyrights 2018."));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
void MainWindow::createMenuAndToolbar()
{
    QToolBar *fileToolBar = ui->mainToolBar;
    QMenu* fileMenu = ui->menuBar->addMenu(tr("&File"));
    const QIcon openIcon = QIcon(":/images/open.png");
    QAction *openAct = fileMenu->addAction(openIcon, tr("&Open..."), this,
                    SLOT(onFileOpen()), QKeySequence::Open);
    openAct->setStatusTip(tr("Open an image file."));
    const QIcon saveIcon = QIcon(":/images/save.png");
    mSaveAsAct = fileMenu->addAction(saveIcon, tr("&Save..."), this,
                                     SLOT(onFileSaveAs()));
    mSaveAsAct->setStatusTip(tr("Save file."));
    mSaveAsAct->setEnabled(false);
    fileMenu->addSeparator();
    const QIcon closeIcon =  QIcon(":/images/exit.png");
    QAction* closeAct = fileMenu->addAction(closeIcon,tr("E&xit"), this,
            SLOT(close()), QKeySequence::Quit);
    closeAct->setStatusTip(tr("Exit this program."));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(mSaveAsAct);
    fileMenu->addSeparator();
    /*******************************************/
    QMenu *editMenu = ui->menuBar->addMenu(tr("&Edit"));
    const QIcon copyIcon =  QIcon(":/images/copy.png");
    mCopyAct = editMenu->addAction(copyIcon, tr("&Copy"), this,
                         SLOT(onEditCopy()),QKeySequence::Copy);
    mCopyAct->setEnabled(false);
    const QIcon pasteIcon =  QIcon(":/images/paste.png");
    QAction *pasteAct = editMenu->addAction(pasteIcon, tr("&Paste"), this,
                         SLOT(onEditPaste()), QKeySequence::Paste);
    pasteAct->setStatusTip(tr("paste from clipboard."));
    /*******************************************/
    QMenu* viewMenu = ui->menuBar->addMenu(tr("&View"));
    const QIcon zoominIcon =  QIcon(":/images/zoomin.png");
    mZoomInAct = viewMenu->addAction(zoominIcon,tr("Zoom &In"), this,
                                    SLOT(onViewZoomin()), QKeySequence::ZoomIn );
    mZoomInAct->setEnabled(false);
    mZoomInAct->setStatusTip(tr("Magfying image."));

    const QIcon zoomoutIcon = QIcon(":/images/zoomout.png");
    mZoomOutAct = viewMenu->addAction(zoomoutIcon,tr("Zoom &Out"), this,
                     SLOT(onViewZoomout()), QKeySequence::ZoomOut );
    mZoomOutAct->setStatusTip(tr("zoom out image."));
    mZoomOutAct->setEnabled(false);
    const QIcon normalSizeIcon = QIcon(":/images/normalsize.png");
    mNormalSizeAct = viewMenu->addAction(normalSizeIcon, tr("Normal &Size"), this,
                     SLOT(onViewNormalSize()), tr("Ctrl+S") );
    viewMenu->addSeparator();
    mNormalSizeAct->setStatusTip(tr("set normal size image."));
    mNormalSizeAct->setEnabled(false);
     const QIcon fitIcon = QIcon(":/images/fittowindow.png");
    mFitToWindowAct = viewMenu->addAction(fitIcon, tr("Fit to Window"), this,
                     SLOT(onViewFitToWindow()), tr("Ctrl+S") );
    mFitToWindowAct->setStatusTip(tr("set normal size image."));
    mFitToWindowAct->setEnabled(false);
    mFitToWindowAct->setCheckable(true);
    mFitToWindowAct->setShortcut(tr("Ctrl+F"));

    fileToolBar->addAction(mZoomInAct);
    fileToolBar->addAction(mZoomOutAct);
    fileToolBar->addAction(mNormalSizeAct);
    fileToolBar->addAction(mFitToWindowAct);
    fileToolBar->addSeparator() ;

    QMenu* helpMenu = ui->menuBar->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon(":/images/about.png");
    QAction* aboutAct = helpMenu->addAction(aboutIcon, tr("A&bout"), this, SLOT(onHelpAbout()));
    aboutAct->setStatusTip(tr("Copyrights."));
    fileToolBar->addAction(aboutAct);


}
void MainWindow::updateActions()
{
    mSaveAsAct->setEnabled(!mImage.isNull());
    mCopyAct->setEnabled(!mImage.isNull());
    mZoomInAct->setEnabled(!mFitToWindowAct->isChecked());
    mZoomOutAct->setEnabled(!mFitToWindowAct->isChecked());
    mNormalSizeAct->setEnabled(!mFitToWindowAct->isChecked());
}
void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(mImageLabel->pixmap());
    mZoomFactor *= factor;
    mImageLabel->resize(mZoomFactor * mImageLabel->pixmap()->size());

    adjustScrollBar(mScrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(mScrollArea->verticalScrollBar(), factor);

    mZoomInAct->setEnabled(mZoomFactor < 10.0);
    mZoomOutAct->setEnabled(mZoomFactor > 0.1);
}
void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

