#ifndef CGEOMATH_H
#define CGEOMATH_H

#include <vector>
#include <string>

class CGeoMath {
public:
    CGeoMath();
    ~CGeoMath();
    static void RotatePointByPoint(double fPt_X, double fPt_Y, double fPt_R_X, double fPt_R_Y, double fTheta, double* pOutX, double* pOutY, bool bNormal = false);
};

#endif // !1
