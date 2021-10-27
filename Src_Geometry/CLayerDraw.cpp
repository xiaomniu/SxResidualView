#include "CLayerDraw.h"

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

////////////////////////////////////////////////////////////
CChunk::CChunk() {
}
CChunk::~CChunk() {
    CChunkAttributeInfo* pChunkAttr = nullptr;
    int i = 1, nFldCount = (int)this->m_pBelongLayer->m_vecLayerFields.size();
    for(i = 1; i<nFldCount; i++){
        pChunkAttr = &this->m_vecAttributeValues[i];
        if(pChunkAttr->m_eAttributeType == ECHUNK_ATTRIBUTE_TYPE::e_char_ptr && pChunkAttr->m_nChunkAttributeValue.m_StrValue != nullptr){
            delete[] pChunkAttr->m_nChunkAttributeValue.m_StrValue;
            pChunkAttr->m_nChunkAttributeValue.m_StrValue = nullptr;
        }
        //else if(pChunkAttr->m_eAttributeType == ECHUNK_ATTRIBUTE_TYPE::e_void_ptr && pChunkAttr->m_nChunkAttributeValue.m_BinValue != nullptr){
        //    delete[] (char*)pChunkAttr->m_nChunkAttributeValue.m_BinValue;
        //    pChunkAttr->m_nChunkAttributeValue.m_BinValue = nullptr;
        //}
    }
}
void CChunk::Clone(CChunk* pOtherChunk) {
    this->m_nOperateState = pOtherChunk->m_nOperateState;
    this->m_nIndxInEleChunks = pOtherChunk->m_nIndxInEleChunks;
    this->m_nChunkType = pOtherChunk->m_nChunkType;
    this->m_pBelongLayer = pOtherChunk->m_pBelongLayer;
    this->m_chunkBound = pOtherChunk->m_chunkBound;

    this->m_vecChunkPoints.clear();
    this->m_vecAttributeValues.clear();
    this->m_mapChunkData.clear();

    int i = 0, nChunkPointsCount = (int)pOtherChunk->m_vecChunkPoints.size();
    for(; i<nChunkPointsCount; i++){
        this->m_vecChunkPoints.push_back(pOtherChunk->m_vecChunkPoints[i]);
    }
}

CChunkAttributeInfo CChunk::GetAttributeByIndex(int nFldIndx){
    CChunkAttributeInfo chunkAttr;
    if (nFldIndx >= 0 && nFldIndx < (int)this->m_vecAttributeValues.size()){
        chunkAttr = this->m_vecAttributeValues[nFldIndx];
    }
    return chunkAttr;
}

CChunkAttributeInfo CChunk::GetAttributeByName(const std::string& sFldName){

    auto itt = this->m_pBelongLayer->m_mapName2FldIndx.find(sFldName);
    if(itt == this->m_pBelongLayer->m_mapName2FldIndx.end()){
        return CChunkAttributeInfo();
    }
    return this->GetAttributeByIndex(itt->second);
}

int CChunk::GetAttributeIndxByName(const std::string& sFldName){

    auto itt = this->m_pBelongLayer->m_mapName2FldIndx.find(sFldName);
    if(itt == this->m_pBelongLayer->m_mapName2FldIndx.end()){
        return -1;
    }
    return itt->second;
}

void CChunk::SetFresh(){
    if(this->m_pBelongLayer){
        this->m_pBelongLayer->SetFresh(this);
    }
}
////////////////////////////////////////////////////////////
//#include <QProcess>
CLayerDraw::CLayerDraw()
{
    this->m_pGLCore = nullptr;

}

CLayerDraw::CLayerDraw(COpenGLCore* pGLCore)
{
    this->m_pGLCore = pGLCore;

    this->Init();

    CLayerField layerFld;
    layerFld.m_sFieldName = "OID";
    layerFld.m_nFieldWidth = 10;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_int;
    this->m_vecLayerFields.push_back(layerFld);
}

CLayerDraw::~CLayerDraw()
{

}

void CLayerDraw::Init() {
}

void CLayerDraw::ShowEnable(int nShowOrHide){
    this->m_nShowOrHide = nShowOrHide;
}
int CLayerDraw::ShowState(){
    return this->m_nShowOrHide;
}

int CLayerDraw::IntersectionLine(const t_Line2D &l1, const t_Line2D &l2)
{
    //快速排斥实验
    if ((l1.x1 > l1.x2 ? l1.x1 : l1.x2) < (l2.x1 < l2.x2 ? l2.x1 : l2.x2) ||
        (l1.y1 > l1.y2 ? l1.y1 : l1.y2) < (l2.y1 < l2.y2 ? l2.y1 : l2.y2) ||
        (l2.x1 > l2.x2 ? l2.x1 : l2.x2) < (l1.x1 < l1.x2 ? l1.x1 : l1.x2) ||
        (l2.y1 > l2.y2 ? l2.y1 : l2.y2) < (l1.y1 < l1.y2 ? l1.y1 : l1.y2))
    {
        return 0;
    }
    //跨立实验
    if ((((l1.x1 - l2.x1)*(l2.y2 - l2.y1) - (l1.y1 - l2.y1)*(l2.x2 - l2.x1))*
        ((l1.x2 - l2.x1)*(l2.y2 - l2.y1) - (l1.y2 - l2.y1)*(l2.x2 - l2.x1))) > 0 ||
        (((l2.x1 - l1.x1)*(l1.y2 - l1.y1) - (l2.y1 - l1.y1)*(l1.x2 - l1.x1))*
        ((l2.x2 - l1.x1)*(l1.y2 - l1.y1) - (l2.y2 - l1.y1)*(l1.x2 - l1.x1))) > 0)
    {
        return 0;
    }
    return 1;
}

int CLayerDraw::GetIntersectionPoint(const t_Line2D &l1, const t_Line2D &l2, glm::dvec2& ptRet){
    glm::dvec2 pt1(l1.x1, l1.y1);
    glm::dvec2 pt2(l1.x2, l1.y2);
    glm::dvec2 pt3(l2.x1, l2.y1);
    glm::dvec2 pt4(l2.x2, l2.y2);
    this->GetIntersectionPoint(pt1, pt2, pt3, pt4, ptRet);
    return 1;
}

inline double Cross(const glm::dvec2& p1, const glm::dvec2& p2, const glm::dvec2& p3, const glm::dvec2& p4)
{
    return (p2.x-p1.x)*(p4.y-p3.y) - (p2.y-p1.y)*(p4.x-p3.x);
}
inline void GeneralEquation(const glm::dvec2& p1, const glm::dvec2& p2, double* pA, double* pB, double* pC) {
    *pA = p2.y - p1.y;
    *pB = p1.x - p2.x;
    *pC = p2.x * p1.y - p1.x * p2.y;
}
int CLayerDraw::GetIntersectionPoint(glm::dvec2& p1, glm::dvec2& p2, glm::dvec2& p3, glm::dvec2& p4, glm::dvec2& ptRet){
    //double k1 = Cross(p1,p2,p1,p3);
    //double k2 = Cross(p1,p2,p1,p4);
    //double k = k1 / k2;
    //ptRet.x = (p3.x + k*p4.x)/(1+k);
    //ptRet.y = (p3.y + k*p4.y)/(1+k);

    double fA1 = 0.0, fB1 = 0.0, fC1 = 0.0;
    double fA2 = 0.0, fB2 = 0.0, fC2 = 0.0;
    GeneralEquation(p1, p2, &fA1, &fB1, &fC1);
    GeneralEquation(p3, p4, &fA2, &fB2, &fC2);
    double fM = fA1 * fB2 - fA2 * fB1;
    ptRet.x = (fC2 * fB1 - fC1 * fB2) / fM;
    ptRet.y = (fC1 * fA2 - fC2 * fA1) / fM;
    return 1;
}

void CLayerDraw::MakeupFields(){

    int i = 0, nFldCnt = (int)this->m_vecLayerFields.size();
    for (i = 0; i < nFldCnt; i++){
        CLayerField& fld = this->m_vecLayerFields[i];
        this->m_mapName2FldIndx.insert(std::pair<std::string, int>(fld.m_sFieldName, i));
    }

}
