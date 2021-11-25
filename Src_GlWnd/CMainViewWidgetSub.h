#ifndef CMAINVIEWWIDGETSUB_H
#define CMAINVIEWWIDGETSUB_H

#include <QtWidgets/QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QtCore/QTimer>
#include <time.h>

namespace Ui {
class CMainViewWidgetSub;
}

class CCamera;
class COpenGLCoreSub;
class CMainViewWidgetSub : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit CMainViewWidgetSub(QWidget *parent = nullptr);
    ~CMainViewWidgetSub();

    QWidget* m_pParent = nullptr;

    CCamera* m_pCamera = nullptr;
    COpenGLCoreSub* m_pGLCore = nullptr;

    QAction* m_pActCtrlAddPoint = nullptr;
    QAction* m_pActCtrlDelPoint = nullptr;

    time_t m_nOrignalTime;
    int    m_nRbtnDown = 0;
    QPoint m_OriginalPos;
    int    m_nMbtnDown = 0;
    QPoint m_MiddleBtnPos;
    int    m_nLbtnDown = 0;

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    void mouseMoveEvent(QMouseEvent *event) override;//鼠标事件
    void wheelEvent(QWheelEvent *event) override;    //滚轮事件
    void mousePressEvent(QMouseEvent *event) override;  //鼠标按下事件
    void mouseDoubleClickEvent(QMouseEvent *event) override;  //鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *event) override; //鼠标释放事件
protected:
    void keyPressEvent(QKeyEvent *event);   //键盘按下事件
    void keyReleaseEvent(QKeyEvent *event);  //键盘释放事件

private:
    Ui::CMainViewWidgetSub *ui;

signals:
    void signalMouseRelease(void* pParam);

public slots:
    void itemTreeMenuAddPointSlot(bool checked);
    void itemTreeMenuDelPointSlot(bool checked);
};

#endif // CMainViewWidgetSub_H
