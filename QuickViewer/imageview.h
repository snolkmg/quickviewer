#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QtDebug>
#include <QGraphicsView>
#include <QDragEnterEvent>
#include "filevolume.h"

/**
 * @brief The SavedPoint class
 * マウスやキーボードによる画像の移動計算の補助を行う。マウスドラッグ開始時の座標を保存し、
 * ドラッグ時にマウスカーソルが相対的に移動した分だけ画像を移動させるための座標を算出する
 */
class SavedPoint : public QPoint
{
public:
    void start(QPoint ptStart) {
        m_ptStart = ptStart;
        m_ptSaved = QPoint(x(),y());
    }
    void step(QPoint ptStep) {
//        qDebug("before: pt:(%d,%d) step:(%d,%d) start:(%d,%d)",
//               x(),y(),
//               ptStep.x(),ptStep.y(),
//               m_ptStart.x(),m_ptStart.y() );
        setX(m_ptSaved.x() + ptStep.x() - m_ptStart.x());
        setY(m_ptSaved.y() + ptStep.y() - m_ptStart.y());
//        qDebug("after:  pt:(%d,%d)", x(),y());
    }
    QPoint pt() const { return QPoint(x(), y()); }
    void reset() { setX(0);setY(0);}

private:
    QPoint m_ptStart;
    QPoint m_ptSaved;
};



/**
 * @brief The ImageView class
 * セットした画像を１枚表示できる。
 * 画像はマウスでドラッグできる。
 */
class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    enum RendererType { Native, OpenGL, Image };
    explicit ImageView(QWidget *parent = Q_NULLPTR);
    void setImage(QImage image);
    void setRenderer(RendererType type = Native);
    void addImage(QImage image, bool pageNext);
    void clearImages();
    void nextPage();
    void prevPage();
    void reloadCurrentPage();
    void setIndexedPage(int idx);
    void setFileVolume(IFileVolume* vol) { m_fileVolume = vol; }
    int currentViewSize() { return viewSizeList[viewSizeIdx]; }
    int currentPage() const {return m_currentPage+1; }
    QString currentPageAsString() const;
    Qt::AnchorPoint hoverState() const { return m_hoverState; }

signals:
    /**
     * @brief anchorHovered ImageViewのフレーム端にマウスカーソルが移動した場合に発生するシグナル
     */
    void anchorHovered(Qt::AnchorPoint anchor) const;
    void pageChanged() const;

protected:
    void paintEvent( QPaintEvent *event );
    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event) override;
//    void dragEnterEvent(QDragEnterEvent *event) { event->accept(); qDebug() << "ImageView::dragEnterEvent"; }
//    void dropEvent( QDropEvent *e ) {qDebug() << "ImageView::dropEvent";}
//    void dragMoveEvent( QDragMoveEvent *e ) {qDebug() << "ImageView::dragMoveEvent";}
//    void dragLeaveEvent( QDragLeaveEvent * e ) {qDebug() << "ImageView::dragLeaveEvent";}
public slots:
    void on_image_changing(QImage image);
    void on_fitting_triggered(bool maximized);
    void on_dualView_triggered(bool viewdual);
    void on_rightSideBook_triggered(bool rightSideBook);
    void on_scaleUp_triggered();
    void on_scaleDown_triggered();

private:
    RendererType m_renderer;
//    QImage m_img;
    QVector<QPixmap> m_imgs;
    /**
     * @brief m_gpiImages
     * 表示画像をQGraphicsItem化したもの。これをsceneに登録することで画像を表示する
     */
    QVector<QGraphicsPixmapItem*> m_gpiImages;
    SavedPoint m_ptLeftTop;
    QGraphicsScene* m_scene;
    bool m_isMouseDown;
    bool m_maximized;
    bool m_dualImage;
    bool m_rightSideBook;
    Qt::AnchorPoint m_hoverState;
    /**
     * @brief 現在表示しているページのうち最も若い番号。filevolume側の内部ページと異なる場合があるので注意
     */
    int m_currentPage;

    IFileVolume* m_fileVolume;
    QList<int> viewSizeList;
    uint viewSizeIdx;
};



#endif // IMAGEVIEW_H