#include "CRasterThumbnail.h"
#include "Src_GlWnd/COpenGLCore.h"

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



#include "gdal/gdal_priv.h"
#include "gdal/ogr_spatialref.h"
#include <QtCore/QFileInfo>
#include "Src_Core/CGlobal.h"


//void Create8BitImage(const char* srcfile, const char* dstfile);
void Create8BitImage(GDALDataset* pDataset, const char* dstfile);
//#include "ogr_spatialref.h"

CRasterThumbnail::CRasterThumbnail(const std::string& sTifFullPath){
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    GDALDataset* pDataset = (GDALDataset*)GDALOpen(sTifFullPath.c_str(), GA_ReadOnly);
    this->CreateThumbnail(pDataset, sTifFullPath);
}
void CRasterThumbnail::CreateThumbnail(GDALDataset* pSrcTiffDataSet, const std::string& sTifFullPath)
{
    //GDALDataset* poDataset = (GDALDataset*)GDALOpen(sTifFullPath.c_str(), GA_ReadOnly);//只读模式

    std::string sTif8BitFullPath;
    GDALRasterBand* pBand = pSrcTiffDataSet->GetRasterBand(1);
    this->m_nDepth = GDALGetDataTypeSize(pBand->GetRasterDataType());	//图像深度
    this->m_nBandCnt = pSrcTiffDataSet->GetRasterCount();
    if (this->m_nDepth == 8) {

        sTif8BitFullPath = sTifFullPath + ".ovr";
        std::string sTiffRrdName = sTifFullPath;
        sTiffRrdName = sTiffRrdName.substr(0, sTiffRrdName.rfind('.'));
        sTiffRrdName = sTiffRrdName + ".rrd";

        QFileInfo fileInfo1(sTif8BitFullPath.c_str());
        QFileInfo fileInfo2(sTiffRrdName.c_str());
        if(false == fileInfo1.exists() && false == fileInfo2.exists()) {
        //if (!SxFile::exists(sTif8BitFullPath.c_str()) && !SxFile::exists(sTiffRrdName.c_str())) {
            int pyramidLevels[20] = { 2,4,6,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536 };
            int nPyrdLevelCnt = 0;

            int nPixWidthTif = pBand->GetXSize();
            int nPixHeightTif = pBand->GetYSize();

            while (nPixWidthTif > 256) {
                nPyrdLevelCnt++;
                nPixWidthTif = (nPixWidthTif >> 1);
            }
            int nPyrdLevelCnt1 = 0;
            while (nPixHeightTif > 256) {
                nPyrdLevelCnt1++;
                nPixHeightTif = (nPixHeightTif >> 1);
            }
            if (nPyrdLevelCnt < nPyrdLevelCnt1)
                nPyrdLevelCnt = nPyrdLevelCnt1;

            //pDataSet->BuildOverviews("NEAREST", 7, pyramidLevels, 0, 0, BuildOverviewProcessDef, (void*)&arrBand[5]);
            pSrcTiffDataSet->BuildOverviews("NEAREST", nPyrdLevelCnt, pyramidLevels, 0, 0, GDALTermProgress, nullptr);
        }

        this->m_sTiffThumbFullPath = sTifFullPath;
        this->m_pDataSet = pSrcTiffDataSet;
    }
    else {
        sTif8BitFullPath = sTifFullPath + ".tmb";
        QFileInfo fileInfo1(sTif8BitFullPath.c_str());
        if (false == fileInfo1.exists()) {
            //SxStretch* pStretch = new SxStretch();
            //pStretch->addTiff(sTifFullPath.c_str());
            //pStretch->startStretch(sTif8BitFullPath.c_str(), SxStretchMode::STRETCH_PERCENTAGE, 1, 0, 0);
            //delete pStretch;
            //pStretch = nullptr;

            //Create8BitImage(pSrcTiffDataSet, sTif8BitFullPath.c_str());
            CGlobal* pGlobal = GetGlobalPtr();
            COpenGLCore* pGLCore = pGlobal->m_pGLCore;

            std::string sCmd = pGLCore->m_sExeDir +"/Stretch16to8.exe ";//"D:/QT_CODE_PRJ/SuperXView01/build-SuperXView01-Desktop_Qt_5_13_0_MSVC2017_64bit-Debug/debug/Stretch16to8.exe ";
            std::string sCmdArgv = sTifFullPath + " -1 " + sTif8BitFullPath + " " + pGLCore->m_sExeDir;
                    //" D:/QT_CODE_PRJ/SuperXView01/build-SuperXView01-Desktop_Qt_5_13_0_MSVC2017_64bit-Debug/debug";
            sCmd = sCmd + " " + sCmdArgv;
            printf("%s\n", sCmd.c_str());
            system(sCmd.c_str());
        }
        this->m_sTiffThumbFullPath = sTif8BitFullPath;

        GDALClose(pSrcTiffDataSet);
        this->m_pDataSet = (GDALDataset*)GDALOpen(sTif8BitFullPath.c_str(), GA_ReadOnly);//只读模式
        pBand = this->m_pDataSet->GetRasterBand(1);

        sTif8BitFullPath = sTifFullPath + ".tmb.ovr";
        std::string sTiffRrdName = sTifFullPath + ".tmb.rrd";
        int pyramidLevels[20] = { 2,4,6,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536 };
        int nPyrdLevelCnt = 0;
        fileInfo1 = QString(sTif8BitFullPath.c_str());
        QFileInfo fileInfo2(sTiffRrdName.c_str());
        if(false == fileInfo1.exists() && false == fileInfo2.exists()) {
        //if (!SxFile::exists(sTif8BitFullPath.c_str()) && !SxFile::exists(sTiffRrdName.c_str())) {

            int nPixWidthTif = pBand->GetXSize();
            int nPixHeightTif = pBand->GetYSize();

            while (nPixWidthTif > 256) {
                nPyrdLevelCnt++;
                nPixWidthTif = (nPixWidthTif >> 1);
            }
            int nPyrdLevelCnt1 = 0;
            while (nPixHeightTif > 256) {
                nPyrdLevelCnt1++;
                nPixHeightTif = (nPixHeightTif >> 1);
            }
            if (nPyrdLevelCnt < nPyrdLevelCnt1)
                nPyrdLevelCnt = nPyrdLevelCnt1;

            //sTif8BitFullPath = sTifFullPath + ".tmb";
            //this->m_pDataSet = (GDALDataset*)GDALOpen(sTif8BitFullPath.c_str(), GA_ReadOnly);//只读模式

            //pDataSet->BuildOverviews("NEAREST", 7, pyramidLevels, 0, 0, BuildOverviewProcessDef, (void*)&arrBand[5]);
            this->m_pDataSet->BuildOverviews("NEAREST", nPyrdLevelCnt, pyramidLevels, 0, 0, GDALTermProgress, nullptr);
        }
        //else {
        //	sTif8BitFullPath = sTifFullPath + ".tmb";
        //	this->m_pDataSet = (GDALDataset*)GDALOpen(sTif8BitFullPath.c_str(), GA_ReadOnly);//只读模式
        //}
    }
}


CRasterThumbnail::CRasterThumbnail(GDALDataset* pSrcTiffDataSet, const std::string& sTifFullPath){
    this->CreateThumbnail(pSrcTiffDataSet, sTifFullPath);
}

CRasterThumbnail::~CRasterThumbnail()
{
    //if (this->m_nDepth == 16) {
        GDALClose(this->m_pDataSet);
    //}
}

void SavePixBufToBMPFile(const char* filePath, unsigned char* pRgbBuff, int width, int height, int bands);
unsigned char* CRasterThumbnail::getBuf(int nLeft, int nTop, int nGetWidth, int nGetHeigth, int nReadWidth, int nReadHeight) {
    unsigned char* pRetBuf = nullptr;
    int arrBand[10] = { 1,2,3,4,5,6,7,8,9,10 };

    unsigned long long nBufSizeLine = (unsigned long long) nReadWidth * 4 * GDALDataType::GDT_Byte;
    unsigned long long nBufSize = (unsigned long long) nReadWidth * (unsigned long long) nReadHeight * 4;


    const int bandCount = 4;
    int nReadBandCnt = this->m_nBandCnt;
    if (nReadBandCnt > 3)
        nReadBandCnt = 3;

    pRetBuf = new unsigned char[nBufSize];
    memset(pRetBuf, 0, nBufSize);

    this->m_pDataSet->RasterIO(GDALRWFlag::GF_Read, nLeft, nTop, nGetWidth, nGetHeigth, pRetBuf, nReadWidth, nReadHeight,
        GDALDataType::GDT_Byte, nReadBandCnt, arrBand, 4 * GDALDataType::GDT_Byte, nBufSizeLine, GDT_Byte);

    unsigned long long nBufSizeBand = (unsigned long long) nReadWidth * (unsigned long long) nReadHeight;
    for (unsigned long long ii = 0; ii < nBufSizeBand; ii++) {
        if (nReadBandCnt == 1) {
            pRetBuf[ii * bandCount + 1] = pRetBuf[ii * bandCount];
            pRetBuf[ii * bandCount + 2] = pRetBuf[ii * bandCount];
        }
        pRetBuf[ii * bandCount + 3] = 0xFF;
    }
    if (0) {
        const char* szFilePath1 = "D:/QT_CODE_PRJ/SuperXView01/Res/images/TiffoutImg.bmp";
        //SxFile::remove(szFilePath1);
        unsigned char* pBuff = new unsigned char[(unsigned long long)nReadWidth * (unsigned long long) nReadHeight * 3];
        unsigned long long nBufSizeBand = (unsigned long long) nReadWidth * (unsigned long long) nReadHeight;
        for (unsigned long long ii = 0; ii < nBufSizeBand; ii++) {
            pBuff[ii * 3 + 0] = pRetBuf[ii * 4 + 0];
            pBuff[ii * 3 + 1] = pRetBuf[ii * 4 + 1];
            pBuff[ii * 3 + 2] = pRetBuf[ii * 4 + 2];
        }
        //SavePixBufToBMPFile(szFilePath1, pBuff, nReadWidth, nReadHeight, 3);
        //getBuf11(nLeft, nTop, nGetWidth, nGetHeigth, nReadWidth, nReadHeight);
    }
    return pRetBuf;
}

unsigned char* CRasterThumbnail::getBuf11(int nLeft, int nTop, int nGetWidth, int nGetHeigth, int nReadWidth, int nReadHeight) {
    unsigned char* pRetBuf = nullptr;
    int arrBand[10] = { 1,2,3,4,5,6,7,8,9,10 };


    const int bandCount = 3;
    int nReadBandCnt = this->m_nBandCnt;
    if (nReadBandCnt > 3)
        nReadBandCnt = 3;

    unsigned long long nBufSizeLine = (unsigned long long) nReadWidth * 3 * GDALDataType::GDT_Byte;
    unsigned long long nBufSize = (unsigned long long) nReadWidth * (unsigned long long) nReadHeight * 3;


    pRetBuf = new unsigned char[nBufSize];
    memset(pRetBuf, 0, nBufSize);

    this->m_pDataSet->RasterIO(GDALRWFlag::GF_Read, nLeft, nTop, nGetWidth, nGetHeigth, pRetBuf, nReadWidth, nReadHeight,
        GDALDataType::GDT_Byte, nReadBandCnt, arrBand, 3 * GDALDataType::GDT_Byte, nBufSizeLine, GDT_Byte);

    const char* szFilePath1 = "D:/QT_CODE_PRJ/SuperXView01/Res/images/TiffoutImg.bmp";
    //SxFile::remove(szFilePath1);
    //SavePixBufToBMPFile(szFilePath1, pRetBuf, nReadWidth, nReadHeight, 3);

    return pRetBuf;
}

typedef unsigned short ushort;
typedef unsigned char uchar;

//直接最大最小值拉伸
void MinMaxStretch(ushort* pBuf, uchar* dstBuf, int bufWidth, int bufHeight, double minVal, double maxVal)
{
    ushort data;
    uchar result;
    for (int x = 0; x < bufWidth; x++)
    {
        for (int y = 0; y < bufHeight; y++)
        {
            data = pBuf[x * bufHeight + y];
            result = (data - minVal) / (maxVal - minVal) * 255;
            dstBuf[x * bufHeight + y] = result;
        }
    }
}


/**
2%-98%最大最小值拉伸，小于最小值的设为0，大于最大值的设为255
@param pBuf 保存16位影像数据的数组，该数组一般直接由Gdal的RasterIO函数得到
@param dstBuf 保存8位影像数据的数组，该数组保存拉伸后的8
@param width 图像的列数
@param height 图像的行数
@param minVal 最小值
@param maxVal 最大值
*/
void MinMaxStretchNew(ushort* pBuf, uchar* dstBuf, unsigned short nNoDataValue, unsigned char nNewNoDataValue, int bufWidth, int bufHeight, double minVal, double maxVal)
{
    ushort data;
    uchar result;
    for (int x = 0; x < bufWidth; x++)
    {
        for (int y = 0; y < bufHeight; y++)
        {
            data = pBuf[x * bufHeight + y];
            if(data == nNoDataValue){
                result = nNewNoDataValue;
            }
            else if (data > maxVal)
                result = 255;
            else if (data < minVal)
                result = 1;
            else{
                result = (data - minVal) / (maxVal - minVal) * 255;
                if(result < 1)
                    result = 1;
            }
            dstBuf[x * bufHeight + y] = result;
        }
    }
}

/**
计算灰度累积直方图概率分布函数，当累积灰度概率为0.02时取最小值，0.98取最大值
@param pBuf 保存16位影像数据的数组，该数组一般直接由Gdal的RasterIO函数得到
@param width 图像的列数
@param height 图像的行数
@param minVal 用于保存计算得到的最小值
@param maxVal 用于保存计算得到的最大值
*/
void HistogramAccumlateMinMax16S(ushort* pBuf, int width, int height, double* minVal, double* maxVal)
{
    int nMaxCnt = 65535;
    double* p = new double[nMaxCnt];
    double* p1 = new double[nMaxCnt];
    double* num = new double[nMaxCnt];

    memset(p, 0, sizeof(double) * nMaxCnt);
    memset(p1, 0, sizeof(double) * nMaxCnt);
    memset(num, 0, sizeof(double) * nMaxCnt);

    long wMulh = height * width;

    //计算灰度分布
    short nVMaxVal = -9999;
    short nVMinVal = 0x7FFF;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            ushort v = pBuf[x * height + y];
            //if(0x8000 == (v & 0x8000))
            //    printf("- Value\n");
            //if(nVMaxVal < ((short)v)){
            //    nVMaxVal = ((short)v);
            //}
            //if(nVMinVal > ((short)v)){
            //    nVMinVal = ((short)v);
            //}
            num[v]++;
        }
    }
    //printf("%d  %d\n", nVMinVal, nVMaxVal);

    if(0){
        std::string sTmpTxt = "C:/temp/GF1/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004/1.txt";
        remove(sTmpTxt.c_str());
        FILE* pff = fopen(sTmpTxt.c_str(), "w");
        char szBuf[64];

        //计算灰度的概率分布
        for (int i = 5; i < nMaxCnt; i++) {
            p[i] = num[i] / wMulh;
            memset(szBuf, 0, 32);
            sprintf(szBuf, "%f\n", num[i]);
            fwrite(szBuf, strlen(szBuf), 1, pff);
        }
        fclose(pff);
    }

    int nCutNum = 5;
    for(int i = 0; i<nCutNum;i++){
        wMulh -= num[i];
    }
    nMaxCnt -= nCutNum;
    //计算灰度的概率分布
    for (int i = 0; i < nMaxCnt; i++) {
        p[i] = num[i+nCutNum] / wMulh;
    }
    ////计算灰度的概率分布
    //for (int i = 0; i < nMaxCnt; i++)
    //	p[i] = num[i] / wMulh;

    int min = 0, max = 0;
    double minProb = 0.0, maxProb = 0.0;
    //计算灰度累积概率
    //当概率为0.02时，该灰度为最小值
    //当概率为0.98时，该灰度为最大值
    while (min < nMaxCnt && minProb < 0.098) //0.095
    {
        minProb += p[min];
        min++;
    }
    do
    {
        maxProb += p[max];
        max++;
    } while (max < nMaxCnt && maxProb < 0.99);

    delete p;
    delete p1;
    delete num;
    *minVal = min;
    *maxVal = max;
}
//void Create8BitImage(const char* srcfile, const char* dstfile)
void Create8BitImage(GDALDataset* pDataset, const char* dstfile)
{
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    //GDALDataset* pDataset = (GDALDataset*)GDALOpen(srcfile, GA_ReadOnly);
    int bandNum = pDataset->GetRasterCount();
    GDALRasterBand* pBand;
    pBand = pDataset->GetRasterBand(1);

    unsigned long long nPixWidthTif = pBand->GetXSize();
    unsigned long long nPixHeightTif = pBand->GetYSize();

    int nDstBandCnt = bandNum;
    if (nDstBandCnt > 3)
        nDstBandCnt = 3;
    if(nDstBandCnt < 3)
        nDstBandCnt = 1;
    //写入光栅数据
    GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset* dstDataset = pDriver->Create(dstfile, nPixWidthTif, nPixHeightTif, nDstBandCnt, GDT_Byte, NULL);
    GDALRasterBand* dstBand;

    double pAdfTransform[6];
    pDataset->GetGeoTransform(pAdfTransform);
    std::string sPrj = pDataset->GetProjectionRef();

    ushort* sbuf = new ushort[nPixWidthTif * nPixHeightTif];
    uchar* cbuf = new uchar[nPixWidthTif * nPixHeightTif];
    int arrBandIdxSrc[10] = {1, 2, 3, 4, 5, 6};
    int arrBandIdxDst[10] = {3, 2, 1, 4, 5, 6};
    if(nDstBandCnt == 1){
        arrBandIdxDst[0] = 1;
    }
    unsigned short nNoDataValue = 0;
    unsigned char nNewNoDataValue = 0;

    unsigned long long nTmbPixW = 0;
    unsigned long long nTmbPixH = 0;

    nTmbPixW = 4096;
    const unsigned long long nTmpSIZE = 4096;

    if (nPixWidthTif <= 5000 && nPixHeightTif <= 5000) {
        nTmbPixW = nPixWidthTif;
        nTmbPixH = nPixHeightTif;
    }
    else {
        if (nTmbPixW < nTmpSIZE) {
            nTmbPixW = nTmpSIZE;
        }
        nTmbPixH = (unsigned long long)((double)nTmbPixW * (double)nPixHeightTif / (double)nPixWidthTif);
    }
    unsigned short* sTmbBuf = new unsigned short[nTmbPixW * nTmbPixH];

    for (int i = 0; i < nDstBandCnt; i++)
    {
        pBand = pDataset->GetRasterBand(arrBandIdxSrc[i]);
        nNoDataValue = (unsigned short)pBand->GetNoDataValue();

        memset(sTmbBuf, 0, nTmbPixW * nTmbPixH);
        pBand->RasterIO(GF_Read, 0, 0, nPixWidthTif, nPixHeightTif, sTmbBuf, nTmbPixW, nTmbPixH, GDT_UInt16, 0, 0);
        double adfMinMax[2];
        int bGotMin, bGotMax;
        adfMinMax[0] = pBand->GetMinimum(&bGotMin);
        adfMinMax[1] = pBand->GetMaximum(&bGotMax);
        double min, max;
        HistogramAccumlateMinMax16S(sTmbBuf, nTmbPixW, nTmbPixH, &min, &max);

        //double min,max;
        //HistogramAccumlateMinMax16S(sbuf, nPixWidthTif, nPixHeightTif, &min, &max);

        pBand->RasterIO(GF_Read, 0, 0, nPixWidthTif, nPixHeightTif, sbuf, nPixWidthTif, nPixHeightTif, GDT_UInt16, 0, 0);
        //int bGotMin, bGotMax;
        //double adfMinMax[2];
        //adfMinMax[0] = pBand->GetMinimum(&bGotMin);
        //adfMinMax[1] = pBand->GetMaximum(&bGotMax);
        ////if (!(bGotMin && bGotMax))
        ////	GDALComputeRasterMinMax((GDALRasterBandH)pBand, TRUE, adfMinMax);
        ////MinMaxStretch(sbuf, cbuf, nPixWidthTif, nPixHeightTif, adfMinMax[0], adfMinMax[1]);
        MinMaxStretchNew(sbuf, cbuf, nNoDataValue, nNewNoDataValue, nPixWidthTif, nPixHeightTif, min, max);
        dstBand = dstDataset->GetRasterBand(arrBandIdxDst[i]);
        dstBand->RasterIO(GF_Write, 0, 0, nPixWidthTif, nPixHeightTif, cbuf, nPixWidthTif, nPixHeightTif, GDT_Byte, 0, 0);

        dstBand->SetNoDataValue(nNewNoDataValue);
    }
    dstDataset->SetGeoTransform(pAdfTransform);
    dstDataset->SetProjection(sPrj.c_str());
    delete[]cbuf;
    delete[]sbuf;
    delete[]sTmbBuf;
    //GDALClose(pDataset);
    GDALClose(dstDataset);
}
