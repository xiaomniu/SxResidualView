#include "CLayerGeoDraw.h"

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
CLayerGeoDraw::CLayerGeoDraw()
{
    this->m_pGLCore = nullptr;
    this->InitBody();
}

CLayerGeoDraw::CLayerGeoDraw(COpenGLCore* pGLCore)
    :CLayerDraw (pGLCore)
{
    this->m_pGLCore = pGLCore;
    this->InitBody();
}

CLayerGeoDraw::CLayerGeoDraw(COpenGLCore* pGLCore, const std::string& sVectorFileFullPath){

    this->m_pGLCore = pGLCore;
    this->InitBody();
    this->Init();

}

CLayerGeoDraw::~CLayerGeoDraw()
{
    this->UninitBody();

    CChunk* pChunk = nullptr;
    int i = 0, nChunkCount = (int)this->m_vecChunkDatas.size();
    for(i = nChunkCount-1; i>=0; i--) {
        pChunk = this->m_vecChunkDatas[i];
        if(pChunk){
            delete pChunk;
            pChunk = nullptr;
        }
    }
}

int CLayerGeoDraw::UninitBody(){
    if(this->m_pVecPoints) {
        this->m_pVecPoints->clear();
        delete this->m_pVecPoints;
        this->m_pVecPoints = nullptr;
    }
    if(this->m_pVecPointInPolyIndx) {
        this->m_pVecPointInPolyIndx->clear();
        delete this->m_pVecPointInPolyIndx;
        this->m_pVecPointInPolyIndx = nullptr;
    }
    if(this->m_pVecPointFlag) {
        this->m_pVecPointFlag->clear();
        delete this->m_pVecPointFlag;
        this->m_pVecPointFlag = nullptr;
    }
    if (this->m_pVecPointFlagOperate){
        this->m_pVecPointFlagOperate->clear();
        delete this->m_pVecPointFlagOperate;
        this->m_pVecPointFlagOperate = nullptr;
    }
    if(this->m_pVecVerticsIndx) {
        this->m_pVecVerticsIndx->clear();
        delete this->m_pVecVerticsIndx;
        this->m_pVecVerticsIndx = nullptr;
    }

    if (this->m_pVecPointsIndxOperate){
        this->m_pVecPointsIndxOperate->clear();
        delete this->m_pVecPointsIndxOperate;
        this->m_pVecPointsIndxOperate = nullptr;
    }

    if (this->m_pVecPointsIndxOperatePointNode){
        this->m_pVecPointsIndxOperatePointNode->clear();
        delete this->m_pVecPointsIndxOperatePointNode;
        this->m_pVecPointsIndxOperatePointNode = nullptr;
    }

    if (this->m_pVecPointsIndxOperateHit){
        this->m_pVecPointsIndxOperateHit->clear();
        delete this->m_pVecPointsIndxOperateHit;
        this->m_pVecPointsIndxOperateHit = nullptr;
    }

    if (this->m_pVecPointsIndxOperatePointNodeHit){
        this->m_pVecPointsIndxOperatePointNodeHit->clear();
        delete this->m_pVecPointsIndxOperatePointNodeHit;
        this->m_pVecPointsIndxOperatePointNodeHit = nullptr;
    }

    if (this->m_pVecEleChunksOperate){
        this->m_pVecEleChunksOperate->clear();
        delete this->m_pVecEleChunksOperate;
        this->m_pVecEleChunksOperate = nullptr;
    }
    if(this->m_pVecChunksName){
        this->m_pVecChunksName->clear();
        delete this->m_pVecChunksName;
        this->m_pVecChunksName = nullptr;
    }
    if(this->m_pMapChunkName2Indx){
        this->m_pMapChunkName2Indx->clear();
        delete this->m_pMapChunkName2Indx;
        this->m_pMapChunkName2Indx = nullptr;
    }
    return 1;
}

void CLayerGeoDraw::InitBody(){

    this->UninitBody();

    this->m_pVecPoints = new std::vector<glm::dvec3>;
    this->m_pVecPointInPolyIndx = new std::vector<unsigned int>;
    this->m_pVecPointFlag = new std::vector<float>;
    this->m_pVecVerticsIndx = new std::vector<unsigned int>;

    this->m_pVecPointsIndxOperate = new std::vector<unsigned int>;
    this->m_pVecPointsIndxOperatePointNode = new std::vector<unsigned int>;

    this->m_pVecPointsIndxOperateHit = new std::vector<unsigned int>;
    this->m_pVecPointsIndxOperatePointNodeHit = new std::vector<unsigned int>;

    this->m_pVecPointFlagOperate = new std::vector<float>;
    this->m_pVecEleChunksOperate = new std::vector<int>;

    this->m_pVecChunksName = new std::vector<std::string>;
    this->m_pMapChunkName2Indx = new std::unordered_map<std::string, int>;
}

void CLayerGeoDraw::Init() {
}

void CLayerGeoDraw::Draw() {
    printf("SxLayerGeoDraw::Draw()\n");
}

int CLayerGeoDraw::UpdateGeoRanderBySelectRange(){
    return 0;
}
int CLayerGeoDraw::DeleteGeometryBySelectRange(){
    return 0;
}

int CLayerGeoDraw::IsOverlaps(double* pSrcRect, double* pDstRect){
    //printf("%f, %f, %f, %f\n", pSrcRect[0], pSrcRect[1], pSrcRect[2], pSrcRect[3]);
    //printf("%f, %f, %f, %f\n", pDstRect[0], pDstRect[1], pDstRect[2], pDstRect[3]);
    if(pSrcRect[0] <= pDstRect[0] && pSrcRect[2] >= pDstRect[2] &&
        pSrcRect[1] <= pDstRect[1] && pSrcRect[3] >= pDstRect[3])
    {
        return 1;
    }
    if(pSrcRect[0] > pDstRect[2] ||
        pSrcRect[1] > pDstRect[3] ||
        pSrcRect[2] < pDstRect[0] ||
        pSrcRect[3] < pDstRect[1])
    {
        return 0;
    }
    return 1;
}

int CLayerGeoDraw::IsContainsByPoint(double* pSrcRect, double* pDstPoint){
    if(pSrcRect[0] <= pDstPoint[0] && pSrcRect[2] >= pDstPoint[0] &&
        pSrcRect[1] <= pDstPoint[1] && pSrcRect[3] >= pDstPoint[1])
    {
        return 1;
    }
    return 0;
    //if(pSrcRect[0] > pDstRect[0] ||
    //    pSrcRect[1] > pDstRect[1] ||
    //    pSrcRect[2] < pDstRect[0] ||
    //    pSrcRect[3] < pDstRect[1])
    //{
    //    return 0;
    //}
    //return 1;
}

//检测  pSrcRect 是否包含 pDstRect
int CLayerGeoDraw::IsContains(double* pSrcRect, double* pDstRect){
    if(fabs(pSrcRect[0] - pDstRect[0]) <= 0.000001 && fabs(pSrcRect[1] - pDstRect[1]) <= 0.000001 &&
        fabs(pSrcRect[2] - pDstRect[2]) <= 0.000001 && fabs(pSrcRect[3] - pDstRect[3]) <= 0.000001)
    {
        return 1;
    }
    if(pSrcRect[0] > pDstRect[0] ||
        pSrcRect[1] > pDstRect[1] ||
        pSrcRect[2] < pDstRect[2] ||
        pSrcRect[3] < pDstRect[3])
    {
        return 0;
    }
    return 1;
}

int CLayerGeoDraw::IsOverlapsByLine(double* pSrcRect, double* pDstLine){
    //printf("%f, %f, %f, %f\n", pSrcRect[0], pSrcRect[1], pSrcRect[2], pSrcRect[3]);
    //printf("%f, %f, %f, %f\n", pDstRect[0], pDstRect[1], pDstRect[2], pDstRect[3]);

    //double fChunkBoundMinX = 0.0;
    //double fChunkBoundMinY = 0.0;
    //double fChunkBoundMaxX = 0.0;
    //double fChunkBoundMaxY = 0.0;

    if(pSrcRect[0] > pDstLine[2] ||
        pSrcRect[1] > pDstLine[3] ||
        pSrcRect[2] < pDstLine[0] ||
        pSrcRect[3] < pDstLine[1])
    {
        return 0;
    }
    return 1;
}

int CLayerGeoDraw::GetFootOfPerpendicular(double* pPointStart, double* pPointEnd, double* pPointOut, double* pPointFoot){
    double dx = pPointStart[0] - pPointEnd[0];
    double dy = pPointStart[1] - pPointEnd[1];
    if(fabs(dx) < 0.000000001 && fabs(dy) < 0.000000001){
        pPointFoot[0] = pPointStart[0];
        pPointFoot[1] = pPointStart[1];
        return 1;
    }
    double u = ((pPointOut[0] - pPointStart[0]) * dx) + ((pPointOut[1] - pPointStart[1]) * dy);
    double fPingFang = dx * dx + dy * dy;
    u = u / fPingFang;
    pPointFoot[0] = pPointStart[0] + u * dx;
    pPointFoot[1] = pPointStart[1] + u * dy;

    return 1;
}


int CLayerGeoDraw::RemoveAllChunks(bool bNeedDelete){
    if(bNeedDelete) {
        int i = 0, nChunkCount = (int)this->m_vecChunkDatas.size();
        CChunk* pChunk = nullptr;
        for(; i<nChunkCount; i++) {
            pChunk = this->m_vecChunkDatas[i];
            delete pChunk;
            pChunk = nullptr;
        }
    }
    this->m_vecChunkDatas.clear();

    this-> m_fMaxLng = -99999999.99999;
    this-> m_fMaxLat = -99999999.99999;
    this-> m_fMinLng = 99999999.99999;
    this-> m_fMinLat = 99999999.99999;

    this->InitBody();
    return 1;
}

void CLayerGeoDraw::ClearOperateChunkState(){
    CChunk* pChunk = nullptr;
    int i = 0, nOperateEleCnt = (int)this->m_pVecEleChunksOperate->size();
    for(; i<nOperateEleCnt; i++){
        pChunk = this->m_vecChunkDatas[this->m_pVecEleChunksOperate->at(i)];
        pChunk->m_nOperateState = ECHUNK_OPERATE_TYPE::e_Chunk_UnKnowState;
    }
}

void CLayerGeoDraw::ReCalcChunkBound(CChunk* pChunk) {
    if(pChunk == nullptr)
        return;

    int j = 0, nChunkPointCount = 0;

    glm::dvec3* pPoint3D = nullptr;
    double fGeoX = 0.0, fGeoY = 0.0;
    double fEnvelopeMinLng = 999999999.999999;
    double fEnvelopeMinLat = 999999999.999999;
    double fEnvelopeMaxLng = -999999999.999999;
    double fEnvelopeMaxLat = -999999999.999999;

    nChunkPointCount = (int)pChunk->m_vecChunkPoints.size();

    fEnvelopeMinLng = 999999999.999999;
    fEnvelopeMinLat = 999999999.999999;
    fEnvelopeMaxLng = -999999999.999999;
    fEnvelopeMaxLat = -999999999.999999;
    for(j = 0; j<nChunkPointCount; j++) {
        pPoint3D = &(pChunk->m_vecChunkPoints[j]);
        fGeoX = pPoint3D->x;
        fGeoY = pPoint3D->y;

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
void CLayerGeoDraw::ReCalcLayerBoundByChunks(int nChunkIdx/* = -1*/){
    CChunk* pChunk = nullptr;
    int i = 0, nOperateEleCnt = (int)this->m_pVecEleChunksOperate->size();

    if(nChunkIdx >= 0 && nChunkIdx < nOperateEleCnt){
        pChunk = this->m_vecChunkDatas[this->m_pVecEleChunksOperate->at(nChunkIdx)];
        this->ReCalcChunkBound(pChunk);
        return;
    }
    for(; i<nOperateEleCnt; i++) {
        pChunk = this->m_vecChunkDatas[this->m_pVecEleChunksOperate->at(i)];
        this->ReCalcChunkBound(pChunk);
    }
}


void CLayerGeoDraw::FreshFilter(int nAttributeIndx/* = 0*/) {
    CChunk* pChunk = nullptr;
    int i = 0, nChunksCount = (int)this->m_vecChunkDatas.size();
    switch (this->m_pLayerFilter->m_nFilterType) {
    case CLayerFilter::e_By_RangeNum: {
        for(; i<nChunksCount; i++) {
            pChunk = this->m_vecChunkDatas[i];
            if (this->m_pLayerFilter->m_nRangeNum > pChunk->m_vecAttributeValues[nAttributeIndx].GetIntValue()){
                pChunk->m_nDisplayState = ESHOWORHIDE::e_hide;
            }
            else{
                pChunk->m_nDisplayState = ESHOWORHIDE::e_show;
            }
        }
        break;
    }
    case CLayerFilter::e_By_RangeNum_double: {
        for(; i<nChunksCount; i++) {
            pChunk = this->m_vecChunkDatas[i];
            if (this->m_pLayerFilter->m_nRangeNum > pChunk->m_vecAttributeValues[nAttributeIndx].GetDoubleValue()){
                pChunk->m_nDisplayState = ESHOWORHIDE::e_hide;
            }
            else{
                pChunk->m_nDisplayState = ESHOWORHIDE::e_show;
            }
        }
        break;
    }
    default:
        return;
    }
    this->MakeUpChunks();
}

void CLayerGeoDraw::FreshFilter(std::vector<int> vecAttributeIndx) {
    CChunk* pChunk = nullptr;
    int i = 0, nChunksCount = (int)this->m_vecChunkDatas.size();
    int j = 0, nAtrributeIndxCnt = (int)vecAttributeIndx.size();
    int nAttributeIndx = -1, nIsShow = 0;
    switch (this->m_pLayerFilter->m_nFilterType) {
    case CLayerFilter::e_By_RangeNum: {
        for(i = 0; i<nChunksCount; i++) {
            pChunk = this->m_vecChunkDatas[i];
            nIsShow = 0;
            for(j = 0; j<nAtrributeIndxCnt ; j++){
                nAttributeIndx = vecAttributeIndx[j];
                if(this->m_pLayerFilter->m_nRangeNum < pChunk->m_vecAttributeValues[nAttributeIndx].GetIntValue()){
                    nIsShow = 1;
                    break;
                }
            }

            if (nIsShow == 0){
                pChunk->m_nDisplayState = ESHOWORHIDE::e_hide;
            }
            else{
                pChunk->m_nDisplayState = ESHOWORHIDE::e_show;
            }
        }
        break;
    }
    case CLayerFilter::e_By_RangeNum_double: {
        for( i = 0; i<nChunksCount; i++) {
            pChunk = this->m_vecChunkDatas[i];
            nIsShow = 0;
            for(j = 0; j<nAtrributeIndxCnt ; j++){
                nAttributeIndx = vecAttributeIndx[j];
                if(this->m_pLayerFilter->m_fRangeNum < pChunk->m_vecAttributeValues[nAttributeIndx].GetDoubleValue()){
                    nIsShow = 1;
                    break;
                }
            }
            if (nIsShow == 0){
                pChunk->m_nDisplayState = ESHOWORHIDE::e_hide;
            }
            else{
                pChunk->m_nDisplayState = ESHOWORHIDE::e_show;
            }
        }
        break;
    }
    default:
        return;
    }
    this->MakeUpChunks();
}
