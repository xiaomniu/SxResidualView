#ifndef SXLAYERPOLYGONSEDIT_H
#define SXLAYERPOLYGONSEDIT_H

#include <string>
#include <vector>
#include <map>
#include "Src_Geometry/CLayerGeoDraw.h"

class CLayerPolygonsEdit: public CLayerGeoDraw
{
public:
    CLayerPolygonsEdit();
    CLayerPolygonsEdit(COpenGLCore* pGLCore);
    ~CLayerPolygonsEdit();

    virtual void Init();
    virtual void Draw();
    virtual int UpdateGeoRanderBySelectRange();
    virtual int DeleteGeometryBySelectRange();
    virtual int MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound);
    virtual int MouseRelease(int nBtnType, double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound);

    int AddChunkPoints(int nPointCnt, double* pChunkPoints);

    virtual int SaveEditor(){
        return 1;
    }
    virtual int BeingEditor(){
        return 1;
    }
    virtual int GetChunkCount(){
        return (int)this->m_vecChunkDatas.size();
    }
    virtual void SetFresh(CChunk* ){

    }

    virtual int MakeUpChunks();
    int MakeUpChunksState();

    int AddPoint();
    int DelPoint();
    int MovPoint();
    void DrawTest();
};

#endif // SXPOINTDRAW_H
