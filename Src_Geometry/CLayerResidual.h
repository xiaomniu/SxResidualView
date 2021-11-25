#ifndef CLAYERRESIDUAL_H
#define CLAYERRESIDUAL_H

#include <string>
#include <vector>
#include <map>
#include "Src_Geometry/CLayerLines.h"

class CLayerResidual : public CLayerLines
{
public:
    CLayerResidual();
    CLayerResidual(COpenGLCore* pGLCore);
    CLayerResidual(COpenGLCore* pGLCore, const std::string& sRasterFileFullPath);
    ~CLayerResidual();

    std::vector<glm::dvec3> m_vecArrowVertics;
    std::vector<float> m_vecArrowVerticsFlag;
    std::vector<unsigned int> m_vecArrowVerticsIndx;

    bool m_bShowArrow = true;
    void ShowArrow(bool bShowArrow){
        this->m_bShowArrow = bShowArrow;
    }

    int InitChunksByResidualInfo(int nLayerType); //  nLayerType: 1-lianjiedian,2-controldian,3-checkdian
    int AddChunk(double fPt0_X, double fPt0_Y, double fPt1_X, double fPt_Y);

    virtual void Init();
    virtual void Draw();
    virtual int MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){
        return  1;
    }
    virtual int AddChunk(CChunk* pChunk);

    virtual int AddChunk(int nPointCount, double* pPoints);
};

#endif // SXPOINTDRAW_H
