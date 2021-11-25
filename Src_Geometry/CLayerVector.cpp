#include "CLayerVector.h"

#include <iostream>
#include "gdal/ogrsf_frmts.h"

#include "Src_Core/DefType.h"
#include "Src_Core/CGlobal.h"
#include "Src_Core/CThreadPool.h"
#include "Src_Core/CGeoRelation.h"
#include "Src_Geometry/CRasterDraw.h"
#include "Src_Geometry/CReadRaster.h"
#include "Src_Geometry/CLayerGeoDraw.h"
#include "Src_Geometry/CLayerPoints.h"
#include "Src_Geometry/CLayerLines.h"
#include "Src_Geometry/CLayerPolygons.h"
#include "Src_Geometry/CLayerPolygonsEdit.h"
#include "Src_Geometry/CLayerResidual.h"
#include "Src_Attribute/CChunkAttribute.h"
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

CLayerVector::CLayerVector() {
    this->m_pGLCore = nullptr;
    //GetGlobalPtr()->m_pLayerVector = this;
}

CLayerVector::CLayerVector(COpenGLCore* pGLCore)
{
    this->m_pGLCore = pGLCore;
    //GetGlobalPtr()->m_pLayerVector = this;
}

CLayerVector::~CLayerVector()
{
}

void CLayerVector::Init(){
}

int CLayerVector::IsContains(double* pSrcRect, double* pDstRect){

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

int CLayerVector::IsOverlaps(double* pSrcRect, double* pDstRect){
    if(pSrcRect[0] > pDstRect[2] ||
        pSrcRect[1] > pDstRect[3] ||
        pSrcRect[2] < pDstRect[0] ||
        pSrcRect[3] < pDstRect[1])
    {
        return 0;
    }
    return 1;
}

void CLayerVector::Draw() {
    if(this->m_nVecLayerItemCnt <= 0)
        return;
    time_t ss = clock();
    int i = 0, nRasterCnt = 0, nFirstShowRaster = 1;
    printf("SxLayerVector::Draw() time: %lld cnt:%d\n", clock() - ss, nRasterCnt);
    CLayerDraw* pSxLayerDraw = nullptr;
    CLayerGeoDraw* pSxLayerGeoDraw = nullptr;
    CRasterDraw* pSxRasterDraw = nullptr;
    CRasterDraw* _pSxRasterDraw = nullptr;
    double* pGeoViewPoly = nullptr;
    double* pOtherGeoViewPoly = nullptr;
    std::vector<double*> vecGeoViewPoly;

    if( this->m_nExistRasterCnt > 0) {
        this->m_pSxThreadPool = CThreadPool::getInstance();

        for(i=this->m_nVecLayerItemCnt-1; i>=0; i--){
            pSxLayerDraw = this->m_vecLayerItem[i];
            pSxLayerDraw->m_nShowOrHideLast = pSxLayerDraw->m_nShowOrHide;
            if(pSxLayerDraw->m_nShowOrHide == ESHOWORHIDE::e_hide){
                continue;
            }
            if( !(pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tif ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tiff ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Img) )
            {
                continue;
            }
            _pSxRasterDraw = (CRasterDraw*)pSxLayerDraw;
            _pSxRasterDraw->m_nPreDrawDone = 0;

            //_pSxRasterDraw->m_nRollerBlinding = 0;
            //if(i == this->m_nLayerIndxRollerBind){
            //    _pSxRasterDraw->m_nRollerBlinding = 1;
            //}

            this->m_pSxThreadPool->startOneJob([&](void* pArg)->void {

                CRasterDraw* pSxRasterDraw1 = (CRasterDraw*)pArg;
                //fGeoQuadArea = geoViewPoly.GetArea();
                //printf("fGeoQuadArea: %d %f\n", i, fGeoQuadArea);
                //if(fGeoQuadArea < 0.000001) {
                //    pSxRasterDraw->m_nShowOrHide = SxLayerDraw::ESHOWORHEDE::e_hide;
                //    continue;
                //}
                double arrGeoIntersect[4]={pSxRasterDraw1->m_pReadRaster->m_fMinLng, pSxRasterDraw1->m_pReadRaster->m_fMinLat,
                                       pSxRasterDraw1->m_pReadRaster->m_fMaxLng, pSxRasterDraw1->m_pReadRaster->m_fMaxLat};
                if(0 == pSxRasterDraw1->m_pGLCore->GetGeoIntersectRangeByView(arrGeoIntersect, pSxRasterDraw1->m_arrGeoIntersect)){

                    pSxRasterDraw1->m_nShowOrHide = ESHOWORHIDE::e_hide;

                    pSxRasterDraw1->m_nPreDrawDone = 1;
                    return;
                }

                //float fGeoQuad[4] = {
                //    (float)pSxRasterDraw1->8m_arrGeoIntersect[0],
                //    (float)pSxRasterDraw1->m_arrGeoIntersect[1],
                //    (float)pSxRasterDraw1->m_arrGeoIntersect[2],
                //    (float)pSxRasterDraw1->m_arrGeoIntersect[3],
                //};
                //
                //pSxRasterDraw1->m_arrGeoIntersect[0] = (double)fGeoQuad[0];
                //pSxRasterDraw1->m_arrGeoIntersect[1] = (double)fGeoQuad[1];
                //pSxRasterDraw1->m_arrGeoIntersect[2] = (double)fGeoQuad[2];
                //pSxRasterDraw1->m_arrGeoIntersect[3] = (double)fGeoQuad[3];

                pSxRasterDraw1->m_nPreDrawDone = 1;
                return;

            }, _pSxRasterDraw);
        }


        double fGeoQuadArea = 0.0;
        std::vector<CRasterDraw*> vecRasterDraw;
        //geoViewPoly.CreateGeoPolygon(4, fGeoQuad);
        for(i = this->m_nVecLayerItemCnt-1; i >= 0; i--) {
            pSxLayerDraw = this->m_vecLayerItem[i];
            if(pSxLayerDraw->m_nShowOrHide == ESHOWORHIDE::e_hide){
                continue;
            }
            if( !(pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tif ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tiff ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Img) )
            {
                continue;
            }

            pSxRasterDraw = (CRasterDraw*)pSxLayerDraw;
            //fGeoQuadArea = geoViewPoly.GetArea();
            //printf("fGeoQuadArea: %d %f\n", i, fGeoQuadArea);
            //if(fGeoQuadArea < 0.000001) {
            //    pSxRasterDraw->m_nShowOrHide = SxLayerDraw::ESHOWORHEDE::e_hide;
            //    continue;
            //}
            //double* arrGeoIntersect = pSxRasterDraw->m_arrGeoIntersect;

            while(pSxRasterDraw->m_nPreDrawDone == 0){
                //printf("Wait for PreDrawDone...\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            }
            double* fOtherGeoQuad = new double[4];
            fOtherGeoQuad[0] = pSxRasterDraw->m_arrGeoIntersect[0];
            fOtherGeoQuad[1] = pSxRasterDraw->m_arrGeoIntersect[1];
            fOtherGeoQuad[2] = pSxRasterDraw->m_arrGeoIntersect[2];
            fOtherGeoQuad[3] = pSxRasterDraw->m_arrGeoIntersect[3];

            if(i == this->m_nLayerIndxRollerBind){
                vecRasterDraw.push_back(pSxRasterDraw);
                continue;
            }
            if(nFirstShowRaster == 1){
                nFirstShowRaster = 0;
                vecGeoViewPoly.push_back(fOtherGeoQuad);
                vecRasterDraw.push_back(pSxRasterDraw);
                continue;
            }

            int nRelationOtherFlag = 0;
            int nVecGeoViewPolyCnt = (int)vecGeoViewPoly.size();
            auto itt0 = vecGeoViewPoly.begin();
            for(int i0 = nVecGeoViewPolyCnt - 1; i0 >= 0; i0--) {
                pGeoViewPoly = vecGeoViewPoly[i0];
                if( this->IsContains(pGeoViewPoly, fOtherGeoQuad)){
                    //printf("%f\n", pGeoViewPoly->GetArea());
                    //printf("%f\n", pOtherGeoViewPoly->GetArea());

                    nRelationOtherFlag = 1;
                    break;
                }
                else if( this->IsContains(fOtherGeoQuad, pGeoViewPoly)){
                    delete pGeoViewPoly;
                    vecGeoViewPoly.erase(itt0 + i0);
                    continue;
                }
                else if (this->IsOverlaps(pGeoViewPoly, fOtherGeoQuad)) {
                    //pGeoViewPoly->UnionGeoPolygon(pOtherGeoViewPoly);
                    nRelationOtherFlag = 2;
                    break;
                }
                //if( 0 == pOtherGeoViewPoly->SubtractGeoPolygon(4, fOtherGeoQuad)) {
                //    pSxRasterDraw->m_nShowOrHide = SxLayerDraw::ESHOWORHEDE::e_hide;
                //}else{
                //    nRasterCnt++;
                //}
            }
            if(nRelationOtherFlag == 0 || nRelationOtherFlag == 2){
                vecGeoViewPoly.push_back(fOtherGeoQuad);
                vecRasterDraw.push_back(pSxRasterDraw);
                continue;
            }
            if(nRelationOtherFlag == 1){
                pSxRasterDraw->m_nShowOrHide = ESHOWORHIDE::e_hide;
            }
            if(nRelationOtherFlag > 0){
                if(pOtherGeoViewPoly) {
                    delete pOtherGeoViewPoly;
                }
            }
            //fGeoQuadArea = geoViewPoly.GetArea();
            //printf("fGeoQuadArea: %d %f\n", i, fGeoQuadArea);
        }

        int nVecGeoViewPolyCnt = (int)vecGeoViewPoly.size();
        for(int i0 = 0; i0 < nVecGeoViewPolyCnt; i0++) {
            pGeoViewPoly = vecGeoViewPoly[i0];
            delete pGeoViewPoly;
        }
        vecGeoViewPoly.clear();

        int nVecRasterDraw = (int)vecRasterDraw.size();
        for(i=0; i<nVecRasterDraw; i++){
            this->m_pSxThreadPool = CThreadPool::getInstance();
            _pSxRasterDraw = vecRasterDraw[i];
            _pSxRasterDraw->m_nPreDrawDone = 0;
            this->m_pSxThreadPool->startOneJob([&](void* pArg)->void {
                CRasterDraw::PreDraw(pArg);
            }, _pSxRasterDraw);
        }
    }
    for(i=0; i<this->m_nVecLayerItemCnt; i++){
        pSxLayerDraw = this->m_vecLayerItem[i];
        if(pSxLayerDraw->m_nShowOrHide == ESHOWORHIDE::e_hide){
            if(this->m_nExistRasterCnt > 0){
                pSxLayerDraw->m_nShowOrHide = pSxLayerDraw->m_nShowOrHideLast;
            }
            continue;
        }

        if(pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePoint ||
            pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolyline ||
            pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolygon)
        {
            pSxLayerGeoDraw = (CLayerGeoDraw*)pSxLayerDraw;
            double arrGeoIntersect[4]={pSxLayerGeoDraw->m_fMinLng, pSxLayerGeoDraw->m_fMinLat,
                                   pSxLayerGeoDraw->m_fMaxLng, pSxLayerGeoDraw->m_fMaxLat};
            if(0 == pSxLayerDraw->m_pGLCore->GetGeoIntersectRangeByView(arrGeoIntersect, arrGeoIntersect)) {
                //continue;
            }

            pSxLayerDraw->m_nShowOrHideLast = pSxLayerDraw->m_nShowOrHide;
        }
        //if(pSxLayerDraw->m_nShowOrHide == SxLayerDraw::ESHOWORHEDE::e_show){
        //    nRasterCnt++;
        //}
        pSxLayerDraw->Draw();
        pSxLayerDraw->m_nShowOrHide = pSxLayerDraw->m_nShowOrHideLast;
    }
    printf("SxLayerVector::Draw() time: %lld cnt:%d\n", clock() - ss, nRasterCnt);
}

void CLayerVector::Draw_Bak() {
    if(this->m_nVecLayerItemCnt <= 0)
        return;
    time_t ss = clock();
    int i = 0, nRasterCnt = 0, nFirstShowRaster = 1;
    CLayerDraw* pSxLayerDraw = nullptr;
    CRasterDraw* pSxRasterDraw = nullptr;
    CRasterDraw* _pSxRasterDraw = nullptr;
    CGeoRelation* pGeoViewPoly = nullptr;
    CGeoRelation* pOtherGeoViewPoly = nullptr;
    std::vector<CGeoRelation*> vecGeoViewPoly;

    if( this->m_nExistRasterCnt > 1) {
        this->m_pSxThreadPool = CThreadPool::getInstance();

        for(i=this->m_nVecLayerItemCnt-1; i>=0; i--){
            pSxLayerDraw = this->m_vecLayerItem[i];
            if(pSxLayerDraw->m_nShowOrHide == ESHOWORHIDE::e_hide){
                continue;
            }
            pSxLayerDraw->m_nShowOrHideLast = pSxLayerDraw->m_nShowOrHide;
            if( !(pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tif ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tiff ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Img) )
            {
                continue;
            }
            _pSxRasterDraw = (CRasterDraw*)pSxLayerDraw;
            _pSxRasterDraw->m_nPreDrawDone = 0;

            this->m_pSxThreadPool->startOneJob([&](void* pArg)->void {

                CRasterDraw* pSxRasterDraw1 = (CRasterDraw*)pArg;
                //fGeoQuadArea = geoViewPoly.GetArea();
                //printf("fGeoQuadArea: %d %f\n", i, fGeoQuadArea);
                //if(fGeoQuadArea < 0.000001) {
                //    pSxRasterDraw->m_nShowOrHide = SxLayerDraw::ESHOWORHEDE::e_hide;
                //    continue;
                //}
                double arrGeoIntersect[4]={pSxRasterDraw1->m_pReadRaster->m_fMinLng, pSxRasterDraw1->m_pReadRaster->m_fMinLat,
                                       pSxRasterDraw1->m_pReadRaster->m_fMaxLng, pSxRasterDraw1->m_pReadRaster->m_fMaxLat};
                if(0 == pSxRasterDraw1->m_pGLCore->GetGeoIntersectRangeByView(arrGeoIntersect, pSxRasterDraw1->m_arrGeoIntersect)){

                    pSxRasterDraw1->m_nShowOrHide = ESHOWORHIDE::e_hide;

                    pSxRasterDraw1->m_nPreDrawDone = 1;
                    return;
                }

                float fGeoQuad[4] = {
                    (float)pSxRasterDraw1->m_arrGeoIntersect[0],
                    (float)pSxRasterDraw1->m_arrGeoIntersect[1],
                    (float)pSxRasterDraw1->m_arrGeoIntersect[2],
                    (float)pSxRasterDraw1->m_arrGeoIntersect[3],
                };

                pSxRasterDraw1->m_arrGeoIntersect[0] = (double)fGeoQuad[0];
                pSxRasterDraw1->m_arrGeoIntersect[1] = (double)fGeoQuad[1];
                pSxRasterDraw1->m_arrGeoIntersect[2] = (double)fGeoQuad[2];
                pSxRasterDraw1->m_arrGeoIntersect[3] = (double)fGeoQuad[3];

                pSxRasterDraw1->m_nPreDrawDone = 1;
                return;

            }, _pSxRasterDraw);
        }


        double fGeoQuadArea = 0.0;
        double fGeoQuad[8] = {
            this->m_pGLCore->m_arrGeoMinMax[0], this->m_pGLCore->m_arrGeoMinMax[1],
            this->m_pGLCore->m_arrGeoMinMax[2], this->m_pGLCore->m_arrGeoMinMax[1],
            this->m_pGLCore->m_arrGeoMinMax[2], this->m_pGLCore->m_arrGeoMinMax[3],
            this->m_pGLCore->m_arrGeoMinMax[0], this->m_pGLCore->m_arrGeoMinMax[3],
        };

        std::vector<CRasterDraw*> vecRasterDraw;
        //geoViewPoly.CreateGeoPolygon(4, fGeoQuad);
        for(i = this->m_nVecLayerItemCnt-1; i >= 0; i--) {
            pSxLayerDraw = this->m_vecLayerItem[i];
            if(pSxLayerDraw->m_nShowOrHide == ESHOWORHIDE::e_hide){
                continue;
            }
            if( !(pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tif ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Tiff ||
                pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_Img) )
            {
                continue;
            }

            pSxRasterDraw = (CRasterDraw*)pSxLayerDraw;
            //fGeoQuadArea = geoViewPoly.GetArea();
            //printf("fGeoQuadArea: %d %f\n", i, fGeoQuadArea);
            //if(fGeoQuadArea < 0.000001) {
            //    pSxRasterDraw->m_nShowOrHide = SxLayerDraw::ESHOWORHEDE::e_hide;
            //    continue;
            //}
            //double* arrGeoIntersect = pSxRasterDraw->m_arrGeoIntersect;

            while(pSxRasterDraw->m_nPreDrawDone == 0){
                //printf("Wait for PreDrawDone...\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            }
            double fOtherGeoQuad[8] = {
                pSxRasterDraw->m_arrGeoIntersect[0], pSxRasterDraw->m_arrGeoIntersect[1],
                pSxRasterDraw->m_arrGeoIntersect[2], pSxRasterDraw->m_arrGeoIntersect[1],
                pSxRasterDraw->m_arrGeoIntersect[2], pSxRasterDraw->m_arrGeoIntersect[3],
                pSxRasterDraw->m_arrGeoIntersect[0], pSxRasterDraw->m_arrGeoIntersect[3],
            };

            if(nFirstShowRaster == 1){
                nFirstShowRaster = 0;
                pGeoViewPoly = new CGeoRelation;
                pGeoViewPoly->CreateGeoPolygon(4, fOtherGeoQuad);
                vecGeoViewPoly.push_back(pGeoViewPoly);
                vecRasterDraw.push_back(pSxRasterDraw);
                continue;
            }
            pOtherGeoViewPoly = new CGeoRelation;
            pOtherGeoViewPoly->CreateGeoPolygon(4, fOtherGeoQuad);
            int nRelationOtherFlag = 0;
            int nVecGeoViewPolyCnt = (int)vecGeoViewPoly.size();
            auto itt0 = vecGeoViewPoly.begin();
            for(int i0 = nVecGeoViewPolyCnt - 1; i0 >= 0; i0--) {
                pGeoViewPoly = vecGeoViewPoly[i0];
                if( pGeoViewPoly->IsContains(pOtherGeoViewPoly)){
                    //printf("%f\n", pGeoViewPoly->GetArea());
                    //printf("%f\n", pOtherGeoViewPoly->GetArea());

                    nRelationOtherFlag = 1;
                    break;
                }
                else if( pOtherGeoViewPoly->IsContains(pGeoViewPoly)){
                    delete pGeoViewPoly;
                    vecGeoViewPoly.erase(itt0 + i0);
                    continue;
                }
                else if (pGeoViewPoly->IsOverlaps(pOtherGeoViewPoly)) {
                    //pGeoViewPoly->UnionGeoPolygon(pOtherGeoViewPoly);
                    nRelationOtherFlag = 2;
                    break;
                }
                //if( 0 == pOtherGeoViewPoly->SubtractGeoPolygon(4, fOtherGeoQuad)) {
                //    pSxRasterDraw->m_nShowOrHide = SxLayerDraw::ESHOWORHEDE::e_hide;
                //}else{
                //    nRasterCnt++;
                //}
            }
            if(nRelationOtherFlag == 0 || nRelationOtherFlag == 2){
                vecGeoViewPoly.push_back(pOtherGeoViewPoly);
                vecRasterDraw.push_back(pSxRasterDraw);
                continue;
            }
            if(nRelationOtherFlag == 1){
                pSxRasterDraw->m_nShowOrHide = ESHOWORHIDE::e_hide;
            }
            if(nRelationOtherFlag > 0){
                delete pOtherGeoViewPoly;
            }
            //fGeoQuadArea = geoViewPoly.GetArea();
            //printf("fGeoQuadArea: %d %f\n", i, fGeoQuadArea);
        }

        int nVecGeoViewPolyCnt = (int)vecGeoViewPoly.size();
        for(int i0 = 0; i0 < nVecGeoViewPolyCnt; i0++) {
            pGeoViewPoly = vecGeoViewPoly[i0];
            delete pGeoViewPoly;
        }
        vecGeoViewPoly.clear();

        int nVecRasterDraw = (int)vecRasterDraw.size();
        for(i=0; i<nVecRasterDraw; i++){
            this->m_pSxThreadPool = CThreadPool::getInstance();
            _pSxRasterDraw = vecRasterDraw[i];
            _pSxRasterDraw->m_nPreDrawDone = 0;
            this->m_pSxThreadPool->startOneJob([&](void* pArg)->void {
                CRasterDraw::PreDraw(pArg);
            }, _pSxRasterDraw);
        }
    }
    for(i=0; i<this->m_nVecLayerItemCnt; i++){
        pSxLayerDraw = this->m_vecLayerItem[i];
        //if(pSxLayerDraw->m_nShowOrHide == SxLayerDraw::ESHOWORHEDE::e_show){
        //    nRasterCnt++;
        //}
        pSxLayerDraw->Draw();
        pSxLayerDraw->m_nShowOrHide = pSxLayerDraw->m_nShowOrHideLast;
    }
    printf("SxLayerVector::Draw() time: %lld cnt:%d\n", clock() - ss, nRasterCnt);
}


int CLayerVector::RemoveOneLayer(CLayerDraw* pLayerDraw, int nDeleteLayer/* = 1*/){
    if(pLayerDraw->m_nLayerType == ELAYERTYPE::e_Tif
            || pLayerDraw->m_nLayerType == ELAYERTYPE::e_Tiff
            || pLayerDraw->m_nLayerType == ELAYERTYPE::e_Img)
    {
        this->m_nExistRasterCnt -= 1;
    }

    auto itt = this->m_mapLayerItem2Indx.find(pLayerDraw);
    if(this->m_mapLayerItem2Indx.end() == itt){
        return 0;
    }
    int nRemoveIndx = itt->second;

    if(nRemoveIndx == this->m_nLayerIndxRollerBind)
        this->m_nLayerIndxRollerBind = -1;
    if(nRemoveIndx == this->m_nBeingEditor)
        this->m_nBeingEditor = 0;
    if(this->m_pGeoLayerEdit == pLayerDraw)
        this->m_pGeoLayerEdit = nullptr;

    this->m_nOperateEleCnt = this->m_nOperateEleCnt - pLayerDraw->m_nOperateEleCnt;
    this->m_nVecLayerItemCnt -= 1;

    int i = 0, nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    for(i = 0; i<nVecLayerItemCnt; i++){
        if(pLayerDraw == this->m_vecLayerItem[i]){
            break;
        }
    }

    if(nDeleteLayer == 1) {
        CLayerDraw* pLayerTmp = this->m_vecLayerItem[i];
        delete pLayerTmp;
        pLayerTmp = nullptr;
    }

    this->m_vecLayerItem.erase(this->m_vecLayerItem.begin() + i);

    nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    this->m_mapLayerItem2Indx.clear();
    for(i = 0; i<nVecLayerItemCnt; i++){
        pLayerDraw = this->m_vecLayerItem[i];
        this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pLayerDraw, i));
    }

    return 1;
}

int CLayerVector::RemoveAllLayers(int nDeleteLayer/* = 1*/){

    this->m_nExistRasterCnt = 0;
    this->m_nLayerIndxRollerBind = -1;
    this->m_nOperateEleCnt = 0;
    this->m_nVecLayerItemCnt = 0;
    this->m_mapLayerItem2Indx.clear();

    this->m_nBeingEditor = 0;
    this->m_pGeoLayerEdit = nullptr;

    if(nDeleteLayer == 1) {
        CLayerDraw* pLayerDraw = nullptr;
        int i = 0, nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
        for(i = 0; i<nVecLayerItemCnt; i++){
            pLayerDraw = this->m_vecLayerItem[i];
            delete pLayerDraw;
            pLayerDraw = nullptr;
        }
    }
    this->m_vecLayerItem.clear();
    return 1;
}

CLayerDraw* CLayerVector::FindLayerByName(const std::string& sLayerName){
    CLayerDraw* pLayer = nullptr;
    for(auto pLayerItem : this->m_vecLayerItem){
        if(pLayerItem->m_sLayerName.compare(sLayerName) == 0){
            pLayer = pLayerItem;
            break;
        }
    }
    return pLayer;
}

CLayerDraw* CLayerVector::GetLayerIndxRollerBlind(){
    if(this->m_nLayerIndxRollerBind > -1 && this->m_nLayerIndxRollerBind < this->m_nVecLayerItemCnt){
        return this->m_vecLayerItem[this->m_nLayerIndxRollerBind];
    }
    return nullptr;
}
void CLayerVector::ClearLayerIndxRollerBlind(){

    CLayerDraw* pLastLayerDraw = this->GetLayerIndxRollerBlind();
    if(pLastLayerDraw){
        pLastLayerDraw->m_nRollerBlinding = 0;
    }
    this->m_nLayerIndxRollerBind = -1;
}
int CLayerVector::UpdateLayerIndxRollerBlind(CLayerDraw* pLayerDraw, int nChecked){
    auto itt = this->m_mapLayerItem2Indx.find(pLayerDraw);
    if(itt == this->m_mapLayerItem2Indx.end()){
        return -1;
    }
    int nLayerIndx = itt->second;

    CLayerDraw* pLastLayerDraw = this->GetLayerIndxRollerBlind();
    if(pLastLayerDraw){
        pLastLayerDraw->m_nRollerBlinding = 0;
    }
    if(nChecked == 1){
        this->m_nLayerIndxRollerBind = nLayerIndx;
        pLayerDraw->m_nRollerBlinding = 1;
    }
    else{
        this->m_nLayerIndxRollerBind = -1;
        pLayerDraw->m_nRollerBlinding = 0;
    }
    return nLayerIndx;
}

CLayerDraw* CLayerVector::AddLayerItem(CLayerDraw* pOtherLayer){
    this->m_vecLayerItem.push_back(pOtherLayer);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pOtherLayer, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pOtherLayer->m_pLayerVector = this;
    this->m_nExistRasterCnt++;
    return pOtherLayer;
}

CRasterDraw* CLayerVector::AddRasterItem(const std::string& sTifFullPath){

    CRasterDraw* pSxRasterDraw = new CRasterDraw(this->m_pGLCore, sTifFullPath);
    if(pSxRasterDraw == nullptr){
        return nullptr;
    }
    pSxRasterDraw->m_sLayerName = sTifFullPath;
    this->m_vecLayerItem.push_back(pSxRasterDraw);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pSxRasterDraw, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pSxRasterDraw->m_pLayerVector = this;
    this->m_nExistRasterCnt++;

    return pSxRasterDraw;
}


CLayerGeoDraw* CLayerVector::CreateEmptyGeoLayer(int nGeoLayerType){

    CLayerGeoDraw* pLayerGeo = nullptr;
    CLayerPoints* pLayerPoints = nullptr;
    CLayerLines* pLayerLines = nullptr;
    CLayerPolygons* pLayerPolygons = nullptr;
    CLayerResidual* pLayerResidual = nullptr;
    int nLayerTypeGeo = 0;
    switch (nGeoLayerType) {
    case 1: {
        pLayerPoints = new CLayerPoints(this->m_pGLCore);
        pLayerGeo = pLayerPoints;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
        nLayerTypeGeo = 1;
        pLayerGeo->m_sLayerName = "new_point_layer";
        break;
    }
    case 2:{
        //pLayerPoints = new SxLayerPoints(this->m_pGLCore);
        pLayerLines = new CLayerLines(this->m_pGLCore);
        pLayerGeo = pLayerLines;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolyline;
        nLayerTypeGeo = 2;
        pLayerGeo->m_sLayerName = "new_polyline_layer";
        break;
    }
    case 3:{
        //pLayerPoints = new SxLayerPoints(this->m_pGLCore);
        pLayerPolygons = new CLayerPolygons(this->m_pGLCore);
        pLayerGeo = pLayerPolygons;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolygon;
        nLayerTypeGeo = 3;
        pLayerGeo->m_sLayerName = "new_polygon_layer";
        break;
    }
    case 4:{
        //pLayerPoints = new SxLayerPoints(this->m_pGLCore);
        pLayerResidual = new CLayerResidual(this->m_pGLCore);
        pLayerGeo = pLayerResidual;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolyline;
        nLayerTypeGeo = 2;
        pLayerGeo->m_sLayerName = "new_polyline_layer";
        break;
    }
    default:
        return nullptr;
    }
    pLayerGeo->Init();

    pLayerGeo->MakeupFields();

    pLayerGeo->MakeUpChunks();


    this->m_vecLayerItem.push_back(pLayerGeo);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pLayerGeo, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pLayerGeo->m_pLayerVector = this;

    return pLayerGeo;
}
CLayerGeoDraw* CLayerVector::ReadShapeFile(const std::string& sShpFileFullPath) {
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//图像路径不支持中文（可设置）
    CPLSetConfigOption("SHAPE_ENCODING","");  //解决中文乱码问题

    GDALDataset* poDS = (GDALDataset*) GDALOpenEx(sShpFileFullPath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL );

    if( poDS == nullptr ) {
        printf( "Open:[%s] failed.\n", sShpFileFullPath.c_str());
        return 0;
    }
    std::string sProjectRef = poDS->GetProjectionRef();

    OGRLayer* poLayer = poDS->GetLayer(0); //读取层
    if(poLayer == nullptr){
        printf( "GetLayer:[%s] failed.\n", sShpFileFullPath.c_str());
        return 0;
    }

    OGRwkbGeometryType nLayerType = poLayer->GetGeomType();
    CLayerGeoDraw* pLayerGeo = nullptr;
    CLayerPoints* pLayerPoints = nullptr;
    CLayerLines* pLayerLines = nullptr;
    CLayerPolygons* pLayerPolygons = nullptr;

    int nLayerTypeGeo = 0;
    switch (nLayerType) {
    case OGRwkbGeometryType::wkbPoint:
    case OGRwkbGeometryType::wkbPointM:
    case OGRwkbGeometryType::wkbPointZM:
    case OGRwkbGeometryType::wkbPoint25D: {
        pLayerPoints = new CLayerPoints(this->m_pGLCore);
        pLayerGeo = pLayerPoints;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
        nLayerTypeGeo = 1;
        break;
    }
    case OGRwkbGeometryType::wkbLineString:
    case OGRwkbGeometryType::wkbLineStringM:
    case OGRwkbGeometryType::wkbLineStringZM:
    case OGRwkbGeometryType::wkbLineString25D:{
        //pLayerPoints = new SxLayerPoints(this->m_pGLCore);
        pLayerLines = new CLayerLines(this->m_pGLCore);
        pLayerGeo = pLayerLines;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolyline;
        nLayerTypeGeo = 2;
        break;
    }
    case OGRwkbGeometryType::wkbPolygon:
    case OGRwkbGeometryType::wkbPolygonM:
    case OGRwkbGeometryType::wkbPolygonZM:
    case OGRwkbGeometryType::wkbPolygon25D:{
        //pLayerPoints = new SxLayerPoints(this->m_pGLCore);
        pLayerPolygons = new CLayerPolygons(this->m_pGLCore);
        pLayerGeo = pLayerPolygons;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolygon;
        nLayerTypeGeo = 3;
        break;
    }
    }
    pLayerGeo->Init();

    OGREnvelope pEnvelope;
    poLayer->GetExtent(&pEnvelope, TRUE);
    double fEnvelopeMinLng = pEnvelope.MinX;
    double fEnvelopeMinLat = pEnvelope.MinY;
    double fEnvelopeMaxLng = pEnvelope.MaxX;
    double fEnvelopeMaxLat = pEnvelope.MaxY;

    OGRFeature* poFeature = nullptr;
    //OGRFieldDefn* poFieldDefn = nullptr;
    OGRGeometry* poGeometry = nullptr;
    OGRwkbGeometryType eGeoType = OGRwkbGeometryType::wkbUnknown;
    OGRPoint* poPoint = nullptr;
    OGRPolygon* poPolygon = nullptr;
    OGRLineString* poLineString = nullptr;
    double fGeoX = 0.0, fGeoY = 0.0, fZVal = 0.0;
    int i = 0, nPointCnt = 0;

    std::string sSrcProject = "PROJCS[\"Xian_1980_3_Degree_GK_Zone_39\",GEOGCS[\"GCS_Xian_1980\",DATUM[\"D_Xian_1980\",SPHEROID[\"Xian_1980\",6378140.0,298.257]],PRIMEM[\"Greenwich\",0.0000],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",39500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",117.0],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
    std::string sDstProject = "GEOGCS[\"GCS_Xian_1980\",DATUM[\"D_Xian_1980\",SPHEROID[\"Xian_1980\",6378140.0,298.257]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433],AUTHORITY[\"EPSG\",4610]]";

    char* szSrcProject = (char*)sSrcProject.c_str();
    char* szDstProject = (char*)sDstProject.c_str();
    OGRSpatialReference srcCoordConvert, dstCoordConvert;
    srcCoordConvert.importFromWkt(&(szSrcProject));
    dstCoordConvert.importFromWkt(&(szDstProject));
    OGRCoordinateTransformation *pTrans = OGRCreateCoordinateTransformation(&srcCoordConvert, &dstCoordConvert);
    if(pTrans == nullptr){
        printf("Coord Convert Faild !\n");
        return 0;
    }
    int nNeedTransfromCoord = false;
    //if(fEnvelopeMinLng > 10000 || fEnvelopeMinLat > 10000 ||
    //        fEnvelopeMaxLng > 10000 || fEnvelopeMaxLat > 10000 ) {
    //    pTrans->Transform(1, &fEnvelopeMinLng, &fEnvelopeMinLat);
    //    pTrans->Transform(1, &fEnvelopeMaxLng, &fEnvelopeMaxLat);
    //    nNeedTransfromCoord = true;
    //}

    pLayerGeo->m_fMinLng = fEnvelopeMinLng;
    pLayerGeo->m_fMinLat = fEnvelopeMinLat;
    pLayerGeo->m_fMaxLng = fEnvelopeMaxLng;
    pLayerGeo->m_fMaxLat = fEnvelopeMaxLat;

    poLayer->ResetReading();

    int nStrValueWidth = 128;
    std::string sFieldName;
    std::vector<int> vecFieldType;
    while( (poFeature = poLayer->GetNextFeature()) != NULL ) {
        poGeometry = poFeature->GetGeometryRef();
        if (poGeometry == nullptr)
            continue;
        if(poGeometry->Is3D())
            pLayerGeo->m_nHasZValue = 1;

        OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
        int nFieldCnt = poFDefn->GetFieldCount(); //获得字段的数目，不包括前两个字段（FID,Shape);
        for(int j = 0; j < nFieldCnt; j++ ) {
            //输出每个字段的值
            //std::cout << poFeature->GetFieldAsString(j) << "    ";
            OGRFieldDefn* poFieldDefn = poFDefn->GetFieldDefn(j);
            if(poFieldDefn == nullptr){
                printf("ERR : Read Shp GetFieldDefn: %d\n", j);
                continue;
            }
            sFieldName = poFieldDefn->GetNameRef();

            CLayerField layerFld;
            layerFld.m_sFieldName = sFieldName;
            layerFld.m_nFieldWidth = poFieldDefn->GetWidth();
            layerFld.m_nPrecision = poFieldDefn->GetPrecision();


            switch (poFieldDefn->GetType()){
            case OGRFieldType::OFTInteger:{
                layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_int;
                break;
            }
            case OGRFieldType::OFTReal:{
                layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
                break;
            }
            case OGRFieldType::OFTString:{
                layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
                if(nStrValueWidth < layerFld.m_nFieldWidth){
                    nStrValueWidth = layerFld.m_nFieldWidth;
                }
                break;
            }
            case OGRFieldType::OFTWideString:{
                layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_wchar_ptr;
                if(nStrValueWidth < layerFld.m_nFieldWidth * 2){
                    nStrValueWidth = layerFld.m_nFieldWidth * 2;
                }
                break;
            }
            case OGRFieldType::OFTBinary:{
                layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_void_ptr;
                break;
            }
            case OGRFieldType::OFTDate:
            case OGRFieldType::OFTTime:
            case OGRFieldType::OFTDateTime:{
                layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
                if(nStrValueWidth < layerFld.m_nFieldWidth){
                    nStrValueWidth = layerFld.m_nFieldWidth;
                }
                break;
            }
            case OGRFieldType::OFTInteger64:{
                layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_long_long;
                break;
            }
            }
            vecFieldType.push_back(layerFld.m_nFieldType);
            pLayerGeo->m_vecLayerFields.push_back(layerFld);


            printf("\n>>>\nfieldName: %s\n", layerFld.m_sFieldName.c_str());
            printf("fieldType: %d\n", layerFld.m_nFieldType);
            printf("fieldWidth: %d\n", layerFld.m_nFieldWidth);
            printf("fieldPreci: %d\n", layerFld.m_nPrecision);


            //if (poFieldDefn->GetType() == OFTInteger) {
            //    printf("%d %d\n", j, poFeature->GetFieldAsInteger(j)); //输出，不同的形式，效果与上句相同
            //}
            //else if (poFieldDefn->GetType() == OFTReal) {
            //    printf("%d %.3f\n", j, poFeature->GetFieldAsDouble(j));
            //}
            //else if (poFieldDefn->GetType() == OFTString) {
            //    printf("%d %s\n", j, QString(poFeature->GetFieldAsString(j)).toLocal8Bit().data());
            //}
            //else {
            //    printf("%d %s\n", j, QString(poFeature->GetFieldAsString(j)).toLocal8Bit().data());
            //}
        }
        break;
    }
    pLayerGeo->MakeupFields();

    int nFieldValue = 0;
    double fFieldValue = 0.0;
    long long nllFieldValue = 0;
    std::string sFieldValue;
    std::wstring wsFieldValue;
    char* szFieldValueBuf = new char[nStrValueWidth];
    memset(szFieldValueBuf, 0, nStrValueWidth);

    int nChunkCount = 0;
    CChunk* pChunk = nullptr;
    int_64 nFeatureCount = poLayer->GetFeatureCount();
    printf("shpFile:%s ,nFeatureCount:%lld\n", sShpFileFullPath.c_str(), nFeatureCount);
    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL ) {
        //if(poFeature->GetFieldAsDouble("AREA")<1) continue; //去掉面积过小的polygon

        poGeometry = poFeature->GetGeometryRef();
        if (poGeometry == nullptr)
            continue;

        eGeoType = wkbFlatten(poGeometry->getGeometryType());
        //if(eGeoType != nLayerType){
        //    printf("ERR : Read Shp: multi geometry\n");
        //    continue;
        //}
        switch (eGeoType)
        {
        case OGRwkbGeometryType::wkbPoint: {
            if(2 == nLayerTypeGeo || 3 == nLayerTypeGeo){
                printf("ERR : Read Shp: multi geometry\n");
                continue;
            }
            poPoint = (OGRPoint*)poGeometry->clone();
            fGeoX = poPoint->getX();
            fGeoY = poPoint->getY();
            if (poPoint->getCoordinateDimension() >= 3)
                fZVal = poPoint->getZ();
            else
                fZVal = -999999.9999;
            if(nNeedTransfromCoord) {
                pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
            }
            pChunk = new CChunk();
            pChunk->m_pBelongLayer = pLayerGeo;
            pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;
            pChunk->m_vecChunkPoints.push_back(glm::dvec3(fGeoX, fGeoY, fZVal));

            pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(nChunkCount++));

            pChunk->m_chunkBound = glm::dvec4(fGeoX, fGeoY, fGeoX, fGeoY);
            pLayerGeo->AddChunk(pChunk);
            break;
        }
        case OGRwkbGeometryType::wkbLineString: {
            if(1 == nLayerTypeGeo || 3 == nLayerTypeGeo){
                printf("ERR : Read Shp: multi geometry\n");
                continue;
            }
            poLineString = (OGRLineString*)poGeometry->clone();
            nPointCnt = poLineString->getNumPoints();
            if(nPointCnt < 2){
                continue;
            }

            pChunk = new CChunk();
            pChunk->m_pBelongLayer = pLayerGeo;
            pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;

            fEnvelopeMinLng = 999999999.999999;
            fEnvelopeMinLat = 999999999.999999;
            fEnvelopeMaxLng = -999999999.999999;
            fEnvelopeMaxLat = -999999999.999999;
            for (i = 0; i < nPointCnt; i++) {
                fGeoX = poLineString->getX(i);
                fGeoY = poLineString->getY(i);
                if (poLineString->getCoordinateDimension() >= 3)
                    fZVal = poLineString->getZ(i);
                else
                    fZVal = -999999.9999;
                if(nNeedTransfromCoord) {
                    pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
                }
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

                pChunk->m_vecChunkPoints.push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
            }
            // 和 Polygon 比较 没有闭合操作

            pChunk->m_chunkBound = glm::dvec4(fEnvelopeMinLng, fEnvelopeMinLat, fEnvelopeMaxLng, fEnvelopeMaxLat);
            pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(nChunkCount++));
            pLayerGeo->AddChunk(pChunk);

            break;
        }
        case OGRwkbGeometryType::wkbPolygon: {
            if(2 == nLayerTypeGeo || 1 == nLayerTypeGeo){
                printf("ERR : Read Shp: multi geometry\n");
                continue;
            }
            poPolygon = (OGRPolygon*)poGeometry->clone();
            poLineString = poPolygon->getExteriorRing();
            nPointCnt = poLineString->getNumPoints();
            nPointCnt = nPointCnt - 1;
            if(nPointCnt < 3){
                continue;
            }

            pChunk = new CChunk();
            pChunk->m_pBelongLayer = pLayerGeo;
            pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;

            fEnvelopeMinLng = 999999999.999999;
            fEnvelopeMinLat = 999999999.999999;
            fEnvelopeMaxLng = -999999999.999999;
            fEnvelopeMaxLat = -999999999.999999;

            for (i = 0; i < nPointCnt; i++) {
                fGeoX = poLineString->getX(i);
                fGeoY = poLineString->getY(i);
                if (poLineString->getCoordinateDimension() >= 3)
                    fZVal = poLineString->getZ(i);
                else
                    fZVal = -999999.9999;
                if(nNeedTransfromCoord) {
                    pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
                }
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

                pChunk->m_vecChunkPoints.push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
            }
            // 闭合操作

            pChunk->m_chunkBound = glm::dvec4(fEnvelopeMinLng, fEnvelopeMinLat, fEnvelopeMaxLng, fEnvelopeMaxLat);
            pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(nChunkCount++));
            pLayerGeo->AddChunk(pChunk);

            break;
        }
        case OGRwkbGeometryType::wkbMultiPolygon: {
            OGRMultiPolygon* poMulPolygon = (OGRMultiPolygon*)poGeometry->clone();
            for (i = 0; i < poMulPolygon->getNumGeometries(); i++) {
                poPolygon = (OGRPolygon*)poMulPolygon->getGeometryRef(i);
            }
            break;
        }
        default:
            printf("unknown geo type\n");
            break;
        }

        OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
        int nFieldCnt = poFDefn->GetFieldCount(); //获得字段的数目，不包括前两个字段（FID,Shape);
        for(int j = 0; j < nFieldCnt; j++ ) {
            OGRFieldDefn* poFieldDefn = poFDefn->GetFieldDefn(j);
            if(poFieldDefn == nullptr){
                printf("ERR : Read Shp GetFieldDefn: %d\n", j);
                continue;
            }
            CChunkAttributeInfo chunkAttrVal;
            switch (vecFieldType[j]) {
            case ECHUNK_ATTRIBUTE_TYPE::e_int: {
                chunkAttrVal.SetAttributeValue(poFeature->GetFieldAsInteger(j));
                break;
            }
            case ECHUNK_ATTRIBUTE_TYPE::e_double:{
                chunkAttrVal.SetAttributeValue(poFeature->GetFieldAsDouble(j));
                break;
            }
            case ECHUNK_ATTRIBUTE_TYPE::e_char_ptr:{
                sFieldValue = poFeature->GetFieldAsString(j);
                chunkAttrVal.SetAttributeValue((char*)sFieldValue.c_str());
                break;
            }
            case ECHUNK_ATTRIBUTE_TYPE::e_wchar_ptr:{
                chunkAttrVal.SetAttributeValue(0);
                break;
            }
            case ECHUNK_ATTRIBUTE_TYPE::e_void_ptr:{
                chunkAttrVal.SetAttributeValue(0);
                break;
            }
            case ECHUNK_ATTRIBUTE_TYPE::e_long_long:{
                chunkAttrVal.SetAttributeValue((long long)poFeature->GetFieldAsInteger64(j));
                break;
            }
            default:{
                chunkAttrVal.SetAttributeValue(0);
            }
            }
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);
        }
        OGRFeature::DestroyFeature( poFeature );
    }
    GDALClose( poDS );

    pLayerGeo->m_sCoordProject = sProjectRef;

    pLayerGeo->MakeUpChunks();

    return pLayerGeo;
}
CLayerGeoDraw* CLayerVector::ReadShapeFile_bak(const std::string& sShpFileFullPath) {
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//图像路径不支持中文（可设置）
    CPLSetConfigOption("SHAPE_ENCODING","");  //解决中文乱码问题

    GDALDataset* poDS = (GDALDataset*) GDALOpenEx(sShpFileFullPath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL );

    if( poDS == nullptr ) {
        printf( "Open:[%s] failed.\n", sShpFileFullPath.c_str());
        return 0;
    }
    std::string sProjectRef = poDS->GetProjectionRef();

    OGRLayer* poLayer = poDS->GetLayer(0); //读取层
    if(poLayer == nullptr){
        printf( "GetLayer:[%s] failed.\n", sShpFileFullPath.c_str());
        return 0;
    }

    OGRwkbGeometryType nLayerType = poLayer->GetGeomType();
    CLayerGeoDraw* pLayerGeo = nullptr;
    CLayerPoints* pLayerPoints = nullptr;
    CLayerLines* pLayerLines = nullptr;
    CLayerPolygons* pLayerPolygons = nullptr;
    std::vector<glm::dvec3>* pVecPoints = nullptr;
    std::vector<unsigned int>* pVecPointInPolyIndx = nullptr;
    std::vector<float>* pVecPointFlag = nullptr;
    std::vector<float>* pVecPointFlagOperate = nullptr;
    std::vector<unsigned int> *pVecVerticsIndx = nullptr;
    std::vector<double>* pVecPointsZValue = nullptr;
    std::vector<std::vector<int>>* pVecEleChunks = nullptr;
    std::vector<glm::dvec4>* pVecEleChunksBound = nullptr;

    int nLayerTypeGeo = 0;
    switch (nLayerType) {
    case OGRwkbGeometryType::wkbPoint:
    case OGRwkbGeometryType::wkbPointM:
    case OGRwkbGeometryType::wkbPointZM:
    case OGRwkbGeometryType::wkbPoint25D: {
        pLayerPoints = new CLayerPoints(this->m_pGLCore);
        pLayerGeo = pLayerPoints;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
        nLayerTypeGeo = 1;
        break;
    }
    case OGRwkbGeometryType::wkbLineString:
    case OGRwkbGeometryType::wkbLineStringM:
    case OGRwkbGeometryType::wkbLineStringZM:
    case OGRwkbGeometryType::wkbLineString25D:{
        //pLayerPoints = new SxLayerPoints(this->m_pGLCore);
        pLayerLines = new CLayerLines(this->m_pGLCore);
        pLayerGeo = pLayerLines;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolyline;
        nLayerTypeGeo = 2;
        break;
    }
    case OGRwkbGeometryType::wkbPolygon:
    case OGRwkbGeometryType::wkbPolygonM:
    case OGRwkbGeometryType::wkbPolygonZM:
    case OGRwkbGeometryType::wkbPolygon25D:{
        //pLayerPoints = new SxLayerPoints(this->m_pGLCore);
        pLayerPolygons = new CLayerPolygons(this->m_pGLCore);
        pLayerGeo = pLayerPolygons;
        pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolygon;
        nLayerTypeGeo = 3;
        break;
    }
    }
    pLayerGeo->Init();
    //pLayerGeo->m_pVecPoints = new std::vector<glm::dvec3>;
    //pLayerGeo->m_pVecPointInPolyIndx = new std::vector<unsigned int>;
    //pLayerGeo->m_pVecPointFlag = new std::vector<float>;
    //pLayerGeo->m_pVecVerticsIndx = new std::vector<unsigned int>;
    ////pLayerGeo->m_pVecPointsZValue = new std::vector<double>;
    //pLayerGeo->m_pVecEleChunks = new std::vector<std::vector<int>>;
    //pLayerGeo->m_pVecEleChunksBound = new std::vector<glm::dvec4>;

    //pLayerGeo->m_pVecPointFlagOperate = new std::vector<float>;

    pVecPoints = pLayerGeo->m_pVecPoints;
    pVecPointInPolyIndx = pLayerGeo->m_pVecPointInPolyIndx;
    pVecPointFlag = pLayerGeo->m_pVecPointFlag;
    pVecVerticsIndx = pLayerGeo->m_pVecVerticsIndx;
    //pVecPointsZValue = pLayerGeo->m_pVecPointsZValue;
    //pVecEleChunks = pLayerGeo->m_pVecEleChunks;
    //pVecEleChunksBound = pLayerGeo->m_pVecEleChunksBound;

    pVecPointFlagOperate = pLayerGeo->m_pVecPointFlagOperate;

    OGREnvelope pEnvelope;
    poLayer->GetExtent(&pEnvelope, TRUE);
    double fEnvelopeMinLng = pEnvelope.MinX;
    double fEnvelopeMinLat = pEnvelope.MinY;
    double fEnvelopeMaxLng = pEnvelope.MaxX;
    double fEnvelopeMaxLat = pEnvelope.MaxY;

    OGRFeature* poFeature = nullptr;
    //OGRFieldDefn* poFieldDefn = nullptr;
    OGRGeometry* poGeometry = nullptr;
    OGRwkbGeometryType eGeoType = OGRwkbGeometryType::wkbUnknown;
    OGRPoint* poPoint = nullptr;
    OGRPolygon* poPolygon = nullptr;
    OGRLineString* poLineString = nullptr;
    double fGeoX = 0.0, fGeoY = 0.0, fZVal = 0.0;
    int i = 0, j = 0, nFieldCnt = 0, nPointCnt = 0;
    unsigned int nEleChunkCnt = 0;
    unsigned int nPointFalg = 1;
    float fPointFalgAA = 1.0f;
    pVecPoints->clear();

    std::string sSrcProject = "PROJCS[\"Xian_1980_3_Degree_GK_Zone_39\",GEOGCS[\"GCS_Xian_1980\",DATUM[\"D_Xian_1980\",SPHEROID[\"Xian_1980\",6378140.0,298.257]],PRIMEM[\"Greenwich\",0.0000],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",39500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",117.0],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
    std::string sDstProject = "GEOGCS[\"GCS_Xian_1980\",DATUM[\"D_Xian_1980\",SPHEROID[\"Xian_1980\",6378140.0,298.257]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433],AUTHORITY[\"EPSG\",4610]]";

    char* szSrcProject = (char*)sSrcProject.c_str();
    char* szDstProject = (char*)sDstProject.c_str();
    OGRSpatialReference srcCoordConvert, dstCoordConvert;
    srcCoordConvert.importFromWkt(&(szSrcProject));
    dstCoordConvert.importFromWkt(&(szDstProject));
    OGRCoordinateTransformation *pTrans = OGRCreateCoordinateTransformation(&srcCoordConvert, &dstCoordConvert);
    if(pTrans == nullptr){
        printf("Coord Convert Faild !\n");
        return 0;
    }
    int nNeedTransfromCoord = false;
    if(fEnvelopeMinLng > 10000 || fEnvelopeMinLat > 10000 ||
            fEnvelopeMaxLng > 10000 || fEnvelopeMaxLat > 10000 ) {
        pTrans->Transform(1, &fEnvelopeMinLng, &fEnvelopeMinLat);
        pTrans->Transform(1, &fEnvelopeMaxLng, &fEnvelopeMaxLat);
        nNeedTransfromCoord = true;
    }

    pLayerGeo->m_fMinLng = fEnvelopeMinLng;
    pLayerGeo->m_fMinLat = fEnvelopeMinLat;
    pLayerGeo->m_fMaxLng = fEnvelopeMaxLng;
    pLayerGeo->m_fMaxLat = fEnvelopeMaxLat;



    poLayer->ResetReading();
    int nPointIndx = 0, nPointIndxTmp = 0;
    while( (poFeature = poLayer->GetNextFeature()) != NULL ) {
        poGeometry = poFeature->GetGeometryRef();
        if (poGeometry == nullptr)
            continue;
        if(poGeometry->Is3D())
            pLayerGeo->m_nHasZValue = 1;
        break;
    }

    int_64 nFeatureCount = poLayer->GetFeatureCount();
    printf("shpFile:%s ,nFeatureCount:%lld\n", sShpFileFullPath.c_str(), nFeatureCount);
    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL ) {
        //if(poFeature->GetFieldAsDouble("AREA")<1) continue; //去掉面积过小的polygon
        //OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
        //nFieldCnt = poFDefn->GetFieldCount(); //获得字段的数目，不包括前两个字段（FID,Shape);
        //for( j = 0; j < nFieldCnt; j++ ) {
        //    //输出每个字段的值
        //    //std::cout << poFeature->GetFieldAsString(j) << "    ";
        //    poFieldDefn = poFDefn->GetFieldDefn(j);
        //    if(poFieldDefn == nullptr){
        //        printf("ERR : Read Shp GetFieldDefn: %d\n", j);
        //        continue;
        //    }
        //    if (poFieldDefn->GetType() == OFTInteger) {
        //        printf("%d %d\n", j, poFeature->GetFieldAsInteger(j)); //输出，不同的形式，效果与上句相同
        //    }
        //    else if (poFieldDefn->GetType() == OFTReal) {
        //        printf("%d %.3f\n", j, poFeature->GetFieldAsDouble(j));
        //    }
        //    else if (poFieldDefn->GetType() == OFTString) {
        //        printf("%d %s\n", j, QString(poFeature->GetFieldAsString(j)).toLocal8Bit().data());
        //    }
        //    else {
        //        printf("%d %s\n", j, QString(poFeature->GetFieldAsString(j)).toLocal8Bit().data());
        //    }
        //}

        poGeometry = poFeature->GetGeometryRef();
        if (poGeometry == nullptr)
            continue;

        eGeoType = wkbFlatten(poGeometry->getGeometryType());
        //if(eGeoType != nLayerType){
        //    printf("ERR : Read Shp: multi geometry\n");
        //    continue;
        //}
        switch (eGeoType)
        {
        case OGRwkbGeometryType::wkbPoint: {
            if(2 == nLayerTypeGeo || 3 == nLayerTypeGeo){
                printf("ERR : Read Shp: multi geometry\n");
                continue;
            }
            poPoint = (OGRPoint*)poGeometry->clone();
            fGeoX = poPoint->getX();
            fGeoY = poPoint->getY();
            if (poPoint->getCoordinateDimension() >= 3)
                fZVal = poPoint->getZ();
            else
                fZVal = -999999.9999;
            if(nNeedTransfromCoord) {
                pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
            }
            pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
            //pVecPointsZValue->push_back(fZVal);
            pVecPointFlag->push_back(0.0f);
            pVecPointFlagOperate->push_back(0.0f);
            pVecVerticsIndx->push_back(nPointIndx++);
            break;
        }
        case OGRwkbGeometryType::wkbLineString: {
            if(1 == nLayerTypeGeo || 3 == nLayerTypeGeo){
                printf("ERR : Read Shp: multi geometry\n");
                continue;
            }
            poLineString = (OGRLineString*)poGeometry->clone();
            nPointCnt = poLineString->getNumPoints();
            if(nPointCnt < 2){
                continue;
            }

            nEleChunkCnt = (unsigned int)pVecEleChunks->size();
            std::vector<int> vecLinesElePoints;
            fGeoX = poLineString->getX(0);
            fGeoY = poLineString->getY(0);
            if (poLineString->getCoordinateDimension() >= 3)
                fZVal = poLineString->getZ(0);
            else
                fZVal = -999999.9999;
            if(nNeedTransfromCoord) {
                pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
            }

            fEnvelopeMinLng = fGeoX;
            fEnvelopeMinLat = fGeoY;
            fEnvelopeMaxLng = fGeoX;
            fEnvelopeMaxLat = fGeoY;
            pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
            //pVecPointsZValue->push_back(fZVal);
            pVecPointInPolyIndx->push_back(nEleChunkCnt);
            pVecPointFlag->push_back(0.0f);
            pVecPointFlagOperate->push_back(0.0f);
            nPointIndx = (int)pVecPoints->size();
            vecLinesElePoints.push_back(nPointIndx-1);
            for (i = 1; i < nPointCnt; i++) {
                fGeoX = poLineString->getX(i);
                fGeoY = poLineString->getY(i);
                if (poLineString->getCoordinateDimension() >= 3)
                    fZVal = poLineString->getZ(i);
                else
                    fZVal = -999999.9999;
                if(nNeedTransfromCoord) {
                    pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
                }
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
                pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
                //pVecPointsZValue->push_back(fZVal);
                pVecPointInPolyIndx->push_back(nEleChunkCnt); // 这个点属于哪个Polyline，对应Polyline的下标

                pVecPointFlag->push_back(0.0f);//用于显示不同的颜色
                pVecPointFlagOperate->push_back(0.0f);//用于被操作的面（如被选中）显示不同颜色

                // 上一个点链接下一个点，存放上一个，下一个坐标的索引，连成线
                pVecVerticsIndx->push_back(nPointIndx-1);
                pVecVerticsIndx->push_back(nPointIndx);

                vecLinesElePoints.push_back(nPointIndx);
                nPointIndx++;
            }
            // 和 Polygon 比较 没有闭合操作

            pVecEleChunks->push_back(vecLinesElePoints);
            pVecEleChunksBound->push_back(glm::dvec4(fEnvelopeMinLng, fEnvelopeMinLat, fEnvelopeMaxLng, fEnvelopeMaxLat));

            break;
        }
        case OGRwkbGeometryType::wkbPolygon: {
            if(2 == nLayerTypeGeo || 1 == nLayerTypeGeo){
                printf("ERR : Read Shp: multi geometry\n");
                continue;
            }
            poPolygon = (OGRPolygon*)poGeometry->clone();
            poLineString = poPolygon->getExteriorRing();
            nPointCnt = poLineString->getNumPoints();
            nPointCnt = nPointCnt - 1;
            if(nPointCnt < 3){
                continue;
            }

            nEleChunkCnt = (unsigned int)pVecEleChunks->size();
            fGeoX = poLineString->getX(0);
            fGeoY = poLineString->getY(0);
            if (poLineString->getCoordinateDimension() >= 3)
                fZVal = poLineString->getZ(0);
            else
                fZVal = -999999.9999;
            if(nNeedTransfromCoord) {
                pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
            }

            fEnvelopeMinLng = fGeoX;
            fEnvelopeMinLat = fGeoY;
            fEnvelopeMaxLng = fGeoX;
            fEnvelopeMaxLat = fGeoY;

            fPointFalgAA = 0.0f;//(((nEleChunkCnt) % 5) * 1.0f);
            pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
            //pVecPointsZValue->push_back(fZVal);
            pVecPointInPolyIndx->push_back(nEleChunkCnt);
            pVecPointFlag->push_back(0.0f);
            pVecPointFlagOperate->push_back(0.0f);
            nPointIndx = (int)pVecPoints->size();
            std::vector<int> vecPolygonsElePoints;
            vecPolygonsElePoints.push_back(nPointIndx-1);
            //nEleChunkCnt = nEleChunkCnt - 1;
            nPointIndxTmp = nPointIndx;
            for (i = 1; i < nPointCnt; i++) {
                fGeoX = poLineString->getX(i);
                fGeoY = poLineString->getY(i);
                if (poLineString->getCoordinateDimension() >= 3)
                    fZVal = poLineString->getZ(i);
                else
                    fZVal = -999999.9999;
                if(nNeedTransfromCoord) {
                    pTrans->Transform(1, &fGeoX, &fGeoY, &fZVal);
                }
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
                pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
                //pVecPointsZValue->push_back(fZVal);
                pVecPointInPolyIndx->push_back(nEleChunkCnt); // 这个点属于哪个Polygon，对应Polygon的下标

                pVecPointFlag->push_back(0.0f);//用于显示不同的颜色
                pVecPointFlagOperate->push_back(0.0f);//用于被操作的面（如被选中）显示不同颜色

                // 上一个点链接下一个点，存放上一个，下一个坐标的索引，连成线
                pVecVerticsIndx->push_back(nPointIndx-1);
                pVecVerticsIndx->push_back(nPointIndx);

                vecPolygonsElePoints.push_back(nPointIndx);                
                nPointIndx++;
            }
            // 闭合操作
            pVecVerticsIndx->push_back(nPointIndx-1);
            pVecVerticsIndx->push_back(nPointIndxTmp-1);

            pVecEleChunks->push_back(vecPolygonsElePoints);
            pVecEleChunksBound->push_back(glm::dvec4(fEnvelopeMinLng, fEnvelopeMinLat, fEnvelopeMaxLng, fEnvelopeMaxLat));
            break;
        }
        case OGRwkbGeometryType::wkbMultiPolygon: {
            OGRMultiPolygon* poMulPolygon = (OGRMultiPolygon*)poGeometry->clone();
            for (i = 0; i < poMulPolygon->getNumGeometries(); i++) {
                poPolygon = (OGRPolygon*)poMulPolygon->getGeometryRef(i);
            }
            break;
        }
        default:
            printf("unknown geo type\n");
            break;
        }


        OGRFeature::DestroyFeature( poFeature );
    }
    GDALClose( poDS );

    pLayerGeo->m_sCoordProject = sProjectRef;

    return pLayerGeo;
}
CLayerGeoDraw* CLayerVector::AddGeometryItem(const std::string& sVectorFullPath){

    CLayerGeoDraw* pSxRasterDraw = this->ReadShapeFile(sVectorFullPath);
    if(pSxRasterDraw == nullptr){
        return nullptr;
    }
    pSxRasterDraw->m_sLayerName = sVectorFullPath;
    //SxLayerGeoDraw* pSxRasterDraw = new SxLayerGeoDraw(this->m_pGLCore, sVectorFullPath);
    //delete pSxRasterDraw;
    //return nullptr;
    this->m_vecLayerItem.push_back(pSxRasterDraw);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pSxRasterDraw, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pSxRasterDraw->m_pLayerVector = this;
    return pSxRasterDraw;
}

CLayerGeoDraw* CLayerVector::ReadPxyFile(const std::string& sPxyFileFullPath){

    CLayerGeoDraw* pLayerGeo = nullptr;
    CLayerPoints* pLayerPoints = nullptr;
    std::vector<glm::dvec3>* pVecPoints = nullptr;
    std::vector<unsigned int>* pVecPointInPolyIndx = nullptr;
    std::vector<float>* pVecPointFlag = nullptr;
    std::vector<float>* pVecPointFlagOperate = nullptr;
    std::vector<unsigned int> *pVecVerticsIndx = nullptr;
    std::vector<double>* pVecPointsZValue = nullptr;
    std::vector<std::vector<int>>* pVecEleChunks = nullptr;
    std::vector<glm::dvec4>* pVecEleChunksBound = nullptr;

    std::vector<std::string>* pVecChunksName = nullptr;
    std::unordered_map<std::string, int>* pMapChunkName2Indx = nullptr;

    pLayerPoints = new CLayerPoints(this->m_pGLCore);
    pLayerGeo = pLayerPoints;

    pLayerGeo->Init();
    pVecPoints = pLayerGeo->m_pVecPoints;
    pVecPointInPolyIndx = pLayerGeo->m_pVecPointInPolyIndx;
    pVecPointFlag = pLayerGeo->m_pVecPointFlag;
    pVecVerticsIndx = pLayerGeo->m_pVecVerticsIndx;
    //pVecPointsZValue = pLayerGeo->m_pVecPointsZValue;
    //pVecEleChunks = pLayerGeo->m_pVecEleChunks;
    //pVecEleChunksBound = pLayerGeo->m_pVecEleChunksBound;
    pVecPointFlagOperate = pLayerGeo->m_pVecPointFlagOperate;
    pVecChunksName = pLayerGeo->m_pVecChunksName;
    pMapChunkName2Indx = pLayerGeo->m_pMapChunkName2Indx;

    double fGeoX = 0.0, fGeoY = 0.0, fZVal = 0.0;
    pVecPoints->clear();
    int nPointIndx = 0;

    FILE* fp = fopen(sPxyFileFullPath.c_str(), "r");
    if (fp == NULL)
    {
        printf("PXY File Open Failed!\n");
        return nullptr;
    }
    int nPointPxyCnt = 0;
    fscanf(fp, "%d", &nPointPxyCnt);
    char szPxyName[64] = {0};
    double fPixX = 0.0, fPixY = 0.0;
    double fEnvelopeMinLng = 999999.0;
    double fEnvelopeMinLat = 999999.0;
    double fEnvelopeMaxLng = -999999.0;
    double fEnvelopeMaxLat = -999999.0;
    for (; nPointIndx < nPointPxyCnt;) {

        fscanf(fp, " %s %lf %lf", szPxyName, &fPixX, &fPixY);
        printf("%s %lf %lf \n", szPxyName, fPixX, fPixY);
        if(szPxyName[0] == '\0'){
            continue;
        }

        //this->m_pAdfTransform[1] = 0.009009009009009009;
        //this->m_pAdfTransform[5] = -0.009009009009009009;
        fGeoX = fPixX * 0.009009009009009009;
        fGeoY = fPixY * -0.009009009009009009;

        if(fEnvelopeMinLng > fGeoX){
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY){
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }
        pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
        //pVecPointsZValue->push_back(fZVal);
        pVecPointFlag->push_back(0.0f);
        pVecPointFlagOperate->push_back(0.0f);
        pVecVerticsIndx->push_back(nPointIndx++);

        pVecChunksName->push_back(szPxyName);
        pMapChunkName2Indx->insert(std::pair<std::string, int>(szPxyName, nPointIndx-1));

        memset(szPxyName, 0, 32);
    }
    fclose(fp);

    pLayerGeo->m_fMinLng = fEnvelopeMinLng;
    pLayerGeo->m_fMinLat = fEnvelopeMinLat;
    pLayerGeo->m_fMaxLng = fEnvelopeMaxLng;
    pLayerGeo->m_fMaxLat = fEnvelopeMaxLat;

    pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
    return pLayerGeo;
}
CLayerGeoDraw* CLayerVector::AddGeometryItemPxy(const std::string& sPxyFullPath){
    CLayerGeoDraw* pSxRasterDraw = this->ReadPxyFile(sPxyFullPath);
    if(pSxRasterDraw == nullptr){
        return nullptr;
    }
    pSxRasterDraw->m_sLayerName = sPxyFullPath;
    this->m_vecLayerItem.push_back(pSxRasterDraw);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pSxRasterDraw, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pSxRasterDraw->m_pLayerVector = this;
    return pSxRasterDraw;
}

CLayerGeoDraw* CLayerVector::ReadGcpFile(const std::string& sGcpFileFullPath){
    CLayerGeoDraw* pLayerGeo = nullptr;
    CLayerPoints* pLayerPoints = nullptr;
    std::vector<glm::dvec3>* pVecPoints = nullptr;
    std::vector<unsigned int>* pVecPointInPolyIndx = nullptr;
    std::vector<float>* pVecPointFlag = nullptr;
    std::vector<float>* pVecPointFlagOperate = nullptr;
    std::vector<unsigned int> *pVecVerticsIndx = nullptr;
    std::vector<double>* pVecPointsZValue = nullptr;
    std::vector<std::vector<int>>* pVecEleChunks = nullptr;
    std::vector<glm::dvec4>* pVecEleChunksBound = nullptr;

    std::vector<std::string>* pVecChunksName = nullptr;
    std::unordered_map<std::string, int>* pMapChunkName2Indx = nullptr;

    pLayerPoints = new CLayerPoints(this->m_pGLCore);
    pLayerGeo = pLayerPoints;

    pLayerGeo->Init();
    pVecPoints = pLayerGeo->m_pVecPoints;
    pVecPointInPolyIndx = pLayerGeo->m_pVecPointInPolyIndx;
    pVecPointFlag = pLayerGeo->m_pVecPointFlag;
    pVecVerticsIndx = pLayerGeo->m_pVecVerticsIndx;
    //pVecPointsZValue = pLayerGeo->m_pVecPointsZValue;
    //pVecEleChunks = pLayerGeo->m_pVecEleChunks;
    //pVecEleChunksBound = pLayerGeo->m_pVecEleChunksBound;
    pVecPointFlagOperate = pLayerGeo->m_pVecPointFlagOperate;
    pVecChunksName = pLayerGeo->m_pVecChunksName;
    pMapChunkName2Indx = pLayerGeo->m_pMapChunkName2Indx;

    double fGeoX = 0.0, fGeoY = 0.0, fZVal = 0.0;
    pVecPoints->clear();
    int nPointIndx = 0, nPtType = 0;

    FILE* fp = fopen(sGcpFileFullPath.c_str(), "r");
    if (fp == NULL)
    {
        printf("PXY File Open Failed!\n");
        return nullptr;
    }
    int nPointPxyCnt = 0;
    fscanf(fp, "%d", &nPointPxyCnt);
    char szPxyName[64] = {0};
    //double fPixX = 0.0, fPixY = 0.0;
    double fEnvelopeMinLng = 999999.0;
    double fEnvelopeMinLat = 999999.0;
    double fEnvelopeMaxLng = -999999.0;
    double fEnvelopeMaxLat = -999999.0;
    for (; nPointIndx < nPointPxyCnt;) {

        fscanf(fp, " %s %lf %lf %lf %d", szPxyName, &fGeoY, &fGeoX, &fZVal, &nPtType);
        printf("%s %lf %lf %lf %d\n", szPxyName, fGeoY, fGeoX, fZVal, nPtType);
        if(szPxyName[0] == '\0'){
            continue;
        }

        if(fEnvelopeMinLng > fGeoX){
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY){
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }
        pVecPoints->push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
        //pVecPointsZValue->push_back(fZVal);
        pVecPointFlag->push_back(0.0f);
        pVecPointFlagOperate->push_back(0.0f);
        pVecVerticsIndx->push_back(nPointIndx++);

        pVecChunksName->push_back(szPxyName);
        pMapChunkName2Indx->insert(std::pair<std::string, int>(szPxyName, nPointIndx-1));

        memset(szPxyName, 0, 32);
    }
    fclose(fp);

    pLayerGeo->m_fMinLng = fEnvelopeMinLng;
    pLayerGeo->m_fMinLat = fEnvelopeMinLat;
    pLayerGeo->m_fMaxLng = fEnvelopeMaxLng;
    pLayerGeo->m_fMaxLat = fEnvelopeMaxLat;

    pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
    return pLayerGeo;
}
CLayerGeoDraw* CLayerVector::AddGeometryItemGcp(const std::string& sGcpFullPath){
    CLayerGeoDraw* pSxRasterDraw = this->ReadGcpFile(sGcpFullPath);
    if(pSxRasterDraw == nullptr){
        return nullptr;
    }
    pSxRasterDraw->m_sLayerName = sGcpFullPath;
    this->m_vecLayerItem.push_back(pSxRasterDraw);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pSxRasterDraw, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pSxRasterDraw->m_pLayerVector = this;
    return pSxRasterDraw;
}

CLayerGeoDraw* CLayerVector::ReadResidualFile(const std::string& sResidualFileFullPath){
    return nullptr;
}
std::vector<CLayerGeoDraw*> CLayerVector::AddGeometryItemResidualInfo(const std::string& sResidualFileFullPath){
    std::vector<CLayerGeoDraw*> vecLayerResidual;

    static CResidualInfo* pResidualInfo = nullptr;
    if(pResidualInfo != nullptr){
        delete pResidualInfo;
    }
    pResidualInfo = new CResidualInfo;
    CResidualInfo& residualInfo = *pResidualInfo;
    residualInfo.LoadResidualFile(sResidualFileFullPath);
    //pResidualInfo = nullptr;
    //////////////////////////////////////////////////////////////////
    //连接点

    CLayerGeoDraw* pLayerGeo = nullptr;
    CLayerPoints* pLayerPoints = nullptr;
    CLayerPolygons* pLayerPolygons = nullptr;
    pLayerPoints = new CLayerPoints(this->m_pGLCore);
    pLayerGeo = pLayerPoints;
    pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
    pLayerGeo->m_nLayerColor = 0xFFFF00AA;

    double fEnvelopeMinLng = 999999999.999999999;
    double fEnvelopeMinLat = 999999999.999999999;
    double fEnvelopeMaxLng = -999999999.999999999;
    double fEnvelopeMaxLat = -999999999.999999999;

    double fGeoX = 0.0, fGeoY = 0.0, fZVal = 0.0;


    CLayerField layerFld;
    layerFld.m_sFieldName = "DianHao";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "DianHao_1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long_long;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "JingDu";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "WeiDu";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "GaoCheng";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "BlockData";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_void_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    //>>>>>>>>>>>>
    layerFld.m_sFieldName = "TifFile1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixX1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixY1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResX1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResY1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResP1"; //12
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixWeit1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);
    //>>>>>>>>>>>>
    layerFld.m_sFieldName = "TifFile2";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixX2";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixY2";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResX2";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResY2";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResP2"; //19
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixWeit2";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);
    //>>>>>>>>>>>>
    pLayerGeo->MakeupFields();

    CChunk* pChunk = nullptr;
    CLianJieDianZuoBiao* pLianJieDianZuoBiao = nullptr;
    int i = 0, nLianJieDianCount = (int)residualInfo.m_LianJieDianZuoBiaoBlock.m_vecLianJieDianZuoBiao.size();
    for(i = 0; i<nLianJieDianCount; i++) {
        pLianJieDianZuoBiao = residualInfo.m_LianJieDianZuoBiaoBlock.m_vecLianJieDianZuoBiao[i];
        fGeoX = pLianJieDianZuoBiao->m_fLng;
        fGeoY = pLianJieDianZuoBiao->m_fLat;
        fZVal = pLianJieDianZuoBiao->m_fHeight;

        pChunk = new CChunk();
        pChunk->m_pBelongLayer = pLayerGeo;
        pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;
        pChunk->m_vecChunkPoints.push_back(glm::dvec3(fGeoX, fGeoY, fZVal));

        pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(i));

        pChunk->m_chunkBound = glm::dvec4(fGeoX, fGeoY, fGeoX, fGeoY);
        pLayerGeo->AddChunk(pChunk);

        if(fEnvelopeMinLng > fGeoX){
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY){
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }

        CChunkAttributeInfo chunkAttrVal;
        chunkAttrVal.SetAttributeValue((char*)pLianJieDianZuoBiao->m_sDianHao.c_str());
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((unsigned long long)pLianJieDianZuoBiao->m_nDianHao);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fGeoX);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fGeoY);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fZVal);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((void*)pLianJieDianZuoBiao);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        if(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1){
            chunkAttrVal.SetAttributeValue((char*)pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1->m_sTifName.c_str());
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1->m_fPix_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1->m_fPix_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1->m_fPix_Residual_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1->m_fPix_Residual_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1->m_fPix_Residual_Plane);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1->m_fWeight);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);
        }
        if(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2){

            chunkAttrVal.SetAttributeValue((char*)pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2->m_sTifName.c_str());
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2->m_fPix_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2->m_fPix_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2->m_fPix_Residual_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2->m_fPix_Residual_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2->m_fPix_Residual_Plane);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2->m_fWeight);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);
        }
    }

    pLayerGeo->m_fMinLng = fEnvelopeMinLng;
    pLayerGeo->m_fMinLat = fEnvelopeMinLat;
    pLayerGeo->m_fMaxLng = fEnvelopeMaxLng;
    pLayerGeo->m_fMaxLat = fEnvelopeMaxLat;

    pLayerGeo->m_pExtData = (void*)pResidualInfo;

    pLayerGeo->MakeUpChunks();
    vecLayerResidual.push_back(pLayerGeo);

    pLayerGeo->m_sLayerName = "Residual_LianJie_Points";
    this->m_vecLayerItem.push_back(pLayerGeo);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pLayerGeo, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pLayerGeo->m_pLayerVector = this;
    //////////////////////////////////////////////////////////////////
    //控制点

    pLayerPoints = new CLayerPoints(this->m_pGLCore);
    pLayerGeo = pLayerPoints;
    pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
    pLayerGeo->m_nLayerColor = 0xFFFFAA55;

    layerFld.m_sFieldName = "DianHao";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "DianHao_1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long_long;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "JingDu";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "WeiDu";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "GaoCheng";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_X";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_Y";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_Plane";//8
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_Height";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Angle";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "BlockData";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_void_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);
    //>>>>>>>>>>>>>>

    layerFld.m_sFieldName = "TifFile";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixX";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixY";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResX";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResY";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixWeit";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);
    //>>>>>>>>>>>>>>
    pLayerGeo->MakeupFields();

    CControlDianPrecision* pControlDianPrecision = nullptr;    
    std::vector<CLianJieDianXiangFangWuCha*>* pVecDianHao2Indx = nullptr;

    int nControlDianCount = (int)residualInfo.m_ControlDianPrecisionBlock.m_vecControlDianPrecision.size();
    for(i = 0; i<nControlDianCount; i++) {
        pControlDianPrecision = residualInfo.m_ControlDianPrecisionBlock.m_vecControlDianPrecision[i];
        fGeoX = pControlDianPrecision->m_fLng;
        fGeoY = pControlDianPrecision->m_fLat;
        fZVal = pControlDianPrecision->m_fHeight;

        pChunk = new CChunk();
        pChunk->m_pBelongLayer = pLayerGeo;
        pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;
        pChunk->m_vecChunkPoints.push_back(glm::dvec3(fGeoX, fGeoY, fZVal));

        pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(i));

        pChunk->m_chunkBound = glm::dvec4(fGeoX, fGeoY, fGeoX, fGeoY);
        pLayerGeo->AddChunk(pChunk);

        if(fEnvelopeMinLng > fGeoX){
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY){
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }

        CChunkAttributeInfo chunkAttrVal;
        chunkAttrVal.SetAttributeValue((char*)pControlDianPrecision->m_sDianHao.c_str());
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((unsigned long long)pControlDianPrecision->m_nDianHao);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fGeoX);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fGeoY);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fZVal);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_fResidual_X);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_fResidual_Y);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_fResidual_Plane);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_fResidual_Height);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_fAngle);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((void*)pControlDianPrecision);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        if(pControlDianPrecision->m_pControlDianXiangFangWuCha){
            chunkAttrVal.SetAttributeValue((char*)pControlDianPrecision->m_pControlDianXiangFangWuCha->m_sTifName.c_str());
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_pControlDianXiangFangWuCha->m_fPix_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_pControlDianXiangFangWuCha->m_fPix_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_pControlDianXiangFangWuCha->m_fPix_Residual_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_pControlDianXiangFangWuCha->m_fPix_Residual_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pControlDianPrecision->m_pControlDianXiangFangWuCha->m_fWeight);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);
        }
    }

    pLayerGeo->m_fMinLng = fEnvelopeMinLng;
    pLayerGeo->m_fMinLat = fEnvelopeMinLat;
    pLayerGeo->m_fMaxLng = fEnvelopeMaxLng;
    pLayerGeo->m_fMaxLat = fEnvelopeMaxLat;

    pLayerGeo->m_pExtData = (void*)pResidualInfo;
    pLayerGeo->MakeUpChunks();
    vecLayerResidual.push_back(pLayerGeo);

    pLayerGeo->m_sLayerName = "Residual_Control_Points";
    this->m_vecLayerItem.push_back(pLayerGeo);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pLayerGeo, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pLayerGeo->m_pLayerVector = this;

    //////////////////////////////////////////////////////////////////
    // 检查点

    pLayerPoints = new CLayerPoints(this->m_pGLCore);
    pLayerGeo = pLayerPoints;
    pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePoint;
    pLayerGeo->m_nLayerColor = 0xFF80FF55;

    layerFld.m_sFieldName = "DianHao";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "DianHao_1";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long_long;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "JingDu";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "WeiDu";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "GaoCheng";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_X";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_Y";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_Plane";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Residual_Height";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Angle";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "BlockData";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_void_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);
    //>>>>>>>>>>>>>>

    layerFld.m_sFieldName = "TifFile";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixX";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixY";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResX";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixResY";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "PixWeit";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);
    //>>>>>>>>>>>>>>
    pLayerGeo->MakeupFields();

    CCheckDianPrecision* pCheckDianPrecision = nullptr;
    int nCheckDianCount = (int)residualInfo.m_CheckDianPrecisionBlock.m_vecCheckDianPrecision.size();
    for(i = 0; i<nCheckDianCount; i++) {
        pCheckDianPrecision = residualInfo.m_CheckDianPrecisionBlock.m_vecCheckDianPrecision[i];
        fGeoX = pCheckDianPrecision->m_fLng;
        fGeoY = pCheckDianPrecision->m_fLat;
        fZVal = pCheckDianPrecision->m_fHeight;

        pChunk = new CChunk();
        pChunk->m_pBelongLayer = pLayerGeo;
        pChunk->m_nChunkType = ELAYERTYPE::e_shapePoint;
        pChunk->m_vecChunkPoints.push_back(glm::dvec3(fGeoX, fGeoY, fZVal));

        pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(i));

        pChunk->m_chunkBound = glm::dvec4(fGeoX, fGeoY, fGeoX, fGeoY);
        pLayerGeo->AddChunk(pChunk);

        if(fEnvelopeMinLng > fGeoX){
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY){
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }

        CChunkAttributeInfo chunkAttrVal;
        chunkAttrVal.SetAttributeValue((char*)pCheckDianPrecision->m_sDianHao.c_str());
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((unsigned long long)pCheckDianPrecision->m_nDianHao);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fGeoX);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fGeoY);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(fZVal);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_fResidual_X);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_fResidual_Y);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_fResidual_Plane);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_fResidual_Height);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_fAngle);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((void*)pCheckDianPrecision);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        if(pCheckDianPrecision->m_pCheckDianXiangFangWuCha){
            chunkAttrVal.SetAttributeValue((char*)pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_sTifName.c_str());
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_fPix_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_fPix_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_fPix_Residual_X);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_fPix_Residual_Y);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

            chunkAttrVal.SetAttributeValue(pCheckDianPrecision->m_pCheckDianXiangFangWuCha->m_fWeight);
            pChunk->m_vecAttributeValues.push_back(chunkAttrVal);
        }

    }

    pLayerGeo->m_fMinLng = fEnvelopeMinLng;
    pLayerGeo->m_fMinLat = fEnvelopeMinLat;
    pLayerGeo->m_fMaxLng = fEnvelopeMaxLng;
    pLayerGeo->m_fMaxLat = fEnvelopeMaxLat;

    pLayerGeo->m_pExtData = (void*)pResidualInfo;
    pLayerGeo->MakeUpChunks();
    vecLayerResidual.push_back(pLayerGeo);

    pLayerGeo->m_sLayerName = "Residual_Check_Points";
    this->m_vecLayerItem.push_back(pLayerGeo);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pLayerGeo, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pLayerGeo->m_pLayerVector = this;
    //////////////////////////////////////////////////////////////////

    pLayerPolygons = new CLayerPolygons(this->m_pGLCore);
    pLayerGeo = pLayerPolygons;
    pLayerGeo->m_nLayerType = ELAYERTYPE::e_shapePolygon;
    pLayerGeo->m_nLayerColor = 0xFF00FFFF;

    layerFld.m_sFieldName = "TifFullPath";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Width";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long_long;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Height";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long_long;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "Resolution";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 10;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_double;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    layerFld.m_sFieldName = "BlockData";
    layerFld.m_nFieldWidth = 20;
    layerFld.m_nPrecision = 0;
    layerFld.m_nFieldType = ECHUNK_ATTRIBUTE_TYPE::e_void_ptr;
    pLayerGeo->m_vecLayerFields.push_back(layerFld);

    pLayerGeo->MakeupFields();

    CRpcAdjustFactor* pRpcAdjustFactor = nullptr;
    int nRpcAdjustFactorCount = (int)residualInfo.m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor.size();
    for(i = 0; i<nRpcAdjustFactorCount; i++) {
        pRpcAdjustFactor = residualInfo.m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor[i];

        pChunk = new CChunk();
        pChunk->m_pBelongLayer = pLayerGeo;
        pChunk->m_nChunkType = ELAYERTYPE::e_shapePolygon;

        fEnvelopeMinLng = 999999999.999999;
        fEnvelopeMinLat = 999999999.999999;
        fEnvelopeMaxLng = -999999999.999999;
        fEnvelopeMaxLat = -999999999.999999;

        for (int i0 = 0; i0 < 4; i0++) {
            fGeoX = pRpcAdjustFactor->m_arrQuadCoord[i0 * 2];
            fGeoY = pRpcAdjustFactor->m_arrQuadCoord[i0 * 2 + 1];
            fZVal = 0.0;

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

            pChunk->m_vecChunkPoints.push_back(glm::dvec3(fGeoX, fGeoY, fZVal));
        }
        // 闭合操作

        if(fEnvelopeMinLng > fGeoX){
            fEnvelopeMinLng = fGeoX;
        }
        if(fEnvelopeMinLat > fGeoY){
            fEnvelopeMinLat = fGeoY;
        }
        if(fEnvelopeMaxLng < fGeoX) {
            fEnvelopeMaxLng = fGeoX;
        }
        if(fEnvelopeMaxLat < fGeoY) {
            fEnvelopeMaxLat = fGeoY;
        }
        pChunk->m_vecAttributeValues.push_back(CChunkAttributeInfo(i));
        pChunk->m_chunkBound = glm::dvec4(fEnvelopeMinLng, fEnvelopeMinLat, fEnvelopeMaxLng, fEnvelopeMaxLat);
        pLayerGeo->AddChunk(pChunk);

        CChunkAttributeInfo chunkAttrVal;
        chunkAttrVal.SetAttributeValue((char*)pRpcAdjustFactor->m_sTifFileFullPath.c_str());
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((long long)pRpcAdjustFactor->m_nPixWidth);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((long long)pRpcAdjustFactor->m_nPixHeight);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue(pRpcAdjustFactor->m_fPixResolution);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);

        chunkAttrVal.SetAttributeValue((void*)pRpcAdjustFactor);
        pChunk->m_vecAttributeValues.push_back(chunkAttrVal);
    }
    pLayerGeo->m_pExtData = (void*)pResidualInfo;
    pLayerGeo->MakeUpChunks();
    vecLayerResidual.push_back(pLayerGeo);

    pLayerGeo->m_sLayerName = "Tif_Quad_Geo";
    this->m_vecLayerItem.push_back(pLayerGeo);
    this->m_mapLayerItem2Indx.insert(std::pair<CLayerDraw*, int>(pLayerGeo, this->m_nVecLayerItemCnt));
    this->m_nVecLayerItemCnt = (int)this->m_vecLayerItem.size();
    pLayerGeo->m_pLayerVector = this;

    //////////////////////////////////////////////////////////////////

    //residualInfo.Clear();

    return vecLayerResidual;
}

int CLayerVector::DeleteGeometryBySelectRange(){
    int i = 0;
    CLayerDraw* pSxLayerDraw = nullptr;
    time_t ss = clock();
    for(i=0; i<this->m_nVecLayerItemCnt; i++) {
        pSxLayerDraw = this->m_vecLayerItem[i];
        if(pSxLayerDraw == nullptr){
            continue;
        }
        if(pSxLayerDraw->m_nShowOrHide == ESHOWORHIDE::e_hide){
            continue;
        }
        if (pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePoint ||
            pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolyline ||
            pSxLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolygon)
        {
            pSxLayerDraw->DeleteGeometryBySelectRange();
        }

    }
    printf("SxLayerVector::DeleteGeometryBySelectRange() time: %lld\n", clock() - ss);

    this->m_pGLCore->UpdateWidgets();
    return 1;
}

int CLayerVector::UpdateGeoRanderBySelectRange(){

    time_t ss = clock();
    CLayerDraw* pSxLayerDraw = nullptr;
    int i = 0;
    this->m_nOperateEleCnt = 0;
    for(i=0; i<this->m_nVecLayerItemCnt; i++){
        pSxLayerDraw = this->m_vecLayerItem[i];
        if(pSxLayerDraw->m_nShowOrHide == ESHOWORHIDE::e_hide){
            continue;
        }
        pSxLayerDraw->UpdateGeoRanderBySelectRange();
        this->m_nOperateEleCnt = this->m_nOperateEleCnt + pSxLayerDraw->m_nOperateEleCnt;
    }
    printf("SxLayerVector::UpdateGeoRanderBySelectRange() :%lld\n", clock() - ss);

    CChunkAttribute* pChunkAttribute = GetGlobalPtr()->m_pChunkAttribute;
    if(pChunkAttribute) {
        pChunkAttribute->FreshViewByChunks();
    }

    return (int)this->m_nOperateEleCnt;
}

int CLayerVector::MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){
    //time_t ss = clock();

    if(this->m_pGeoLayerEdit){
        int nSnapType = this->m_pGeoLayerEdit->MouseMoving(fGeoMouseX, fGeoMouseY, arrMousePointBound);
        this->m_pGLCore->UpdateCursorType(nSnapType);
    }
    return 1;
    CLayerDraw* pSxLayerDraw = nullptr;
    int i = 0;

    int nSnapType = 0;
    for(i=0; i<this->m_nVecLayerItemCnt; i++){
        pSxLayerDraw = this->m_vecLayerItem[i];
        nSnapType = pSxLayerDraw->MouseMoving(fGeoMouseX, fGeoMouseY, arrMousePointBound);
        if (0 != nSnapType){
            break;
        }
    }
    //printf("SxLayerVector::MouseMoving() :%lld\n", clock() - ss);
    this->m_pGLCore->UpdateCursorType(nSnapType);
    return 1;
}

int CLayerVector::MouseRelease(int nBtnType, double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){

    if(this->m_pGeoLayerEdit){
        int nSnapType = this->m_pGeoLayerEdit->MouseMoving(fGeoMouseX, fGeoMouseY, arrMousePointBound);
        this->m_pGLCore->UpdateCursorType(nSnapType);
    }
    return 1;
}
void CLayerVector::BeingEditor(CChunk* pChunk){
    if(this->m_pGeoLayerEdit == nullptr){
        this->m_pGeoLayerEdit = new CLayerPolygonsEdit(this->m_pGLCore);
    }
    this->m_pGeoLayerEdit->m_nBeingEditor = this->m_nBeingEditor;
    if(this->m_nBeingEditor == 0){
        return ;
    }
    this->m_pGeoLayerEdit->RemoveAllChunks(false);
    this->m_pGeoLayerEdit->AddChunk(pChunk);    
    this->m_pGeoLayerEdit->MakeUpChunks();
}

void CLayerVector::DrawTest() {
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
