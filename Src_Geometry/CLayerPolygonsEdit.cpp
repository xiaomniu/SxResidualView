#include "CLayerPolygonsEdit.h"

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
CLayerPolygonsEdit::CLayerPolygonsEdit()
{
    this->m_pGLCore = nullptr;
    this->Init();
}

CLayerPolygonsEdit::CLayerPolygonsEdit(COpenGLCore* pGLCore)
    :CLayerGeoDraw (pGLCore)
{
    this->m_pGLCore = pGLCore;

    this->Init();
}

CLayerPolygonsEdit::~CLayerPolygonsEdit()
{
    if(this->m_pHitParam){
        delete this->m_pHitParam;
        this->m_pHitParam = nullptr;
    }
    //if(this->m_pSxChunk) {//不在这释放
    //    delete this->m_pSxChunk;
    //    this->m_pSxChunk = nullptr;
    //}
}

void CLayerPolygonsEdit::Init() {
    this->m_nLayerColor = 0xFF0000FF;
    this->m_pHitParam = new CHitParam;
    this->m_nLayerType = ELAYERTYPE::e_shapePolygon;
}

void CLayerPolygonsEdit::Draw() {
    if(this->m_nShowOrHide == ESHOWORHIDE::e_hide) {
        return ;
    }
    //if(this->m_fMinLng > this->m_pGLCore->m_nGeoMousePosX ||
    //    this->m_fMinLat > this->m_pGLCore->m_nGeoMousePosY ||
    //    this->m_fMaxLng < this->m_pGLCore->m_nGeoMousePosX ||
    //    this->m_fMaxLat > this->m_pGLCore->m_nGeoMousePosY){
    //    return ;
    //}
    time_t ss=clock();
    int nPointCnt = (int)this->m_pVecPoints->size();
    int nPointEleCnt = (int)this->m_pVecVerticsIndx->size();
    //nPointEleCnt = (nPointEleCnt >> 1);
    int nVecPointFlagOperate = (int)this->m_pVecPointFlagOperate->size();
    int nVecPointsIndxOperate = (int)this->m_pVecPointsIndxOperate->size();
    int nVecPointsIndxOperatePoint = (int)this->m_pVecPointsIndxOperatePointNode->size();
    int nVecPointsIndxOperateHit = (int)this->m_pVecPointsIndxOperateHit->size();
    int nVecPointsIndxOperatePointHit = (int)this->m_pVecPointsIndxOperatePointNodeHit->size();
    //printf("SxLayerPolygonsEdit::Draw()  pointsCnt: %d\n", nPointCnt);
    //this->m_pGLCore->DrawLinesElementEx(nPointCnt, (float*)this->m_pVecPoints->data(), nPointCnt, (float*)this->m_pVecPointFlag->data(), nPointEleCnt, (unsigned int*)this->m_pVecVerticsIndx->data(), 0xFF00FFFF);
    this->m_pGLCore->DrawLinesElementOperate(nPointCnt, (double*)this->m_pVecPoints->data(),
                                             nPointCnt, (float*)this->m_pVecPointFlag->data(),
                                             nPointEleCnt, (unsigned int*)this->m_pVecVerticsIndx->data(),
                                             nVecPointFlagOperate, (float*)this->m_pVecPointFlagOperate->data(),
                                             nVecPointsIndxOperate, (unsigned int*)this->m_pVecPointsIndxOperate->data(),
                                             nVecPointsIndxOperatePoint, this->m_pVecPointsIndxOperatePointNode->data(),
                                             nVecPointsIndxOperateHit, (unsigned int*)this->m_pVecPointsIndxOperateHit->data(),
                                             nVecPointsIndxOperatePointHit, this->m_pVecPointsIndxOperatePointNodeHit->data(),
                                             this->m_nLayerColor);
    //printf("SxLayerPolygonsEdit::Draw(): %lld\n", clock()-ss);
}

int CLayerPolygonsEdit::UpdateGeoRanderBySelectRange() {

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
    t_Line2D tLineLeft(fSelectRangeGeo[0], fSelectRangeGeo[3], fSelectRangeGeo[0], fSelectRangeGeo[1]);
    t_Line2D tLineBottom(fSelectRangeGeo[0], fSelectRangeGeo[1], fSelectRangeGeo[2], fSelectRangeGeo[1]);
    t_Line2D tLineRight(fSelectRangeGeo[2], fSelectRangeGeo[1], fSelectRangeGeo[2], fSelectRangeGeo[3]);
    t_Line2D tLineTop(fSelectRangeGeo[0], fSelectRangeGeo[3], fSelectRangeGeo[2], fSelectRangeGeo[3]);

    //memset(this->m_pVecPointFlag->data(), 0, this->m_pVecPointFlag->size()* sizeof(float));
    int i0 = 0, nChunkPtIndx = 0, nChunkPtIndxStart = 0, nChunksPointCnt = 0, nChunksPointCnt_Sub1 = 0, nIntersect = 0;
    int i = 0, nEleChunksCnt = (int)this->m_vecChunkDatas.size();
    int nEleChunksCntExchange = nEleChunksCnt - 1;

    this->m_nOperateEleCnt = 0;
    this->m_pVecEleChunksOperate->clear();
    this->m_pVecPointsIndxOperate->clear();
    this->m_pVecPointsIndxOperatePointNode->clear();
    memset(this->m_pVecPointFlagOperate->data(), 0, sizeof(float)*this->m_pVecPointFlagOperate->size());

    CChunk* pChunk = nullptr;
    t_Line2D tLineEdge;
    for(i = nEleChunksCntExchange; i >= 0; i--) {

        pChunk = this->m_vecChunkDatas.at(i);
        nChunksPointCnt = (int)pChunk->m_vecChunkPoints.size();
        if(nChunksPointCnt < 3){
            continue;
        }
        if (0 == this->IsOverlaps(fSelectRangeGeo, glm::value_ptr(pChunk->m_chunkBound))){
            continue;
        }
        if(1 == this->IsContains(fSelectRangeGeo, glm::value_ptr(pChunk->m_chunkBound))){
            goto l_SxLayerPolygonsEdit_UpdateGeoRanderBySelectRange_FindIntersect;
        }else{
            //glm::vec3& vecPointStart = this->m_pVecPoints->at(vecChunkIndx[0]);
            nChunkPtIndxStart = 0;
            nChunksPointCnt_Sub1 = nChunksPointCnt - 1;
            nIntersect = 0;
            {
                glm::dvec3& vecPoint0 = pChunk->m_vecChunkPoints.at(nChunkPtIndx);
                glm::dvec3& vecPoint1 = pChunk->m_vecChunkPoints.at(nChunksPointCnt_Sub1);
                //glm::vec3& vecPoint1 = this->m_pVecPoints->at(());
                tLineEdge.SetPoints((double)vecPoint0.x, (double)vecPoint0.y, (double)vecPoint1.x, (double)vecPoint1.y);
                if ((this->IntersectionLine(tLineLeft, tLineEdge) == 1) ||
                    (this->IntersectionLine(tLineBottom, tLineEdge) == 1) ||
                    (this->IntersectionLine(tLineRight, tLineEdge) == 1) ||
                    (this->IntersectionLine(tLineTop, tLineEdge) == 1))
                {
                    nIntersect = 1;
                }
            }
            if(nIntersect != 1) {
                for (i0 = 0; i0 < nChunksPointCnt - 1; i0++) {
                    glm::dvec3& vecPoint0 = pChunk->m_vecChunkPoints.at(i0);
                    glm::dvec3& vecPoint1 = pChunk->m_vecChunkPoints.at(i0 + 1);
                    //glm::vec3& vecPoint1 = this->m_pVecPoints->at(());
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
            }
            if(nIntersect == 0){
                continue;
            }
        }
    l_SxLayerPolygonsEdit_UpdateGeoRanderBySelectRange_FindIntersect:
        pChunk->m_nOperateState = ECHUNK_OPERATE_TYPE::e_Chunk_Selected;
    }
    this->m_nOperateEleCnt = this->m_pVecEleChunksOperate->size();
    return (int)this->m_nOperateEleCnt;
}

int CLayerPolygonsEdit::DeleteGeometryBySelectRange(){

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

int CLayerPolygonsEdit::MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){
    if(this->m_nBeingEditor == 0){
        return 0;
    }
    if(this->m_nMovingPoint == 1){
        glm::dvec3& geoPoint = this->m_pVecPoints->at(this->m_pHitParam->m_nPointIndx);

        geoPoint.x = fGeoMouseX;
        geoPoint.y = fGeoMouseY;

        double* pChunkPoints = (double*)this->m_pVecPoints->data();
        double fGeoX = pChunkPoints[0];
        double fGeoY = pChunkPoints[1];

        glm::dvec4& pBoundPoint = this->m_vecChunkDatas.at(0)->m_chunkBound;
        double fEnvelopeMinLng = fGeoX;
        double fEnvelopeMinLat = fGeoY;
        double fEnvelopeMaxLng = fGeoX;
        double fEnvelopeMaxLat = fGeoY;

        int nPointCnt = (int)this->m_pVecPoints->size();
        for (int i = 1; i < nPointCnt; i++) {
            fGeoX = pChunkPoints[i * 3 + 0];
            fGeoY = pChunkPoints[i * 3 + 1];

            if(fEnvelopeMinLng > fGeoX){
                fEnvelopeMinLng = fGeoX;
            }
            if(fEnvelopeMaxLng < fGeoX) {
                fEnvelopeMaxLng = fGeoX;
            }
            if(fEnvelopeMinLat > fGeoY){
                fEnvelopeMinLat = fGeoY;
            }
            if(fEnvelopeMaxLat < fGeoY) {
                fEnvelopeMaxLat = fGeoY;
            }
        }
        pBoundPoint.x = fEnvelopeMinLng;
        pBoundPoint.y = fEnvelopeMinLat;
        pBoundPoint.z = fEnvelopeMaxLng;
        pBoundPoint.w = fEnvelopeMaxLat;

        this->m_fMinLng = fEnvelopeMinLng;
        this->m_fMinLat = fEnvelopeMinLat;
        this->m_fMaxLng = fEnvelopeMaxLng;
        this->m_fMaxLat = fEnvelopeMaxLat;

        glm::dvec3& geoPoint1 = this->m_vecChunkDatas[0]->m_vecChunkPoints.at(this->m_pHitParam->m_nPointIndx);
        geoPoint1.x = fGeoMouseX;
        geoPoint1.y = fGeoMouseY;

        return 1;
    }
    double* fSelectRangeGeo = arrMousePointBound;
    if (fabs(fSelectRangeGeo[0] - fSelectRangeGeo[2]) <= 1.0E-12 &&
        fabs(fSelectRangeGeo[1] - fSelectRangeGeo[3]) <= 1.0E-12)
    {
        return 0;
    }
    t_Line2D tLineLeft(fSelectRangeGeo[0], fSelectRangeGeo[3], fSelectRangeGeo[0], fSelectRangeGeo[1]);
    t_Line2D tLineBottom(fSelectRangeGeo[0], fSelectRangeGeo[1], fSelectRangeGeo[2], fSelectRangeGeo[1]);
    t_Line2D tLineRight(fSelectRangeGeo[2], fSelectRangeGeo[1], fSelectRangeGeo[2], fSelectRangeGeo[3]);
    t_Line2D tLineTop(fSelectRangeGeo[0], fSelectRangeGeo[3], fSelectRangeGeo[2], fSelectRangeGeo[3]);

    //memset(this->m_pVecPointFlag->data(), 0, this->m_pVecPointFlag->size()* sizeof(float));
    int i0 = 0, nChunkPtIndx = 0, nChunkPtIndx_1 = 0, nChunkPtIndxStart = 0, nChunksPointCnt = 0, nChunksPointCnt_Sub1 = 0, nIntersect = 0;
    int i = 0, nEleChunksCnt = (int)this->m_vecChunkDatas.size();
    int nEleChunksCntExchange = nEleChunksCnt - 1;

    CChunk* pChunk = nullptr;
    t_Line2D tLineEdge;
    for(i = nEleChunksCntExchange; i >= 0; i--) {

        pChunk = this->m_vecChunkDatas.at(i);
        nChunksPointCnt = (int)pChunk->m_vecChunkPoints.size();
        if(nChunksPointCnt < 3){
            continue;
        }
        if (0 == this->IsOverlaps(fSelectRangeGeo, glm::value_ptr(pChunk->m_chunkBound))){
            continue;
        }
        if(1 == this->IsContains(fSelectRangeGeo, glm::value_ptr(pChunk->m_chunkBound))){
            nIntersect = 2;
            goto l_SxLayerPolygonsEdit_MouseMoving_FindIntersect;
        }else{
            //glm::vec3& vecPointStart = this->m_pVecPoints->at(vecChunkIndx[0]);
            nChunkPtIndxStart = 0;
            nChunksPointCnt_Sub1 = nChunksPointCnt - 1;
            nIntersect = 0;
            {
                glm::dvec3& vecPoint0 = pChunk->m_vecChunkPoints.at(nChunksPointCnt_Sub1);
                glm::dvec3& vecPoint1 = pChunk->m_vecChunkPoints.at(nChunkPtIndxStart);
                //glm::vec3& vecPoint1 = this->m_pVecPoints->at(());
                tLineEdge.SetPoints((double)vecPoint0.x, (double)vecPoint0.y, (double)vecPoint1.x, (double)vecPoint1.y);
                if ((this->IntersectionLine(tLineLeft, tLineEdge) == 1) ||
                    (this->IntersectionLine(tLineBottom, tLineEdge) == 1) ||
                    (this->IntersectionLine(tLineRight, tLineEdge) == 1) ||
                    (this->IntersectionLine(tLineTop, tLineEdge) == 1))
                {
                    if(this->IsContainsByPoint(arrMousePointBound, (double*)(glm::value_ptr(vecPoint0)))) {
                        this->m_pHitParam->m_nPointIndx = nChunksPointCnt_Sub1;
                        this->m_pHitParam->m_point = vecPoint0;
                        nIntersect = 2;
                    }
                    else if (this->IsContainsByPoint(arrMousePointBound, (double*)(glm::value_ptr(vecPoint1)))) {
                        this->m_pHitParam->m_nPointIndx = nChunkPtIndxStart;
                        this->m_pHitParam->m_point = vecPoint1;
                        nIntersect = 2;
                    }
                    else{
                        this->m_pHitParam->m_nPointIndx = nChunksPointCnt_Sub1;
                        this->m_pHitParam->m_nPointIndxNext = nChunkPtIndxStart;
                        this->m_pHitParam->m_point = vecPoint0;
                        this->m_pHitParam->m_pointNext = vecPoint1;
                        nIntersect = 1;
                    }
                }
            }
            if(nIntersect == 0) {
                for (i0 = 0; i0 < nChunksPointCnt - 1; i0++) {
                    glm::dvec3& vecPoint0 = pChunk->m_vecChunkPoints.at(i0);
                    glm::dvec3& vecPoint1 = pChunk->m_vecChunkPoints.at(i0 + 1);
                    //glm::vec3& vecPoint1 = this->m_pVecPoints->at(());
                    tLineEdge.SetPoints((double)vecPoint0.x, (double)vecPoint0.y, (double)vecPoint1.x, (double)vecPoint1.y);
                    if((this->IntersectionLine(tLineLeft, tLineEdge) == 1) ||
                        (this->IntersectionLine(tLineBottom, tLineEdge) == 1) ||
                        (this->IntersectionLine(tLineRight, tLineEdge) == 1) ||
                        (this->IntersectionLine(tLineTop, tLineEdge) == 1) )
                    {
                        if(this->IsContainsByPoint(arrMousePointBound, (double*)(glm::value_ptr(vecPoint0)))) {
                            this->m_pHitParam->m_nPointIndx = i0;
                            this->m_pHitParam->m_point = vecPoint0;
                            nIntersect = 2;
                        }
                        else if(this->IsContainsByPoint(arrMousePointBound, (double*)(glm::value_ptr(vecPoint1)))) {
                            this->m_pHitParam->m_nPointIndx = i0 + 1;
                            this->m_pHitParam->m_point = vecPoint1;
                            nIntersect = 2;
                        }
                        else{
                            this->m_pHitParam->m_nPointIndx = i0;
                            this->m_pHitParam->m_nPointIndxNext = i0 + 1;
                            this->m_pHitParam->m_point = vecPoint0;
                            this->m_pHitParam->m_pointNext = vecPoint1;
                            nIntersect = 1;
                        }
                        break;
                    }
                }
            }
            if(nIntersect == 0){
                continue;
            }
        }
    l_SxLayerPolygonsEdit_MouseMoving_FindIntersect:
        //if(nIntersect == 1) {
        //} else if(nIntersect == 2) {
        //}
        this->m_pHitParam->m_fGeoMouseX = fGeoMouseX;
        this->m_pHitParam->m_fGeoMouseY = fGeoMouseY;
        //this->m_nHitParamFlag = nIntersect;
        //this->m_pGLCore->UpdateCursorType(nIntersect);
        break;
    }
    this->m_nHitParamFlag = nIntersect;
    return nIntersect;
}
int CLayerPolygonsEdit::MouseRelease(int nBtnType, double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){

    printf("SxLayerPolygonsEdit::MouseRelease\n");
    return 1;
}


int CLayerPolygonsEdit::AddChunkPoints(int nPointCnt, double* pChunkPoints){
    if(nPointCnt < 3 || pChunkPoints == nullptr){
        return 0;
    }
    CChunk* pChunk = this->m_vecChunkDatas[0];
    pChunk->m_vecChunkPoints.resize(nPointCnt);
    memcpy(pChunk->m_vecChunkPoints.data(), pChunkPoints, sizeof(double)*3*nPointCnt);
    this->MakeUpChunks();
    return 1;
}

int CLayerPolygonsEdit::AddPoint(){

    double pPointFoot[2] = {0};
    double pPointOut[2] = {this->m_pHitParam->m_fGeoMouseX, this->m_pHitParam->m_fGeoMouseY};
    this->GetFootOfPerpendicular(
        glm::value_ptr(this->m_pHitParam->m_point),
        glm::value_ptr(this->m_pHitParam->m_pointNext),
        pPointOut,
        pPointFoot);

    this->m_pVecPoints->insert(this->m_pVecPoints->begin() + this->m_pHitParam->m_nPointIndx + 1, glm::dvec3(pPointFoot[0], pPointFoot[1], 0.0));

    std::vector<glm::dvec3> newVecPoints(this->m_pVecPoints->begin(), this->m_pVecPoints->end());

    int nPointCnt = (int)newVecPoints.size();
    double* pChunkPoints = (double*)newVecPoints.data();

    this->AddChunkPoints(nPointCnt, pChunkPoints);
    return 1;
}

int CLayerPolygonsEdit::DelPoint(){
    this->m_pVecPoints->erase(this->m_pVecPoints->begin() + this->m_pHitParam->m_nPointIndx);

    std::vector<glm::dvec3> newVecPoints(this->m_pVecPoints->begin(), this->m_pVecPoints->end());

    int nPointCnt = (int)newVecPoints.size();
    double* pChunkPoints = (double*)newVecPoints.data();

    this->AddChunkPoints(nPointCnt, pChunkPoints);
    return 1;
}

int CLayerPolygonsEdit::MakeUpChunks() {

    CChunk* pChunk = nullptr;
    int nAllPointCountTmp = 0;

    double fEnvelopeMinLng = 999999.0;
    double fEnvelopeMinLat = 999999.0;
    double fEnvelopeMaxLng = -999999.0;
    double fEnvelopeMaxLat = -999999.0;
    double fGeoX = 0.0, fGeoY = 0.0;

    this->InitBody();
    int i = 0, j = 0, nChunkPointCount = 0, nAllPointCount = 0, nRealChunkCount = 0, nChunkCount = (int)this->m_vecChunkDatas.size();
    for( ; i < nChunkCount; i++){
        pChunk = this->m_vecChunkDatas[i];
        if( pChunk == nullptr)
            continue;
        pChunk->m_vecChunkPointsIndx.clear();

        nChunkPointCount = (int)pChunk->m_vecChunkPoints.size();
        if(nChunkPointCount <= 0)
            continue;

        nAllPointCountTmp = nAllPointCount;
        nChunkPointCount = nChunkPointCount - 1;

        for(j = 0; j<nChunkPointCount; j++) {
            this->m_pVecPoints->push_back(pChunk->m_vecChunkPoints[j]);
            this->m_pVecPointFlag->push_back(0.0f);
            this->m_pVecVerticsIndx->push_back(nAllPointCount);
            this->m_pVecVerticsIndx->push_back(nAllPointCount + 1);
            this->m_pVecPointFlagOperate->push_back(0.0f);

            this->m_pVecPointsIndxOperatePointNode->push_back(nAllPointCount);
            pChunk->m_vecChunkPointsIndx.push_back(nAllPointCount);

            nAllPointCount++;

            fGeoX = pChunk->m_vecChunkPoints[j].x;
            fGeoY = pChunk->m_vecChunkPoints[j].y;
            if(fEnvelopeMinLng > fGeoX){
                fEnvelopeMinLng = fGeoX;
            }
            if(fEnvelopeMaxLng < fGeoX) {
                fEnvelopeMaxLng = fGeoX;
            }
            if(fEnvelopeMinLat > fGeoY){
                fEnvelopeMinLat = fGeoY;
            }
            if(fEnvelopeMaxLat < fGeoY) {
                fEnvelopeMaxLat = fGeoY;
            }
        }
        this->m_pVecPoints->push_back(pChunk->m_vecChunkPoints[nChunkPointCount]);
        this->m_pVecPointFlag->push_back(0.0f);
        this->m_pVecVerticsIndx->push_back(nAllPointCount);
        this->m_pVecVerticsIndx->push_back(nAllPointCountTmp);
        this->m_pVecPointFlagOperate->push_back(0.0f);

        this->m_pVecPointsIndxOperatePointNode->push_back(nAllPointCount);
        pChunk->m_vecChunkPointsIndx.push_back(nAllPointCount);
        nAllPointCount++;

        fGeoX = pChunk->m_vecChunkPoints[nChunkPointCount].x;
        fGeoY = pChunk->m_vecChunkPoints[nChunkPointCount].y;
        if(fEnvelopeMinLng > fGeoX){
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY){
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }

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

int CLayerPolygonsEdit::MakeUpChunksState() {

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
        this->m_pVecPointsIndxOperate->push_back(nPointOperateIndx);
        this->m_pVecPointsIndxOperate->push_back(pChunk->m_vecChunkPointsIndx[0]);
        this->m_pVecPointFlagOperate->at(nPointOperateIndx) = 2.0f;
        this->m_pVecPointsIndxOperatePointNode->push_back(nPointOperateIndx);
    }
    return 1;
}


