#ifndef SXRASTERDRAW_H
#define SXRASTERDRAW_H

#include <string>
#include <vector>
#include <map>

#include "Src_Geometry/CLayerDraw.h"
//#include "Src_Geometry/SxRasterThumbnail.h"

class CReadRaster;
class CRasterThumbnail;
class CRasterDraw : public CLayerDraw
{
public:
    CRasterDraw();
    CRasterDraw(COpenGLCore* pGLCore);
    CRasterDraw(COpenGLCore* pGLCore, const std::string& sRasterFileFullPath);
    ~CRasterDraw();

    virtual void Init();
    virtual void Draw();
    virtual int UpdateGeoRanderBySelectRange();
    virtual int DeleteGeometryBySelectRange();
    virtual int MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){
        return  1;
    }
    static void PreDraw(void* pParam);
    void DrawTest();
    void DrawMosaicLines_bak(GLuint nTextureID);
    void DrawMosaicLines_bak1(GLuint nTextureID);
    void DrawMosaicLines(GLuint nTextureID);
    void CreateMosaicLines();

    void GetPixCoordByGeo(double fGeoLng, double fGeoLat, double* pPixX, double* pPixY);
    int GetScreenIntersectPoints(glm::dvec2& pt0, glm::dvec2& pt1,
        t_Line2D& tLineLeft, t_Line2D& tLineBottom, t_Line2D& tLineRight, t_Line2D& tLineTop, double* pIntersectRet);

    int m_nHasExistMosaicLine = 0;
    int m_nPreDrawDone = 1;
    GLuint m_nTextureID;
    unsigned char* m_pPixData = nullptr;
    std::string m_sRasterImgFileFullPath;
    double m_fViewLeftBottomX, m_fViewLeftBottomY, m_fViewShowLenX, m_fViewShowLenY;
    int m_nViewIntersectLenX = 0, m_nViewIntersectLenY = 0;

    double m_pIntersectBound[8];
    CChunk* m_pChunkMosaic = nullptr;
    std::vector<glm::dvec2> m_vecMosaicPoints;

    CReadRaster* m_pReadRaster = nullptr;
    CRasterThumbnail* m_pRasterThumbnail = nullptr;

    int m_nShowAlpha = ESHOWORHIDE::e_hide;
    int m_nShowNoData = ESHOWORHIDE::e_show;
};

#endif // SXRASTERDRAW_H
