#include "CLayerLines.h"

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
CLayerLines::CLayerLines()
{
    this->m_pGLCore = nullptr;

}

CLayerLines::CLayerLines(COpenGLCore* pGLCore)
    :CLayerGeoDraw (pGLCore)
{
    this->m_pGLCore = pGLCore;

    this->Init();

}

CLayerLines::~CLayerLines()
{

}

void CLayerLines::Init() {

    this->m_nLayerType = ELAYERTYPE::e_shapePolyline;
}

void CLayerLines::Draw() {

    this->m_pRelationLayer->m_nShowOrHide = this->m_nShowOrHide;
    if(this->m_nShowOrHide == ESHOWORHIDE::e_hide){
        return ;
    }

    //this->m_pGLCore->DrawLines(5, arrPointsEle, 0xFF0000FF);
    //this->m_pGLCore->DrawLinesElement(5, arrGeoPointsEle, 5, arrPointsEleIndx, 0xFF00FFFF);
    int nPointCnt = (int)this->m_pVecPoints->size();
    int nPointEleCnt = (int)this->m_pVecVerticsIndx->size();

    printf("SxLayerLines::Draw()  pointsCnt: %d\n", nPointCnt);

    //nPointCnt = (nPointCnt >> 1);
    //nPointEleCnt = (nPointEleCnt >> 1);
    //this->m_pGLCore->DrawLines(nPointCnt, pViewPoing, 0xFF0000FF);
    //this->m_pGLCore->DrawLinesElement(nPointCnt, (float*)this->m_pVecPoints->data(), nPointEleCnt, (unsigned int*)this->m_pVecVerticsIndx->data(), 0xFF00FFFF);

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
}

int CLayerLines::UpdateGeoRanderBySelectRange() {

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

    double fLayerBound[4] = {this->m_fMinLng, this->m_fMinLat, this->m_fMaxLng, this->m_fMaxLat};

    t_Line2D tLineLeft(fSelectRangeGeo[0], fSelectRangeGeo[3], fSelectRangeGeo[0], fSelectRangeGeo[1]);
    t_Line2D tLineBottom(fSelectRangeGeo[0], fSelectRangeGeo[1], fSelectRangeGeo[2], fSelectRangeGeo[1]);
    t_Line2D tLineRight(fSelectRangeGeo[2], fSelectRangeGeo[1], fSelectRangeGeo[2], fSelectRangeGeo[3]);
    t_Line2D tLineTop(fSelectRangeGeo[0], fSelectRangeGeo[3], fSelectRangeGeo[2], fSelectRangeGeo[3]);

    int i0 = 0, nChunkPtIndxStart = 0, nChunksPointCnt = 0, nIntersect = 0;
    int i = 0, nEleChunksCnt = (int)this->m_vecChunkDatas.size();
    int nEleChunksCntExchange = nEleChunksCnt - 1;

    this->ClearOperateChunkState();
    this->m_nOperateEleCnt = 0;
    this->m_pVecEleChunksOperate->clear();

    CChunk* pChunk = nullptr;
    t_Line2D tLineEdge;
    if (1 == this->IsOverlaps(fSelectRangeGeo, fLayerBound)){
        for(i = nEleChunksCntExchange; i >= 0; i--) {

            pChunk = this->m_vecChunkDatas.at(i);
            if (0 == this->IsOverlaps(fSelectRangeGeo, glm::value_ptr(pChunk->m_chunkBound))){
                continue;
            }

            nChunksPointCnt = (int)pChunk->m_vecChunkPoints.size();
            if(nChunksPointCnt < 2){
                continue;
            }
            if(1 == this->IsContains(fSelectRangeGeo, glm::value_ptr(pChunk->m_chunkBound))){
                goto l_FindIntersect;
            } else {
                nChunkPtIndxStart = 0;
                nIntersect = 0;
                for (i0 = 0; i0 < nChunksPointCnt - 1; i0++) {
                    glm::dvec3& vecPoint0 = pChunk->m_vecChunkPoints.at(i0);
                    glm::dvec3& vecPoint1 = pChunk->m_vecChunkPoints.at(i0 + 1);
                    tLineEdge.SetPoints((double)vecPoint0.x, (double)vecPoint0.y, (double)vecPoint1.x, (double)vecPoint1.y);
                    if(this->IntersectionLine(tLineLeft, tLineEdge) == 1){
                        nIntersect = 1;
                        break;
                    }
                    if(this->IntersectionLine(tLineBottom, tLineEdge) == 1){
                        nIntersect = 1;
                        break;
                    }
                    if(this->IntersectionLine(tLineRight, tLineEdge) == 1){
                        nIntersect = 1;
                        break;
                    }
                    if(this->IntersectionLine(tLineTop, tLineEdge) == 1){
                        nIntersect = 1;
                        break;
                    }
                }
                if(nIntersect == 0){
                    continue;
                }
            }
        l_FindIntersect:
            pChunk->m_nOperateState = ECHUNK_OPERATE_TYPE::e_Chunk_Selected;

            this->m_pVecEleChunksOperate->push_back(i);
        }
    }
    this->m_nOperateEleCnt = this->m_pVecEleChunksOperate->size();

    this->MakeUpChunksState();

    return (int)this->m_nOperateEleCnt;
}

int CLayerLines::DeleteGeometryBySelectRange(){

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

int CLayerLines::MakeUpChunks() {

    CChunk* pChunk = nullptr;
    this->InitBody();

    double fEnvelopeMinLng = 999999.0;
    double fEnvelopeMinLat = 999999.0;
    double fEnvelopeMaxLng = -999999.0;
    double fEnvelopeMaxLat = -999999.0;
    double fGeoX = 0.0, fGeoY = 0.0;


    int i = 0, j = 0, nChunkPointCount = 0, nAllPointCount = 0, nRealChunkCount = 0, nChunkCount = (int)this->m_vecChunkDatas.size();
    for( ; i < nChunkCount; i++){
        pChunk = this->m_vecChunkDatas[i];
        if( pChunk == nullptr)
            continue;
        pChunk->m_vecChunkPointsIndx.clear();

        nChunkPointCount = (int)pChunk->m_vecChunkPoints.size();
        if(nChunkPointCount <= 0)
            continue;

        fEnvelopeMinLng = 999999.0;
        fEnvelopeMinLat = 999999.0;
        fEnvelopeMaxLng = -999999.0;
        fEnvelopeMaxLat = -999999.0;

        nChunkPointCount = nChunkPointCount - 1;
        for(j = 0; j<nChunkPointCount; j++){
            this->m_pVecPoints->push_back(pChunk->m_vecChunkPoints[j]);
            this->m_pVecPointFlag->push_back(0.0f);
            this->m_pVecPointFlagOperate->push_back(0.0f);
            this->m_pVecVerticsIndx->push_back(nAllPointCount);
            this->m_pVecVerticsIndx->push_back(nAllPointCount + 1);

            pChunk->m_vecChunkPointsIndx.push_back(nAllPointCount);
            nAllPointCount++;

            fGeoX = pChunk->m_vecChunkPoints[j].x;
            fGeoY = pChunk->m_vecChunkPoints[j].y;
            if(fEnvelopeMinLng > fGeoX) {
                fEnvelopeMinLng = fGeoX;
            }
            if(fEnvelopeMaxLng < fGeoX) {
                fEnvelopeMaxLng = fGeoX;
            }
            if(fEnvelopeMinLat > fGeoY) {
                fEnvelopeMinLat = fGeoY;
            }
            if(fEnvelopeMaxLat < fGeoY) {
                fEnvelopeMaxLat = fGeoY;
            }
        }
        fGeoX = pChunk->m_vecChunkPoints[nChunkPointCount].x;
        fGeoY = pChunk->m_vecChunkPoints[nChunkPointCount].y;
        if(fEnvelopeMinLng > fGeoX) {
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY) {
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }

        this->m_pVecPoints->push_back(pChunk->m_vecChunkPoints[nChunkPointCount]);
        this->m_pVecPointFlag->push_back(0.0f);
        this->m_pVecPointFlagOperate->push_back(0.0f);

        pChunk->m_vecChunkPointsIndx.push_back(nAllPointCount);
        nAllPointCount++;

        pChunk->m_nIndxInEleChunks = nRealChunkCount;
        nRealChunkCount++;

        pChunk->m_chunkBound = glm::dvec4(fEnvelopeMinLng, fEnvelopeMinLat, fEnvelopeMaxLng, fEnvelopeMaxLat);

        if(this->m_fMinLng > fEnvelopeMinLng){
            this->m_fMinLng = fEnvelopeMinLng;
        }
        if(this->m_fMaxLng < fEnvelopeMaxLng) {
            this->m_fMaxLng = fEnvelopeMaxLng;
        }
        if(this->m_fMinLat > fEnvelopeMinLat){
            this->m_fMinLat = fEnvelopeMinLat;
        }
        if(this->m_fMaxLat < fEnvelopeMaxLat) {
            this->m_fMaxLat = fEnvelopeMaxLat;
        }
    }
    return 1;
}


int CLayerLines::MakeUpChunksState(){
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

        for(j = 0; j<nChunkPointCount; j++) {
            nPointOperateIndx = pChunk->m_vecChunkPointsIndx[j];
            this->m_pVecPointsIndxOperate->push_back(nPointOperateIndx);
            this->m_pVecPointsIndxOperate->push_back(pChunk->m_vecChunkPointsIndx[j + 1]);

            this->m_pVecPointFlagOperate->at(nPointOperateIndx) = 2.0f;
            this->m_pVecPointsIndxOperatePointNode->push_back(nPointOperateIndx);
        }
        nPointOperateIndx = pChunk->m_vecChunkPointsIndx[nChunkPointCount];
        this->m_pVecPointFlagOperate->at(nPointOperateIndx) = 2.0f;
        this->m_pVecPointsIndxOperatePointNode->push_back(nPointOperateIndx);
    }
    return 1;
}

/*
fState : 2.0f - selected， 3.0f - freshState，
*/
void CLayerLines::SetFreshState(CChunk* pChunk, float fState){
    if( pChunk == nullptr)
        return;
    int nChunkPointCount = (int)pChunk->m_vecChunkPointsIndx.size();
    if(nChunkPointCount <= 0)
        return;

    this->m_pVecPointsIndxOperateHit->clear();
    this->m_pVecPointsIndxOperatePointNodeHit->clear();

    nChunkPointCount = nChunkPointCount - 1;
    int nPointOperateIndx = 0;
    for(int j = 0; j<nChunkPointCount; j++) {
        nPointOperateIndx = pChunk->m_vecChunkPointsIndx[j];
        this->m_pVecPointsIndxOperateHit->push_back(nPointOperateIndx);
        this->m_pVecPointsIndxOperateHit->push_back(pChunk->m_vecChunkPointsIndx[j + 1]);

        this->m_pVecPointFlagOperate->at(nPointOperateIndx) = fState;
        this->m_pVecPointsIndxOperatePointNodeHit->push_back(nPointOperateIndx);
    }
    nPointOperateIndx = pChunk->m_vecChunkPointsIndx[nChunkPointCount];
    this->m_pVecPointFlagOperate->at(nPointOperateIndx) = fState;
    this->m_pVecPointsIndxOperatePointNodeHit->push_back(nPointOperateIndx);
}

void CLayerLines::SetFresh(CChunk* pChunk){
    if(this->m_pHitChunk == pChunk)
        return;
    this->SetFreshState(this->m_pHitChunk, 2.0f);
    this->SetFreshState(pChunk, 3.0f);
    this->m_pHitChunk = pChunk;
    this->m_pGLCore->UpdateWidgets();
}
