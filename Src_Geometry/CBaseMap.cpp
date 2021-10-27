#include "CBaseMap.h"

#ifdef _DEBUG
    void CheckGLCoreError(const char *file, int line, const char *func, COpenGLCore* pCore);
#   ifdef __FUNCTION__
#       define CHECK_GL_ERROR_1(a) CheckGLCoreError(__FILE__, __LINE__, __FUNCTION__, a)
#   else
#       define CHECK_GL_ERROR_1(a) CheckGLCoreError(__FILE__, __LINE__, "", a)
#   endif
#else
#   define CHECK_GL_ERROR_1(a)// ((void)(0))
#endif

//#include <QProcess>
CBaseMap::CBaseMap(COpenGLCore* pGLCore)
{
    this->m_pGLCore = pGLCore;
}

CBaseMap::~CBaseMap()
{
}

std::string CBaseMap::FormatImgFileFullPath(int nLvlIndx, int  nDirIdx, int nImgIdx) {
    std::string sImgFileFullPath = this->m_sMapTileDir+std::to_string(nLvlIndx)+"/"+std::to_string(nDirIdx)+"/"+std::to_string(nImgIdx)+".jpg";
    return sImgFileFullPath;
}

int CBaseMap::GetCurrLevel(){
    int i = 0;
    printf("SxBaseMap::GetCurrLevel\n");
    for(i = this->m_nMaxLevelIndx - 1; i >= 0; i--) {
        //printf("%d %f %f\n", i, this->m_pCurrLevelScale[i], this->m_pGLCore->m_fView2GeoScaleH);
        if(this->m_pCurrLevelScale[i] <= this->m_pGLCore->m_fView2GeoScaleH){
            this->m_nCurrLevel = i;
            return i;
        }
    }
    this->m_nCurrLevel = 0;
    return 0;
}

void CBaseMap::Init(){

    this->m_sMapTileDir = this->m_pGLCore->m_sExeDir+"/tiles/";
    //
    //this->m_sMapTileDir = "E:/YNDW/AdjustPoint_YunNan0323/aaa/tiles/";
    this->m_sMapTileDir = "D:/OpenCV/opencv-4.0.1/build/x64/Release/tiles/";
    printf("this->m_sMapTileDir  : %s\n", this->m_sMapTileDir.c_str());
    this->m_nCurrLevel = 0;

    double fBlockGeoRangeH = 90.0;
    //double fCurrGeoRangeV = 180.0;
    double fCurrLevelScale = 1.0;
    //this->m_pCurrLevelScale = new double[this->m_nMaxLevelIndx];
    //this->m_pLevelGeoRange =  new double[this->m_nMaxLevelIndx];
    int i = 1;
    for(i = 1; i<=this->m_nMaxLevelIndx; i++) {
        fCurrLevelScale = 256.0 / fBlockGeoRangeH;
        this->m_pLevelGeoRange[i - 1] = fBlockGeoRangeH;
        this->m_pCurrLevelScale[i - 1] = fCurrLevelScale;
        fBlockGeoRangeH = fBlockGeoRangeH / 2.0;
    }

    //int nLvlIndx = this->m_nCurrLevel, nDirIdx = 0, nImgIdx = 1;
    //std::string sImgFileFullPath = this->FormatImgFileFullPath(nLvlIndx, nDirIdx, nImgIdx);
    //GLuint nMapTextureID = this->m_pGLCore->LoadTextureFromFile(sImgFileFullPath.c_str());
    //
    //CHECK_GL_ERROR_1(this->m_pGLCore);
    //this->m_sImgFileFullPath = sImgFileFullPath;
    //this->m_vecMapTextureID.push_back(nMapTextureID);
    //this->m_mapImgName2TextrueID.insert(std::pair<std::string, GLuint>(sImgFileFullPath, nMapTextureID));
}

void CBaseMap::GetNewMapTiles(double fLngMin, double fLatMin, double fLngMax, double fLatMax) {
    printf("SxBaseMap::GetNewMapTiles\n");
    double fLeftStart = fLngMin - (-180.0);
    double fRightEnd = fLngMax - (-180.0);
    double fBottomStart = fLatMin - (-90.0);
    double fTopEnd = fLatMax - (-90.0);

    double fLevelGeoRange = this->m_pLevelGeoRange[this->m_nCurrLevel];

    int n2Mi = (int)pow(2.0, (double)this->m_nCurrLevel);
    int nMaxCol = n2Mi * 4;
    int nMaxRow = n2Mi * 2;

    int nColStart = (int)(fLeftStart / fLevelGeoRange);
    nColStart = nColStart > 0 ? nColStart : 0;

    int nRowStart = (int)(fBottomStart / fLevelGeoRange);
    nRowStart = nRowStart > 0 ? nRowStart : 0;

    int nColEnd = (int)(fRightEnd / fLevelGeoRange) + 1;
    nColEnd = nColEnd > nMaxCol ? nMaxCol : nColEnd;

    int nRowEnd = (int)(fTopEnd / fLevelGeoRange) + 1;
    nRowEnd = nRowEnd > nMaxRow ? nMaxRow : nRowEnd;

    this->m_vecMapTextureID.clear();
    this->m_vecTextureFileName.clear();
    int i = 0, j = 0, nIdIndx = 0;
    GLuint nMapTextureID = -1;
    std::string sImgFileFullPath;
    for(i = nColStart; i<nColEnd; i++){
        for(j = nRowStart; j<nRowEnd; j++){
            sImgFileFullPath = this->FormatImgFileFullPath(this->m_nCurrLevel+1, i, j);
            nMapTextureID = this->m_pGLCore->LoadTextureFromFile(sImgFileFullPath.c_str());

            //printf("%d %s %d\n", nIdIndx++, sImgFileFullPath.c_str(), nMapTextureID);
            //CHECK_GL_ERROR_1(this->m_pGLCore);
            this->m_sImgFileFullPath = sImgFileFullPath;
            //printf("%d %s %d\n", nIdIndx++, sImgFileFullPath.c_str(), nMapTextureID);
            this->m_vecMapTextureID.push_back(nMapTextureID);
            //printf("%d %s %d\n", nIdIndx++, sImgFileFullPath.c_str(), nMapTextureID);
            this->m_vecTextureFileName.push_back(sImgFileFullPath.substr(sImgFileFullPath.rfind(47)+1));
            //printf("%d %s %d\n", nIdIndx++, sImgFileFullPath.c_str(), nMapTextureID);
            //if(this->m_mapImgName2TextrueID.find(sImgFileFullPath) == this->m_mapImgName2TextrueID.end()){
            //    this->m_mapImgName2TextrueID.insert(std::pair<std::string, GLuint>(sImgFileFullPath, nMapTextureID));
            //}

        }
    }
    //printf("%d %s %d\n", nIdIndx++, sImgFileFullPath.c_str(), nMapTextureID);

    double fColCnt = (double)(nColEnd - nColStart);
    double fRowCnt = (double)(nRowEnd - nRowStart);
    this->m_nColStart = nColStart;
    this->m_nRowStart = nRowStart;
    this->m_nColEnd = nColEnd;
    this->m_nRowEnd = nRowEnd;
    this->m_fLngStart = ((double)nColStart) * fLevelGeoRange + (-180.0);
    this->m_fLatStart = ((double)nRowStart) * fLevelGeoRange + (-90.0);
    this->m_fLngEnd = this->m_fLngStart + fLevelGeoRange * fColCnt;
    this->m_fLatEnd = this->m_fLatStart + fLevelGeoRange * fRowCnt;

    double nViewRangeH = 0, nViewRangeV = 0;
    double nViewRangeH1 = 0, nViewRangeV1 = 0;
    this->m_pGLCore->Geo2ViewPosition(this->m_fLngStart, this->m_fLatStart, &nViewRangeH, &nViewRangeV);
    this->m_pGLCore->Geo2ViewPosition(this->m_fLngEnd, this->m_fLatEnd, &nViewRangeH1, &nViewRangeV1);

    this->m_fBlockViewRangeH = (float)(nViewRangeH1 - nViewRangeH);
    this->m_fBlockViewRangeV = (float)(nViewRangeV1 - nViewRangeV);
    this->m_fBlockViewRangeH = (float)((this->m_fBlockViewRangeH / (float)(fColCnt)));
    this->m_fBlockViewRangeV = (float)((this->m_fBlockViewRangeV / (float)(fRowCnt)));

    this->m_fBlock0ViewStartX = (float)(nViewRangeH);
    this->m_fBlock0ViewStartY = (float)(nViewRangeV);
}

void CBaseMap::Draw() {
    if(this->m_pGLCore->m_nNeedUpdate == 0){
        //this->Init();

        this->GetCurrLevel();

        this->GetNewMapTiles(
                    this->m_pGLCore->t_geo_minmax.fGeoLeftBottomX,
                    this->m_pGLCore->t_geo_minmax.fGeoLeftBottomY,
                    this->m_pGLCore->t_geo_minmax.fGeoRightTopX,
                    this->m_pGLCore->t_geo_minmax.fGeoRightTopY);
    }

    int i = 0, j = 0, nIdIndx = 0;
    GLuint nMapTextureID = -1;
    float xPos1,yPos1, xPos2, yPos2;
    int ii = 0, jj = 0;
    xPos2 = this->m_fBlockViewRangeH;
    yPos2 = this->m_fBlockViewRangeV;
    //xPos1 = this->m_fBlock0ViewStartX - this->m_fBlockViewRangeH;
    for(i = this->m_nColStart; i<this->m_nColEnd; i++, ii++){
        //xPos1 = xPos1 + this->m_fBlockViewRangeH;
        xPos1 = this->m_fBlock0ViewStartX + this->m_fBlockViewRangeH * ii;
        //yPos1 = this->m_fBlock0ViewStartY - this->m_fBlockViewRangeV;
        for(j = this->m_nRowStart, jj = 0; j<this->m_nRowEnd; j++, jj++){
            nMapTextureID = this->m_vecMapTextureID[nIdIndx];
            //yPos1 = yPos1 + this->m_fBlockViewRangeV;
            yPos1 = this->m_fBlock0ViewStartY + this->m_fBlockViewRangeV * jj;
            //printf("Draw nIdIndx = 1 : %f %f %f %f\n", nIdIndx, xPos1,yPos1, xPos2, yPos2);
            this->m_pGLCore->DrawImage(
                        xPos1,
                        yPos1,
                        this->m_fBlockViewRangeH, this->m_fBlockViewRangeV, nMapTextureID);
            this->m_pGLCore->DrawString(this->m_vecTextureFileName[nIdIndx].c_str(),
                                        xPos1,
                                        yPos1,
                                        0.5f, 0xFF0000FF);
            nIdIndx++;
        }
    }
    printf("SxBaseMap::Draw\n");

    //this->m_pGLCore->DrawImage(10, 310, 256, 256, "D:/OpenCV/opencv-4.0.1/build/x64/Release/tiles/8/454/178.jpg");
    //this->m_pGLCore->DrawImage(10 + 256, 310, 256, 256, "D:/OpenCV/opencv-4.0.1/build/x64/Release/tiles/8/455/178.jpg");
    //this->m_pGLCore->DrawImage(10, 310 - 256-1, 256, 256, "D:/OpenCV/opencv-4.0.1/build/x64/Release/tiles/8/454/177.jpg");
}

void CBaseMap::DrawTest() {
    //this->m_pGLCore->DrawImage(100, 9100, 200, 200, this->m_sImgFileFullPath.c_str());
    //this->m_pGLCore->DrawImage(210, 210, 200, 200, this->m_sImgFileFullPath.c_str());

    std::vector<glm::vec2> vecPoints;
    vecPoints.push_back(glm::vec2(200.0f, 200.0f));
    vecPoints.push_back(glm::vec2(202.0f, 201.0f));
    this->m_pGLCore->DrawPoint(30.0f, 60.0f, 0xFFFF00FF);
    this->m_pGLCore->DrawPoints(vecPoints.size(), (float*)vecPoints.data(), 0xFF0000FF);
    this->m_pGLCore->DrawPoints(vecPoints.size(), (float*)vecPoints.data(), 0xFF00FF00);
    this->m_pGLCore->DrawEllipse(100.0f, 150.0f, 20.0f, 30.0f, 0xFF0000FF);
    this->m_pGLCore->DrawEllipse(100.0f, 150.0f, 30.0f, 20.0f, 0xFF00FFFF);
    //this->m_pGLCore->DrawCircle(10.0f, 20.0f, 600.0f, 0xFF00FF00, 1.0f);
    this->m_pGLCore->DrawCircle(10.0f, 20.0f, 600.0f, 0xFF0000FF, 20.0f);
    this->m_pGLCore->DrawLine(30.0f, 30.0f, 40.0f, 40.0f, 0xFF00FF00, 8.0f);
    this->m_pGLCore->DrawLine(34.0f, 34.0f, 40.0f, 40.0f, 0xFF0000FF, 5.0f);
    vecPoints.clear();
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    vecPoints.push_back(glm::vec2(200.0f+rand()% 20, 200.0f+rand()% 20));
    this->m_pGLCore->DrawLines(vecPoints.size(), (float*)vecPoints.data(), 0xFF0000FF);

    vecPoints.clear();
    vecPoints.push_back(glm::vec2(10.0f, 20.0f));
    vecPoints.push_back(glm::vec2(100.0f, 200.0f));
    //vecPoints.push_back(glm::vec2(100.0f, 200.0f));
    vecPoints.push_back(glm::vec2(200.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(200.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(300.0f, 200.0f));
    //vecPoints.push_back(glm::vec2(300.0f, 200.0f));
    //vecPoints.push_back(glm::vec2(500.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(500.0f, 300.0f));
    //vecPoints.push_back(glm::vec2(10.0f, 20.0f));
    this->m_pGLCore->DrawPolygon(vecPoints.size(), (float*)vecPoints.data(), 0xFF0000FF, 6.0f);
}
