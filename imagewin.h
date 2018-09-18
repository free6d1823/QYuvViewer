#ifndef IMAGEVWIN_H
#define IMAGEVWIN_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qscrollarea.h>
class QLabel;
class ImageWin : public QScrollArea
{
    Q_OBJECT
//    Q_PROPERTY(bool widgetResizable READ widgetResizable WRITE setWidgetResizable)
//    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)

public:
    explicit ImageWin(QWidget *parent = 0);
    ~ImageWin();
    void setImage(const QImage &newImage);
    void adjustSize( );
    void scaleImage(double factor);
    QImage* getImage(){ return &mImage;}

protected:

private:
    QImage mImage;
    QLabel *mImageLabel;
};

#endif // IMAGEVWIN_H
