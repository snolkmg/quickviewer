#include "imageview.h"
#include "qvapplication.h"

#include <QMouseEvent>
#include <QtDebug>
#include <QPainter>
#include <QGLWidget>
#include <QGraphicsPixmapItem>

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
    , m_renderer(Native)
    , m_maximized(false)
//    , my_gpi(nullptr)
    , m_dualImage(false)
    , m_rightSideBook(false)
    , m_fileVolume(nullptr)
    , m_hoverState(Qt::AnchorHorizontalCenter)
    , m_currentPage(0)
{
    viewSizeList << 25 << 33 << 50 << 75 << 100 << 150 << 200 << 300 << 400 << 800;
    viewSizeIdx = 4; // 100

    setScene(new QGraphicsScene(this));
//    setTransformationAnchor(AnchorUnderMouse);
//    setDragMode(ScrollHandDrag);
//    setViewportUpdateMode(FullViewportUpdate);
    setAcceptDrops(false);
//    setDragMode(DragDropMode::InternalMove);
//    setBackgroundBrush(QBrush(Qt::gray));
    setRenderer(OpenGL);

    setMouseTracking(true);


}


void ImageView::setImage(QImage image)
{
    if(m_fileVolume == nullptr) return;
    clearImages();
    addImage(image, true);
}

void ImageView::addImage(QImage image, bool pageNext)
{
    if(m_fileVolume == nullptr) return;
    m_ptLeftTop.reset();
    QGraphicsScene *s = scene();

    QPixmap pixmap = QPixmap::fromImage(image);
    if(pageNext)
        m_imgs.push_back(pixmap);
    else
        m_imgs.push_front(pixmap);

    QGraphicsPixmapItem* gpi = s->addPixmap(pixmap);
    // if we show the image with resizing more smooth, must be called
    gpi->setTransformationMode(Qt::SmoothTransformation);
    if(pageNext)
        m_gpiImages.push_back(gpi);
    else
        m_gpiImages.push_front(gpi);
}

void ImageView::clearImages()
{
    if(m_fileVolume == nullptr) return;
    QGraphicsScene *s = scene();
    for(int i = 0; i < m_gpiImages.length(); i++) {
        s->removeItem((QGraphicsItem*)m_gpiImages[i]);
        delete m_gpiImages[i];
    }
    m_gpiImages.resize(0);
    m_imgs.resize(0);
}

void ImageView::nextPage()
{
    if(m_fileVolume == nullptr) return;
    bool result = m_fileVolume->nextFile();
    if(!result) return;
    m_currentPage += m_dualImage ? 2 : 1;

    reloadCurrentPage();
    emit pageChanged();
}

void ImageView::reloadCurrentPage()
{
    if(m_fileVolume == nullptr) return;
    clearImages();

    addImage(m_fileVolume->currentImage(), true);
    if(m_dualImage) {
        bool result = m_fileVolume->nextFile();
        if(!result) return;
        addImage(m_fileVolume->currentImage(), true);
    }
    emit pageChanged();
}


void ImageView::prevPage()
{
    if(m_fileVolume == nullptr) return;
    bool result = m_fileVolume->prevFile();
    if(!result) return;
    clearImages();
    m_currentPage -= m_dualImage ? 2 : 1;


    addImage(m_fileVolume->currentImage(), false);
    if(m_dualImage) {
        result = m_fileVolume->prevFile();
        if(!result) return;
        addImage(m_fileVolume->currentImage(), false);
    }
    emit pageChanged();
}

void ImageView::setIndexedPage(int idx)
{
    if(m_fileVolume == nullptr) return;
    clearImages();
    bool result = m_fileVolume->setIndexedFile(idx);
    if(!result) return;
    m_currentPage = idx;

    addImage(m_fileVolume->currentImage(), true);
    if(m_dualImage) {
        result = m_fileVolume->nextFile();
        if(!result) return;
        addImage(m_fileVolume->currentImage(), true);
    }
    emit pageChanged();
}

void ImageView::setRenderer(RendererType type)
{
    m_renderer = type;

    if (m_renderer == OpenGL) {
#ifndef QT_NO_OPENGL
        QGLWidget* w = new QGLWidget(QGLFormat(QGL::SampleBuffers));
        setViewport(w);
//        setRenderHint(QPainter::HighQualityAntialiasing, true);
//        setRenderHint(QPainter::Antialiasing, true);
//        setRenderHint(QPainter::SmoothPixmapTransform, true);
#endif
    } else {
        setViewport(new QWidget);
    }
}

QString ImageView::currentPageAsString() const
{
    if(m_dualImage) {
        return QString("%1-%2/%3").arg(m_currentPage+1).arg(m_currentPage+2).arg(m_fileVolume->size());
    } else {
        return QString("%1/%3").arg(m_currentPage+1).arg(m_fileVolume->size());
    }
}

void ImageView::paintEvent( QPaintEvent *event )
{
    if(!m_gpiImages.empty()) {
        if(m_maximized) {
            for(int i = 0; i < m_gpiImages.size(); i++) {
                QSize size = viewport()->size();
                QSize imgsize = m_imgs[i].size();
                QSize sizeofview = m_dualImage ? QSize(size.width()/2,size.height()) : size;

                QSize newsize = imgsize.scaled(sizeofview, Qt::KeepAspectRatio);
                m_gpiImages[i]->setScale(1.0*newsize.width()/imgsize.width());

                int offset = m_rightSideBook ? -i : i-1;
                if(newsize.height() == size.height()) { // 上下に内接
                    m_gpiImages[i]->setPos(m_dualImage ? sizeofview.width()+offset*newsize.width() : (size.width()-newsize.width())/2, 0);
                } else { // 左右に内接
                    m_gpiImages[i]->setPos(m_dualImage ? sizeofview.width()+offset*newsize.width() : 0, (size.height()-newsize.height())/2);
                }
            }
        } else {
            for(int i = 0; i < m_gpiImages.size(); i++) {
                qreal scale = 1.0*currentViewSize()/100;
                m_gpiImages[i]->setScale(scale);
                QSize size = viewport()->size();
                QSize imgsize = m_imgs[i].size();
                imgsize *= scale;
                if(m_dualImage)
                    m_gpiImages[i]->setPos(size.width()/2-i*imgsize.width(), 0);
                else
                    m_gpiImages[i]->setPos(size.width()/2-imgsize.width()/2, 0);
            }
//            my_gpi->parentItem()->setScale(1.0);
        }

    }
    QGraphicsView::paintEvent(event);
}

void ImageView::resizeEvent(QResizeEvent *event)
{
    if(scene()) {
        scene()->setSceneRect(QRect(QPoint(), event->size()));
    }
    QGraphicsView::resizeEvent(event);
}

#define HOVER_BORDER 20

void ImageView::mouseMoveEvent(QMouseEvent *e)
{
//    qDebug() << e;
    if(e->pos().x() < HOVER_BORDER) {
        if(m_hoverState != Qt::AnchorLeft)
            emit anchorHovered(Qt::AnchorLeft);
        m_hoverState = Qt::AnchorLeft;
        QApplication::setOverrideCursor(Qt::PointingHandCursor);
        return;
    }
    if(e->pos().x() > width()-HOVER_BORDER) {
        if(m_hoverState != Qt::AnchorRight)
            emit anchorHovered(Qt::AnchorRight);
        m_hoverState = Qt::AnchorRight;
        QApplication::setOverrideCursor(Qt::PointingHandCursor);
        return;
    }
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    if(e->pos().y() < HOVER_BORDER) {
        if(m_hoverState != Qt::AnchorTop)
           emit anchorHovered(Qt::AnchorTop);
        m_hoverState = Qt::AnchorTop;
        return;
    }
    if(e->pos().y() > height()-HOVER_BORDER) {
        if(m_hoverState != Qt::AnchorBottom)
           emit anchorHovered(Qt::AnchorBottom);
        m_hoverState = Qt::AnchorBottom;
        return;
    }
    if(m_hoverState != Qt::AnchorHorizontalCenter)
       emit anchorHovered(Qt::AnchorHorizontalCenter);
    m_hoverState = Qt::AnchorHorizontalCenter;
}

//void ImageView::mousePressEvent(QMouseEvent *e)
//{
//    if(e->button() && Qt::LeftButton) {
//        m_isMouseDown = true;
//        m_ptLeftTop.start(e->pos());
////        qDebug("mousePressEvent x:%d y:%d button:%x\n", e->x(), e->y(), e->button());
//    }
//}

//void ImageView::mouseReleaseEvent(QMouseEvent *)
//{
//    if(m_isMouseDown) {
//        m_isMouseDown = false;
////        qDebug("mouseReleaseEvent x:%d y:%d\n", e->x(), e->y());
//    }
//}

void ImageView::on_image_changing(QImage image)
{
    setImage(image);
    repaint();
}

void ImageView::on_fitting_triggered(bool maximized)
{
    m_maximized = maximized;
    qApp->setFitting(maximized);
    repaint();
}

void ImageView::on_dualView_triggered(bool viewdual)
{
    bool before = m_dualImage;
    m_dualImage = viewdual;
    qApp->setDualView(viewdual);

    if(before)
        prevPage();
    else
        reloadCurrentPage();
    repaint();
}

void ImageView::on_rightSideBook_triggered(bool rightSideBook)
{
    m_rightSideBook = rightSideBook;
    qApp->setRightSideBook(rightSideBook);
    repaint();
}

void ImageView::on_scaleUp_triggered()
{
    if(viewSizeIdx < viewSizeList.size() -1)
        viewSizeIdx++;
    repaint();
}

void ImageView::on_scaleDown_triggered()
{
    if(viewSizeIdx > 0)
        viewSizeIdx--;
    repaint();
}
