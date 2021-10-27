#ifndef __SXTIFVALIDBOUND_H_
#define __SXTIFVALIDBOUND_H_

#include <deque>

#include <unordered_map>

//提取GeoTiff 的有效区域线（去黑边区域）
//主要用于镶嵌功能
//传入数据：单波段的buf 或 多波段buf

class CGeoTiff;

class CTifAvailBound {
public:
    unsigned char * m_buf;
    unsigned char ** m_ppBuf;
    int m_nBandCount = 1;
    int m_bufW = 0;//buf宽度
    int m_bufH = 0;//buf高度

    int* m_pixX = nullptr;//
    int* m_pixY = nullptr;
    int* m_pixReverseX = nullptr;//
    int* m_pixReverseY = nullptr;
    double* m_pGeoX = nullptr;
    double* m_pGeoY = nullptr;
    double* m_pGeoReverseX = nullptr;
    double* m_pGeoReverseY = nullptr;
    int m_pointCount = 0;//点数
    int m_pointCountReverse = 0;//点数

    double* m_pAdfTransForm = nullptr;

    int m_LeftPointCount = 0; //左半部分有效点个数
    int m_RightPointCount = 0;//右半部分有效点个数

    int m_ReverseLeftPointCount = 0; //左半部分有效点个数
    int m_ReverseRightPointCount = 0;//右半部分有效点个数
public:

    int m_nArrLeftTopX[8];//左边和上边部分   有像素的位置的 四角坐标（非 NoData 区域的四角坐标，不是外接矩形的四角坐标）
    int m_nArrRightBottomX[8];// 右边和下边部分  有像素的位置的 四角坐标（非 NoData 区域的四角坐标，不是外接矩形的四角坐标）
    //int m_nX0, m_nY0, m_nX1, m_nY1, m_nX2, m_nY2, m_nX3, m_nY3;//有像素的位置的 四角坐标（非 NoData 区域的四角坐标，不是外接矩形的四角坐标）
    //int m_1nX0, m_1nY0, m_1nX1, m_1nY1, m_1nX2, m_1nY2, m_1nX3, m_1nY3;//有像素的位置的 四角坐标（非 NoData 区域的四角坐标，不是外接矩形的四角坐标）

    std::unordered_map<int, int> m_mapTopX; // 上半部分 从左到右 的 x y 键值对
    std::unordered_map<int, int> m_mapBottomX; // 下半部分 从右到左 的 x y 键值对

    std::unordered_map<int, int> m_mapLeftY; // 左半部分  从上到下的  y x 键值对
    std::unordered_map<int, int> m_mapRightY; // 有半部分  从下到上的 y x 键值对

    int m_nPixMinX = 99999999;
    int m_nPixMinY = 99999999;

    int m_nPixMaxX = 0;
    int m_nPixMaxY = 0;

    struct ST_CORNER_PT {
        int x;
        int y;
    }   m_st_Left_Up_Down = { 0 }, m_st_Up_Left_Right = { 0 },
        m_st_Right_Down_Up = { 0 }, m_st_Down_Right_Left = { 0 },

        m_st_Left_Up_Down1 = { 0 }, m_st_Up_Left_Right1 = { 0 },
        m_st_Right_Down_Up1 = { 0 }, m_st_Down_Right_Left1 = { 0 };
public:
    ~CTifAvailBound();
    //输入一个波段的数据
    CTifAvailBound();
    CTifAvailBound(unsigned char* buffer, int w, int h);
    CTifAvailBound(unsigned char** ppBuffer, int nBandCount, int w, int h, int bAllocMm = 1, double* pAdfTransForm = nullptr);//【 double* pAdfTransForm 】这个参数不准备使用，暂时先写在这

    void computePoints();
    int  getGeoPointByPixPoint(int nPixX, int nPixY, double* _pGeoX, double* _pGeoY);
    void computePointsByMultiBand(int nCalcGeoPoint = 0);
    void computeCornerPointsByMultiBand();
    void computeCornerPointsByMultiBand_16Bit();
    int  initByCacheFile(const char* szCacheFile);
    int  saveCacheFile(const char* szCacheFile);

    //结果返回
    int* xArray();
    //结果返回
    int* yArray();
    //结果返回
    int* xArrayReverse();
    //结果返回
    int* yArrayReverse();
    //结果返回
    int size();
    int leftPointCount();
    int rightPointCount();

    int ReverseleftPointCount();
    int ReverserightPointCount();

    int ConvertPix2Geo(CGeoTiff* pGeoTiff);
    int ConvertPix2Geo(double* pAdfTransForm);
    int ConvertPix2Geo(double* pAdfTransForm, double** _ppGeoPoints);
};
#endif
