#include "CToolMenuHandler.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QPushButton>
#include <QtCore/QTextCodec>

#include <thread>

#include "ui_CMenuBar.h"

#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtGui/QIcon>

#include "gdal/ogrsf_frmts.h"
#include "gdal/gdal_priv.h"

#include "Src_Core/Misc.h"
#include "Src_Core/CGlobal.h"

#include "Src_MenuBar/CMenuBar.h"
#include "Src_MenuBar/CMenuItemWidget.h"

#include "Src_Geometry/CLayerVector.h"
#include "Src_Geometry/CDrawGround.h"
#include "Src_Geometry/CLayerPoints.h"

#include "Src_Attribute/CChunkAttribute.h"

#include "Src_LayerManager/CLayerManagerWidget.h"

#include "Src_MatchPoints/ReadResidualFile.h"

#include "Src_GlWnd/COpenGLCore.h"
#include "Src_GlWnd/COpenGLCoreSub.h"

#include "Src_GlWnd/CMainViewWidget.h"
#include "Src_GlWnd/CMainViewWidgetSub.h"


CToolMenuHandler::CToolMenuHandler(QObject *parent) :
    QObject(parent)
{
    this->m_pMenuBar = (CMenuBar*)parent;

}


//Lab_Undo,            ":/img/ToolBar/Res/ImgToolBar/undo_01.png", tr("UnDo"));//::fromLocal8Bit("撤销"));
//Lab_Redo,            ":/img/ToolBar/Res/ImgToolBar/redo_01.png", tr("ReDo"));//QString::fromLocal8Bit("重做"));
//Lab_Select,          ":/img/ToolBar/Res/ImgToolBar/select_01.png", tr("Select"));//QString::fromLocal8Bit("选取"));
//Lab_Grab,            ":/img/ToolBar/Res/ImgToolBar/grab_01.png", (u8"测距"));//QString::fromLocal8Bit("抓取"));
//Lab_Scale,           ":/img/ToolBar/Res/ImgToolBar/zoom_in_01.png", tr("ZoomIn"));//QString::fromLocal8Bit("放大"));
//Lab_CreatePoint,     ":/img/ToolBar/Res/ImgToolBar/create_gcp_01.png", tr("GroundPoint"));//QString::fromLocal8Bit("地面点"));
//Lab_CreateLinkPoint, ":/img/ToolBar/Res/ImgToolBar/link_point_01.png", tr("TiePoint"));//QString::fromLocal8Bit("连接点"));
//Lab_Del_GroundPoint, ":/img/ToolBar/Res/ImgToolBar/delete_point_01.png", tr("DelGroundPt"));//QString::fromLocal8Bit("删除地面点"));
//Lab_Del_RasterPoint, ":/img/ToolBar/Res/ImgToolBar/delete_raster_point_01.png", tr("DelRasterPt"));//QString::fromLocal8Bit("删除影像点"));
//Lab_MovePoint,       ":/img/ToolBar/Res/ImgToolBar/move_point_01.png", (u8"卷帘"));//QString::fromLocal8Bit("移动点"));


void CToolMenuHandler::Clicked_Lab_Select(){
    //QMessageBox::information(this, "tt", "Clicked_Lab_Selectt");
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    if(pGLCore == nullptr) {
        return;
    }
    if(pGLCore->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select){
        pGLCore->m_nMouseWorkStatus = pGLCore->m_nMouseWorkStatusLast;
    }
    else{
        pGLCore->m_nMouseWorkStatus = COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select;
    }
    printf("pGLCore->m_nMouseWorkStatus :%d\n", pGLCore->m_nMouseWorkStatus);
}

void CToolMenuHandler::Clicked_Lab_CalcDistance(){
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    if(pGLCore == nullptr) {
        return;
    }
    if(pGLCore->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance){
        pGLCore->m_nMouseWorkStatus = pGLCore->m_nMouseWorkStatusLast;
    }
    else{
        pGLCore->m_nMouseWorkStatus = COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance;
    }
    printf("pGLCore->m_nMouseWorkStatus :%d\n", pGLCore->m_nMouseWorkStatus);
}

void CToolMenuHandler::Clicked_Lab_DelGroundPt(){
    //QMessageBox::information(this, "tt", "Clicked_Lab_Selectt");
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    if(pGLCore == nullptr){
        return;
    }
    if(pGLCore->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select){
        pGLCore->m_pLayerVector->DeleteGeometryBySelectRange();
    }
    printf("pGLCore->m_nMouseWorkStatus :%d\n", pGLCore->m_nMouseWorkStatus);

    CChunkAttribute* pChunkAttribute = GetGlobalPtr()->m_pChunkAttribute;
    if(pChunkAttribute == nullptr){
        return;
    }
    if(pChunkAttribute->m_nShowHide == 1) {
        pChunkAttribute->FreshViewByChunks();
    }
}

void CToolMenuHandler::Clicked_Lab_RollerBlind(){
    //QMessageBox::information(this, "tt", "Clicked_Lab_Selectt");
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    if(pGLCore == nullptr) {
        return;
    }
    if(pGLCore->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_RollerBlind){
        pGLCore->m_nMouseWorkStatus = pGLCore->m_nMouseWorkStatusLast;
        pGLCore->m_pLayerVector->ClearLayerIndxRollerBlind();
    }
    else{
        pGLCore->m_nMouseWorkStatus = COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_RollerBlind;
    }
    printf("pGLCore->m_nMouseWorkStatus :%d\n", pGLCore->m_nMouseWorkStatus);
}

void CToolMenuHandler::Clicked_Lab_ChunkAttribute(){
    CChunkAttribute* pChunkAttribute = GetGlobalPtr()->m_pChunkAttribute;
    if(pChunkAttribute == nullptr){
        return;
    }
    pChunkAttribute->m_nShowHide = ! pChunkAttribute->m_nShowHide;
    pChunkAttribute->FreshViewByChunks();
}
///////////////////////////////////

void CToolMenuHandler::Clicked_Btn_MouseReleaseDone(double fGeoMouseX, double fGeoMouseY){

    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    if(pGLCore == nullptr) {
        return;
    }
    if(pGLCore->m_nMouseWorkStatus != COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_SelectOnePoint)
        return;
    //char szMouseGeoBuf[64]={0};
    //if(this->m_nSelectPointIndx == 1){
    //    sprintf(szMouseGeoBuf, "%.6f", fGeoMouseX);
    //    this->m_pMenuBar->ui->lineEdit_Pt1_X->setText(szMouseGeoBuf);
    //    sprintf(szMouseGeoBuf, "%.6f", fGeoMouseY);
    //    this->m_pMenuBar->ui->lineEdit_Pt1_Y->setText(szMouseGeoBuf);
    //    this->m_nSelectPointIndx = 2;
    //}else if(this->m_nSelectPointIndx == 2){
    //    sprintf(szMouseGeoBuf, "%.6f", fGeoMouseX);
    //    this->m_pMenuBar->ui->lineEdit_Pt2_X->setText(szMouseGeoBuf);
    //    sprintf(szMouseGeoBuf, "%.6f", fGeoMouseY);
    //    this->m_pMenuBar->ui->lineEdit_Pt2_Y->setText(szMouseGeoBuf);
    //    this->m_nSelectPointIndx = 1;
    //}
}
///////////////////////////////////

void CToolMenuHandler::Clicked_Btn_TestFilterPoint(){
    QString sFilterNum = this->m_pMenuBar->ui->lineEdit_TestFilterPoint->text();
    int nFilterNum = sFilterNum.toInt();

    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerPoint = (CLayerPoints*)pLayerVector->m_vecLayerItem.at(0);

    CLayerFilter* pLayerFilter = pLayerPoint->m_pLayerFilter;
    if(pLayerFilter == nullptr) {
        pLayerFilter = new CLayerFilter;
        pLayerPoint->SetFilter(pLayerFilter);
    }
    pLayerFilter->SetRangeNumFilter(nFilterNum);
    pLayerPoint->FreshFilter(0);
    pGLCore->UpdateWidgets();
}

///////////////////////////////////
void CToolMenuHandler::Clicked_Btn_DemImgFullPath(){
    QString _sDemImgFullPath = QFileDialog::getOpenFileName(this->m_pMenuBar,
                                                                             tr("File Dialog"),
                                                                             "",
                                                                             tr("Raster")+"(*tif *tiff *img);;"
                                                                             );
    std::string sDemImgFullPath = _sDemImgFullPath.toLocal8Bit().data();
    QString sDemImgFullPathUtf8 = CMisc::AnsiToUtf8(sDemImgFullPath.c_str()).c_str();
    this->m_s3DViewDemImgFullPath = sDemImgFullPath;

    this->m_pMenuBar->ui->lineEdit_DemImgFullPath->setText(sDemImgFullPathUtf8);
}

void CToolMenuHandler::Clicked_Lab_3DView(){
    if(0 == CMisc::CheckFileExists(this->m_s3DViewDemImgFullPath.c_str())) {
        QString sMsg = CMisc::AnsiToUtf8(this->m_s3DViewDemImgFullPath.c_str()).c_str();
        QMessageBox::information(this->m_pMenuBar, u8"友情提示", sMsg+u8": 路径不存在！！！");
        return;
    }
    QString _sTifImgFullPath = QFileDialog::getOpenFileName(this->m_pMenuBar,
                                                                             tr("File Dialog"),
                                                                             "",
                                                                             tr("Raster")+"(*tif *tiff *img);;"
                                                                             );
    std::string sTifImgFullPath = _sTifImgFullPath.toLocal8Bit().data();
    this->m_s3DViewTifImgFullPath = sTifImgFullPath;
    if(0 == CMisc::CheckFileExists(this->m_s3DViewTifImgFullPath.c_str())) {
        QString sMsg = CMisc::AnsiToUtf8(this->m_s3DViewTifImgFullPath.c_str()).c_str();
        QMessageBox::information(this->m_pMenuBar, u8"友情提示", sMsg+u8": 路径不存在！！！");
        return;
    }

    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    if(pGLCore == nullptr) {
        return;
    }
    CDrawGround* pDrawGround = pGLCore->m_pDrawGround;
    pDrawGround->InitScene(this->m_s3DViewTifImgFullPath.c_str(), this->m_s3DViewDemImgFullPath.c_str());
}

void CToolMenuHandler::Clicked_Lab_ResidualInfo() {

    CLayerManagerWidget* pLayerMan = GetGlobalPtr()->m_pDlgLayerManager;
    pLayerMan->Clicked_AddResidualFile_Slot();
}

int g_DbgFlag = 1;
#define DBG(a) if(g_DbgFlag==1){printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);a;}
void CToolMenuHandler::Clicked_Lab_Residual_SaveFile(){
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerLianJiePoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_LianJie_Points");
    CLayerPoints* pLayerControlPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Control_Points");
    CLayerPoints* pLayerCheckPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Check_Points");
    //CLayerPoints* pLayerTifGeoPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Tif_Quad_Geo");

    DBG();
    if (pLayerLianJiePoint == nullptr || pLayerControlPoint == nullptr || pLayerCheckPoint == nullptr) {
        return;
    }
    DBG();
    CResidualInfo* pResidualTool = (CResidualInfo*)pLayerLianJiePoint->m_pExtData;
    if(pResidualTool == nullptr){
        return;
    }
    DBG();
    class GCP_FILE_ITEM{
    public:
        GCP_FILE_ITEM(const std::string& sDianHao, double fPix_X, double fPix_Y, double fLat, double fLng, double fHeight){
            this->sDianHao = sDianHao;
            this->fPix_X = fPix_X;
            this->fPix_Y = fPix_Y;
            this->fLat = fLat;
            this->fLng = fLng;
            this->fHeight = fHeight;
        }
        std::string sDianHao;
        double fPix_X, fPix_Y, fLat,  fLng, fHeight;
    };
    std::unordered_map<std::string, std::vector<CLianJieDianXiangFangWuCha*>*> mapTifName2LianJieDian;
    std::unordered_map<std::string, std::vector<GCP_FILE_ITEM>*> mapTifName2ControlDian;
    std::unordered_map<std::string, std::vector<GCP_FILE_ITEM>*> mapTifName2CheckDian;

    CRpcAdjustFactor* pRpcAdjustFactor = nullptr;
    std::vector<CLianJieDianXiangFangWuCha*>* pVecLianJieDianXiangFangWuCha = nullptr;
    std::vector<GCP_FILE_ITEM>* pVecControlDianPrecision = nullptr;
    std::vector<GCP_FILE_ITEM>* pVecCheckDianPrecision = nullptr;
    for(auto item : pResidualTool->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor){
        pRpcAdjustFactor = item;
        pVecLianJieDianXiangFangWuCha = new std::vector<CLianJieDianXiangFangWuCha*>;
        mapTifName2LianJieDian.insert(
                    std::pair<std::string, std::vector<CLianJieDianXiangFangWuCha*>*>(pRpcAdjustFactor->m_sTifName, pVecLianJieDianXiangFangWuCha));

        pVecControlDianPrecision = new std::vector<GCP_FILE_ITEM>;
        mapTifName2ControlDian.insert(
                    std::pair<std::string, std::vector<GCP_FILE_ITEM>*>(pRpcAdjustFactor->m_sTifName, pVecControlDianPrecision));

        pVecCheckDianPrecision = new std::vector<GCP_FILE_ITEM>;
        mapTifName2CheckDian.insert(
                    std::pair<std::string, std::vector<GCP_FILE_ITEM>*>(pRpcAdjustFactor->m_sTifName, pVecCheckDianPrecision));
    }

    DBG();
    int nAllCtlChkPointCount = 0;

    CChunk* pChunk = nullptr;
    int nAttrIndx_DianHao_1 = -1;
    int nAttrIndx_TifName1 = -1;
    int nAttrIndx_TifName2 = -1;
    CLianJieDianZuoBiao* pLianJieDianZuoBiao = nullptr;
    if((int)pLayerLianJiePoint->m_vecChunkDatas.size() > 0){
        pChunk = pLayerLianJiePoint->m_vecChunkDatas[0];
        nAttrIndx_DianHao_1 = pChunk->GetAttributeIndxByName("DianHao_1");
        nAttrIndx_TifName1 = pChunk->GetAttributeIndxByName("TifFile1");
        nAttrIndx_TifName2 = pChunk->GetAttributeIndxByName("TifFile2");
        for(auto item : pLayerLianJiePoint->m_vecChunkDatas){
            pChunk = item;
            pLianJieDianZuoBiao = pResidualTool->m_LianJieDianZuoBiaoBlock.m_mapLianJieDianZuoBiao2VecIndx[
                    pChunk->GetAttributeByIndex(nAttrIndx_DianHao_1).GetuLLongValue()];
            pVecLianJieDianXiangFangWuCha = mapTifName2LianJieDian[pChunk->GetAttributeByIndex(nAttrIndx_TifName1).GetStrValue()];
            if(pVecLianJieDianXiangFangWuCha) {
                pVecLianJieDianXiangFangWuCha->push_back(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1);
            }

            pVecLianJieDianXiangFangWuCha = mapTifName2LianJieDian[pChunk->GetAttributeByIndex(nAttrIndx_TifName2).GetStrValue()];
            if(pVecLianJieDianXiangFangWuCha) {
                pVecLianJieDianXiangFangWuCha->push_back(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2);
            }
        }
    }

    DBG(printf("pLayerLianJiePoint->m_vecChunkDatas : %d\n", (int)pLayerLianJiePoint->m_vecChunkDatas.size()););
    //CControlDianPrecision* pControlDianPrecision = nullptr;
    int nAttrIndx_DianHao = -1;
    int nAttrIndx_JingDu = -1;
    int nAttrIndx_WeiDu = -1;
    int nAttrIndx_GaoCheng = -1;
    int nAttrIndx_PixX = -1;
    int nAttrIndx_PixY = -1;

    double fJingDu = 0.0;
    double fWeiDu = 0.0;
    double fGaoCheng = 0.0;
    double fPixX = 0.0;
    double fPixY = 0.0;
    std::string sDianHao;
    if((int)pLayerControlPoint->m_vecChunkDatas.size() > 0){
        pChunk = pLayerControlPoint->m_vecChunkDatas[0];
        nAttrIndx_TifName1 = pChunk->GetAttributeIndxByName("TifFile");

        nAttrIndx_DianHao = pChunk->GetAttributeIndxByName("DianHao");
        nAttrIndx_JingDu = pChunk->GetAttributeIndxByName("JingDu");
        nAttrIndx_WeiDu = pChunk->GetAttributeIndxByName("WeiDu");
        nAttrIndx_GaoCheng = pChunk->GetAttributeIndxByName("GaoCheng");
        nAttrIndx_PixX = pChunk->GetAttributeIndxByName("PixX");
        nAttrIndx_PixY = pChunk->GetAttributeIndxByName("PixY");
        for(auto item : pLayerControlPoint->m_vecChunkDatas){
            pChunk = item;
            //pControlDianPrecision = pResidualTool->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx[
            //        pChunk->GetAttributeByIndex(nAttrIndx_DianHao_1).GetuLLongValue()];
            pVecControlDianPrecision = mapTifName2ControlDian[pChunk->GetAttributeByIndex(nAttrIndx_TifName1).GetStrValue()];
            if(pVecControlDianPrecision) {
                sDianHao = pChunk->GetAttributeByIndex(nAttrIndx_DianHao).GetStrValue();
                fJingDu = pChunk->GetAttributeByIndex(nAttrIndx_JingDu).GetDoubleValue();
                fWeiDu = pChunk->GetAttributeByIndex(nAttrIndx_WeiDu).GetDoubleValue();
                fGaoCheng = pChunk->GetAttributeByIndex(nAttrIndx_GaoCheng).GetDoubleValue();
                fPixX = pChunk->GetAttributeByIndex(nAttrIndx_PixX).GetDoubleValue();
                fPixY = pChunk->GetAttributeByIndex(nAttrIndx_PixY).GetDoubleValue();
                pVecControlDianPrecision->push_back(
                            GCP_FILE_ITEM(sDianHao, fPixX, fPixY, fWeiDu, fJingDu, fGaoCheng));
                nAllCtlChkPointCount++;
            }
        }
    }

    DBG(printf("pLayerControlPoint->m_vecChunkDatas : %d\n", (int)pLayerControlPoint->m_vecChunkDatas.size()););
    //CCheckDianPrecision* pCheckDianPrecision = nullptr;
    if((int)pLayerCheckPoint->m_vecChunkDatas.size() > 0){
        pChunk = pLayerCheckPoint->m_vecChunkDatas[0];
        nAttrIndx_TifName1 = pChunk->GetAttributeIndxByName("TifFile");

        nAttrIndx_DianHao = pChunk->GetAttributeIndxByName("DianHao");
        nAttrIndx_JingDu = pChunk->GetAttributeIndxByName("JingDu");
        nAttrIndx_WeiDu = pChunk->GetAttributeIndxByName("WeiDu");
        nAttrIndx_GaoCheng = pChunk->GetAttributeIndxByName("GaoCheng");
        nAttrIndx_PixX = pChunk->GetAttributeIndxByName("PixX");
        nAttrIndx_PixY = pChunk->GetAttributeIndxByName("PixY");
        for(auto item : pLayerCheckPoint->m_vecChunkDatas){
            pChunk = item;
            //pCheckDianPrecision = pResidualTool->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx[
            //        pChunk->GetAttributeByIndex(nAttrIndx_DianHao_1).GetuLLongValue()];
            pVecCheckDianPrecision = mapTifName2CheckDian[pChunk->GetAttributeByIndex(nAttrIndx_TifName1).GetStrValue()];
            if(pVecCheckDianPrecision) {
                sDianHao = pChunk->GetAttributeByIndex(nAttrIndx_DianHao).GetStrValue();
                fJingDu = pChunk->GetAttributeByIndex(nAttrIndx_JingDu).GetDoubleValue();
                fWeiDu = pChunk->GetAttributeByIndex(nAttrIndx_WeiDu).GetDoubleValue();
                fGaoCheng = pChunk->GetAttributeByIndex(nAttrIndx_GaoCheng).GetDoubleValue();
                fPixX = pChunk->GetAttributeByIndex(nAttrIndx_PixX).GetDoubleValue();
                fPixY = pChunk->GetAttributeByIndex(nAttrIndx_PixY).GetDoubleValue();
                pVecCheckDianPrecision->push_back(
                            GCP_FILE_ITEM(sDianHao, fPixX, fPixY, fWeiDu, fJingDu, fGaoCheng));
                nAllCtlChkPointCount++;
            }
        }
    }

    DBG(printf("pLayerCheckPoint->m_vecChunkDatas : %d\n", (int)pLayerCheckPoint->m_vecChunkDatas.size()););
    int nVecItemCount = 0;
    CLianJieDianXiangFangWuCha* pLianJieDianXiangFangWuCha = nullptr;
    std::string sCurrTime = CMisc::GetCurrTimeString();
    std::string sControlCheckPoints;
    std::vector<std::string> vec_All_Control_Check_Points;

    std::string sGcpFileFullPath = pResidualTool->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor[0]->m_sPxyFileFullPath;
    sGcpFileFullPath = sGcpFileFullPath.substr(0, sGcpFileFullPath.rfind("/")) + "/gcp.gcp";

    if(CMisc::CheckFileExists(sGcpFileFullPath) == 1){
        rename(sGcpFileFullPath.c_str(), (sGcpFileFullPath + sCurrTime).c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        remove(sGcpFileFullPath.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    DBG();
    FILE* pffGcp = fopen(sGcpFileFullPath.c_str(), "w");
    if(pffGcp == nullptr){
        QMessageBox::information(this->m_pMenuBar, u8"友情提示", QString(u8"文件创建失败：") + sGcpFileFullPath.c_str());
        return;
    }
    fprintf(pffGcp, "%d\n", nAllCtlChkPointCount);

    int nProcessingCnt = 0;
    DBG(printf("nAllCtlChkPointCount:%d\n", nAllCtlChkPointCount));
    for(auto item : pResidualTool->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor){
        DBG(printf("done:%d\n", nProcessingCnt++));
        pRpcAdjustFactor = item;
        if(pRpcAdjustFactor == nullptr)
            continue;
        if(CMisc::CheckFileExists(pRpcAdjustFactor->m_sPxyFileFullPath) == 1){
            rename(pRpcAdjustFactor->m_sPxyFileFullPath.c_str(), (pRpcAdjustFactor->m_sPxyFileFullPath+sCurrTime).c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            remove(pRpcAdjustFactor->m_sPxyFileFullPath.c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        FILE* pff = fopen(pRpcAdjustFactor->m_sPxyFileFullPath.c_str(), "w");
        if(pff == nullptr){
            QMessageBox::information(this->m_pMenuBar, u8"友情提示", QString(u8"文件创建失败：")+pRpcAdjustFactor->m_sPxyFileFullPath.c_str());
            continue;
        }
        pVecLianJieDianXiangFangWuCha = mapTifName2LianJieDian[pRpcAdjustFactor->m_sTifName];
        if(pVecLianJieDianXiangFangWuCha == nullptr)
            continue;
        nVecItemCount = (int)pVecLianJieDianXiangFangWuCha->size();
        //DBG();printf("nVecItemCount:%d\n", nVecItemCount);

        pVecControlDianPrecision = mapTifName2ControlDian[pRpcAdjustFactor->m_sTifName];
        if(pVecControlDianPrecision){
            nVecItemCount = nVecItemCount + (int)pVecControlDianPrecision->size();
            //DBG();printf("nVecItemCount:%d\n", nVecItemCount);
        }
        pVecCheckDianPrecision = mapTifName2CheckDian[pRpcAdjustFactor->m_sTifName];
        if(pVecCheckDianPrecision){
            nVecItemCount = nVecItemCount + (int)pVecCheckDianPrecision->size();
            //DBG();printf("nVecItemCount:%d\n", nVecItemCount);
        }

        fprintf(pff, "%d\n", nVecItemCount);

        for(auto item : *pVecLianJieDianXiangFangWuCha){
            if(item == nullptr)
                continue;
            pLianJieDianXiangFangWuCha = item;

            fprintf(pff, "%s\t%f\t%f\n",
                pLianJieDianXiangFangWuCha->m_sDianHao.c_str(),
                pLianJieDianXiangFangWuCha->m_fPix_X,
                pLianJieDianXiangFangWuCha->m_fPix_Y);
        }

        /////////////////////////////////////////////////////////////////////////////
        if(pVecControlDianPrecision) {
            //DBG();
            for(auto item : *pVecControlDianPrecision){
                fprintf(pff, "%s\t%f\t%f\n",
                    item.sDianHao.c_str(),
                    item.fPix_X,
                    item.fPix_Y);

                fprintf(pffGcp, "%s\t%f\t%f\t%f\t1\n",
                    item.sDianHao.c_str(),
                    item.fLat,
                    item.fLng,
                    item.fHeight);
            }
        }

        /////////////////////////////////////////////////////////////////////////////
        if(pVecCheckDianPrecision) {
            //DBG();
            for(auto item : *pVecCheckDianPrecision){

                fprintf(pff, "%s\t%f\t%f\n",
                    item.sDianHao.c_str(),
                    item.fPix_X,
                    item.fPix_Y);

                fprintf(pffGcp, "%s\t%f\t%f\t%f\t0\n",
                    item.sDianHao.c_str(),
                    item.fLat,
                    item.fLng,
                    item.fHeight);
            }
        }

        fclose(pff);
        /////////////////////////////////////////////////////////////////////////////
    }
    fclose(pffGcp);
    DBG();

    for(auto item : pResidualTool->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor){
        pRpcAdjustFactor = item;
        if(item == nullptr)
            continue;

        pVecLianJieDianXiangFangWuCha = mapTifName2LianJieDian[pRpcAdjustFactor->m_sTifName];
        if(pVecLianJieDianXiangFangWuCha) {
            pVecLianJieDianXiangFangWuCha->clear();
            delete pVecLianJieDianXiangFangWuCha;
            pVecLianJieDianXiangFangWuCha = nullptr;
        }

        pVecControlDianPrecision = mapTifName2ControlDian[pRpcAdjustFactor->m_sTifName];
        if(pVecControlDianPrecision) {
            pVecControlDianPrecision->clear();
            delete pVecControlDianPrecision;
            pVecControlDianPrecision = nullptr;
        }
        pVecCheckDianPrecision = mapTifName2CheckDian[pRpcAdjustFactor->m_sTifName];
        if(pVecCheckDianPrecision) {
            pVecCheckDianPrecision->clear();
            delete pVecCheckDianPrecision;
            pVecCheckDianPrecision = nullptr;
        }

        //DBG();
    }

    QMessageBox::information(this->m_pMenuBar, u8"友情提示", QString(u8"文件保存完成") + sGcpFileFullPath.c_str());
}

void CToolMenuHandler::Clicked_Lab_Residual_LianJie_Filter(){
    QString sFilterNum = this->m_pMenuBar->ui->lineEdit_Residual_Threshold_LianJie->text();
    double fFilterNum = sFilterNum.toDouble();

    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_LianJie_Points");
    if(pLayerPoint == nullptr){
        return;
    }

    CLayerFilter* pLayerFilter = pLayerPoint->m_pLayerFilter;
    if(pLayerFilter == nullptr) {
        pLayerFilter = new CLayerFilter;
        pLayerPoint->SetFilter(pLayerFilter);
    }
    pLayerFilter->SetRangeNumFilter(fFilterNum);
    std::vector<int> vecAttrIndx;
    vecAttrIndx.push_back(12);
    vecAttrIndx.push_back(19);
    pLayerPoint->FreshFilter(vecAttrIndx);
    pGLCore->UpdateWidgets();
}

void CToolMenuHandler::Clicked_Lab_Residual_Control_Filter(){
    QString sFilterNum = this->m_pMenuBar->ui->lineEdit_Residual_Threshold_Control->text();
    double fFilterNum = sFilterNum.toDouble();

    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Control_Points");
    if(pLayerPoint == nullptr){
        return;
    }

    CLayerFilter* pLayerFilter = pLayerPoint->m_pLayerFilter;
    if(pLayerFilter == nullptr) {
        pLayerFilter = new CLayerFilter;
        pLayerPoint->SetFilter(pLayerFilter);
    }
    pLayerFilter->SetRangeNumFilter(fFilterNum);
    std::vector<int> vecAttrIndx;
    vecAttrIndx.push_back(8);
    pLayerPoint->FreshFilter(vecAttrIndx);
    pGLCore->UpdateWidgets();
}
void CToolMenuHandler::Clicked_Lab_Residual_Check_Filter(){
    QString sFilterNum = this->m_pMenuBar->ui->lineEdit_Residual_Threshold_Check->text();
    double fFilterNum = sFilterNum.toDouble();

    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Check_Points");
    if(pLayerPoint == nullptr){
        return;
    }

    CLayerFilter* pLayerFilter = pLayerPoint->m_pLayerFilter;
    if(pLayerFilter == nullptr) {
        pLayerFilter = new CLayerFilter;
        pLayerPoint->SetFilter(pLayerFilter);
    }
    pLayerFilter->SetRangeNumFilter(fFilterNum);
    std::vector<int> vecAttrIndx;
    vecAttrIndx.push_back(8);
    pLayerPoint->FreshFilter(vecAttrIndx);
    pGLCore->UpdateWidgets();
}

void CToolMenuHandler::Clicked_Lab_TransTo_CheckPoint(){
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerControlPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Control_Points");
    CLayerPoints* pLayerCheckPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Check_Points");
    if(pLayerControlPoint == nullptr || pLayerCheckPoint == nullptr){
        return;
    }
    pLayerControlPoint->ConvertSelectedChunksToOtherLayer(pLayerCheckPoint);

    pGLCore->UpdateWidgets();
}

void CToolMenuHandler::Clicked_Lab_TransTo_ControlPoint(){
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerControlPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Control_Points");
    CLayerPoints* pLayerCheckPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Check_Points");
    if(pLayerControlPoint == nullptr || pLayerCheckPoint == nullptr){
        return;
    }
    pLayerCheckPoint->ConvertSelectedChunksToOtherLayer(pLayerControlPoint);

    pGLCore->UpdateWidgets();
}

void CToolMenuHandler::Clicked_Lab_Show_PAN_Tif(){
    CGlobal* pGlobal = GetGlobalPtr();
    if(pGlobal->m_vecDockSub.size() != 2){
        return;
    }
    if(pGlobal->m_vecDockSub[0]->isHidden()){
//        pGlobal->m_vecDockSub[0]->setMaximumSize(pGlobal->m_vecDockSub[0]->maximumSize());
//        pGlobal->m_vecDockSub[0]->setWindowFlags(
//                    Qt::Widget
//                    //| Qt::WindowCloseButtonHint
//                    | Qt::WindowMaximizeButtonHint
//                    //| Qt::WindowMinimizeButtonHint
//                                                 );

        pGlobal->m_vecDockSub[0]->show();
        pGlobal->m_vecDockSub[1]->show();

    }else{
        pGlobal->m_vecDockSub[0]->hide();
        pGlobal->m_vecDockSub[1]->hide();
    }
}

void CenterViewByPoint(COpenGLCore* pGLCoreSub, double fLng, double fLat){
    double fGcpPointLng = fLng;
    double fGcpPointLat = fLat;

    static double fNewGeoHeight = 2.0;
    double fPercent20H = fNewGeoHeight / 2.0;
    double fMinLat = fGcpPointLat - fPercent20H;
    double fMaxLat = fGcpPointLat + fPercent20H;

    COpenGLCore* pGLCore = pGLCoreSub;
    double fNewGeoWidth = fNewGeoHeight * pGLCore->m_fGeoWidthHeightScale;
    fNewGeoWidth = fNewGeoWidth / 2.0;

    double fMinLng = fGcpPointLng - fNewGeoWidth;
    double fMaxLng = fGcpPointLng + fNewGeoWidth;

    pGLCore->m_arrGeoMinMax[0] = fMinLng;
    pGLCore->m_arrGeoMinMax[1] = fMinLat;
    pGLCore->m_arrGeoMinMax[2] = fMaxLng;
    pGLCore->m_arrGeoMinMax[3] = fMaxLat;
    pGLCore->InitGeometry();
    pGLCore->UpdateWidgets();
}
void CToolMenuHandler::slotMouseRelease(void *pParam)
{
    if(pParam == nullptr){
        return;
    }
    CGlobal* pGlobal = GetGlobalPtr();
    if(pGlobal->m_vecDockSub.size() != 2){
        return;
    }
    if(pGlobal->m_vecDockSub[0]->isHidden()){
        return;
    }
    static std::unordered_map<std::string, CRasterDraw*> mapTifName2RasterLayer;

    COpenGLCore* pGLCore = pGlobal->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerControlPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_LianJie_Points");
    if(pLayerControlPoint == nullptr){
        return;
    }
    CResidualInfo* pResidualInfo = (CResidualInfo* )pLayerControlPoint->m_pExtData;
    if(pResidualInfo == nullptr)
        return;

    CChunk* pSelectChunk = pLayerControlPoint->GetSelectChunk();
    if(pSelectChunk == nullptr)
        return;

    std::string sTifFile1 = pSelectChunk->GetAttributeByName("TifFile1").GetStrValue();
    std::string sTifFile2 = pSelectChunk->GetAttributeByName("TifFile2").GetStrValue();
    CRpcAdjustFactor* pRpcAdjustFactor1 = pResidualInfo->m_RpcAdjustFactorBlock.m_mapRpcAdjustFactorName2VecIndx[sTifFile1];
    if(pRpcAdjustFactor1 == nullptr)
        return ;

    CMainViewWidgetSub* pWndSub0 = (CMainViewWidgetSub*)pGlobal->m_vecWndSub[0];
    if(pWndSub0 == nullptr){
        return;
    }
    CChunk* pChunk = nullptr;
    CLayerVector* pVecLayer = pWndSub0->m_pGLCore->m_pLayerVector;
    CRasterDraw* pRasterLayer = nullptr;
    CLayerPoints* pPixPtLayer = nullptr;
    //  pWndSub0  //////////////////////////////////////////////////////
    pVecLayer->RemoveAllLayers();
    if(pVecLayer->m_vecLayerItem.size()<=0){
        //std::string sPanTifFileFullPath = "C:/temp/GF1_SaveDir/SrcDataImg/GF1_PMS2_E114.7_N30.7_20200519_L1A0004809649-PAN2.tiff";

        pRasterLayer = (CRasterDraw*)pVecLayer->AddRasterItem(pRpcAdjustFactor1->m_sTifFileFullPath);
        pPixPtLayer = (CLayerPoints*)pVecLayer->CreateEmptyGeoLayer(1);

        pChunk = new CChunk();
        pChunk->m_pBelongLayer = pPixPtLayer;
        pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;
        pChunk->m_vecChunkPoints.push_back(glm::dvec3(0.0, 0.0, 0.0));

        pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(0));

        pChunk->m_chunkBound = glm::dvec4(0.0, 0.0, 0.0, 0.0);
        pPixPtLayer->AddChunk(pChunk);

        //mapTifName2RasterLayer.insert(std::pair<std::string, CRasterDraw*>(sTifFile1, pRasterLayer));

    } else {
        pRasterLayer = (CRasterDraw*)pVecLayer->m_vecLayerItem[0];
        pPixPtLayer = (CLayerPoints*)pVecLayer->m_vecLayerItem[1];

        auto itt = mapTifName2RasterLayer.find(sTifFile1);
        if(itt == mapTifName2RasterLayer.end()){
            pVecLayer->RemoveAllLayers(0);
            pRasterLayer = (CRasterDraw*)pVecLayer->AddRasterItem(pRpcAdjustFactor1->m_sTifFileFullPath);
            pVecLayer->AddLayerItem((CLayerDraw*)pPixPtLayer);
            mapTifName2RasterLayer.insert(std::pair<std::string, CRasterDraw*>(sTifFile1, pRasterLayer));
        }else{
            CRasterDraw* pTmpLayer = itt->second;
            if (pTmpLayer != pRasterLayer){
                pVecLayer->RemoveAllLayers(0);
                pVecLayer->AddLayerItem((CLayerDraw*)pTmpLayer);
                pVecLayer->AddLayerItem((CLayerDraw*)pPixPtLayer);
            }
        }
        pChunk = pPixPtLayer->GetChunkByIndex(0);
    }

    if(pChunk == nullptr)
        return;
    double fPixX1 = pSelectChunk->GetAttributeByName("PixX1").GetDoubleValue();
    double fPixY1 = pSelectChunk->GetAttributeByName("PixY1").GetDoubleValue();

    //pSelectChunk->GetAttributeByName("PixX2").GetDoubleValue();
    //pSelectChunk->GetAttributeByName("PixY2").GetDoubleValue();
    double fLng = fPixX1 *0.009009009009009009;
    double fLat = fPixY1 *-0.009009009009009009;

    pChunk->m_vecChunkPoints[0].x = fLng;
    pChunk->m_vecChunkPoints[0].y = fLat;

    pPixPtLayer->MakeUpChunks();

    //pVecLayer->m_pGLCore->UpdateWidgets();
    CenterViewByPoint(pVecLayer->m_pGLCore, fLng, fLat);


    //  pWndSub1  //////////////////////////////////////////////////////
    CMainViewWidgetSub* pWndSub1 = (CMainViewWidgetSub*)pGlobal->m_vecWndSub[1];
    if(pWndSub1 == nullptr){
        return;
    }
    CRpcAdjustFactor* pRpcAdjustFactor2 = pResidualInfo->m_RpcAdjustFactorBlock.m_mapRpcAdjustFactorName2VecIndx[sTifFile2];
    if(pRpcAdjustFactor2 == nullptr)
        return ;
    pVecLayer = pWndSub1->m_pGLCore->m_pLayerVector;
    pVecLayer->RemoveAllLayers();
    if(pVecLayer->m_vecLayerItem.size()<=0){
        //std::string sPanTifFileFullPath = "C:/temp/GF1_SaveDir/SrcDataImg/GF1_PMS2_E114.7_N30.7_20200519_L1A0004809649-PAN2.tiff";

        pRasterLayer = (CRasterDraw*)pVecLayer->AddRasterItem(pRpcAdjustFactor2->m_sTifFileFullPath);
        pPixPtLayer = (CLayerPoints*)pVecLayer->CreateEmptyGeoLayer(1);

        pChunk = new CChunk();
        pChunk->m_pBelongLayer = pPixPtLayer;
        pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;
        pChunk->m_vecChunkPoints.push_back(glm::dvec3(0.0, 0.0, 0.0));

        pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(0));

        pChunk->m_chunkBound = glm::dvec4(0.0, 0.0, 0.0, 0.0);
        pPixPtLayer->AddChunk(pChunk);

        //mapTifName2RasterLayer.insert(std::pair<std::string, CRasterDraw*>(sTifFile2, pRasterLayer));

    } else {
        pRasterLayer = (CRasterDraw*)pVecLayer->m_vecLayerItem[0];
        pPixPtLayer = (CLayerPoints*)pVecLayer->m_vecLayerItem[1];

        auto itt = mapTifName2RasterLayer.find(sTifFile2);
        if(itt == mapTifName2RasterLayer.end()){
            pVecLayer->RemoveAllLayers(0);
            pRasterLayer = (CRasterDraw*)pVecLayer->AddRasterItem(pRpcAdjustFactor2->m_sTifFileFullPath);
            pVecLayer->AddLayerItem((CLayerDraw*)pPixPtLayer);
            mapTifName2RasterLayer.insert(std::pair<std::string, CRasterDraw*>(sTifFile2, pRasterLayer));
        }else{
            CRasterDraw* pTmpLayer = itt->second;
            if (pTmpLayer != pRasterLayer) {
                pVecLayer->RemoveAllLayers(0);
                pVecLayer->AddLayerItem((CLayerDraw*)pTmpLayer);
                pVecLayer->AddLayerItem((CLayerDraw*)pPixPtLayer);
            }
        }
        pChunk = pPixPtLayer->GetChunkByIndex(0);
    }

    if(pChunk == nullptr)
        return;
    double fPixX2 = pSelectChunk->GetAttributeByName("PixX2").GetDoubleValue();
    double fPixY2 = pSelectChunk->GetAttributeByName("PixY2").GetDoubleValue();

    //pSelectChunk->GetAttributeByName("PixX2").GetDoubleValue();
    //pSelectChunk->GetAttributeByName("PixY2").GetDoubleValue();
    fLng = fPixX2 *0.009009009009009009;
    fLat = fPixY2 *-0.009009009009009009;

    pChunk->m_vecChunkPoints[0].x = fLng;
    pChunk->m_vecChunkPoints[0].y = fLat;

    pPixPtLayer->MakeUpChunks();

    //pVecLayer->m_pGLCore->UpdateWidgets();
    CenterViewByPoint(pVecLayer->m_pGLCore, fLng, fLat);

}
