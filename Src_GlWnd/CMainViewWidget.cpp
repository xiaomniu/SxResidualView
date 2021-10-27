#include "CMainViewWidget.h"
#include "ui_CMainViewWidget.h"

#include <QtWidgets/QMenu>

#include "Src_Core/CGlobal.h"
#include "Src_Geometry/CLayerDraw.h"
#include "Src_Geometry/CLayerVector.h"
#include "Src_Geometry/CLayerPolygonsEdit.h"
#include "Src_GlWnd/CCamera.h"

CMainViewWidget::CMainViewWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::CMainViewWidget)
{
    ui->setupUi(this);

    this->m_pParent = parent;
    this->setMouseTracking(true);

    QAction* pActCtrl0 = nullptr;
    pActCtrl0 = new QAction(QStringLiteral("增加点"), this);
    //pActCtrl0->setCheckable(true);
    //pActCtrl0->setChecked(true);
    connect(pActCtrl0, &QAction::triggered, this, &CMainViewWidget::itemTreeMenuAddPointSlot);
    this->m_pActCtrlAddPoint = pActCtrl0;

    QAction* pActCtrl1 = nullptr;
    pActCtrl1 = new QAction(QStringLiteral("删除点"), this);
    //pActCtrl1->setCheckable(true);
    //pActCtrl1->setChecked(false);
    connect(pActCtrl1, &QAction::triggered, this, &CMainViewWidget::itemTreeMenuDelPointSlot);
    this->m_pActCtrlDelPoint = pActCtrl1;
}

CMainViewWidget::~CMainViewWidget()
{
    delete ui;

    if(this->m_pGLCore) {
        delete this->m_pGLCore;
        this->m_pGLCore = nullptr;
    }
    if(this->m_pCamera){
        delete this->m_pCamera;
        this->m_pCamera = nullptr;
    }
}

void CMainViewWidget::initializeGL(){
    CGlobal* pGlobal = GetGlobalPtr();
    this->m_pGLCore = new COpenGLCore;
    pGlobal->m_pGLCore = this->m_pGLCore;

    this->m_pGLCore->InitSceneItem();
    this->m_pGLCore->m_pParentWidget = this;

    this->m_pCamera = new CCamera(this->m_pGLCore);

}

void CMainViewWidget::resizeGL(int w, int h){
    this->m_pGLCore->ViewPortResize(w, h);
}

void CMainViewWidget::paintGL() {

    time_t ss = clock();
    //if(ss - this->m_nOrignalTime > 50){
    //    this->m_nOrignalTime = ss;
    //}
    printf("CMainWindowExtra01::paintGL(): %lld\n", clock()-ss);
    float fDelta = static_cast<float>(ss) - static_cast<float>(this->m_nOrignalTime);
    this->m_nOrignalTime = ss;
    //if(this->m_pCamera->m_nMoveKeyDown == 1)
    this->m_pCamera->Update(fDelta / 1000.0f);
    this->m_pGLCore->m_MatView = this->m_pCamera->m_matView;
    this->m_pGLCore->Draw();

    //printf("CMainWindowExtra01::paintGL(): %lld\n", clock()-ss);
}


void CMainViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    int xpos = event->pos().x();
    int ypos = event->pos().y();
    if (this->m_nLbtnDown == 1) {

    }
    else if (this->m_nMbtnDown == 1) {

        int deltaX = xpos - this->m_MiddleBtnPos.x();
        int deltaY = ypos - this->m_MiddleBtnPos.y();
        this->m_pGLCore->m_nMouseStatus = COpenGLCore::e_Mouse_MIDDLE_DOWN_MOVING;
        this->m_pGLCore->MidleDownMoving(deltaX, -deltaY);
    }
    else if(this->m_nRbtnDown == 1) {

        int deltaX = xpos - this->m_OriginalPos.x();
        int deltaY = ypos - this->m_OriginalPos.y();
        this->m_OriginalPos.setX(xpos);
        this->m_OriginalPos.setY(ypos);

        this->m_pCamera->UpdateDirection(deltaX, deltaY);
        this->m_pGLCore->m_nMouseStatus = COpenGLCore::e_Camera_MOVING;
    }
    this->m_pGLCore->MouseMoving(xpos, ypos);
    this->update();
}

void CMainViewWidget::wheelEvent(QWheelEvent *event)
{
    //QPoint ptAngleDelta = event->angleDelta();
    //QPoint ptPixelDelta = event->pixelDelta();
    int nDelta = event->delta();
    //printf("Wheel Pos : %d %d\n", event->x(), event->y());
    //printf("%d,%d  %d  %d,%d\n", ptAngleDelta.x(), ptAngleDelta.y(), nDelta, ptPixelDelta.x(), ptPixelDelta.y());
    this->m_pGLCore->WheelEvent(nDelta, event->x(), event->y());
    this->update();
}

void CMainViewWidget::mousePressEvent(QMouseEvent *event)
{
    int xpos = event->pos().x();
    int ypos = event->pos().y();
    Qt::MouseButton eMouseBtnType = event->button();
    switch(eMouseBtnType) {
    case Qt::LeftButton:{
        this->setFocus();
        this->m_nLbtnDown = 1;
        this->m_pGLCore->MousePress(1, xpos, ypos);
        if(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit && this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nHitParamFlag == 2) {
            this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nMovingPoint = 1;
        }
        break;}
    case Qt::RightButton:{
        this->m_nRbtnDown = 1;
        this->m_OriginalPos.setX(xpos);
        this->m_OriginalPos.setY(ypos);

        break;}
    case Qt::MiddleButton:{
        this->m_nMbtnDown = 1;
        this->m_pGLCore->UpdateMidleDownMovingPos();
        this->m_MiddleBtnPos.setX(xpos);
        this->m_MiddleBtnPos.setY(ypos);
        printf("xpos:%d ypos:%d  MiddleButton Mouse Down\n", xpos, ypos);
        break;}
    }
}

void CMainViewWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    int xpos = event->pos().x();
    int ypos = event->pos().y();
    Qt::MouseButton eMouseBtnType = event->button();
    switch(eMouseBtnType) {
    case Qt::LeftButton:{
        this->setFocus();
        this->m_pGLCore->MouseDbPress(1, xpos, ypos);
        break;}
    }
}

void CMainViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Qt::MouseButton eMouseBtnType = event->button();
    switch(eMouseBtnType){
    case Qt::LeftButton:{
        int xpos = event->pos().x();
        int ypos = event->pos().y();
        this->m_pGLCore->MouseRelease(1, xpos, ypos);
        this->m_nLbtnDown = 0;

        if(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit) {
            this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nMovingPoint = 0;
        }

        emit this->mouseReleaseDone(this->m_pGLCore->m_nGeoMousePosX, this->m_pGLCore->m_nGeoMousePosY);
        break;}
    case Qt::RightButton:{
        int xpos = event->pos().x();
        int ypos = event->pos().y();
        this->m_pGLCore->MouseRelease(2, xpos, ypos);
        this->m_nRbtnDown = 0;
        if(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit) {
            this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nMovingPoint = 0;

            if(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nHitParamFlag != 0) {
                QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                            QMenu::item{padding:3px 20px 3px 20px;}\
                            QMenu::indicator{width:13px;height:13px;}\
                            QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                            QMenu::separator{height:1px;background:#757575;}";

                QMenu menu;
                menu.setStyleSheet(qss);
                if(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nHitParamFlag == 1)
                    menu.addAction(this->m_pActCtrlAddPoint);
                //menu.addSeparator();
                if(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nHitParamFlag == 2)
                    menu.addAction(this->m_pActCtrlDelPoint);

                menu.exec(QCursor::pos());  //显示菜单
            }
        }

        break;}
    case Qt::MiddleButton:{
        this->m_nMbtnDown = 0;
        printf("MiddleButton MouseUP\n");
        this->m_pGLCore->m_nMouseStatus = 0;
        this->m_pGLCore->MidleDownMovingEnd();
        this->update();
        break;}
    }
}

void CMainViewWidget::keyPressEvent(QKeyEvent *event)
{
    int nFlag = 0;
    const int nKeyFlag = 1;
    int nKeyTpye = event->key();
    printf("key press : %d\n", nKeyTpye);
    switch (nKeyTpye) {
    case Qt::Key_W:{
        this->m_pCamera->m_nMoveForward = nKeyFlag;
        nFlag++;
        break;}
    case Qt::Key_S:{
        this->m_pCamera->m_nMoveBack = nKeyFlag;
        nFlag++;
        break;}
    case Qt::Key_A:{
        this->m_pCamera->m_nMoveLeft = nKeyFlag;
        nFlag++;
        break;}
    case Qt::Key_D:{
        this->m_pCamera->m_nMoveRight = nKeyFlag;
        nFlag++;
        break;}
    case Qt::Key_Q:{
        this->m_pCamera->m_nMoveUp = nKeyFlag;
        nFlag++;
        break;}
    case Qt::Key_E:{
        this->m_pCamera->m_nMoveDown = nKeyFlag;
        nFlag++;
        break;}
    case Qt::Key_X:{
        this->m_pCamera->m_nViewReset = nKeyFlag;
        //printf("XXXXXXXXXX\n");
        nFlag++;
        break;}
    }
    if(nFlag != 0){
        this->m_pGLCore->m_nMouseStatus = COpenGLCore::e_Camera_MOVING;
        this->m_pCamera->m_nMoveKeyDown = nKeyFlag;
        time_t ddCha = clock() - this->m_nOrignalTime;
        if(ddCha > 100){
            this->m_nOrignalTime = clock();
            //printf("time once : %lld\n", ddCha);
        }
        //this->update();
    }
    //this->handleKeyPressEvent(event);
}

void CMainViewWidget::keyReleaseEvent(QKeyEvent *event)
{
    int nFlag = 0;
    const int nKeyFlag = 0;
    int nKeyTpye = event->key();
    printf("key up : %d\n", nKeyTpye);
    switch (nKeyTpye) {
    case Qt::Key_W:{
        this->m_pCamera->m_nMoveForward = nKeyFlag;
        --nFlag;
        break;}
    case Qt::Key_S:{
        this->m_pCamera->m_nMoveBack = nKeyFlag;
        --nFlag;
        break;}
    case Qt::Key_A:{
        this->m_pCamera->m_nMoveLeft = nKeyFlag;
        --nFlag;
        break;}
    case Qt::Key_D:{
        this->m_pCamera->m_nMoveRight = nKeyFlag;
        --nFlag;
        break;}
    case Qt::Key_Q:{
        this->m_pCamera->m_nMoveUp = nKeyFlag;
        --nFlag;
        break;}
    case Qt::Key_E:{
        this->m_pCamera->m_nMoveDown = nKeyFlag;
        --nFlag;
        break;}
    case Qt::Key_X:{
        this->m_pCamera->m_nViewReset = nKeyFlag;
        --nFlag;
        break;}
    }
    if(nFlag != 0){
        this->m_pGLCore->m_nMouseStatus = COpenGLCore::e_Camera_MOVING;
        //this->m_pCamera->m_nMoveKeyDown = nKeyFlag;
        this->update();
    }
    //this->handleKeyReleaseEvent(event);
}

void CMainViewWidget::itemTreeMenuAddPointSlot(bool checked)
{
    printf("itemTreeMenuAddPointSlot:%d\n", checked);
    if( !(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit && this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nHitParamFlag != 0))
    {
        return;
    }
    ((CLayerPolygonsEdit*)this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit)->AddPoint();
    this->update();
}

void CMainViewWidget::itemTreeMenuDelPointSlot(bool checked)
{
    printf("itemTreeMenuDelPointSlot:%d\n", checked);
    if( !(this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit && this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit->m_nHitParamFlag != 0))
    {
        return;
    }
    ((CLayerPolygonsEdit*)this->m_pGLCore->m_pLayerVector->m_pGeoLayerEdit)->DelPoint();
    this->update();
}
