#include "CLayerResidual.h"
#include "Src_Core/CGeoMath.h"
#include "Src_MatchPoints/ReadResidualFile.h"

#ifdef _DEBUG
    void CheckGLCoreError(const char *file, int line, const char *func, COpenGLCore* pCore);
#   ifdef __FUNCTION__
#       define CHECK_GL_ERROR_1(a) CheckGLCoreError(__FILE__, __LINE__, __FUNCTION__, a)
#   else
#       define CHECK_GL_ERROR_1(a) CheckGLCoreError(__FILE__, __LINE__, "", a)
#   endif
#else
#   define CHECK_GL_ERROR_1(a)// ((void)(0))
#endif

//#include <QProcess>
CLayerResidual::CLayerResidual()
{
    this->m_pGLCore = nullptr;
}

CLayerResidual::CLayerResidual(COpenGLCore* pGLCore)
    :CLayerLines (pGLCore)
{
    this->m_pGLCore = pGLCore;
    this->Init();
}

CLayerResidual::~CLayerResidual()
{

}

void CLayerResidual::Init() {
    this->m_nLayerColor = 0xFF0000FF;
    this->m_nLayerType = ELAYERTYPE::e_shapePolyline;
}

int CLayerResidual::AddChunk(CChunk* pChunk){
    this->m_vecChunkDatas.push_back(pChunk);
    return 1;
}

int CLayerResidual::AddChunk(double fPt0_X, double fPt0_Y, double fPt1_X, double fPt1_Y){

    CChunk* pChunk = new CChunk();
    pChunk->m_pBelongLayer = this;
    pChunk->m_nChunkType = ELAYERTYPE::e_shapePolyline;

    pChunk->m_vecChunkPoints.push_back(glm::dvec3(fPt0_X, fPt0_Y, 0.0));
    pChunk->m_vecChunkPoints.push_back(glm::dvec3(fPt1_X, fPt1_Y, 0.0));

    pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(this->GetChunkCount()));

    pChunk->m_chunkBound = glm::dvec4(0.0, 0.0, 0.0, 0.0);
    this->AddChunk(pChunk);

    const double ROTATE_THETA_0 = 15.0 / 180.0 * PI;
    const double ROTATE_THETA_1 = -15.0 / 180.0 * PI;

    double fNewPointX = 0.0, fNewPointY = 0.0;
    //this->m_vecArrowPoints.push_back({pPoints[0], pPoints[1], 0.0});

    CGeoMath::RotatePointByPoint(fPt0_X, fPt0_Y, fPt1_X, fPt1_Y, ROTATE_THETA_0, &fNewPointX, &fNewPointY, false);

    this->m_vecArrowVerticsIndx.push_back((unsigned int)this->m_vecArrowVertics.size());
    this->m_vecArrowVertics.push_back(glm::dvec3(fPt1_X, fPt1_Y, 0.0));
    this->m_vecArrowVerticsFlag.push_back(0.0f);

    fNewPointX = (fNewPointX - fPt1_X) / 15.0 + fPt1_X;
    fNewPointY = (fNewPointY - fPt1_Y) / 15.0 + fPt1_Y;
    this->m_vecArrowVerticsIndx.push_back((unsigned int)this->m_vecArrowVertics.size());
    this->m_vecArrowVertics.push_back(glm::dvec3(fNewPointX, fNewPointY, 0.0));
    this->m_vecArrowVerticsFlag.push_back(0.0f);

    CGeoMath::RotatePointByPoint(fPt0_X, fPt0_Y, fPt1_X, fPt1_Y, ROTATE_THETA_1, &fNewPointX, &fNewPointY, false);

    this->m_vecArrowVerticsIndx.push_back((unsigned int)this->m_vecArrowVertics.size());
    this->m_vecArrowVertics.push_back(glm::dvec3(fPt1_X, fPt1_Y, 0.0));
    this->m_vecArrowVerticsFlag.push_back(0.0f);

    fNewPointX = (fNewPointX - fPt1_X) / 15.0 + fPt1_X;
    fNewPointY = (fNewPointY - fPt1_Y) / 15.0 + fPt1_Y;
    this->m_vecArrowVerticsIndx.push_back((unsigned int)this->m_vecArrowVertics.size());
    this->m_vecArrowVertics.push_back(glm::dvec3(fNewPointX, fNewPointY, 0.0));
    this->m_vecArrowVerticsFlag.push_back(0.0f);
    return 1;
}

int CLayerResidual::InitChunksByResidualInfo(int nLayerType){
    //nLayerType: 1-lianjiedian,2-controldian,3-checkdian

    int i = 0, j = 0;
    int nPointCount = 0;

    CResidualInfo* pResidualInfo = (CResidualInfo*)this->m_pExtData;
    if(nLayerType == 2){
        CControlDianPrecision* pControlDianPrecision = nullptr;
        CControlDianPrecisionBlock* pControlDianPrecisionBlock = &pResidualInfo->m_ControlDianPrecisionBlock;
        nPointCount = (int)pControlDianPrecisionBlock->m_vecControlDianPrecision.size();
        for(i = 0; i < nPointCount; i++) {
            pControlDianPrecision = pControlDianPrecisionBlock->m_vecControlDianPrecision[i];
            this->AddChunk(pControlDianPrecision->m_fLng,
                           pControlDianPrecision->m_fLat,
                           pControlDianPrecision->m_fAdjLng,
                           pControlDianPrecision->m_fAdjLat);
        }
    }else if(nLayerType == 3){
        CCheckDianPrecision* pCheckDianPrecision = nullptr;
        CCheckDianPrecisionBlock* pCheckDianPrecisionBlock  = &pResidualInfo->m_CheckDianPrecisionBlock;
        nPointCount = (int)pCheckDianPrecisionBlock->m_vecCheckDianPrecision.size();
        for(i = 0; i < nPointCount; i++) {
            pCheckDianPrecision = pCheckDianPrecisionBlock->m_vecCheckDianPrecision[i];
            this->AddChunk(pCheckDianPrecision->m_fLng,
                           pCheckDianPrecision->m_fLat,
                           pCheckDianPrecision->m_fAdjLng,
                           pCheckDianPrecision->m_fAdjLat);
        }
    }

    this->MakeUpChunks();
    return 1;
}

int CLayerResidual::AddChunk(int nPointCount, double* pPoints) {
    //pPoints[0,1,2] //实际的经纬度
    //pPoints[3,4,5] //减掉 x，y 方向的残差后的 新的 经纬度
    if(nPointCount != 2){
        return 0;
    }
    this->AddChunk(pPoints[0], pPoints[1], pPoints[2], pPoints[3]);
    return 1;
}

#include <time.h>
void CLayerResidual::Draw() {

    if(this->m_nShowOrHide == ESHOWORHIDE::e_hide){
        return ;
    }

    int nPointCnt = (int)this->m_pVecPoints->size();
    int nPointEleCnt = (int)this->m_pVecVerticsIndx->size();
    printf("CLayerResidual::Draw()  pointsCnt: %d\n", nPointCnt);
    int nVecPointFlagOperate = (int)this->m_pVecPointFlagOperate->size();
    int nVecPointsIndxOperate = (int)this->m_pVecPointsIndxOperate->size();
    int nVecPointsIndxOperatePoint = (int)this->m_pVecPointsIndxOperatePointNode->size();
    int nVecPointsIndxOperateHit = (int)this->m_pVecPointsIndxOperateHit->size();
    int nVecPointsIndxOperatePointHit = (int)this->m_pVecPointsIndxOperatePointNodeHit->size();
    this->m_pGLCore->DrawLinesElementOperate(nPointCnt, (double*)this->m_pVecPoints->data(),
                                             nPointCnt, (float*)this->m_pVecPointFlag->data(),
                                             nPointEleCnt, (unsigned int*)this->m_pVecVerticsIndx->data(),
                                             nVecPointFlagOperate, (float*)this->m_pVecPointFlagOperate->data(),
                                             nVecPointsIndxOperate, (unsigned int*)this->m_pVecPointsIndxOperate->data(),
                                             nVecPointsIndxOperatePoint, this->m_pVecPointsIndxOperatePointNode->data(),
                                             nVecPointsIndxOperateHit, (unsigned int*)this->m_pVecPointsIndxOperateHit->data(),
                                             nVecPointsIndxOperatePointHit, this->m_pVecPointsIndxOperatePointNodeHit->data(),
                                             this->m_nLayerColor);

    if(this->m_bShowArrow == 0)
        return;
    int nArrowCount = (int)this->m_vecArrowVertics.size();
    this->m_pGLCore->DrawLinesElementOperate(nArrowCount, (double*)this->m_vecArrowVertics.data(),
                                             nArrowCount, (float*)this->m_vecArrowVerticsFlag.data(),
                                             nArrowCount, (unsigned int*)this->m_vecArrowVerticsIndx.data(),
                                             0, nullptr,
                                             0, nullptr,
                                             0, nullptr,
                                             0, nullptr,
                                             0, nullptr,
                                             0xFF00FFFF);
}
