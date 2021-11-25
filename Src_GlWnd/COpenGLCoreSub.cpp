#include "COpenGLCoreSub.h"

#include <QCoreApplication>
#include <algorithm>

#include "SOIL2/SOIL2.h"

#include "FontRenderer.h"
#include "Src_Geometry/CDrawGround.h"
#include "Src_Geometry/CBaseMap.h"
#include "Src_Geometry/CRasterDraw.h"
#include "Src_Geometry/CLayerVector.h"
#include "Src_Geometry/CLayerGeoDraw.h"

extern const char *g_ErrCantLoadOGL;
extern const char *g_ErrCantUnloadOGL;

const color32 COLOR32_BLACK     = 0xff000000;   // Black
const color32 COLOR32_WHITE     = 0xffffffff;   // White
const color32 COLOR32_ZERO      = 0x00000000;   // Zero
const color32 COLOR32_RED       = 0xffff0000;   // Red
const color32 COLOR32_GREEN     = 0xff00ff00;   // Green
const color32 COLOR32_BLUE      = 0xff0000ff;   // Blue
//  ---------------------------------------------------------------------------

#ifdef _DEBUG
    void CheckGLCoreError(const char *file, int line, const char *func, COpenGLCoreSub* pCore)
    {
        GLenum err=0;
        char msg[256];
        while( (err=pCore->glGetError())!=0 )
        {
            sprintf(msg, "%s(%d) : [%s] GL_CORE_ERROR=0x%x\n", file, line, func, err);
            #ifdef ANT_WINDOWS
                OutputDebugString(msg);
            #endif
            fprintf(stderr, msg);
        }
    }
#   ifdef __FUNCTION__
#       define CHECK_GL_ERROR CheckGLCoreError(__FILE__, __LINE__, __FUNCTION__, this)
#   else
#       define CHECK_GL_ERROR CheckGLCoreError(__FILE__, __LINE__, "")
#   endif
#else
#   define CHECK_GL_ERROR// ((void)(0))
#endif

//  ---------------------------------------------------------------------------

COpenGLCoreSub::COpenGLCoreSub()
{
}

COpenGLCoreSub::~COpenGLCoreSub(){
}

//  ---------------------------------------------------------------------------

int COpenGLCoreSub::Draw(){

    this->m_MatMVPCore = this->m_MatPerspectProject * this->m_MatView * this->m_MatModel;

    //time_t ss=clock();
    //this->TestDraw();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(this->m_nMouseStatus != COpenGLCoreSub::e_Mouse_MIDDLE_DOWN_MOVING
            && this->m_nMouseStatus != COpenGLCoreSub::e_Mouse_WHEEL_SCROLLING
            && this->m_nMouseStatus != COpenGLCoreSub::e_Mouse_MOVING)
    {

        //ss=clock();
        this->FboBind();
        //printf("this->FboBind(): %lld\n", clock()-ss);

        //ss=clock();
        //this->m_pDrawGround->Draw();
        //this->m_pBaseMap->Draw();
        //printf("this->m_pBaseMap->Draw(): %lld\n", clock()-ss);

        //ss=clock();
        if(this->m_pLayerVector)
            this->m_pLayerVector->Draw();
        //printf("this->m_pLayerVector->Draw(): %lld\n", clock()-ss);

        //this->m_pRasterDraw->Draw();
        //this->DrawImage(100, 9100, 200, 200, this->m_sImgFileFullPath.c_str());

        if(this->m_nMouseWorkStatus == COpenGLCoreSub::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance){
            this->DrawCalcDistanceTrackFBO();
        }
        //ss=clock();
        this->FboUnbind();
        //printf("this->FboUnbind(): %lld\n", clock()-ss);
        //this->m_nMouseMovingDeltaX = 0;
        //this->m_nMouseMovingDeltaY = 0;
        //this->m_nMouseMovingLastDeltaX = 0;
        //this->m_nMouseMovingLastDeltaY = 0;
        printf("---------------------RestPos\n");
        if (this->m_nMouseStatus == COpenGLCoreSub::e_Mouse_RESET_VIEWPORT){
            this->m_nMouseStatus = this->m_nMouseStatusLast;
        }
    }
    //static int nnn = 1;
    //printf("Draw %d: %d, %d, ,%d, %d\n", nnn++,
    //       this->m_nMouseMovingDeltaX, this->m_nMouseMovingDeltaY, this->m_nMouseMovingLastDeltaX, this->m_nMouseMovingLastDeltaY);
    //this->DrawImage(0 + this->m_nMouseMovingLastDeltaX + this->m_nMouseMovingDeltaX - this->m_nWheelOffX,
    //                0 + this->m_nMouseMovingLastDeltaY + this->m_nMouseMovingDeltaY - this->m_nWheelOffY,
    //                this->m_nWndWidth * this->m_nWheelScale, this->m_nWndHeight * this->m_nWheelScale, this->m_FboColorTextureID, 1);

    //ss=clock();
    this->DrawImage(
                this->m_nViewBaseLeftBottomX,
                this->m_nViewBaseLeftBottomY,
                this->m_nViewBaseWidth,
                this->m_nViewBaseHeight,
                this->m_FboColorTextureID, 1);

    if(this->m_nMouseWorkStatus == COpenGLCoreSub::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance){
        this->DrawCalcDistanceTrack();
    }

    float fHalfW = (float)(int)(this->m_nWndWidth >> 1);
    float fHalfH = (float)(int)(this->m_nWndHeight >> 1);
    //this->DrawLine(fHalfW - 8.0f, fHalfH, fHalfW + 8.0f, fHalfH, 0xFFFFFF00);
    //this->DrawLine(fHalfW, fHalfH - 8.0f, fHalfW, fHalfH + 8.0f, 0xFFFFFF00);
    this->DrawCircle(fHalfW, fHalfH, 8.0f, 0xFFFFFF00);

    //this->DrawString(this->m_sMsg.c_str(), this->m_nMousePosX, this->m_nMousePosY, 0.5f, 0xFF0000FF);
    this->DrawString(this->m_sMsg.c_str(), 25.0f, 20.0f, 0.5f, 0xFF0000FF);
    std::string sCornorMsg = std::to_string(this->t_geo_minmax.fGeoRightTopX - this->t_geo_minmax.fGeoLeftBottomX) +
            ": "+std::to_string(this->m_fView2GeoScaleH) + ", "+std::to_string(this->m_fView2GeoScaleV) +
            ", SelEleCnt: " + std::to_string(this->m_pLayerVector->m_nOperateEleCnt);
    this->DrawString(sCornorMsg.c_str(), 25.0f, 40.0f, 0.5f, 0xFF0000FF);
    //this->DrawString("我们是共产主义接班人一二三四|—— asdf ?.", 25.0f, 200.0f, 0.5f, 0xFF33FFFF);


    if(this->m_nMouseWorkBegin == 1 && this->m_nMouseWorkStatus == COpenGLCoreSub::EMOUSE_WORK_STATUS::e_Mouse_Work_Select){
        this->DrawRect((float)this->m_nMouseSelectPosStartX, (float)this->m_nMouseSelectPosStartY, (float)this->m_nMouseSelectPosEndX, (float)this->m_nMouseSelectPosEndY, 0xFFFFFF00);
        double fSelectGeoStartX, fSelectGeoStartY, fSelectGeoEndX, fSelectGeoEndY;
        this->View2GeoPosition(this->m_nMouseSelectPosStartX, this->m_nMouseSelectPosStartY, &fSelectGeoStartX, &fSelectGeoStartY);
        this->View2GeoPosition(this->m_nMouseSelectPosEndX, this->m_nMouseSelectPosEndY, &fSelectGeoEndX, &fSelectGeoEndY);
        double fSelectGeoArea = (fabs(fSelectGeoEndX - fSelectGeoStartX)) * (fabs(fSelectGeoEndY - fSelectGeoStartY));
        std::string sCornorMsg1 = std::to_string(fSelectGeoArea);
        this->DrawString(sCornorMsg1.c_str(), 25.0f, 60.0f, 0.5f, 0xFF0000FF);
    }

    //printf("this->DrawString: %lld\n", clock()-ss);
    if(this->m_nMouseStatus != COpenGLCoreSub::e_Mouse_MIDDLE_DOWN_MOVING
            && this->m_nMouseStatus != COpenGLCoreSub::e_Mouse_WHEEL_SCROLLING){
        this->m_nMouseStatus = COpenGLCoreSub::e_Mouse_MOVING;

        this->m_nViewBaseLeftBottomX = 0;
        this->m_nViewBaseLeftBottomY = 0;
        this->m_nViewBaseWidth = this->m_nWndWidth;
        this->m_nViewBaseHeight = this->m_nWndHeight;

        if(this->m_pLayerVector->m_nBeingEditor == 1 && this->m_pLayerVector->m_pGeoLayerEdit){
            this->m_pLayerVector->m_pGeoLayerEdit->Draw();
        }
    }

    this->DrawLine(1, 0, 1, this->m_nWndHeight, 0xFFFFFFFF);
    this->DrawLine(this->m_nWndWidth, 0, this->m_nWndWidth, this->m_nWndHeight, 0xFFFFFFFF);
    this->DrawLine(1, 1, this->m_nWndWidth, 1, 0xFFFFFFFF);
    return 1;
}

void COpenGLCoreSub::MouseMoving(int xPos, int yPos){
    this->m_nMousePosX = xPos;
    this->m_nMousePosY = this->m_nWndHeight - yPos;

    double fGeoX, fGeoY;
    this->View2GeoPosition(this->m_nMousePosX, this->m_nMousePosY, &fGeoX, &fGeoY);
    char szGeoXY[64] = {0};
    sprintf(szGeoXY, "%.9f, %.9f", fGeoX, fGeoY);
    this->m_sMsg = std::string(szGeoXY) + " : "+std::to_string(this->m_nMousePosX)+", "+std::to_string(this->m_nMousePosY);
    this->m_nGeoMousePosX = fGeoX;
    this->m_nGeoMousePosY = fGeoY;

    if(this->m_pLayerVector->m_vecLayerItem.size() > 0){
        CRasterDraw* pRasterDraw = (CRasterDraw*)this->m_pLayerVector->m_vecLayerItem[0];
        double fPixX, fPixY;
        pRasterDraw->GetPixCoordByGeo(fGeoX, fGeoY, &fPixX, &fPixY);
        //this->m_sMsg = this->m_sMsg + "\n" + std::to_string(fGeoX)+", "+std::to_string(fGeoY) + " : "+std::to_string(fPixX)+", "+std::to_string(fPixY);
        this->m_sMsg = std::to_string(fPixX)+", "+std::to_string(fPixY) + " " + this->m_sMsg;

    }

}
