#include "CLayerPoints.h"

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
CLayerPoints::CLayerPoints()
{
    this->m_pGLCore = nullptr;
}

CLayerPoints::CLayerPoints(COpenGLCore* pGLCore)
    :CLayerGeoDraw (pGLCore)
{
    this->m_pGLCore = pGLCore;
    this->Init();
}

CLayerPoints::~CLayerPoints()
{

}

void CLayerPoints::Init() {
    this->m_nLayerType = ELAYERTYPE::e_shapePoint;
}

#include <time.h>
void CLayerPoints::Draw() {

    if(this->m_nShowOrHide == ESHOWORHIDE::e_hide){
        return ;
    }
    time_t ss=clock();
    int nPointCnt = (int)this->m_pVecPoints->size();
    int nPointEleCnt = (int)this->m_pVecVerticsIndx->size();
    printf("SxLayerPoints::Draw() pointsCnt: %d\n", nPointCnt);
    //this->m_pGLCore->DrawPointsElement(nPointCnt, (float*)this->m_pVecPoints->data(), nPointEleCnt, (unsigned int*)this->m_pVecVerticsIndx->data(), 0xFF00FFFF);

    int nVecPointFlagOperate = (int)this->m_pVecPointFlagOperate->size();
    int nVecPointsIndxOperate = (int)this->m_pVecPointsIndxOperate->size();

    int nVecPointsIndxOperateHit = (int)this->m_pVecPointsIndxOperateHit->size();

    this->m_pGLCore->DrawPointsElementOperate(nPointCnt, (double*)this->m_pVecPoints->data(),
                                              nPointCnt, (float*)this->m_pVecPointFlag->data(),
                                              nPointEleCnt, (unsigned int*)this->m_pVecVerticsIndx->data(),
                                              nVecPointFlagOperate, (float*)this->m_pVecPointFlagOperate->data(),
                                              nVecPointsIndxOperate, (unsigned int*)this->m_pVecPointsIndxOperate->data(),
                                              nVecPointsIndxOperateHit, this->m_pVecPointsIndxOperateHit->data(),
                                              this->m_nLayerColor);
    printf("SxLayerPoints::Draw(): %lld\n", clock()-ss);
}

int CLayerPoints::UpdateGeoRanderBySelectRange() {

    double fSelectRangeGeo[4] = {
        (double)this->m_pGLCore->m_arrMouseSelectPoints[0],
        (double)this->m_pGLCore->m_arrMouseSelectPoints[1],
        (double)this->m_pGLCore->m_arrMouseSelectPoints[2],
        (double)this->m_pGLCore->m_arrMouseSelectPoints[3]
    };

    if (fabs(fSelectRangeGeo[0] - fSelectRangeGeo[2]) <= 0.00000001 &&
        fabs(fSelectRangeGeo[1] - fSelectRangeGeo[3]) <= 0.00000001)
    {
        return 0;
    }

    double fLayerBound[4] = {this->m_fMinLng, this->m_fMinLat, this->m_fMaxLng, this->m_fMaxLat};

    int i = 0, nElePointsCnt = (int)this->m_vecChunkDatas.size();
    int nElePointsCntExchange = nElePointsCnt - 1;

    this->ClearOperateChunkState();
    this->m_nOperateEleCnt = 0;
    this->m_pVecEleChunksOperate->clear();

    CChunk* pChunk = nullptr;
    t_Line2D tLineEdge;
    if (1 == this->IsOverlaps(fSelectRangeGeo, fLayerBound)){
        for(i = nElePointsCntExchange; i >= 0; i--) {
            pChunk = this->m_vecChunkDatas[i];
            if(pChunk->m_nDisplayState == ESHOWORHIDE::e_hide){
                continue;
            }
            if (0 == this->IsContainsByPoint(fSelectRangeGeo, glm::value_ptr(pChunk->m_vecChunkPoints.at(0)))){
                continue;
            }

            pChunk->m_nOperateState = ECHUNK_OPERATE_TYPE::e_Chunk_Selected;
            this->m_pVecEleChunksOperate->push_back(i);
        }
    }
    this->m_nOperateEleCnt = this->m_pVecEleChunksOperate->size();

    this->MakeUpChunksState();

    return (int)this->m_nOperateEleCnt;
}

int CLayerPoints::DeleteGeometryBySelectRange() {

    int nLeftDeleteCnt = (int)this->m_pVecEleChunksOperate->size();
    if(nLeftDeleteCnt <= 0){
        return 0;
    }

    CChunk* pChunk = nullptr;
    auto ittBegin = this->m_vecChunkDatas.begin();
    int i = 0, nDeleteIndx = -1;
    for(i = 0; i < nLeftDeleteCnt; i++) {
        nDeleteIndx = this->m_pVecEleChunksOperate->at(i);
        pChunk = this->m_vecChunkDatas[nDeleteIndx];
        this->m_vecChunkDatas.erase(ittBegin + nDeleteIndx);
        delete pChunk;
        pChunk = nullptr;
    }

    this->m_pVecEleChunksOperate->clear();
    this->m_nOperateEleCnt = 0;

    this->MakeUpChunks();

    return 1;
}

int CLayerPoints::DeleteGeometryBySelectRange_bak(){
    int nDelPointCnt = 0;

    int j = 0;
    double fSelectGeoMinX = this->m_pGLCore->m_arrMouseSelectPoints[0];
    double fSelectGeoMinY = this->m_pGLCore->m_arrMouseSelectPoints[1];
    double fSelectGeoMaxX = this->m_pGLCore->m_arrMouseSelectPoints[2];
    double fSelectGeoMaxY = this->m_pGLCore->m_arrMouseSelectPoints[3];

    double fSelectRangeGeo[4] = {
        (double)this->m_pGLCore->m_arrMouseSelectPoints[0],
        (double)this->m_pGLCore->m_arrMouseSelectPoints[1],
        (double)this->m_pGLCore->m_arrMouseSelectPoints[2],
        (double)this->m_pGLCore->m_arrMouseSelectPoints[3]
    };
    if (fabs(fSelectRangeGeo[0] - fSelectRangeGeo[2]) <= 1.0E-12 &&
        fabs(fSelectRangeGeo[1] - fSelectRangeGeo[3]) <= 1.0E-12)
    {
        return 0;
    }

    double fLayerBound[4] = {this->m_fMinLat, this->m_fMinLng, this->m_fMaxLat, this->m_fMaxLng};
    if (0 == this->IsOverlaps(fSelectRangeGeo, fLayerBound)){
        return 0;
    }

    int nPointCnt = 0, nNewPointCnt = 0;
    double fGeoX = 0, fGeoY = 0;

    nPointCnt = (int)this->m_pVecPoints->size();
    if(nPointCnt <= 0){
        return 0;
    }
    std::vector<glm::dvec3>* pVecPoints = new std::vector<glm::dvec3>;
    this->m_pVecVerticsIndx->clear();
    std::vector<unsigned int>* pVecVerticsIndx = this->m_pVecVerticsIndx;
    for(j = 0; j<nPointCnt; j++) {
        fGeoX = this->m_pVecPoints->at(j).x;
        fGeoY = this->m_pVecPoints->at(j).y;
        if (fGeoX >= fSelectGeoMinX && fGeoX <= fSelectGeoMaxX &&
            fGeoY >= fSelectGeoMinY && fGeoY <= fSelectGeoMaxY)
        {
            nDelPointCnt++;
            continue;
        }
        pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, 0.0));
        pVecVerticsIndx->push_back(nNewPointCnt++);
    }
    this->m_pVecPoints->clear();
    delete this->m_pVecPoints;
    this->m_pVecPoints = pVecPoints;
    this->m_pVecVerticsIndx = pVecVerticsIndx;

    return nDelPointCnt;
}

int CLayerPoints::MakeUpChunks() {

    this->InitBody();
    CChunk* pChunk = nullptr;
    double fGeoX = 0.0, fGeoY = 0.0;
    int i = 0, nRealChunkCount = 0, nChunkCount = (int)this->m_vecChunkDatas.size();
    for( ; i < nChunkCount; i++){
        pChunk = this->m_vecChunkDatas[i];
        if( pChunk == nullptr)
            continue;
        pChunk->m_vecChunkPointsIndx.clear();

        if(pChunk->m_nDisplayState == ESHOWORHIDE::e_hide){
            continue;
        }

        if(pChunk->m_vecChunkPoints.size() <= 0)
            continue;

        fGeoX = pChunk->m_vecChunkPoints[0].x;
        fGeoY = pChunk->m_vecChunkPoints[0].y;

        this->m_pVecPoints->push_back(pChunk->m_vecChunkPoints[0]);
        this->m_pVecPointFlag->push_back(0.0f);
        this->m_pVecPointFlagOperate->push_back(0.0f);
        this->m_pVecVerticsIndx->push_back(nRealChunkCount);

        pChunk->m_vecChunkPointsIndx.push_back(nRealChunkCount);

        pChunk->m_nIndxInEleChunks = nRealChunkCount;
        nRealChunkCount++;

        if(this->m_fMinLng > fGeoX) {
            this->m_fMinLng = fGeoX;
        }
        if(this->m_fMaxLng < fGeoX) {
            this->m_fMaxLng = fGeoX;
        }
        if(this->m_fMinLat > fGeoY) {
            this->m_fMinLat = fGeoY;
        }
        if(this->m_fMaxLat < fGeoY) {
            this->m_fMaxLat = fGeoY;
        }
    }
    return 1;
}

int CLayerPoints::MakeUpChunksState() {

    CChunk* pChunk = nullptr;

    int nOperateChunksCount = (int)this->m_pVecEleChunksOperate->size();
    int i = 0, j = 0, nChunkOperateIndx = 0, nPointOperateIndx = 0, nChunkPointCount = 0;

    this->m_pVecPointsIndxOperate->clear();
    this->m_pVecPointsIndxOperatePointNode->clear();
    memset(this->m_pVecPointFlagOperate->data(), 0, sizeof(float)*this->m_pVecPointFlagOperate->size());

    for( ; i < nOperateChunksCount; i++){
        nChunkOperateIndx = this->m_pVecEleChunksOperate->at(i);
        pChunk = this->m_vecChunkDatas[nChunkOperateIndx];
        if( pChunk == nullptr)
            continue;
        nChunkPointCount = (int)pChunk->m_vecChunkPointsIndx.size();
        if(nChunkPointCount <= 0)
            continue;
        nChunkPointCount = nChunkPointCount - 1;
        nPointOperateIndx = pChunk->m_vecChunkPointsIndx[nChunkPointCount];
        this->m_pVecPointsIndxOperate->push_back(nPointOperateIndx);
        this->m_pVecPointFlagOperate->at(nPointOperateIndx) = 2.0f;
        this->m_pVecPointsIndxOperatePointNode->push_back(nPointOperateIndx);
    }
    return 1;
}

/*
fState : 2.0f - selected， 3.0f - freshState，
*/
void CLayerPoints::SetFreshState(CChunk* pChunk, float fState){
    if( pChunk == nullptr)
        return;
    int nChunkPointCount = (int)pChunk->m_vecChunkPointsIndx.size();
    if(nChunkPointCount <= 0)
        return;
    this->m_pVecPointsIndxOperateHit->clear();
    //this->m_pVecPointsIndxOperatePointNodeHit->clear();

    nChunkPointCount = nChunkPointCount - 1;
    int nPointOperateIndx = pChunk->m_vecChunkPointsIndx[nChunkPointCount];
    this->m_pVecPointsIndxOperateHit->push_back(nPointOperateIndx);
    this->m_pVecPointFlagOperate->at(nPointOperateIndx) = fState;
    //this->m_pVecPointsIndxOperatePointNodeHit->push_back(nPointOperateIndx);
}

void CLayerPoints::SetFresh(CChunk* pChunk){
    if(this->m_pHitChunk == pChunk)
        return;
    this->SetFreshState(this->m_pHitChunk, 2.0f);
    this->SetFreshState(pChunk, 3.0f);
    this->m_pHitChunk = pChunk;
    this->m_pGLCore->UpdateWidgets();
}
