#ifndef SXLAYERPOLYGONS_H
#define SXLAYERPOLYGONS_H

#include <string>
#include <vector>
#include <map>
#include "Src_Geometry/CLayerGeoDraw.h"

class CLayerPolygons : public CLayerGeoDraw
{
public:
    CLayerPolygons();
    CLayerPolygons(COpenGLCore* pGLCore);
    CLayerPolygons(COpenGLCore* pGLCore, const std::string& sRasterFileFullPath);
    ~CLayerPolygons();

    virtual void Init();
    virtual void Draw();
    virtual int UpdateGeoRanderBySelectRange();
    virtual int DeleteGeometryBySelectRange();
    virtual int MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound);
    virtual int BeingEditor();
    virtual int GetChunkCount(){
        return (int)this->m_vecChunkDatas.size();
    }
    virtual int SaveEditor(){
        return 1;
    }
    virtual int AddChunk(CChunk* pChunk){
        this->m_vecChunkDatas.push_back(pChunk);
        return 1;
    }
    virtual void SetFresh(CChunk* pChunk);
    void SetFreshState(CChunk* pChunk, float fState);

    virtual int MakeUpChunks();
    int MakeUpChunksState();

    CChunk* GetChunkByIndx(int nChunkIndx);
    void DrawTest();

    CChunk* m_pEditChunk = nullptr;
};

#endif // SXPOINTDRAW_H
