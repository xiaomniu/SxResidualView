#ifndef SXGEORELATION_H
#define SXGEORELATION_H

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Src_Core/DefType.h"

#include <vector>
#include <string>

class OGRGeometry;
class OGRPolygon;
class OGRLinearRing;
class CGeoRelation {
public:
    CGeoRelation();
    ~CGeoRelation();

    void CreateGeometry();
    //static
    OGRPolygon* m_pSelfPolygon = nullptr;
    OGRLinearRing* m_pSelfTifLinearRing = nullptr;
    void CreateGeoPolygon(int_32 nPointCnt, double* pPoints);
    int_32 UnionGeoPolygon(int_32 nPointCnt, double* pPoints);
    int_32 IntersectGeoPolygon(int_32 nPointCnt, double* pPoints);
    OGRGeometry* IsContains(int_32 nPointCnt, double* pPoints);
    int_32 UnionGeoPolygon(CGeoRelation* pGeometry);
    int_32 IsContains(CGeoRelation* pGeometry);
    int_32 IsOverlaps(CGeoRelation* pGeometry);


    int_32 SubtractGeoPolygon(int_32 nPointCnt, double* pPoints);
    double GetArea();

    int_32 GetPoints(std::vector<std::vector<glm::dvec2>>& vecPoints);
};

#endif // !1
