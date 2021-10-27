#ifndef SXREADRASTER_H
#define SXREADRASTER_H
#include <string>

class SxRPC;
class CTifAvailBound;
class GDALDataset;
class CReadRaster {
public :
    CReadRaster();
    CReadRaster(const std::string& sRasterFileFullName);
    ~CReadRaster();

    int OpenRaster(const std::string& sRasterFileFullName);

    void* ReadPixData(int nBandIndx, int nSrcLT_X, int nSrcLT_Y, int nSrcWidth, int nSrcHeight, int nReadWidth, int nReadHeight);
    //void* ReadPixData(int nSrcLT_X, int nSrcLT_Y, int nSrcWidth, int nSrcHeight, int nReadBandCnt, int nReadWidth, int nReadHeight);
    void* ReadPixDataFull(int nSrcLT_X, int nSrcLT_Y, int nSrcWidth, int nSrcHeight, int nReadBandCnt, int nReadWidth, int nReadHeight);

    void  RestoreAlphaPixData(unsigned char* pPixData, unsigned long long nOneReadBandSize);

    void* ReadPixData(int nBandIndx, double fMinLng, double fMinLat, double fMaxLng, double fMaxLat, int nReadWidth, int nReadHeight);

    void GeoPos2PixPos(double fLng, double fLat, double* pPixX, double* pPixY);
    void GeoPos2PixPos(double fMinLng, double fMinLat, double fMaxLng, double fMaxLat, double* pMinX, double* pMinY, double* pMaxX, double* pMaxY);

    void PixPos2GeoPos(double fPixX, double fPixY, double* pGeoX, double* pGeoY);

    int GetValidBound(double** _ppGeoPoints);

    std::string m_sRasterFileFullName;

    GDALDataset* m_pDataset = nullptr;
    int m_nDataDepth = 0;
    int m_nDataType = 0;
    std::string m_sDataType;
    double m_pAdfTransform[6] = { 0 };
    int m_nBandCnt = 0;
    double m_fWidthScaleHeight = 0.0;
    unsigned long long m_nPixWidth = 0;
    unsigned long long m_nPixHeight = 0;
    double m_fMaxLng, m_fMaxLat, m_fMinLng, m_fMinLat;
    double m_fLngWidht, m_fLatHeight;

    unsigned char* m_pPixAlphaData = nullptr;
    unsigned long long m_nOneReadBandSize = 0;

    SxRPC* m_pModelRpc = nullptr;
    CTifAvailBound* m_pSxTifAvailBound = nullptr;

    int m_nShowAlpha = 0;
    int m_nShowNoData = 1;
    int m_arrNoDataValue[16] = {0};
private: 
};
#endif
