#ifndef SXRASTERTHUMBNAIL_H
#define SXRASTERTHUMBNAIL_H

#include <string>
#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

class GDALDataset;
class CRasterThumbnail
{
public:
    CRasterThumbnail(const std::string& sTifFullPath);
    CRasterThumbnail(GDALDataset* pSrcTiffDataSet, const std::string& sTifFullPath);
    ~CRasterThumbnail();

    void CreateThumbnail(GDALDataset* pSrcTiffDataSet, const std::string& sTifFullPath);

    int m_nDepth = 0;
    int m_nBandCnt = 0;
    std::string m_sTiffThumbFullPath;
    GDALDataset* m_pDataSet = nullptr;

    unsigned char* getBuf(int nLeft, int nTop, int nGetWidth, int nGetHeigth, int nReadWidth, int nReadHeight);
    unsigned char* getBuf11(int nLeft, int nTop, int nGetWidth, int nGetHeigth, int nReadWidth, int nReadHeight);
};


#endif // SXRASTERTHUMBNAIL_H
