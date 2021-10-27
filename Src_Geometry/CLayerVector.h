#ifndef SXLAYERRASTER_H
#define SXLAYERRASTER_H

#include <string>
#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

class CChunk;
class CLayerDraw;
class CRasterDraw;
class CLayerGeoDraw;
class COpenGLCore;
class CThreadPool;
class CResidualInfo;

class CLayerVector
{
public:
    CLayerVector();
    CLayerVector(COpenGLCore* pGLCore);
    ~CLayerVector();
    COpenGLCore* m_pGLCore = nullptr;

    void Init();
    void Draw();
    void Draw_Bak();
    void DrawTest();


    int RemoveOneLayer(CLayerDraw* pLayerDraw);
    int RemoveAllLayers();


    int IsContains(double* pSrcRect, double* pDstRect);
    int IsOverlaps(double* pSrcRect, double* pDstRect);



    int m_nExistRasterCnt = 0;
    CRasterDraw* AddRasterItem(const std::string& sTifFullPath);

    CLayerGeoDraw* ReadShapeFile(const std::string& sShpFileFullPath);
    CLayerGeoDraw* ReadShapeFile_bak(const std::string& sShpFileFullPath);
    CLayerGeoDraw* AddGeometryItem(const std::string& sVectorFullPath);

    CLayerGeoDraw* ReadPxyFile(const std::string& sPxyFileFullPath);
    CLayerGeoDraw* AddGeometryItemPxy(const std::string& sPxyFullPath);

    CLayerGeoDraw* ReadGcpFile(const std::string& sGcpFileFullPath);
    CLayerGeoDraw* AddGeometryItemGcp(const std::string& sGcpFullPath);

    CLayerGeoDraw* ReadResidualFile(const std::string& sResidualFileFullPath);
    std::vector<CLayerGeoDraw*> AddGeometryItemResidualInfo(const std::string& sResidualFileFullPath);

    int DeleteGeometryBySelectRange();

    int UpdateGeoRanderBySelectRange();
    int MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound);
    int MouseRelease(int nBtnType, double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound);

    int m_nLayerIndxRollerBind = -1;
    CLayerDraw* GetLayerIndxRollerBlind();
    void ClearLayerIndxRollerBlind();
    int UpdateLayerIndxRollerBlind(CLayerDraw* pLayerDraw, int nChecked);

    CThreadPool* m_pSxThreadPool = nullptr;
    unsigned long long m_nOperateEleCnt = 0;
    int m_nVecLayerItemCnt = 0;
    std::vector<CLayerDraw*> m_vecLayerItem;
    std::map<CLayerDraw*, int> m_mapLayerItem2Indx;

    int m_nBeingEditor = 0;
    CLayerGeoDraw* m_pGeoLayerEdit = nullptr;
    void BeingEditor(CChunk* pChunk);

    CLayerDraw* FindLayerByName(const std::string& sLayerName);
};

#endif // SXLAYERRASTER_H
