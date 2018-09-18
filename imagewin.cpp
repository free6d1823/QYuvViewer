#include <QAbstractScrollArea>
#include <QLabel>
#include "imagewin.h"

ImageWin::ImageWin(QWidget *parent) : QScrollArea(parent),
    mImageLabel(new QLabel)
{
    mImageLabel->setBackgroundRole(QPalette::Base);
    mImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mImageLabel->setScaledContents(true);

    setBackgroundRole(QPalette::Dark);
    setWidget(mImageLabel);
    setVisible(false);

}

ImageWin::~ImageWin()
{

}

void ImageWin::setImage(const QImage &newImage)
{
    mImage = newImage;
    mImageLabel->setPixmap(QPixmap::fromImage(mImage));
    setVisible(true);
}
void ImageWin::adjustSize( )
{
     mImageLabel->adjustSize();
}

void ImageWin::scaleImage(double factor)
{
    mImageLabel->resize(factor * mImageLabel->pixmap()->size());

}
