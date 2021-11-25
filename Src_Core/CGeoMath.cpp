#include "CGeoMath.h"

void CGeoMath::RotatePointByPoint(double fPt_X, double fPt_Y, double fPt_R_X, double fPt_R_Y, double fTheta, double* pOutX, double* pOutY, bool bNormal/* = false*/){
    // （fPt_X，fPt_Y） 绕点（fPt_R_X，fPt_R_Y）旋转fThreta度（fTheta是弧度）

    double fOutX = (fPt_X - fPt_R_X)*cos(fTheta) - (fPt_Y - fPt_R_Y)*sin(fTheta) + fPt_R_X;
    double fOutY = (fPt_X - fPt_R_X)*sin(fTheta) + (fPt_Y - fPt_R_Y)*cos(fTheta) + fPt_R_Y;
    if(bNormal == true){
        double fLen = sqrt(fOutX * fOutX + fOutY * fOutY);
        fOutX = fOutX / fLen;
        fOutY = fOutY / fLen;
    }
    *pOutX = fOutX;
    *pOutY = fOutY;
}
