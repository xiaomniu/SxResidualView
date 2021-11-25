#ifndef CLAYERPOINTS_H
#define CLAYERPOINTS_H

#include <string>
#include <vector>
#include <map>
#include "Src_Geometry/CLayerGeoDraw.h"

class CLayerPoints : public CLayerGeoDraw
{
public:
    CLayerPoints();
    CLayerPoints(COpenGLCore* pGLCore);
    CLayerPoints(COpenGLCore* pGLCore, const std::string& sRasterFileFullPath);
    ~CLayerPoints();
    int DeleteGeometryBySelectRange_bak();

    virtual void Init();
    virtual void Draw();
    virtual int UpdateGeoRanderBySelectRange();
    virtual int DeleteGeometryBySelectRange();
    virtual int MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){
        return  1;
    }
    virtual int SaveEditor(){
        return 1;
    }
    virtual int BeingEditor(){
        return 1;
    }
    virtual int AddChunk(CChunk* pChunk){
        this->m_vecChunkDatas.push_back(pChunk);
        return 1;
    }
    virtual int GetChunkCount(){
        return (int)this->m_vecChunkDatas.size();
    }
    virtual void SetFresh(CChunk* pChunk);
    void SetFreshState(CChunk* pChunk, float fState);

    virtual int MakeUpChunks();
    int MakeUpChunksState();

    int ConvertSelectedChunksToOtherLayer(CLayerGeoDraw* pOtherLayer);

    void DrawTest();
};

#endif // SXPOINTDRAW_H
