#ifndef SXBASEMAP_H
#define SXBASEMAP_H

#include <string>
#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Src_GlWnd/COpenGLCore.h"

class COpenGLCore;
class CBaseMap
{
public:
    CBaseMap(COpenGLCore* pGLCore);
    ~CBaseMap();
    COpenGLCore* m_pGLCore = nullptr;

    void Init();
    void Draw();
    void DrawTest();

    int GetCurrLevel();
    void GetNewMapTiles(double fLngMin, double fLatMinx, double fLngMax, double fLatMax);

    std::string FormatImgFileFullPath(int nLvlIndx, int  nDirIdx, int nImgIdx);

    int m_nCurrLevel = 0;

    int m_nMaxLevelIndx = 8;
    double m_pCurrLevelScale[10];
    double m_pLevelGeoRange[10];
    std::string m_sMapTileDir, m_sImgFileFullPath;

    int m_nColStart, m_nRowStart, m_nColEnd, m_nRowEnd;
    double m_fLngStart = 0.0, m_fLatStart = 0.0, m_fLngEnd = 0.0, m_fLatEnd = 0.0;
    float m_fBlockViewRangeH, m_fBlockViewRangeV;
    float m_fBlock0ViewStartX, m_fBlock0ViewStartY;

    std::map<std::string, GLuint> m_mapImgName2TextrueID;
    std::vector<GLuint> m_vecMapTextureID;
    std::vector<std::string> m_vecTextureFileName;
};

#endif // CDRAWGROUND_H
