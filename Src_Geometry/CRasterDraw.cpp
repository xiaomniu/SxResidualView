#include "CRasterDraw.h"

#include <thread>

#include "Src_Core/Misc.h"
#include "Src_Core/CGeoRelation.h"
#include "Src_Geometry/CRasterThumbnail.h"
#include "Src_Geometry/CReadRaster.h"

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
CRasterDraw::CRasterDraw()
{
    this->m_pGLCore = nullptr;;

}

CRasterDraw::CRasterDraw(COpenGLCore* pGLCore)
{
    this->m_pGLCore = pGLCore;

}

CRasterDraw::CRasterDraw(COpenGLCore* pGLCore, const std::string& sRasterFileFullPath)
{
    this->m_pGLCore = pGLCore;
    this->m_sRasterImgFileFullPath = sRasterFileFullPath;
    this->Init();
}

CRasterDraw::~CRasterDraw()
{
    if(this->m_pReadRaster){
        delete this->m_pReadRaster;
        this->m_pReadRaster = nullptr;
    }
    if(this->m_pRasterThumbnail){
        delete this->m_pRasterThumbnail;
        this->m_pRasterThumbnail = nullptr;
    }
    if(this->m_pChunkMosaic){
        delete this->m_pChunkMosaic;
        this->m_pChunkMosaic = nullptr;
    }
}

void CRasterDraw::Init(){
    //std::string sTifFilePath;
    //sTifFilePath = "C:/temp/GF3/GF3_KAS_FSI_014480_E103.4_N31.0_20190511_L1A_HHHV_L10003992720/GF3_KAS_FSI_014480_E103.4_N31.0_20190511_L1A_HH_L10003992720.tiff";
    //sTifFilePath = "C:/temp/Mss_Data/Rectify/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-PAN2_rectify.tiff";
    //sTifFilePath = "C:/temp/Mss_Data/Rectify/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify.tiff";
    //sTifFilePath = "C:/temp/Mss_Data/ColorMap/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_fuse_8Bit.tiff";
    //this->m_sRasterImgFileFullPath = sTifFilePath;

    int nLayerType = 0;
    std::string sFileExtName = this->m_sRasterImgFileFullPath;
    sFileExtName = sFileExtName.substr(sFileExtName.rfind('.') + 1);
    std::transform(sFileExtName.begin(), sFileExtName.end(), sFileExtName.begin(), ::toupper);
    if(sFileExtName.compare("TIF")==0){
        nLayerType = ELAYERTYPE::e_Tif;
    } else if(sFileExtName.compare("TIFF")==0){
        nLayerType = ELAYERTYPE::e_Tiff;
    } else if(sFileExtName.compare("IMG")==0){
        nLayerType = ELAYERTYPE::e_Img;
    }
    this->m_nLayerType = nLayerType;

    this->m_pRasterThumbnail = new CRasterThumbnail(this->m_sRasterImgFileFullPath);

    this->m_pReadRaster = new CReadRaster(this->m_pRasterThumbnail->m_sTiffThumbFullPath.c_str());

    double fGeoWidth = this->m_pReadRaster->m_fLngWidht;
    double fGeoHeight = this->m_pReadRaster->m_fLatHeight;
    fGeoWidth = fGeoWidth / 3.0;
    fGeoHeight = fGeoHeight / 3.0;
    this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMinLng, this->m_pReadRaster->m_fMinLat));
    //this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMinLng + fGeoWidth, this->m_pReadRaster->m_fMinLat + fGeoHeight));
    this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMaxLng, this->m_pReadRaster->m_fMinLat));
    //this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMaxLng - fGeoWidth, this->m_pReadRaster->m_fMinLat + fGeoHeight));
    this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMaxLng, this->m_pReadRaster->m_fMaxLat));
    ///this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMaxLng - fGeoWidth, this->m_pReadRaster->m_fMaxLat - fGeoHeight));
    this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMinLng, this->m_pReadRaster->m_fMaxLat));
    this->m_vecMosaicPoints.push_back(glm::dvec2(this->m_pReadRaster->m_fMinLng + fGeoWidth, this->m_pReadRaster->m_fMaxLat - fGeoHeight));
}


void CRasterDraw::CreateMosaicLines() {
    if(this->m_nHasExistMosaicLine == 1){
        this->m_pLayerVector->m_nBeingEditor = 1;
        this->m_pLayerVector->BeingEditor(this->m_pChunkMosaic);

        this->m_nShowMosaicLine = 1;
        this->m_nHasExistMosaicLine = 1;
        return;
    }
    double* pGeoPoints3D = nullptr;
    int nGeoPoints3DCnt = this->m_pReadRaster->GetValidBound(&pGeoPoints3D);
    if(pGeoPoints3D == nullptr || nGeoPoints3DCnt <= 0){
        return;
    }
    if(this->m_pChunkMosaic == nullptr) {
        this->m_pChunkMosaic = new CChunk;
    }
    this->m_pChunkMosaic->m_nChunkType = ECHUNK_TYPE::e_Chunk_MosaicPolyline;
    this->m_pChunkMosaic->m_pBelongLayer = this;
    this->m_pChunkMosaic->m_vecChunkPoints.clear();
    //delete pGeoPoints3D;
    //nGeoPoints3DCnt = 4;
    //double pGeoPoints3D11[4 * 3] = {
    //    this->m_pReadRaster->m_fMinLng, this->m_pReadRaster->m_fMinLat, 0.0,
    //    this->m_pReadRaster->m_fMaxLng, this->m_pReadRaster->m_fMinLat, 0.0,
    //    this->m_pReadRaster->m_fMaxLng, this->m_pReadRaster->m_fMaxLat, 0.0,
    //    this->m_pReadRaster->m_fMinLng, this->m_pReadRaster->m_fMaxLat, 0.0,
    //};
    //pGeoPoints3D = new double[4 * 3];
    //memcpy(pGeoPoints3D, pGeoPoints3D11, sizeof(double) * 4 * 3);

    //this->m_pChunkMosaic->m_nChunkPointCnt = nGeoPoints3DCnt;
    this->m_pChunkMosaic->m_vecChunkPoints.resize(nGeoPoints3DCnt);
    memcpy(this->m_pChunkMosaic->m_vecChunkPoints.data(), pGeoPoints3D, sizeof(double) * 3 * nGeoPoints3DCnt);

    //this->m_pChunkMosaic->m_vecChunkPoints.push_back(this->m_pChunkMosaic->m_vecChunkPoints[0]);
    this->m_pLayerVector->m_nBeingEditor = 1;
    this->m_pLayerVector->BeingEditor(this->m_pChunkMosaic);

    this->m_nShowMosaicLine = 1;
    this->m_nHasExistMosaicLine = 1;
}

int CRasterDraw::GetScreenIntersectPoints(glm::dvec2& pt0, glm::dvec2& pt1,
    t_Line2D& tLineLeft, t_Line2D& tLineBottom, t_Line2D& tLineRight, t_Line2D& tLineTop, double* pIntersectRet){

    t_Line2D tLineEdge;
    tLineEdge.SetPoints(pt0.x, pt0.y, pt1.x, pt1.y);

    glm::dvec2 pInter0;
    glm::dvec2 pInter1;
    glm::dvec2 ptIntersectRet;
    double fChaX = 0.0;
    double fChaY = 0.0;
    double fChaX0 = 0.0;
    double fChaY0 = 0.0;
    double fChaX2 = 0.0, fChaY2 = 0.0;
    int nIntersect = 0;
    if(this->IntersectionLine(tLineLeft, tLineEdge) == 1){
        this->GetIntersectionPoint(tLineLeft, tLineEdge, ptIntersectRet);
        pIntersectRet[nIntersect * 2] = ptIntersectRet.x;
        pIntersectRet[nIntersect * 2 + 1] = ptIntersectRet.y;
        nIntersect += 1;
    }
    if(this->IntersectionLine(tLineBottom, tLineEdge) == 1){
        this->GetIntersectionPoint(tLineBottom, tLineEdge, ptIntersectRet);
        pIntersectRet[nIntersect * 2] = ptIntersectRet.x;
        pIntersectRet[nIntersect * 2 + 1] = ptIntersectRet.y;
        nIntersect += 1;
    }
    if(nIntersect > 1){
        goto l_SxRasterDraw_GetScreenIntersectPoints_GetIntersect;
    }
    if(this->IntersectionLine(tLineRight, tLineEdge) == 1){
        this->GetIntersectionPoint(tLineRight, tLineEdge, ptIntersectRet);
        pIntersectRet[nIntersect * 2] = ptIntersectRet.x;
        pIntersectRet[nIntersect * 2 + 1] = ptIntersectRet.y;
        nIntersect += 1;
    }
    if(nIntersect > 1){
        goto l_SxRasterDraw_GetScreenIntersectPoints_GetIntersect;
    }
    if(this->IntersectionLine(tLineTop, tLineEdge) == 1){
        this->GetIntersectionPoint(tLineTop, tLineEdge, ptIntersectRet);
        pIntersectRet[nIntersect * 2] = ptIntersectRet.x;
        pIntersectRet[nIntersect * 2 + 1] = ptIntersectRet.y;
        nIntersect += 1;
    }
l_SxRasterDraw_GetScreenIntersectPoints_GetIntersect:
    if(nIntersect < 2){
        return nIntersect;
    }
    pInter0.x = pIntersectRet[0];
    pInter0.y = pIntersectRet[1];
    pInter1.x = pIntersectRet[2];
    pInter1.y = pIntersectRet[3];

    if((fabs(pInter1.y - pInter0.y) <= 0.0000000001) && (fabs(pInter1.x - pInter0.x) <= 0.0000000001)){
        return 1;
    }
    fChaX = pt1.x - pt0.x;
    fChaY = pt1.y - pt0.y;
    fChaX0 = pInter1.x - pInter0.x;
    fChaY0 = pInter1.y - pInter0.y;
    fChaX2 = fChaX * fChaX0;
    fChaY2 = fChaY * fChaY0;

    if(fChaX2 < 0.0 || fChaY2 < 0.0)
    {
        pIntersectRet[0] = pInter1.x;
        pIntersectRet[1] = pInter1.y;
        pIntersectRet[2] = pInter0.x;
        pIntersectRet[3] = pInter0.y;
    }
    return nIntersect;

    //if(fabs(pt1.y - pt0.y) <= 0.0000000001){
    //    if(fabs(pt1.x - pt0.x) <= 0.0000000001){//不可能
    //        return 1;
    //    }else if (pt1.x > pt0.x) {
    //        if(pInter1.x > pInter0.x){
    //
    //        }else{
    //            ptIntersectRet[0] = pInter1.x;
    //            ptIntersectRet[1] = pInter1.y;
    //            ptIntersectRet[2] = pInter0.x;
    //            ptIntersectRet[3] = pInter0.y;
    //        }
    //    } else {
    //        if(pInter1.x > pInter0.x){
    //            ptIntersectRet[0] = pInter1.x;
    //            ptIntersectRet[1] = pInter1.y;
    //            ptIntersectRet[2] = pInter0.x;
    //            ptIntersectRet[3] = pInter0.y;
    //        }else{
    //
    //        }
    //    }
    //} else if (pt1.y < pt0.y) {
    //
    //} else {
    //    ptIntersectRet[0] = pInter1.x;
    //    ptIntersectRet[1] = pInter1.y;
    //    ptIntersectRet[2] = pInter0.x;
    //    ptIntersectRet[3] = pInter0.y;
    //}
}

bool PolygonClip(const std::vector<glm::dvec2> &poly1,const std::vector<glm::dvec2> &poly2, std::vector<glm::dvec2> &interPoly);

inline int IsInScreenRect(double* arrScreenBound, double fGeoX, double fGeoY){

/*
 *              2,3
    1 \  2 \  3
  ----------------
    4 \  5 \  6
  ----------------
    7 \  8 \  9
0,1
*/
    if(fGeoX < arrScreenBound[0] && fGeoY > arrScreenBound[3]){
        return 1;
    }
    if(fGeoX >= arrScreenBound[0] && fGeoX <= arrScreenBound[2]
            && fGeoY > arrScreenBound[3]){
        return 2;
    }
    if(fGeoX > arrScreenBound[2] && fGeoY > arrScreenBound[3]){
        return 3;
    }
    if(fGeoX < arrScreenBound[0]
            && fGeoY >= arrScreenBound[1] && fGeoY <= arrScreenBound[3]){
        return 4;
    }
    if(fGeoX >= arrScreenBound[0] && fGeoX <= arrScreenBound[2]
            && fGeoY >= arrScreenBound[1] && fGeoY <= arrScreenBound[3]){
        return 5;
    }
    if(fGeoX > arrScreenBound[2]
            && fGeoY >= arrScreenBound[1] && fGeoY <= arrScreenBound[3]){
        return 6;
    }
    if(fGeoX < arrScreenBound[0] && fGeoY < arrScreenBound[1]){
        return 7;
    }
    if(fGeoX >= arrScreenBound[0] && fGeoX <= arrScreenBound[2]
            && fGeoY < arrScreenBound[1]){
        return 8;
    }
    if(fGeoX > arrScreenBound[2] && fGeoY < arrScreenBound[1]){
        return 9;
    }
    return 0;
}
void CRasterDraw::DrawMosaicLines(GLuint nTextureID) {
    this->m_vecMosaicPoints.clear();
    if(this->m_pChunkMosaic == 0)
        return ;
    double* pGeoPoints3D = (double*)this->m_pChunkMosaic->m_vecChunkPoints.data();
    //unsigned long long nVecMosaicPointsCnt = (unsigned long long)this->m_pChunkMosaic->m_nChunkPointCnt;
    unsigned long long nVecMosaicPointsCnt = (unsigned long long)this->m_pChunkMosaic->m_vecChunkPoints.size();

    std::vector<glm::vec2> vecViewPointsf11;
    unsigned long long i = 0, j = 0, nPtr = 0;

    double fValX = 0.0, fValY = 0.0;
    double fValX0 = 9999999.9999, fValY0 = 9999999.9999;
    double fValX1 = 0.0, fValY1 = 0.0;
    double fValX2 = 0.0, fValY2 = 0.0;
    double fPixDistanceX = 5.0 * this->m_pGLCore->m_fGeo2ViewScaleH;
    double fPixDistanceY = 5.0 * this->m_pGLCore->m_fGeo2ViewScaleV;

    double fMinLng = this->m_pGLCore->t_geo_minmax.fGeoLeftBottomX;
    double fMinLat = this->m_pGLCore->t_geo_minmax.fGeoLeftBottomY;
    double fMaxLng = this->m_pGLCore->t_geo_minmax.fGeoRightTopX;
    double fMaxLat = this->m_pGLCore->t_geo_minmax.fGeoRightTopY;
    double arrScreenBound[4] = {fMinLng, fMinLat, fMaxLng, fMaxLat};
    t_Line2D tLineLeft(arrScreenBound[0], arrScreenBound[3], arrScreenBound[0], arrScreenBound[1]);
    t_Line2D tLineBottom(arrScreenBound[0], arrScreenBound[1], arrScreenBound[2], arrScreenBound[1]);
    t_Line2D tLineRight(arrScreenBound[2], arrScreenBound[1], arrScreenBound[2], arrScreenBound[3]);
    t_Line2D tLineTop(arrScreenBound[0], arrScreenBound[3], arrScreenBound[2], arrScreenBound[3]);

    std::vector<glm::dvec2> vecBoundPoint;
    std::vector<glm::dvec2> vecMoscPoints;

    vecBoundPoint.push_back(glm::dvec2(fMinLng, fMinLat));
    vecBoundPoint.push_back(glm::dvec2(fMaxLng, fMinLat));
    vecBoundPoint.push_back(glm::dvec2(fMaxLng, fMaxLat));
    vecBoundPoint.push_back(glm::dvec2(fMinLng, fMaxLat));

    //5.0 ^ 2 >= ((GeoX1 - GeoX2) * pCore->m_fView2GeoScaleH) ^ 2 + ((GeoY1 - GeoY2) * pCore->m_fView2GeoScaleV) ^ 2
    const double fLimitRange = 25.0;
    double fPointDistance = 0.0;
    double View2GeoScaleH_2 = this->m_pGLCore->m_fView2GeoScaleH * this->m_pGLCore->m_fView2GeoScaleH;
    double View2GeoScaleV_2 = this->m_pGLCore->m_fView2GeoScaleV * this->m_pGLCore->m_fView2GeoScaleV;

    double arrIntersectRet[4] = {0};
    int nLastPointOutside = 0;
    fValX = pGeoPoints3D[0];
    fValY = pGeoPoints3D[1];

    int nFullOutside = 0;
    int nIntersectPointCnt = 0;
    int nCurrPointOutside = 0;

    int nPrevRecordRange = -99, nInScreenRange = 0, nPrevRecordRangeIndx = -1, nRefreshNewIndx = 0;
    double fValRecordX = -999999999.999999999, fValRecordY = -999999999.999999999;
    double fRangeRecordX = -999999999.999999999, fRangeRecordY = -999999999.999999999;
    std::map<int, int> mapHasRecord;
    mapHasRecord.clear();
    glm::dvec2 pt0(fValX, fValY);
    glm::dvec2 pt1(fValX1, fValY1);
    nVecMosaicPointsCnt = nVecMosaicPointsCnt - 1;

    mapHasRecord.insert(std::pair<int, int>(0, 1));
    this->m_vecMosaicPoints.push_back(glm::dvec2(pGeoPoints3D[0], pGeoPoints3D[1]));

    for(i = 1; i<nVecMosaicPointsCnt; i++) {
        nPtr = i * 3;
        fValX = pGeoPoints3D[nPtr + 0];
        fValY = pGeoPoints3D[nPtr + 1];
        fValX1 = pGeoPoints3D[nPtr + 0 + 3];
        fValY1 = pGeoPoints3D[nPtr + 1 + 3];

        fPointDistance = (fValX - fValRecordX) * (fValX - fValRecordX) * View2GeoScaleH_2 + (fValY - fValRecordY) * (fValY - fValRecordY) * View2GeoScaleV_2;
        if(fPointDistance <= fLimitRange){
            continue;
        }

        fValRecordX = fValX;
        fValRecordY = fValY;
        nInScreenRange = IsInScreenRect(arrScreenBound, fValX, fValY);
        if(nInScreenRange == 5) {
            if(mapHasRecord.end() == mapHasRecord.find(i - 1)){
                mapHasRecord.insert(std::pair<int, int>(i - 1, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(pGeoPoints3D[nPtr + 0 - 3], pGeoPoints3D[nPtr + 0 - 2]));
            }
            if(mapHasRecord.end() == mapHasRecord.find(i)){
                mapHasRecord.insert(std::pair<int, int>(i, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            }
            if(mapHasRecord.end() == mapHasRecord.find(i + 1)){
                mapHasRecord.insert(std::pair<int, int>(i + 1, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX1, fValY1));
            }
            nPrevRecordRange = -99;
            nPrevRecordRangeIndx = -1;
            continue;
        }

        pt0.x = fValX;
        pt0.y = fValY;
        pt1.x = fValX1;
        pt1.y = fValY1;
        nIntersectPointCnt = this->GetScreenIntersectPoints(pt0, pt1, tLineLeft, tLineBottom, tLineRight, tLineTop, arrIntersectRet);
        if(nIntersectPointCnt > 0){
            if(mapHasRecord.end() == mapHasRecord.find(i - 1)){
                mapHasRecord.insert(std::pair<int, int>(i - 1, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(pGeoPoints3D[nPtr + 0 - 3], pGeoPoints3D[nPtr + 0 - 2]));
            }
            if(mapHasRecord.end() == mapHasRecord.find(i)){
                mapHasRecord.insert(std::pair<int, int>(i, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            }
            if(mapHasRecord.end() == mapHasRecord.find(i + 1)){
                mapHasRecord.insert(std::pair<int, int>(i + 1, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX1, fValY1));
            }
            nPrevRecordRange = -99;
            nPrevRecordRangeIndx = -1;
            continue;
        }

        if(nPrevRecordRange != nInScreenRange){
            if(nPrevRecordRangeIndx != -1 && mapHasRecord.end() == mapHasRecord.find(nPrevRecordRangeIndx)){
                mapHasRecord.insert(std::pair<int, int>(nPrevRecordRangeIndx, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(pGeoPoints3D[nPrevRecordRangeIndx * 3], pGeoPoints3D[nPrevRecordRangeIndx*3+1]));
            }
            if(mapHasRecord.end() == mapHasRecord.find(i - 1)){
                mapHasRecord.insert(std::pair<int, int>(i - 1, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(pGeoPoints3D[nPtr + 0 - 3], pGeoPoints3D[nPtr + 0 - 2]));
            }
            if(mapHasRecord.end() == mapHasRecord.find(i)){
                mapHasRecord.insert(std::pair<int, int>(i, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            }
            if(mapHasRecord.end() == mapHasRecord.find(i + 1)){
                mapHasRecord.insert(std::pair<int, int>(i + 1, 1));
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX1, fValY1));
            }
        }
        nRefreshNewIndx = 0;
        if(nPrevRecordRangeIndx == -1){
            fRangeRecordX = fValX;
            fRangeRecordY = fValY;
            nRefreshNewIndx = 1;
        }

        if(nPrevRecordRange == nInScreenRange){
            switch (nInScreenRange) {
            case 1:{
                if(/*fValX < fRangeRecordX || */fValY > fRangeRecordY) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            case 2:{
                if(fValY > fRangeRecordY) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            case 3:{
                if(/*fValX > fRangeRecordX || */fValY > fRangeRecordY) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            case 4:{
                if(fValX < fRangeRecordX) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            case 5:{
                break;
            }
            case 6:{
                if(fValX > fRangeRecordX) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            case 7:{
                if(/*fValX < fRangeRecordX || */fValY < fRangeRecordY) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            case 8:{
                if(fValY < fRangeRecordY) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            case 9:{
                if(fValX > fRangeRecordX || fValY < fRangeRecordY) {
                    nRefreshNewIndx = 1;
                }
                break;
            }
            }
        }
        if(1 == nRefreshNewIndx){
            fRangeRecordX = fValX;
            fRangeRecordY = fValY;
            nPrevRecordRangeIndx = (int)i;
        }

        nPrevRecordRange = nInScreenRange;
    }
    if(nPrevRecordRangeIndx != -1 && mapHasRecord.end() == mapHasRecord.find(nPrevRecordRangeIndx)){
        mapHasRecord.insert(std::pair<int, int>(nPrevRecordRangeIndx, 1));
        this->m_vecMosaicPoints.push_back(glm::dvec2(pGeoPoints3D[nPrevRecordRangeIndx * 3], pGeoPoints3D[nPrevRecordRangeIndx*3+1]));
    }
    if(mapHasRecord.end() == mapHasRecord.find(nVecMosaicPointsCnt)) {
        nPtr = nVecMosaicPointsCnt * 3;
        fValX = pGeoPoints3D[nPtr + 0];
        fValY = pGeoPoints3D[nPtr + 1];
        this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
    }

    nVecMosaicPointsCnt = this->m_vecMosaicPoints.size();
    CGeoRelation geometry;
    geometry.CreateGeoPolygon((int)nVecMosaicPointsCnt, (double*)this->m_vecMosaicPoints.data());
    geometry.IntersectGeoPolygon(4, this->m_pIntersectBound);

    std::vector<std::vector<glm::dvec2>> vecViewPointsAll;
    std::vector<std::vector<glm::dvec3>> vecViewPointsAllff;
    geometry.GetPoints(vecViewPointsAll);
    int nVecViewPointsAllCnt = (int)vecViewPointsAll.size();
    if(nVecViewPointsAllCnt <= 0) {
        int npi = 0;
        for(npi = 0; npi < nVecMosaicPointsCnt; npi++){
            printf("%03d %.9f  %.9f\n", npi, this->m_vecMosaicPoints[npi].x, this->m_vecMosaicPoints[npi].y);
        }
        //printf("%.9f  %.9f  %.9f  %.9f\n", fMinLng, fMinLat, fMaxLng, fMaxLat);

        for (npi = 0; npi < 8; npi++) {
            printf("%.9f  %.9f\n", this->m_pIntersectBound[npi*2], this->m_pIntersectBound[npi*2 + 1]);
        }
        printf("There is no Intersect Mosaic\n");
        return ;
    }

    int nVecViewPointsCnt = 0;
    std::vector<glm::dvec2> vecTrianglePoints;
    for(i = 0; i<nVecViewPointsAllCnt; i++) {
        std::vector<glm::dvec2>& vecViewPoints = vecViewPointsAll[i];
        vecViewPoints.erase(vecViewPoints.begin()+vecViewPoints.size()-1);
        {
            std::vector<glm::dvec3> vecViewPointsfff;
            nVecViewPointsCnt = (int)vecViewPoints.size();
            for(j = 0; j < nVecViewPointsCnt; j++) {
                fValX0 = vecViewPoints[j].x;
                fValY0 = vecViewPoints[j].y;
                //this->m_pGLCore->Geo2ViewPosition(fValX, fValY, &fValX0, &fValY0);
                vecViewPointsfff.push_back(glm::dvec3(fValX0, fValY0, 0.0));
            }
            vecViewPointsAllff.push_back(vecViewPointsfff);
        }
        std::vector<glm::dvec2> vecTrianglePoints1;
        CMisc::SnipTriangles(vecViewPoints, vecTrianglePoints1);
        vecTrianglePoints.insert(vecTrianglePoints.begin(), vecTrianglePoints1.begin(), vecTrianglePoints1.end());
    }
    int nVecTrianglePointsCnt = (int)vecTrianglePoints.size();
    if(nVecTrianglePointsCnt <= 0) {
        printf("There is no vecTrianglePoints\n");
        return;
    }

    double fIntersectBoundLeftBottomX = this->m_pIntersectBound[0];
    double fIntersectBoundLeftBottomY = this->m_pIntersectBound[1];
    double fIntersectBoundWidth = this->m_pIntersectBound[2] - this->m_pIntersectBound[0];
    double fIntersectBoundHeight = this->m_pIntersectBound[7] - this->m_pIntersectBound[1];

    std::vector<glm::vec2> vecViewPointsf;
    std::vector<glm::vec4> vecView_TexCoordPoints;

    for( i = 0; i<nVecTrianglePointsCnt; i++) {
        fValX = vecTrianglePoints[i].x;
        fValY = vecTrianglePoints[i].y;
        this->m_pGLCore->Geo2ViewPosition(fValX, fValY, &fValX0, &fValY0);
        fValX1 = (fValX - fIntersectBoundLeftBottomX) / fIntersectBoundWidth;
        fValY1 = (fValY - fIntersectBoundLeftBottomY) / fIntersectBoundHeight;
        fValY1 = 1.0 - fValY1;
        if(fValX1 < 0.00000001){
            fValX1 = 0.0;
        }
        if(fValY1 > 1.0){
            fValY1 = 1.0;
        }
        vecView_TexCoordPoints.push_back(glm::vec4(fValX0, fValY0, fValX1, fValY1));
        vecViewPointsf.push_back(glm::vec2(fValX0, fValY0));
    }
    this->m_pGLCore->DrawImage(nVecTrianglePointsCnt, (float*)vecView_TexCoordPoints.data(), nTextureID);
    const int nDrawToolLine = 0;
    if(nDrawToolLine){
        this->m_pGLCore->DrawTriPolygon(nVecTrianglePointsCnt, (float*)vecViewPointsf.data(), 0xFF00FFFF);

        for(i = 0; i<nVecViewPointsAllCnt; i++){
            std::vector<glm::dvec3>* pVecViewPointsfff = &(vecViewPointsAllff[i]);
            nVecViewPointsCnt = (int)pVecViewPointsfff->size();
            if(nVecViewPointsCnt < 3){
                continue;
            }
            std::vector<unsigned int> vecEleIndx;
            for(j = 0; j<nVecViewPointsCnt; j++)
                vecEleIndx.push_back(j);
            this->m_pGLCore->DrawLinesLoopElement(nVecViewPointsCnt, (double*)pVecViewPointsfff->data(), nVecViewPointsCnt, (unsigned int*)vecEleIndx.data(), 0xFFFF00FF, 2.0f);
        }
    }
}

void CRasterDraw::DrawMosaicLines_bak1(GLuint nTextureID) {
    this->m_vecMosaicPoints.clear();

    std::vector<glm::vec2> vecViewPointsf11;
    int i = 0, j = 0;
    double fValX = 0.0, fValY = 0.0;
    double fValX0 = 9999999.9999, fValY0 = 9999999.9999;
    double fValX1 = 0.0, fValY1 = 0.0;
    double fValX2 = 0.0, fValY2 = 0.0;
    double* pGeoPoints3D = (double*)this->m_pChunkMosaic->m_vecChunkPoints.data();
    //int nVecMosaicPointsCnt = this->m_pChunkMosaic->m_nChunkPointCnt;
    int nVecMosaicPointsCnt = (int)this->m_pChunkMosaic->m_vecChunkPoints.size();
    double fPixDistanceX = 5.0 * this->m_pGLCore->m_fGeo2ViewScaleH;
    double fPixDistanceY = 5.0 * this->m_pGLCore->m_fGeo2ViewScaleV;
    double fPixOffX = 0.0;//2.0 * this->m_pGLCore->m_fGeo2ViewScaleH;
    double fPixOffY = 0.0;//2.0 * this->m_pGLCore->m_fGeo2ViewScaleV;
    ;
    double fMinLng = this->m_pGLCore->t_geo_minmax.fGeoLeftBottomX;
    double fMinLat = this->m_pGLCore->t_geo_minmax.fGeoLeftBottomY;
    double fMaxLng = this->m_pGLCore->t_geo_minmax.fGeoRightTopX;
    double fMaxLat = this->m_pGLCore->t_geo_minmax.fGeoRightTopY;
    double arrScreenBound[4] = {fMinLng, fMinLat, fMaxLng, fMaxLat};
    t_Line2D tLineLeft(arrScreenBound[0], arrScreenBound[3], arrScreenBound[0], arrScreenBound[1]);
    t_Line2D tLineBottom(arrScreenBound[0], arrScreenBound[1], arrScreenBound[2], arrScreenBound[1]);
    t_Line2D tLineRight(arrScreenBound[2], arrScreenBound[1], arrScreenBound[2], arrScreenBound[3]);
    t_Line2D tLineTop(arrScreenBound[0], arrScreenBound[3], arrScreenBound[2], arrScreenBound[3]);

    std::vector<glm::dvec2> vecBoundPoint;
    std::vector<glm::dvec2> vecMoscPoints;
    //vecBoundPoint.resize(8);
    //memcpy(vecBoundPoint.data(), this->m_pGLCore->GetScreenGeoBound(), sizeof(double)*8);
    vecBoundPoint.push_back(glm::dvec2(fMinLng, fMinLat));
    vecBoundPoint.push_back(glm::dvec2(fMaxLng, fMinLat));
    vecBoundPoint.push_back(glm::dvec2(fMaxLng, fMaxLat));
    vecBoundPoint.push_back(glm::dvec2(fMinLng, fMaxLat));

    unsigned long long nPtr = 0;
    //for(i = 0; i<nVecMosaicPointsCnt; i++){
    //    nPtr = i*3;
    //    fValX  = pGeoPoints3D[nPtr];
    //    fValY  = pGeoPoints3D[nPtr + 1];
    //    vecMoscPoints.push_back(glm::dvec2(fValX,fValY));
    //}
    //PolygonClip(vecMoscPoints, vecBoundPoint, this->m_vecMosaicPoints);

    double arrIntersectRet[4] = {0};
    int nLastPointOutside = 0;
    fValX = pGeoPoints3D[0];
    fValY = pGeoPoints3D[1];
    //if(fValX <= fMaxLng && fValY <= fMaxLat && fValX >= fMinLng && fValY >= fMinLat){
    //    nLastPointOutside = 0;
    //} else {
    //    nLastPointOutside = 1;
    //}
    //if((fValX > fMaxLng && fValY > fMaxLat)){
    //    fValX = fMaxLng;
    //    fValY = fMaxLat;
    //}else if(fValX < fMinLng && fValY < fMinLat){
    //    fValX = fMinLng;
    //    fValY = fMinLat;
    //}
    //this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
    //vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
    //fValX0 = fValX;
    //fValY0 = fValY;

    //fValX1 = fValX;
    //fValY1 = fValY;

    int nFullOutside = 0;
    int nIntersectPointCnt = 0;
    int nCurrPointOutside = 0;
    int nExeFlag = 0;
//    if(0){
    for(i = 0; i<nVecMosaicPointsCnt; i++){
        nPtr = i*3;
        fValX  = pGeoPoints3D[nPtr];
        fValY  = pGeoPoints3D[nPtr + 1];

        fValX1 = pGeoPoints3D[nPtr + 3];
        fValY1 = pGeoPoints3D[nPtr + 4];
        if((fValX <= fMaxLng && fValY <= fMaxLat && fValX >= fMinLng && fValY >= fMinLat)){
            nCurrPointOutside = 0;

            if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
                continue;
            }

            this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
            fValX0 = fValX;
            fValY0 = fValY;
            //continue;

            if((fValX1 <= fMaxLng && fValY1 <= fMaxLat && fValX1 >= fMinLng && fValY1 >= fMinLat)){
                continue;
            }
        }

        //fValX2 = pGeoPoints3D[nPtr - 3];
        //fValY2 = pGeoPoints3D[nPtr - 2];


        glm::dvec2 pt0(fValX, fValY);
        glm::dvec2 pt1(fValX1, fValY1);
        nIntersectPointCnt = this->GetScreenIntersectPoints(pt0, pt1, tLineLeft, tLineBottom, tLineRight, tLineTop, arrIntersectRet);

        if(nIntersectPointCnt <= 0) {
            double fMinX = std::min(fValX, fValX1);
            double fMinY = std::min(fValY, fValY1);
            double fMaxX = std::max(fValX, fValX1);
            double fMaxY = std::max(fValY, fValY1);
            if(fMinX > fMaxLng){
                fValX = fMaxLng + fPixOffX;
                if(fValY > fMaxLat){
                    fValY = fMaxLat;
                }else if(fValY < fMinLat){
                    fValY = fMinLat;
                }
                nFullOutside = 1;
            }else if(fMaxX < fMinLng){
                fValX = fMinLng - fPixOffX;
                if(fValY > fMaxLat){
                    fValY = fMaxLat;
                }else if(fValY < fMinLat){
                    fValY = fMinLat;
                }
                nFullOutside = 1;
            }else if(fMinY > fMaxLat){
                fValY = fMaxLat;
                if(fValX > fMaxLng){
                    fValX = fMaxLng;
                }else if(fValX < fMinLng){
                    fValX = fMinLng;
                }
                nFullOutside = 1;
            }else if(fMaxY < fMinLat){
                fValY = fMinLat;
                if(fValX > fMaxLng){
                    fValX = fMaxLng;
                }else if(fValX < fMinLng){
                    fValX = fMinLng;
                }
                nFullOutside = 1;
            }
            if(nFullOutside == 1) {
                if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
                    continue;
                }
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
                vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
                fValX0 = fValX;
                fValY0 = fValY;
                continue;
            }
            {
                if(fValX < fMinLng){
                    fValX = fMinLng;
                    if(fValY > fValY1){
                        fValY = fMinLat;
                    } else {
                        fValY = fMaxLat;
                    }
                } else if(fValX > fMaxLng){
                    fValX = fMaxLng;
                    if(fValY > fValY1){
                        fValY = fMinLat;
                    } else {
                        fValY = fMaxLat;
                    }
                }else{
                    if (fValY < fMinLat){
                        fValY = fMinLat;
                        if(fValX > fValX1){
                            fValX = fMinLng;
                        } else {
                            fValX = fMaxLng;
                        }
                    } else {
                        fValY = fMaxLat;
                        if (fValX > fValX1) {
                            fValX = fMinLng;
                        } else {
                            fValX = fMaxLng;
                        }
                    }
                }
                if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
                    continue;
                }
                this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
                vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
                fValX0 = fValX;
                fValY0 = fValY;
            }
            continue;
        }

        for(int jjj = 0; jjj < nIntersectPointCnt;jjj++){

            fValX = arrIntersectRet[jjj * 2];
            fValY = arrIntersectRet[jjj * 2 + 1];
            if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
                continue;
            }

            this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
            fValX0 = fValX;
            fValY0 = fValY;
        }
        continue;

        if((fValX <= fMaxLng && fValY <= fMaxLat && fValX >= fMinLng && fValY >= fMinLat) ||
            (fValX1 <= fMaxLng && fValY1 <= fMaxLat && fValX1 >= fMinLng && fValY1 >= fMinLat) ||
            (fValX2 <= fMaxLng && fValY2 <= fMaxLat && fValX2 >= fMinLng && fValY2 >= fMinLat)){
            nCurrPointOutside = 0;

            if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
                continue;
            }

            this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
            fValX0 = fValX;
            fValY0 = fValY;
            continue;

        } else {
            nCurrPointOutside = 1;
        }


        nFullOutside = 0;
        double fMinX = std::min(fValX, fValX1);
        double fMinY = std::min(fValY, fValY1);
        double fMaxX = std::max(fValX, fValX1);
        double fMaxY = std::max(fValY, fValY1);
        if(fMinX > fMaxLng){
            fValX = fMaxLng + fPixOffX;
            if(fValY > fMaxLat){
                fValY = fMaxLat;
            }else if(fValY < fMinLat){
                fValY = fMinLat;
            }
            nFullOutside = 1;
        }else if(fMaxX < fMinLng){
            fValX = fMinLng - fPixOffX;
            if(fValY > fMaxLat){
                fValY = fMaxLat;
            }else if(fValY < fMinLat){
                fValY = fMinLat;
            }
            nFullOutside = 1;
        }else if(fMinY > fMaxLat){
            fValY = fMaxLat;
            if(fValX > fMaxLng){
                fValX = fMaxLng;
            }else if(fValX < fMinLng){
                fValX = fMinLng;
            }
            nFullOutside = 1;
        }else if(fMaxY < fMinLat){
            fValY = fMinLat;
            if(fValX > fMaxLng){
                fValX = fMaxLng;
            }else if(fValX < fMinLng){
                fValX = fMinLng;
            }
            nFullOutside = 1;
        }
        if(nFullOutside == 1) {
            if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
                continue;
            }
            this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
            fValX0 = fValX;
            fValY0 = fValY;
            continue;
        }
        if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
            continue;
        }

        this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
        vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
        fValX0 = fValX;
        fValY0 = fValY;
        continue;

        //glm::dvec2 pt0(fValX, fValY);
        //glm::dvec2 pt1(fValX1, fValY1);

        nIntersectPointCnt = this->GetScreenIntersectPoints(pt0, pt1, tLineLeft, tLineBottom, tLineRight, tLineTop, arrIntersectRet);

        if (nIntersectPointCnt <= 0){
            nIntersectPointCnt = 1;

            if(fValX > fMaxLng){
                fValX = fMaxLng + fPixOffX;
                if(fValY > fMaxLat){
                    fValY = fMaxLat;
                }else{
                    fValY = fMinLat;
                }
            }else{
                fValX = fMinLng + fPixOffX;
                if(fValY > fMaxLat){
                    fValY = fMaxLat;
                }else{
                    fValY = fMinLat;
                }
            }

            arrIntersectRet[0] = fValX;
            arrIntersectRet[1] = fValY;
        }

        nLastPointOutside = nCurrPointOutside;
        for(int jjj = 0; jjj < nIntersectPointCnt;jjj++){

            fValX = arrIntersectRet[jjj * 2];
            fValY = arrIntersectRet[jjj * 2 + 1];
            if(fabs(fValX - fValX0) < fPixDistanceX && fabs(fValY - fValY0) < fPixDistanceY){
                continue;
            }

            this->m_vecMosaicPoints.push_back(glm::dvec2(fValX, fValY));
            vecViewPointsf11.push_back(glm::vec2(fValX, fValY));
            fValX0 = fValX;
            fValY0 = fValY;
        }
    }

    double* pGeoPoints2D = nullptr;
    std::vector<int> vecDirectionIndx;
    int nDirectionFlag = 0, nDirection = 0, nDelIndxCnt = 0, nDelIndx = 0;
    int nLoopCnt = 4, nLoopFlag = 1;

l_SxRasterDraw_DrawMosaicLines_RemoveRepeatIndx:
    nVecMosaicPointsCnt = (int)this->m_vecMosaicPoints.size();    
    pGeoPoints2D = (double*)this->m_vecMosaicPoints.data();
    if(nLoopFlag == 1){
        nLoopCnt = nVecMosaicPointsCnt;
    }
    for(i = nVecMosaicPointsCnt - 1; i>=2; i--){
        nPtr = i*2;
        fValX0 = pGeoPoints2D[nPtr];
        fValY0 = pGeoPoints2D[nPtr + 1];

        fValX1 = pGeoPoints2D[nPtr - 2];
        fValY1 = pGeoPoints2D[nPtr + 1 - 2];

        vecDirectionIndx.clear();

        nDirection = 0;
        nDirectionFlag = 0;
        if(fabs(fValX0 - fValX1) <= 0.000000001){
            nDirectionFlag = 1;
            if(nDirection == 0){
                if(fValY1 > fValY0){
                    nDirection = 1;
                }else{
                    nDirection = -1;
                }
            }
        }else if (fabs(fValY0 - fValY1) <= 0.000000001){
            nDirectionFlag = -1;
            if(nDirection == 0){
                if(fValX1 > fValX0){
                    nDirection = 1;
                }else{
                    nDirection = -1;
                }
            }

        }else{
            continue;
        }
        //vecDirectionIndx.push_back(i);
        vecDirectionIndx.push_back(i - 1);

        for(j = i - 2 ; j >=2 ;j --){
            nPtr = j*2;
            fValX2 = pGeoPoints2D[nPtr];
            fValY2 = pGeoPoints2D[nPtr + 1];

            if(nDirectionFlag == 1) {
                if( !(fabs(fValX2 - fValX0) <= 0.00000001) ){
                    break;
                }
            } else {
                if( !(fabs(fValY2 - fValY0) <= 0.00000001) ){
                    break;
                }
            }
            vecDirectionIndx.push_back(j);
        }

        nDelIndxCnt = (int)vecDirectionIndx.size();
        if(nDelIndxCnt < 3){
            continue;
        }

        nDelIndxCnt = nDelIndxCnt - 1;
        for(j = 0; j< nDelIndxCnt; j++){
            nDelIndx = vecDirectionIndx[j];
            this->m_vecMosaicPoints.erase(this->m_vecMosaicPoints.begin() + nDelIndx);
        }
        goto l_SxRasterDraw_DrawMosaicLines_RemoveRepeatIndx;
    }
    if(nLoopCnt > 0){
        this->m_vecMosaicPoints.push_back(this->m_vecMosaicPoints[0]);
        this->m_vecMosaicPoints.erase(this->m_vecMosaicPoints.begin());
        nLoopCnt--;
        nLoopFlag = 0;
        goto l_SxRasterDraw_DrawMosaicLines_RemoveRepeatIndx;
    }

    nLoopFlag = 1;
l_SxRasterDraw_DrawMosaicLines_RemoveRepeatIndx2:
    nVecMosaicPointsCnt = (int)this->m_vecMosaicPoints.size();
    pGeoPoints2D = (double*)this->m_vecMosaicPoints.data();
    if(nLoopFlag == 1){
        nLoopCnt = nVecMosaicPointsCnt;
    }
    for(i = 0; i<nVecMosaicPointsCnt-2; i++){
        nPtr = i*2;
        fValX0 = pGeoPoints2D[nPtr];
        fValY0 = pGeoPoints2D[nPtr + 1];

        fValX1 = pGeoPoints2D[nPtr + 2];
        fValY1 = pGeoPoints2D[nPtr + 1 + 2];

        fValX2 = pGeoPoints2D[nPtr + 4];
        fValY2 = pGeoPoints2D[nPtr + 1 + 4];

        if(fabs(fValX0 - fValX1) <= 0.000000001 && fabs(fValX2 - fValX1) <= 0.000000001){
            if((fValY0 > fValY1 && fValY2 > fValY1) || (fValY0 < fValY1 && fValY2 < fValY1) ){

                this->m_vecMosaicPoints.erase(this->m_vecMosaicPoints.begin() + i + 1);
                goto l_SxRasterDraw_DrawMosaicLines_RemoveRepeatIndx2;
            }
        }else if (fabs(fValY0 - fValY1) <= 0.000000001 && fabs(fValY2 - fValY1) <= 0.000000001){
            if((fValX0 > fValX1 && fValX2 > fValX1) || (fValX0 < fValX1 && fValX2 < fValX1) ){

                this->m_vecMosaicPoints.erase(this->m_vecMosaicPoints.begin() + i + 1);
                goto l_SxRasterDraw_DrawMosaicLines_RemoveRepeatIndx2;
            }
        }

    }
    if(nLoopCnt > 0){
        this->m_vecMosaicPoints.push_back(this->m_vecMosaicPoints[0]);
        this->m_vecMosaicPoints.erase(this->m_vecMosaicPoints.begin());
        nLoopCnt--;
        nLoopFlag = 0;
        goto l_SxRasterDraw_DrawMosaicLines_RemoveRepeatIndx2;
    }

    CGeoRelation geometry;
    geometry.CreateGeoPolygon(nVecMosaicPointsCnt, (double*)this->m_vecMosaicPoints.data());
    geometry.IntersectGeoPolygon(4, this->m_pIntersectBound);

    std::vector<std::vector<glm::dvec2>> vecViewPointsAll;
    std::vector<std::vector<glm::dvec3>> vecViewPointsAllff;
    geometry.GetPoints(vecViewPointsAll);
    int nVecViewPointsAllCnt = (int)vecViewPointsAll.size();
    if(nVecViewPointsAllCnt <= 0){
        printf("There is no Intersect Mosaic\n");
        return ;
    }

    int nVecViewPointsCnt = 0;
    std::vector<glm::dvec2> vecTrianglePoints;
    for(i = 0; i<nVecViewPointsAllCnt; i++) {
        std::vector<glm::dvec2>& vecViewPoints = vecViewPointsAll[i];
        vecViewPoints.erase(vecViewPoints.begin()+vecViewPoints.size()-1);
        {
            std::vector<glm::dvec3> vecViewPointsfff;
            nVecViewPointsCnt = (int)vecViewPoints.size();
            for(j = 0; j < nVecViewPointsCnt; j++) {
                fValX0 = vecViewPoints[j].x;
                fValY0 = vecViewPoints[j].y;
                //this->m_pGLCore->Geo2ViewPosition(fValX, fValY, &fValX0, &fValY0);
                vecViewPointsfff.push_back(glm::dvec3(fValX0, fValY0, 0.0));
            }
            vecViewPointsAllff.push_back(vecViewPointsfff);
        }
        std::vector<glm::dvec2> vecTrianglePoints1;
        CMisc::SnipTriangles(vecViewPoints, vecTrianglePoints1);
        vecTrianglePoints.insert(vecTrianglePoints.begin(), vecTrianglePoints1.begin(), vecTrianglePoints1.end());
    }
    int nVecTrianglePointsCnt = (int)vecTrianglePoints.size();
    if(nVecTrianglePointsCnt <= 0) {
        printf("There is no vecTrianglePoints\n");
        return ;
    }

    double fIntersectBoundLeftBottomX = this->m_pIntersectBound[0];
    double fIntersectBoundLeftBottomY = this->m_pIntersectBound[1];
    double fIntersectBoundWidth = this->m_pIntersectBound[2] - this->m_pIntersectBound[0];
    double fIntersectBoundHeight = this->m_pIntersectBound[7] - this->m_pIntersectBound[1];

    std::vector<glm::vec2> vecViewPointsf;
    std::vector<glm::vec4> vecView_TexCoordPoints;

    for( i = 0; i<nVecTrianglePointsCnt; i++) {
        fValX = vecTrianglePoints[i].x;
        fValY = vecTrianglePoints[i].y;
        this->m_pGLCore->Geo2ViewPosition(fValX, fValY, &fValX0, &fValY0);
        fValX1 = (fValX - fIntersectBoundLeftBottomX) / fIntersectBoundWidth;
        fValY1 = (fValY - fIntersectBoundLeftBottomY) / fIntersectBoundHeight;
        fValY1 = 1.0 - fValY1;
        if(fValX1 < 0.00000001){
            fValX1 = 0.0;
        }
        if(fValY1 > 1.0){
            fValY1 = 1.0;
        }
        vecView_TexCoordPoints.push_back(glm::vec4(fValX0, fValY0, fValX1, fValY1));
        vecViewPointsf.push_back(glm::vec2(fValX0, fValY0));
    }
    this->m_pGLCore->DrawImage(nVecTrianglePointsCnt, (float*)vecView_TexCoordPoints.data(), nTextureID);
    this->m_pGLCore->DrawTriPolygon(nVecTrianglePointsCnt, (float*)vecViewPointsf.data(), 0xFF00FFFF);

    for(i = 0; i<nVecViewPointsAllCnt; i++){
        std::vector<glm::dvec3>* pVecViewPointsfff = &(vecViewPointsAllff[i]);
        nVecViewPointsCnt = (int)pVecViewPointsfff->size();
        if(nVecViewPointsCnt < 3){
            continue;
        }
        std::vector<unsigned int> vecEleIndx;
        for(j = 0; j<nVecViewPointsCnt; j++)
            vecEleIndx.push_back(j);
        this->m_pGLCore->DrawLinesLoopElement(nVecViewPointsCnt, (double*)pVecViewPointsfff->data(), nVecViewPointsCnt, (unsigned int*)vecEleIndx.data(), 0xFFFF00FF, 2.0f);
    }
}

void CRasterDraw::DrawMosaicLines_bak(GLuint nTextureID){
    std::vector<glm::vec4> vecView_TexCoordPoints;
    std::vector<glm::dvec2> vecViewPoints;
    std::vector<glm::dvec2> vecViewPoints1;
    std::vector<glm::dvec2> vecTrianglePoints;
    std::vector<glm::vec2> vecViewPointsf;
    CMisc::SnipTriangles(this->m_vecMosaicPoints, vecViewPoints1);

    double fValX = 0.0, fValY = 0.0;
    double fValX0 = 0.0, fValY0 = 0.0;
    double fValX1 = 0.0, fValY1 = 0.0;
    int i = 0, nVecMosaicPointsCnt = (int)this->m_vecMosaicPoints.size();
    for( i = 0; i<nVecMosaicPointsCnt; i++) {
        fValX = this->m_vecMosaicPoints[i].x;
        fValY = this->m_vecMosaicPoints[i].y;
        this->m_pGLCore->Geo2ViewPosition(fValX, fValY, &fValX0, &fValY0);
        vecViewPoints.push_back(glm::dvec2(fValX0, fValY0));
    }

    CMisc::SnipTriangles(vecViewPoints, vecTrianglePoints);

    int nVecTrianglePointsCnt = (int)vecTrianglePoints.size();
    double fMinLng = this->m_pReadRaster->m_fMinLng;
    double fMinLat = this->m_pReadRaster->m_fMinLat;
    double fGeoWidth = this->m_pReadRaster->m_fLngWidht;
    double fGeoHeight = this->m_pReadRaster->m_fLatHeight;

    double ff0 = vecTrianglePoints[i].x, ff1 = vecTrianglePoints[i].y, ff2 = ff0, ff3 = ff1;
    for( i = 0; i<nVecTrianglePointsCnt; i++) {
        fValX0 = vecTrianglePoints[i].x;
        fValY0 = vecTrianglePoints[i].y;
        //this->m_pGLCore->View2GeoPosition(fValX, fValY, &fValX0, &fValY0);
        fValX = vecViewPoints1[i].x;
        fValY = vecViewPoints1[i].y;
        fValX1 = (fValX - fMinLng) / fGeoWidth;
        fValY1 = (fValY - fMinLat) / fGeoHeight;
        if(fValX1 < 0.00000001){
            fValX1 = 0.0;
        }
        if(fValY1 > 1.0){
            fValY1 = 1.0;
        }
        vecView_TexCoordPoints.push_back(glm::vec4(fValX0, fValY0, fValX1, fValY1));
        vecViewPointsf.push_back(glm::vec2(fValX0, fValY0));
        if(ff0 > fValX)
            ff0 = fValX;
        if(ff1 > fValY)
            ff1 = fValY;
        if(ff2 < fValX)
            ff2 = fValX;
        if(ff3 < fValY)
            ff3 = fValY;
    }
    //this->m_pGLCore->DrawImage((float)this->m_fViewLeftBottomX, (float)this->m_fViewLeftBottomY, (float)this->m_fViewShowLenX, (float)this->m_fViewShowLenY, this->m_nTextureID);

    this->m_pGLCore->DrawImage(nVecTrianglePointsCnt, (float*)vecView_TexCoordPoints.data(), nTextureID);

    //this->m_pGLCore->DrawImage((float)ff0, (float)ff1, (float)(ff2 - ff0), (float)(ff3 - ff1), this->m_nTextureID);
    this->m_pGLCore->DrawPolygon(nVecTrianglePointsCnt, (float*)vecViewPointsf.data(), 0xFF00FFFF);

    //this->m_pGLCore->DrawPolygon(nVecTrianglePointsCnt, (float*)vecViewPoints.data(), 0xFF00FFFF);
}

void CRasterDraw::Draw() {

    if(this->m_nShowOrHide == ESHOWORHIDE::e_hide){
        this->m_nPreDrawDone = 1;
        return ;
    }
    while (this->m_nPreDrawDone == 0){
        //printf("111 Wait for PreDrawDone...\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    }
    if (this->m_pPixData == nullptr){
        return;
    }
    int nDeleteTextrueIDRet = this->m_pGLCore->RemoveTextureByFile(this->m_sRasterImgFileFullPath.c_str());
    //printf("nDeleteTextrueIDRet : %d\n", nDeleteTextrueIDRet);
    //GLuint nTextureID = pThis->m_pGLCore->LoadTextureFromFile(pThis->m_sRasterImgFileFullPath.c_str());
    GLuint nTextureID = this->m_pGLCore->CreateTexture2DFromImgBuffer(
                this->m_sRasterImgFileFullPath.c_str(), this->m_pPixData, this->m_nViewIntersectLenX, this->m_nViewIntersectLenY, 4);

    this->m_nTextureID = nTextureID;
    if (this->m_nTextureID == -1) {
        return;
    }

    delete[] this->m_pPixData;
    this->m_pPixData = nullptr;

    if(this->m_nShowMosaicLine == 1) {
        this->DrawMosaicLines(this->m_nTextureID);
    } else {
        this->m_pGLCore->DrawImage((float)this->m_fViewLeftBottomX, (float)this->m_fViewLeftBottomY, (float)this->m_fViewShowLenX, (float)this->m_fViewShowLenY, this->m_nTextureID);
    }

    //this->m_pGLCore->DrawRect(fViewLeftBottomX, fViewLeftBottomY, fViewLeftBottomX+nViewIntersectLenX, fViewLeftBottomY+nViewIntersectLenY,
    //                          0xFFFF00FF,0xFF0000FF, 0xFFFF00FF, 0xFF00FFFF);
    //this->m_pGLCore->DrawLines(4, fViewRect, 0xFF0000FF);
    //this->m_pGLCore->DrawRect((float)this->m_fViewLeftBottomX, (float)this->m_fViewLeftBottomY,
    //                          (float)this->m_fViewShowLenX + this->m_fViewLeftBottomX, (float)this->m_fViewShowLenY + m_fViewLeftBottomY, 0xFF00FFFF);
    this->m_pGLCore->DrawString((this->m_sRasterImgFileFullPath.substr(this->m_sRasterImgFileFullPath.rfind('/')+1)).c_str(),
                                this->m_fViewLeftBottomX, this->m_fViewLeftBottomY,
                                0.5f, 0xFF0000FF);
}

void CRasterDraw::PreDraw(void* pParam) {
    time_t ss = clock();
    CRasterDraw* pThis = (CRasterDraw*)pParam;
    pThis->m_nPreDrawDone = 0;
    pThis->m_nTextureID = -1;
    pThis->m_pPixData = nullptr;
    //time_t ss = clock();
    if(pThis->m_nShowOrHide == ESHOWORHIDE::e_hide){
        pThis->m_nPreDrawDone = 1;
        return ;
    }
    double arrGeoIntersect[4]={pThis->m_pReadRaster->m_fMinLng, pThis->m_pReadRaster->m_fMinLat,
                           pThis->m_pReadRaster->m_fMaxLng, pThis->m_pReadRaster->m_fMaxLat};

    double fViewLeftBottomX = 0, fViewLeftBottomY = 0;

    if(pThis->m_nRollerBlinding == 1){
        if(0 == pThis->m_pGLCore->GetGeoIntersectRangeByViewEx(arrGeoIntersect, arrGeoIntersect)){
            pThis->m_nPreDrawDone = 1;
            return;
        }
    }else{
        if(0 == pThis->m_pGLCore->GetGeoIntersectRangeByView(arrGeoIntersect, arrGeoIntersect)){
            pThis->m_nPreDrawDone = 1;
            return;
        }
    }

    double fPixX = 0, fPixY = 0;
    pThis->m_pReadRaster->GeoPos2PixPos(arrGeoIntersect[0], arrGeoIntersect[1], &fPixX, &fPixY);
    int nPixX0 = (int)fPixX;
    int nPixY0 = (int)ceil(fPixY);
    if(nPixY0 > (int)pThis->m_pReadRaster->m_nPixHeight){
        nPixY0 = (int)pThis->m_pReadRaster->m_nPixHeight;
    }
    pThis->m_pReadRaster->GeoPos2PixPos(arrGeoIntersect[2], arrGeoIntersect[3], &fPixX, &fPixY);
    int nPixX1 = (int)ceil(fPixX);
    if(nPixX1 > (int)pThis->m_pReadRaster->m_nPixWidth){
        nPixX1 = (int)pThis->m_pReadRaster->m_nPixWidth;
    }
    int nPixY1 = (int)(fPixY);

    int nReadSrcX = nPixX1 - nPixX0;
    int nReadSrcY = nPixY0 - nPixY1;
    if(nReadSrcX <= 0)
        nReadSrcX = 1;
    if(nReadSrcY <= 0)
        nReadSrcY = 1;
    double fViewLeftBottomX0, fViewLeftBottomY0, fViewLeftBottomX1, fViewLeftBottomY1;
    double fGeoX0 = 0.0, fGeoY0 = 0.0;
    pThis->m_pReadRaster->PixPos2GeoPos((double)nPixX0, (double)nPixY0, &fGeoX0, &fGeoY0);
    pThis->m_pGLCore->Geo2ViewPosition(fGeoX0, fGeoY0, &fViewLeftBottomX0, &fViewLeftBottomY0);

    double fGeoX1 = 0.0, fGeoY1 = 0.0;
    pThis->m_pReadRaster->PixPos2GeoPos((double)nPixX1, (double)nPixY1, &fGeoX1, &fGeoY1);
    pThis->m_pGLCore->Geo2ViewPosition(fGeoX1, fGeoY1, &fViewLeftBottomX1, &fViewLeftBottomY1);

    pThis->m_pIntersectBound[0] = fGeoX0;
    pThis->m_pIntersectBound[1] = fGeoY0;
    pThis->m_pIntersectBound[2] = fGeoX1;
    pThis->m_pIntersectBound[3] = fGeoY0;
    pThis->m_pIntersectBound[4] = fGeoX1;
    pThis->m_pIntersectBound[5] = fGeoY1;
    pThis->m_pIntersectBound[6] = fGeoX0;
    pThis->m_pIntersectBound[7] = fGeoY1;

    double fViewIntersectLenX = fViewLeftBottomX1 - fViewLeftBottomX0;
    double fViewIntersectLenY = fViewLeftBottomY1 - fViewLeftBottomY0;
    double fViewShowLenX = (fViewIntersectLenX);
    double fViewShowLenY = (fViewIntersectLenY);

    if(fViewIntersectLenX < 2000.0 || fViewIntersectLenY < 2000.0){
        fViewIntersectLenX = fViewIntersectLenX * 2.0;
        fViewIntersectLenY = fViewIntersectLenY * 2.0;
    }
    if(fViewIntersectLenX > pThis->m_pReadRaster->m_nPixWidth){
        fViewIntersectLenX = pThis->m_pReadRaster->m_nPixWidth;
        fViewIntersectLenY = fViewIntersectLenX / pThis->m_pReadRaster->m_fWidthScaleHeight;
    }
    else if (fViewIntersectLenY > pThis->m_pReadRaster->m_nPixHeight){
        fViewIntersectLenY = pThis->m_pReadRaster->m_nPixHeight;
        fViewIntersectLenX = fViewIntersectLenY * pThis->m_pReadRaster->m_fWidthScaleHeight;
    }
    int nViewIntersectLenX = (int)fViewIntersectLenX;
    int nViewIntersectLenY = (int)fViewIntersectLenY;

    if(nReadSrcX <= 2 && nViewIntersectLenX > pThis->m_pGLCore->m_nWndWidth * 3){
        nViewIntersectLenY = (int)((double)pThis->m_pGLCore->m_nWndWidth * 3.0 * fViewIntersectLenY / fViewIntersectLenX);
        nViewIntersectLenX = pThis->m_pGLCore->m_nWndWidth * 3;
    }
    else if(nReadSrcY <= 2 && nViewIntersectLenY > pThis->m_pGLCore->m_nWndHeight){
        nViewIntersectLenX = (int)((double)pThis->m_pGLCore->m_nWndHeight * 3.0 * fViewIntersectLenX / fViewIntersectLenY);
        nViewIntersectLenY = pThis->m_pGLCore->m_nWndHeight * 3;
    }

    pThis->m_pReadRaster->m_nShowAlpha = pThis->m_nShowAlpha;
    pThis->m_pReadRaster->m_nShowNoData = pThis->m_nShowNoData;

    pThis->m_pPixData = (unsigned char*)pThis->m_pReadRaster->ReadPixDataFull(nPixX0, nPixY1, nReadSrcX, nReadSrcY,
                                         pThis->m_pReadRaster->m_nBandCnt, nViewIntersectLenX, nViewIntersectLenY);
    if(pThis->m_pPixData == nullptr){
        pThis->m_nPreDrawDone = 1;
        return ;
    }

    //int nDeleteTextrueIDRet = pThis->m_pGLCore->RemoveTextureByFile(pThis->m_sRasterImgFileFullPath.c_str());
    //printf("nDeleteTextrueIDRet : %d\n", nDeleteTextrueIDRet);
    ////GLuint nTextureID = pThis->m_pGLCore->LoadTextureFromFile(pThis->m_sRasterImgFileFullPath.c_str());
    //GLuint nTextureID = pThis->m_pGLCore->CreateTexture2DFromImgBuffer(
    //            pThis->m_sRasterImgFileFullPath.c_str(), pThis->m_pPixData, nViewIntersectLenX, nViewIntersectLenY, 4);
    //
    //pThis->m_nTextureID = nTextureID;
    pThis->m_fViewLeftBottomX = fViewLeftBottomX0;
    pThis->m_fViewLeftBottomY = fViewLeftBottomY0;
    pThis->m_fViewShowLenX = (double)fViewShowLenX;
    pThis->m_fViewShowLenY = (double)fViewShowLenY;
    pThis->m_nViewIntersectLenX = nViewIntersectLenX;
    pThis->m_nViewIntersectLenY = nViewIntersectLenY;

    pThis->m_nPreDrawDone = 1;
    printf("PreDraw : time : %lld \n", clock() -ss);
    //printf("SxRasterDraw::Draw() time: %lld : %s\n",
    //       clock() - ss, (pThis->m_sRasterImgFileFullPath.substr(pThis->m_sRasterImgFileFullPath.rfind('/')+1)).c_str());
}

int CRasterDraw::UpdateGeoRanderBySelectRange(){
    return 0;
}
int CRasterDraw::DeleteGeometryBySelectRange(){
    return 0;
}

void CRasterDraw::GetPixCoordByGeo(double fGeoLng, double fGeoLat, double* pPixX, double* pPixY){

    double fPixX = 0.0, fPixY = 0.0;
    this->m_pReadRaster->GeoPos2PixPos(fGeoLng, fGeoLat, &fPixX, &fPixY);
    *pPixX = fPixX;
    *pPixY = fPixY;
}

void CRasterDraw::DrawTest() {
    //this->m_pGLCore->DrawImage(100, 9100, 200, 200, this->m_sImgFileFullPath.c_str());
    //this->m_pGLCore->DrawImage(210, 210, 200, 200, this->m_sImgFileFullPath.c_str());

    std::vector<glm::vec2> vecPoints;
    vecPoints.push_back(glm::vec2(200.0f, 200.0f));
    vecPoints.push_back(glm::vec2(202.0f, 201.0f));
    this->m_pGLCore->DrawPoint(30.0f, 60.0f, 0xFFFF00FF);
    this->m_pGLCore->DrawPoints(vecPoints.size(), (float*)vecPoints.data(), 0xFF0000FF);
    this->m_pGLCore->DrawPoints(vecPoints.size(), (float*)vecPoints.data(), 0xFF00FF00);
    this->m_pGLCore->DrawEllipse(100.0f, 150.0f, 20.0f, 30.0f, 0xFF0000FF);
    this->m_pGLCore->DrawEllipse(100.0f, 150.0f, 30.0f, 20.0f, 0xFF00FFFF);
    //this->m_pGLCore->DrawCircle(10.0f, 20.0f, 600.0f, 0xFF00FF00, 1.0f);
    this->m_pGLCore->DrawCircle(10.0f, 20.0f, 600.0f, 0xFF0000FF, 20.0f);
    this->m_pGLCore->DrawLine(30.0f, 30.0f, 40.0f, 40.0f, 0xFF00FF00, 8.0f);
    this->m_pGLCore->DrawLine(34.0f, 34.0f, 40.0f, 40.0f, 0xFF0000FF, 5.0f);
    vecPoints.clear();
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    this->m_pGLCore->DrawLines(vecPoints.size(), (float*)vecPoints.data(), 0xFF0000FF);

    vecPoints.clear();
    vecPoints.push_back(glm::vec2(10.0f, 20.0f));
    vecPoints.push_back(glm::vec2(100.0f, 200.0f));
    //vecPoints.push_back(glm::vec2(100.0f, 200.0f));
    vecPoints.push_back(glm::vec2(200.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(200.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(300.0f, 200.0f));
    //vecPoints.push_back(glm::vec2(300.0f, 200.0f));
    //vecPoints.push_back(glm::vec2(500.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(500.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(10.0f, 20.0f));
    this->m_pGLCore->DrawPolygon(vecPoints.size(), (float*)vecPoints.data(), 0xFF0000FF, 6.0f);
}

///////////////////////////////////////////////////////////////////

//若点a大于点b,即点a在点b顺时针方向,返回true,否则返回false
bool PointCmp(const glm::dvec2 &a,const glm::dvec2 &b,const glm::dvec2 &center)
{
    if (a.x >= 0.0 && b.x < 0.0)
        return true;
    if (a.x == 0.0 && b.x == 0.0)
        return a.y > b.y;
    //向量OA和向量OB的叉积
    double det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
    if (det < 0)
        return true;
    if (det > 0)
        return false;
    //向量OA和向量OB共线，以距离判断大小
    double d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
    double d2 = (b.x - center.x) * (b.x - center.y) + (b.y - center.y) * (b.y - center.y);
    return d1 > d2;
}
void ClockwiseSortPoints(std::vector<glm::dvec2> &vPoints)
{
    //计算重心
    glm::dvec2 center;
    double x = 0,y = 0;
    int nVPointsSize = (int)vPoints.size();
    for (int i = 0;i < nVPointsSize;i++)
    {
        x += vPoints[i].x;
        y += vPoints[i].y;
    }
    center.x = x/(double)nVPointsSize;
    center.y = y/(double)nVPointsSize;

    //冒泡排序
    for(int i = 0;i < nVPointsSize - 1;i++)
    {
        for (int j = 0;j < nVPointsSize - i - 1;j++)
        {
            if (PointCmp(vPoints[j],vPoints[j+1],center))
            {
                glm::dvec2 tmp = vPoints[j];
                vPoints[j] = vPoints[j + 1];
                vPoints[j + 1] = tmp;
            }
        }
    }
}



//  The function will return YES if the point x,y is inside the polygon, or
//  NO if it is not.  If the point is exactly on the edge of the polygon,
//  then the function may return YES or NO.
bool IsPointInPolygon(const std::vector<glm::dvec2>& poly, const glm::dvec2& pt)
{
    int i,j;
    bool c = false;
    int nPolySize = (int)poly.size();
    for (i = 0,j = nPolySize - 1;i < nPolySize;j = i++)
    {
        if ((((poly[i].y <= pt.y) && (pt.y < poly[j].y)) ||
            ((poly[j].y <= pt.y) && (pt.y < poly[i].y)))
            && (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y)/(poly[j].y - poly[i].y) + poly[i].x))
        {
            c = !c;
        }
    }
    return c;
}

//排斥实验
bool IsRectCross(const glm::dvec2 &p1,const glm::dvec2 &p2,const glm::dvec2 &q1,const glm::dvec2 &q2)
{
    bool ret = std::min(p1.x,p2.x) <= std::max(q1.x,q2.x)    &&
                std::min(q1.x,q2.x) <= std::max(p1.x,p2.x) &&
                std::min(p1.y,p2.y) <= std::max(q1.y,q2.y) &&
                std::min(q1.y,q2.y) <= std::max(p1.y,p2.y);
    return ret;
}
//跨立判断
bool IsLineSegmentCross(const glm::dvec2 &pFirst1,const glm::dvec2 &pFirst2,const glm::dvec2 &pSecond1,const glm::dvec2 &pSecond2)
{
    long line1,line2;
    line1 = (long)(pFirst1.x * (pSecond1.y - pFirst2.y) +
        pFirst2.x * (pFirst1.y - pSecond1.y) +
        pSecond1.x * (pFirst2.y - pFirst1.y));
    line2 = (long)(pFirst1.x * (pSecond2.y - pFirst2.y) +
        pFirst2.x * (pFirst1.y - pSecond2.y) +
        pSecond2.x * (pFirst2.y - pFirst1.y));
    if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
        return false;

    line1 = (long)(pSecond1.x * (pFirst1.y - pSecond2.y) +
        pSecond2.x * (pSecond1.y - pFirst1.y) +
        pFirst1.x * (pSecond2.y - pSecond1.y));
    line2 = (long)(pSecond1.x * (pFirst2.y - pSecond2.y) +
        pSecond2.x * (pSecond1.y - pFirst2.y) +
        pFirst2.x * (pSecond2.y - pSecond1.y));
    if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
        return false;
    return true;
}

bool GetCrossPoint(const glm::dvec2 &p1,const glm::dvec2 &p2,const glm::dvec2 &q1,const glm::dvec2 &q2, double &x,double &y)
{
    if(IsRectCross(p1,p2,q1,q2))
    {
        if (IsLineSegmentCross(p1,p2,q1,q2))
        {
            //求交点
            double tmpLeft,tmpRight;
            tmpLeft = (q2.x - q1.x) * (p1.y - p2.y) - (p2.x - p1.x) * (q1.y - q2.y);
            tmpRight = (p1.y - q1.y) * (p2.x - p1.x) * (q2.x - q1.x) + q1.x * (q2.y - q1.y) * (p2.x - p1.x) - p1.x * (p2.y - p1.y) * (q2.x - q1.x);

            x = ((double)tmpRight/(double)tmpLeft);

            tmpLeft = (p1.x - p2.x) * (q2.y - q1.y) - (p2.y - p1.y) * (q1.x - q2.x);
            tmpRight = p2.y * (p1.x - p2.x) * (q2.y - q1.y) + (q2.x- p2.x) * (q2.y - q1.y) * (p1.y - p2.y) - q2.y * (q1.x - q2.x) * (p2.y - p1.y);
            y = ((double)tmpRight/(double)tmpLeft);
            return true;
        }
    }
    return false;
}

bool PolygonClip(const std::vector<glm::dvec2> &poly1,const std::vector<glm::dvec2> &poly2, std::vector<glm::dvec2> &interPoly)
{
    if (poly1.size() < 3 || poly2.size() < 3)
    {
        return false;
    }

    double x,y;
    //计算多边形交点
    int nPoly1Cnt = (int)poly1.size();
    int nPoly2Cnt = (int)poly2.size();
    for (int i = 0;i < nPoly1Cnt;i++)
    {
        int poly1_next_idx = (i + 1) % nPoly1Cnt;
        for (int j = 0;j < nPoly2Cnt;j++)
        {
            int poly2_next_idx = (j + 1) % nPoly2Cnt;
            if (GetCrossPoint(poly1[i],poly1[poly1_next_idx],
                poly2[j],poly2[poly2_next_idx],
                x,y))
            {
                interPoly.push_back(glm::dvec2(x,y));
            }
        }
    }

    //计算多边形内部点
    for(int i = 0;i < nPoly1Cnt;i++)
    {
        if (IsPointInPolygon(poly2,poly1[i]))
        {
            interPoly.push_back(poly1[i]);
        }
    }
    for (int i = 0;i < nPoly2Cnt;i++)
    {
        if (IsPointInPolygon(poly1,poly2[i]))
        {
            interPoly.push_back(poly2[i]);
        }
    }

    if(interPoly.size() <= 0)
        return false;

    //点集排序
    ClockwiseSortPoints(interPoly);
    return true;
}
