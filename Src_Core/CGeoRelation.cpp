#include "CGeoRelation.h"

//#include "geos/geos_c.h"
//#include "geos/geos/geom.h"

#include "gdal/gdal_priv.h"
#include "gdal/ogrsf_frmts.h"

CGeoRelation::CGeoRelation() {
    this->m_pSelfPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
    this->m_pSelfTifLinearRing = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
}
CGeoRelation::~CGeoRelation() {

    if(this->m_pSelfPolygon) {
        OGRGeometryFactory::destroyGeometry(this->m_pSelfPolygon);
    }
    if(this->m_pSelfTifLinearRing) {
        this->m_pSelfTifLinearRing->setNumPoints(0);
        OGRGeometryFactory::destroyGeometry(this->m_pSelfTifLinearRing);
    }
}

void CGeoRelation::CreateGeoPolygon(int_32 nPointCnt, double* pPoints){
    if(nPointCnt <=0 || pPoints == nullptr){
        return;
    }
    this->m_pSelfTifLinearRing->setNumPoints(0);
    int_32 i = 0;
    for (i=0; i<nPointCnt; i++) {
        this->m_pSelfTifLinearRing->addPoint(pPoints[i*2], pPoints[i*2 + 1]);
    }
    this->m_pSelfTifLinearRing->closeRings();

    this->m_pSelfPolygon->addRing(this->m_pSelfTifLinearRing);
}

int_32 CGeoRelation::UnionGeoPolygon(int_32 nPointCnt, double* pPoints){
    if(nPointCnt <=0 || pPoints == nullptr) {
        return 0;
    }
    OGRPolygon* pOtherPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
    if(pOtherPolygon == nullptr){
        return 0;
    }
    this->m_pSelfTifLinearRing->setNumPoints(0);
    int_32 i = 0;
    for (i=0; i<nPointCnt; i++) {
        this->m_pSelfTifLinearRing->addPoint(pPoints[i*2], pPoints[i*2 + 1]);
    }
    this->m_pSelfTifLinearRing->closeRings();
    pOtherPolygon->addRing(this->m_pSelfTifLinearRing);

    OGRGeometry* pGeoRet = this->m_pSelfPolygon->Union(pOtherPolygon);
    OGRGeometryFactory::destroyGeometry(pOtherPolygon);
    if(pGeoRet == nullptr) {
        return 0;
    }
    OGRGeometryFactory::destroyGeometry(this->m_pSelfPolygon);
    this->m_pSelfPolygon = (OGRPolygon*)pGeoRet;
    return 1;
}


int_32 CGeoRelation::IntersectGeoPolygon(int_32 nPointCnt, double* pPoints){
    if(nPointCnt <=0 || pPoints == nullptr) {
        return 0;
    }
    OGRPolygon* pOtherPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
    if(pOtherPolygon == nullptr){
        return 0;
    }
    this->m_pSelfTifLinearRing->setNumPoints(0);
    int_32 i = 0;
    for (i=0; i<nPointCnt; i++) {
        this->m_pSelfTifLinearRing->addPoint(pPoints[i*2], pPoints[i*2 + 1]);
    }
    this->m_pSelfTifLinearRing->closeRings();
    pOtherPolygon->addRing(this->m_pSelfTifLinearRing);

    OGRGeometry* pGeoRet = this->m_pSelfPolygon->Intersection(pOtherPolygon);
    OGRGeometryFactory::destroyGeometry(pOtherPolygon);
    if(pGeoRet == nullptr) {
        return 0;
    }
    OGRGeometryFactory::destroyGeometry(this->m_pSelfPolygon);
    this->m_pSelfPolygon = (OGRPolygon*)pGeoRet;
    return 1;
}
OGRGeometry* CGeoRelation::IsContains(int_32 nPointCnt, double* pPoints){
    if(nPointCnt <=0 || pPoints == nullptr) {
        return 0;
    }
    OGRPolygon* pOtherPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
    if(pOtherPolygon == nullptr){
        return 0;
    }
    this->m_pSelfTifLinearRing->setNumPoints(0);
    int_32 i = 0;
    for (i=0; i<nPointCnt; i++) {
        this->m_pSelfTifLinearRing->addPoint(pPoints[i*2], pPoints[i*2 + 1]);
    }
    this->m_pSelfTifLinearRing->closeRings();
    pOtherPolygon->addRing(this->m_pSelfTifLinearRing);

    this->m_pSelfPolygon->Contains(pOtherPolygon);
    return (OGRGeometry*)pOtherPolygon;
}

int_32 CGeoRelation::UnionGeoPolygon(CGeoRelation* pGeometry){
    OGRGeometry* pNewPolygon = this->m_pSelfPolygon->Union(pGeometry->m_pSelfPolygon);
    if(pNewPolygon == nullptr){
        return 0;
    }
    OGRGeometryFactory::destroyGeometry(this->m_pSelfPolygon);
    this->m_pSelfPolygon = (OGRPolygon*)pNewPolygon;
    return 1;
}

int_32 CGeoRelation::IsContains(CGeoRelation* pGeometry){

    return (int_32)(this->m_pSelfPolygon->Contains(pGeometry->m_pSelfPolygon));
    //return 1;
}

int_32 CGeoRelation::IsOverlaps(CGeoRelation* pGeometry){
    return (int_32)(this->m_pSelfPolygon->Overlaps(pGeometry->m_pSelfPolygon));
}


int_32 CGeoRelation::SubtractGeoPolygon(int_32 nPointCnt, double* pPoints){
    if(nPointCnt <=0 || pPoints == nullptr) {
        return 0;
    }
    OGRPolygon* pOtherPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
    if(pOtherPolygon == nullptr){
        return 0;
    }
    this->m_pSelfTifLinearRing->setNumPoints(0);
    int_32 i = 0;
    for (i=0; i<nPointCnt; i++) {
        this->m_pSelfTifLinearRing->addPoint(pPoints[i*2], pPoints[i*2 + 1]);
    }
    this->m_pSelfTifLinearRing->closeRings();
    pOtherPolygon->addRing(this->m_pSelfTifLinearRing);

    OGRGeometry* pNewPolygon = nullptr;
    if(this->m_pSelfPolygon->Contains(pOtherPolygon) || this->m_pSelfPolygon->Overlaps(pOtherPolygon)){
        //pNewPolygon = this->m_pSelfPolygon->Intersection(pOtherPolygon);
        //if(pNewPolygon){
        //    //printf("pNewPolygon:intersection:area:%f\n", ((OGRPolygon*)pNewPolygon)->get_Area());
        //    if(((OGRPolygon*)pNewPolygon)->get_Area() <= 0.000001){
        //
        //        OGRGeometryFactory::destroyGeometry(pNewPolygon);
        //        OGRGeometryFactory::destroyGeometry(pOtherPolygon);
        //        return 0;
        //    }
        //    OGRGeometryFactory::destroyGeometry(pNewPolygon);
        //
        //}
        pNewPolygon = this->m_pSelfPolygon->Difference(pOtherPolygon);
    }
    OGRGeometryFactory::destroyGeometry(pOtherPolygon);
    if(pNewPolygon == nullptr){
        return 0;
    }
    OGRGeometryFactory::destroyGeometry(this->m_pSelfPolygon);
    this->m_pSelfPolygon = (OGRPolygon*)pNewPolygon;
    return 1;
}

double CGeoRelation::GetArea() {
    OGRLinearRing* pLinear = (OGRLinearRing*)this->m_pSelfPolygon->getExteriorRing();//getLinearGeometry();
    printf("%d\n", pLinear->getNumPoints());
    return this->m_pSelfPolygon->get_Area();
}

int_32 CGeoRelation::GetPoints(std::vector<std::vector<glm::dvec2>>& vecPoints) {
    OGRwkbGeometryType eDataType = this->m_pSelfPolygon->getGeometryType();
    int_32 i = 0, j = 0;
    double fGeoX, fGeoY;
    int_32 nPointCnt = 0;
    int_32 nPointCntAll = 0;
    OGRLinearRing* pLinearRing = nullptr;
    switch (eDataType) {
    case wkbPolygon:{
        pLinearRing = this->m_pSelfPolygon->getExteriorRing();
        if(pLinearRing == nullptr){
            return 0;
        }
        nPointCnt = pLinearRing->getNumPoints();
        if(nPointCnt < 3){
            return 0;
        }
        std::vector<glm::dvec2> vecPointsOnePolygon;
        for (i = 0; i < nPointCnt; i++) {
            fGeoX = pLinearRing->getX(i);
            fGeoY = pLinearRing->getY(i);
            vecPointsOnePolygon.push_back(glm::dvec2(fGeoX, fGeoY));
        }
        nPointCntAll = nPointCnt;
        vecPoints.push_back(vecPointsOnePolygon);
        break;
    }
    case wkbMultiPolygon:{
        OGRPolygon* pOnePolygon = nullptr;
        OGRMultiPolygon* pMulPolygon = (OGRMultiPolygon*)this->m_pSelfPolygon;
        for (i = 0; i < pMulPolygon->getNumGeometries(); i++) {
            pOnePolygon = (OGRPolygon*)(pMulPolygon->getGeometryRef(i));
            pLinearRing = (OGRLinearRing*)((OGRPolygon*)pOnePolygon)->getExteriorRing();
            //printf("%d : type: %d, boundPtCnt : %d, Area: %g\n", i, pTifPoly->getGeometryType(), pTifLinearRing->getNumPoints(), ((OGRPolygon*)pPolygon1)->get_Area());
            nPointCnt = pLinearRing->getNumPoints();
            if(nPointCnt < 3){
                continue;
            }
            std::vector<glm::dvec2> vecPointsOnePolygon;
            for (j = 0; j < nPointCnt; j++) {
                fGeoX = pLinearRing->getX(j);
                fGeoY = pLinearRing->getY(j);
                vecPointsOnePolygon.push_back(glm::dvec2(fGeoX, fGeoY));
            }
            nPointCntAll += nPointCnt;
            vecPoints.push_back(vecPointsOnePolygon);
        }
        break;
    }
    case wkbGeometryCollection:{
        OGRwkbGeometryType eDataTypeSub = wkbUnknown;
        OGRGeometry* pGeo = nullptr;
        OGRPolygon* pOnePolygon = nullptr;
        OGRGeometryCollection* pMulPolygon = (OGRGeometryCollection*)this->m_pSelfPolygon;
        int_32 nGeoIndx = 0, nGeoColectCnt = pMulPolygon->getNumGeometries();
        for (nGeoIndx = 0; nGeoIndx < nGeoColectCnt; nGeoIndx++) {
            pGeo = pMulPolygon->getGeometryRef(nGeoIndx);
            if( pGeo == nullptr)
                continue;

            eDataTypeSub = pGeo->getGeometryType();
            if(wkbLineString == eDataTypeSub) {
                pLinearRing = (OGRLinearRing*)pGeo;
                nPointCnt = pLinearRing->getNumPoints();
                if(nPointCnt < 3){
                    continue;
                }
                std::vector<glm::dvec2> vecPointsOnePolygon;
                for (i = 0; i < nPointCnt; i++) {
                    fGeoX = pLinearRing->getX(i);
                    fGeoY = pLinearRing->getY(i);
                    vecPointsOnePolygon.push_back(glm::dvec2(fGeoX, fGeoY));
                }
                nPointCntAll = nPointCnt;
                vecPoints.push_back(vecPointsOnePolygon);
            }
            else if(wkbPolygon == eDataTypeSub){
                pOnePolygon = (OGRPolygon*)pGeo;
                pLinearRing = pOnePolygon->getExteriorRing();
                if(pLinearRing == nullptr){
                    continue;
                }
                nPointCnt = pLinearRing->getNumPoints();
                if(nPointCnt < 3){
                    continue;
                }
                std::vector<glm::dvec2> vecPointsOnePolygon;
                for (i = 0; i < nPointCnt; i++) {
                    fGeoX = pLinearRing->getX(i);
                    fGeoY = pLinearRing->getY(i);
                    vecPointsOnePolygon.push_back(glm::dvec2(fGeoX, fGeoY));
                }
                nPointCntAll = nPointCnt;
                vecPoints.push_back(vecPointsOnePolygon);
            } else if (wkbMultiPolygon ==  eDataTypeSub) {
                pMulPolygon = (OGRMultiPolygon*)pGeo;
                for (i = 0; i < pMulPolygon->getNumGeometries(); i++) {
                    pOnePolygon = (OGRPolygon*)(pMulPolygon->getGeometryRef(i));
                    pLinearRing = (OGRLinearRing*)((OGRPolygon*)pOnePolygon)->getExteriorRing();
                    //printf("%d : type: %d, boundPtCnt : %d, Area: %g\n", i, pTifPoly->getGeometryType(), pTifLinearRing->getNumPoints(), ((OGRPolygon*)pPolygon1)->get_Area());
                    nPointCnt = pLinearRing->getNumPoints();
                    if(nPointCnt < 3){
                        continue;
                    }
                    std::vector<glm::dvec2> vecPointsOnePolygon;
                    for (j = 0; j < nPointCnt; j++) {
                        fGeoX = pLinearRing->getX(j);
                        fGeoY = pLinearRing->getY(j);
                        vecPointsOnePolygon.push_back(glm::dvec2(fGeoX, fGeoY));
                    }
                    nPointCntAll += nPointCnt;
                    vecPoints.push_back(vecPointsOnePolygon);
                }
            }
        }
        break;
    }
    }
    return nPointCntAll;
}

void CGeoRelation::CreateGeometry(){


    this->m_pSelfPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
    this->m_pSelfTifLinearRing = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
    this->m_pSelfTifLinearRing->addPoint(20.0, 34.0);

    this->m_pSelfPolygon->addRing(this->m_pSelfTifLinearRing);
    this->m_pSelfTifLinearRing->setNumPoints(0);

//    printf("GEOS库版本为：%s\n", GEOS_VERSION);

//    typedef geos::geom::Coordinate PT;
//    geos::geom::GeometryFactory factory;
//    geos::geom::CoordinateArraySequenceFactory csf; //构建第一个矩形p1
//    geos::geom::CoordinateSequence* cs1 = csf.create(5, 2);//五个2维点，第三维度z始终为0
//    cs1->setAt(PT(0, 0), 0);
//    cs1->setAt(PT(3, 0), 1);
//    cs1->setAt(PT(3, 3), 2);
//    cs1->setAt(PT(0, 3), 3);
//    cs1->setAt(PT(0, 0), 4); //与第一个点相等，构成闭合
//    geos::geom::LinearRing* ring1 = factory.createLinearRing(cs1); //点构成线
//    geos::geom::Geometry* p1 = factory.createPolygon(ring1, NULL); //线构成面

//    geos::geom::CoordinateSequence* cs2 = csf.create(5, 2); //构建一个四边形p2
//    cs2->setAt(PT(2, 2), 0);
//    cs2->setAt(PT(4, 5), 1);
//    cs2->setAt(PT(5, 5), 2);
//    cs2->setAt(PT(5, 4), 3);
//    cs2->setAt(PT(2, 2), 4);
//    geos::geom::LinearRing* ring2 = factory.createLinearRing(cs2);
//    geos::geom::Geometry* p2 = (factory.createPolygon(ring2, NULL));

//    geos::geom::CoordinateSequence* cs3 = new CoordinateArraySequence(); //构建一个三角形p3
//    int_32 xoffset = 4, yoffset = 4, side = 2;
//    cs3->add(PT(xoffset, yoffset));
//    cs3->add(PT(xoffset, yoffset + side));
//    cs3->add(PT(xoffset + side, yoffset + side));
//    cs3->add(PT(xoffset, yoffset));
//    geos::geom::LinearRing* ring3 = factory.createLinearRing(cs3);
//    geos::geom::Geometry* p3 = (factory.createPolygon(ring3, NULL));
//    p1 = p1->Union(p2);
//    factory.destroyGeometry(p1);
//    bool flag12 = p1->intersects(p2);
//    bool flag13 = p1->intersects(p3);
//    bool flag23 = p2->intersects(p3);
}
