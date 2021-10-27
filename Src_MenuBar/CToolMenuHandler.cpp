#include "CToolMenuHandler.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QPushButton>
#include <QtCore/QTextCodec>

#include <thread>

#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtGui/QIcon>

#include "gdal/ogrsf_frmts.h"
#include "gdal/gdal_priv.h"

#include "Src_Core/Misc.h"
#include "Src_Core/CGlobal.h"
#include "ui_CMenuBar.h"
#include "Src_MenuBar/CMenuBar.h"
#include "Src_MenuBar/CMenuItemWidget.h"
#include "Src_GlWnd/COpenGLCore.h"
#include "Src_Geometry/CLayerVector.h"
#include "Src_Geometry/CDrawGround.h"
#include "Src_Geometry/CLayerPoints.h"
#include "Src_Attribute/CChunkAttribute.h"
#include "Src_LayerManager/CLayerManagerWidget.h"
#include "Src_MatchPoints/ReadResidualFile.h"


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

void CToolMenuHandler::Clicked_Lab_Residual_DelPoints(){
    QString sFilterNum = this->m_pMenuBar->ui->lineEdit_Residual_Threshold->text();
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

void CToolMenuHandler::Clicked_Lab_Residual_SaveFile(){
    COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
    CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
    CLayerPoints* pLayerLianJiePoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_LianJie_Points");
    CLayerPoints* pLayerControlPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Control_Points");
    CLayerPoints* pLayerCheckPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Residual_Check_Points");
    //CLayerPoints* pLayerTifGeoPoint = (CLayerPoints*)pLayerVector->FindLayerByName("Tif_Quad_Geo");

    if (pLayerLianJiePoint == nullptr || pLayerControlPoint == nullptr || pLayerCheckPoint == nullptr) {
        return;
    }

    CResidualInfo* pResidualTool = (CResidualInfo*)pLayerLianJiePoint->m_pExtData;
    if(pResidualTool == nullptr){
        return;
    }

    std::unordered_map<std::string, std::vector<CLianJieDianXiangFangWuCha*>*> mapTifName2LianJieDian;
    std::unordered_map<std::string, std::vector<CControlDianPrecision*>*> mapTifName2ControlDian;
    std::unordered_map<std::string, std::vector<CCheckDianPrecision*>*> mapTifName2CheckDian;

    CRpcAdjustFactor* pRpcAdjustFactor = nullptr;
    std::vector<CLianJieDianXiangFangWuCha*>* pVecLianJieDianXiangFangWuCha = nullptr;
    std::vector<CControlDianPrecision*>* pVecControlDianPrecision = nullptr;
    std::vector<CCheckDianPrecision*>* pVecCheckDianPrecision = nullptr;
    for(auto item : pResidualTool->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor){
        pRpcAdjustFactor = item;
        pVecLianJieDianXiangFangWuCha = new std::vector<CLianJieDianXiangFangWuCha*>;
        mapTifName2LianJieDian.insert(
                    std::pair<std::string, std::vector<CLianJieDianXiangFangWuCha*>*>(pRpcAdjustFactor->m_sTifName, pVecLianJieDianXiangFangWuCha));

        pVecControlDianPrecision = new std::vector<CControlDianPrecision*>;
        mapTifName2ControlDian.insert(
                    std::pair<std::string, std::vector<CControlDianPrecision*>*>(pRpcAdjustFactor->m_sTifName, pVecControlDianPrecision));

        pVecCheckDianPrecision = new std::vector<CCheckDianPrecision*>;
        mapTifName2CheckDian.insert(
                    std::pair<std::string, std::vector<CCheckDianPrecision*>*>(pRpcAdjustFactor->m_sTifName, pVecCheckDianPrecision));
    }

    int nAllCtlChkPointCount = 0;

    CLianJieDianZuoBiao* pLianJieDianZuoBiao = nullptr;
    CChunk* pChunk = pLayerLianJiePoint->m_vecChunkDatas[0];
    int nAttrIndx_DianHao_1 = pChunk->GetAttributeIndxByName("DianHao_1");
    int nAttrIndx_TifName1 = pChunk->GetAttributeIndxByName("TifFile1");
    int nAttrIndx_TifName2 = pChunk->GetAttributeIndxByName("TifFile2");
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

    CControlDianPrecision* pControlDianPrecision = nullptr;
    pChunk = pLayerControlPoint->m_vecChunkDatas[0];
    nAttrIndx_DianHao_1 = pChunk->GetAttributeIndxByName("DianHao_1");
    nAttrIndx_TifName1 = pChunk->GetAttributeIndxByName("TifFile");
    for(auto item : pLayerControlPoint->m_vecChunkDatas){
        pChunk = item;
        pControlDianPrecision = pResidualTool->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx[
                pChunk->GetAttributeByIndex(nAttrIndx_DianHao_1).GetuLLongValue()];
        pVecControlDianPrecision = mapTifName2ControlDian[pChunk->GetAttributeByIndex(nAttrIndx_TifName1).GetStrValue()];
        if(pVecControlDianPrecision) {
            pVecControlDianPrecision->push_back(pControlDianPrecision);
            nAllCtlChkPointCount++;
        }
    }

    CCheckDianPrecision* pCheckDianPrecision = nullptr;
    pChunk = pLayerCheckPoint->m_vecChunkDatas[0];
    nAttrIndx_DianHao_1 = pChunk->GetAttributeIndxByName("DianHao_1");
    nAttrIndx_TifName1 = pChunk->GetAttributeIndxByName("TifFile");
    for(auto item : pLayerCheckPoint->m_vecChunkDatas){
        pChunk = item;
        pCheckDianPrecision = pResidualTool->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx[
                pChunk->GetAttributeByIndex(nAttrIndx_DianHao_1).GetuLLongValue()];
        pVecCheckDianPrecision = mapTifName2CheckDian[pChunk->GetAttributeByIndex(nAttrIndx_TifName1).GetStrValue()];
        if(pVecCheckDianPrecision) {
            pVecCheckDianPrecision->push_back(pCheckDianPrecision);
            nAllCtlChkPointCount++;
        }
    }

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

    FILE* pffGcp = fopen(sGcpFileFullPath.c_str(), "w");
    if(pffGcp == nullptr){
        QMessageBox::information(this->m_pMenuBar, u8"友情提示", QString(u8"文件创建失败：") + sGcpFileFullPath.c_str());
        return;
    }
    fprintf(pffGcp, "%d\n", nAllCtlChkPointCount);

    for(auto item : pResidualTool->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor){
        pRpcAdjustFactor = item;

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

        nVecItemCount = (int)pVecLianJieDianXiangFangWuCha->size();
        fprintf(pff, "%d\n", nVecItemCount);

        for(auto item : *pVecLianJieDianXiangFangWuCha){
            pLianJieDianXiangFangWuCha = item;

            fprintf(pff, "%s\t%f\t%f\n",
                pLianJieDianXiangFangWuCha->m_sDianHao.c_str(),
                pLianJieDianXiangFangWuCha->m_fPix_X,
                pLianJieDianXiangFangWuCha->m_fPix_Y);
        }

        /////////////////////////////////////////////////////////////////////////////
        pVecControlDianPrecision = mapTifName2ControlDian[pRpcAdjustFactor->m_sTifName];
        for(auto item : *pVecControlDianPrecision){
            pControlDianPrecision = item;

            fprintf(pff, "%s\t%f\t%f\n",
                pControlDianPrecision->m_sDianHao.c_str(),
                pControlDianPrecision->m_pControlDianXiangFangWuCha->m_fPix_X,
                pControlDianPrecision->m_pControlDianXiangFangWuCha->m_fPix_Y);

            fprintf(pffGcp, "%s\t%f\t%f\t%f\t1\n",
                pControlDianPrecision->m_sDianHao.c_str(),
                pControlDianPrecision->m_fLng,
                pControlDianPrecision->m_fLat,
                pControlDianPrecision->m_fHeight);
        }


        /////////////////////////////////////////////////////////////////////////////
        pVecCheckDianPrecision = mapTifName2CheckDian[pRpcAdjustFactor->m_sTifName];
        for(auto item : *pVecCheckDianPrecision){
            pCheckDianPrecision = item;

            fprintf(pff, "%s\t%f\t%f\n",
                pCheckDianPrecision->m_sDianHao.c_str(),
                pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_fPix_X,
                pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_fPix_Y);

            fprintf(pffGcp, "%s\t%f\t%f\t%f\t0\n",
                pCheckDianPrecision->m_sDianHao.c_str(),
                pCheckDianPrecision->m_fLng,
                pCheckDianPrecision->m_fLat,
                pCheckDianPrecision->m_fHeight);
        }

        fclose(pff);
        /////////////////////////////////////////////////////////////////////////////
    }
    fclose(pffGcp);

    for(auto item : pResidualTool->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor){
        pRpcAdjustFactor = item;

        pVecLianJieDianXiangFangWuCha = mapTifName2LianJieDian[pRpcAdjustFactor->m_sTifName];
        pVecLianJieDianXiangFangWuCha->clear();
        delete pVecLianJieDianXiangFangWuCha;
        pVecLianJieDianXiangFangWuCha = nullptr;

        pVecControlDianPrecision = mapTifName2ControlDian[pRpcAdjustFactor->m_sTifName];
        pVecControlDianPrecision->clear();
        delete pVecControlDianPrecision;
        pVecControlDianPrecision = nullptr;

        pVecCheckDianPrecision = mapTifName2CheckDian[pRpcAdjustFactor->m_sTifName];
        pVecCheckDianPrecision->clear();
        delete pVecCheckDianPrecision;
        pVecCheckDianPrecision = nullptr;

    }
}
