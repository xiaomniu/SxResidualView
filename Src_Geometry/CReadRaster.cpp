#include "CReadRaster.h"
//#include "Src_SuperX/SxRPC.h"

#include "gdal/gdal.h"
#include "gdal/cpl_conv.h"
#include "gdal/gdal_priv.h"

#include "Src_Geometry/CTifAvailBound.h"

CReadRaster::CReadRaster() {

}

CReadRaster::CReadRaster(const std::string& sRasterFileFullName){
    this->OpenRaster(sRasterFileFullName);
}

CReadRaster::~CReadRaster() {
    if(this->m_pDataset) {
        GDALClose(this->m_pDataset);
    }
}

int CReadRaster::OpenRaster(const std::string &sRasterFileFullName)
{
    this->m_sRasterFileFullName = sRasterFileFullName;
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//图像路径不支持中文（可设置）
    CPLSetConfigOption("GDAL_DATA", "./data");			//图像缓存路径
    GDALAllRegister();									//注册图像驱动
    this->m_pDataset = (GDALDataset*)GDALOpen(this->m_sRasterFileFullName.c_str(), GA_Update);

    this->m_nBandCnt = this->m_pDataset->GetRasterCount();

    this->m_pDataset->GetGeoTransform(this->m_pAdfTransform);
    if(this->m_pAdfTransform[1] >= 1.0 || this->m_pAdfTransform[5] >= 1.0) {
        //if(m_pModelRpc)
        this->m_pAdfTransform[1] = 0.009009009009009009;
        this->m_pAdfTransform[5] = -0.009009009009009009;
    }

    GDALRasterBand* pRasterBand = this->m_pDataset->GetRasterBand(1);
    this->m_nPixWidth = pRasterBand->GetXSize();
    this->m_nPixHeight = pRasterBand->GetYSize();
    if(this->m_nPixHeight != 0){
        this->m_fWidthScaleHeight = ((double)this->m_nPixWidth) / ((double)this->m_nPixHeight);
    }
    if (this->m_nPixHeight == 0 || this->m_nPixWidth == 0) {
        printf("ERR:ReadRaster::PixHeight = 0");
        return 0;
    }
    this->m_nDataType = pRasterBand->GetRasterDataType();

    double fGeoWidth = (double)this->m_nPixWidth * this->m_pAdfTransform[1];
    double fGeoHeight = (double)this->m_nPixHeight * this->m_pAdfTransform[5];

    this->m_fMinLat = this->m_pAdfTransform[3] + fGeoHeight;
    this->m_fMaxLat = this->m_pAdfTransform[3];
    this->m_fMinLng = this->m_pAdfTransform[0];
    this->m_fMaxLng = this->m_pAdfTransform[0] + fGeoWidth;

    this->m_fLngWidht = fGeoWidth;
    this->m_fLatHeight = fabs(fGeoHeight);

    int  i = 0;
    for(i = 1 ; i <= this->m_nBandCnt; i++){
        pRasterBand = this->m_pDataset->GetRasterBand(i);
        this->m_arrNoDataValue[i - 1] = (int)(pRasterBand->GetNoDataValue());
    }

    GDALDataType nDataType = static_cast<GDALDataType>(this->m_nDataType);
    switch (nDataType) {
    case GDT_Unknown:{
        // Unknown or unspecified type
        this->m_sDataType = "未知";
        this->m_nDataDepth = -1;
        break;
    }
    case GDT_Byte:{
        // Eight bit unsigned integer
        this->m_sDataType = "字节型";
        this->m_nDataDepth = 8;
        break;
    }
    case GDT_UInt16:{
        // Sixteen bit unsigned integer
        this->m_sDataType = "无符号短整型";
        this->m_nDataDepth = 16;
        break;
    }
    case GDT_Int16:{
        // Sixteen bit signed integer
        this->m_sDataType = "短整型";
        this->m_nDataDepth = 16;
        break;
    }
    case GDT_UInt32:{
        // Thirty two bit unsigned integer
        this->m_sDataType = "无符号整型";
        this->m_nDataDepth = 32;
        break;
    }
    case GDT_Int32:{
        // Thirty two bit signed integer
        this->m_sDataType = "整型";
        this->m_nDataDepth = 32;
        break;
    }
    case GDT_Float32:{
        // Thirty two bit floating point
        this->m_sDataType = "单精度浮点型";
        this->m_nDataDepth = 32;
        break;
    }
    case GDT_Float64:{
        // Sixty four bit floating point
        this->m_sDataType = "双精度浮点型";
        this->m_nDataDepth = 64;
        break;
    }
    case GDT_CInt16:{
        // Complex Int16  16bit复整型
        this->m_sDataType = "复合短整型";
        this->m_nDataDepth = 16;
        break;
    }
    case GDT_CInt32:{
        // Complex Int32 32bit复整型
        this->m_sDataType = "复合整型";
        this->m_nDataDepth = 32;
        break;
    }
    case GDT_CFloat32:{
        // Complex Float32 32bit复浮点型
        this->m_sDataType = "复合单精度浮点型";
        this->m_nDataDepth = 32;
        break;
    }
    case GDT_CFloat64:{
        // Complex Float64 64bit复浮点型
        this->m_sDataType = "复合双精度浮点型";
        this->m_nDataDepth = 64;
        break;
    }
    case GDT_TypeCount:
    default:{
        // maximum type # + 1
        this->m_sDataType = "未知";
        this->m_nDataDepth = -1;
        break;
    }
    }

    return 1;
}

void* CReadRaster::ReadPixData(int nBandIndx, int nSrcLT_X, int nSrcLT_Y, int nSrcWidth, int nSrcHeight, int nReadWidth, int nReadHeight) {

    void* pDataRead = nullptr;
    GDALDataType nDataType = static_cast<GDALDataType>(this->m_nDataType);

    unsigned long long nReadSize = (unsigned long long )nReadWidth * (unsigned long long )nReadHeight;

    switch (nDataType) {
    case GDT_Unknown:{
        // Unknown or unspecified type
        return pDataRead;
        break;
    }
    case GDT_Byte:{
        // Eight bit unsigned integer
        pDataRead = new unsigned char[nReadSize];
        break;
    }
    case GDT_UInt16:
    case GDT_Int16:{
        // Sixteen bit unsigned integer
        // Sixteen bit signed integer
        pDataRead = new unsigned short[nReadSize];
        break;
    }
    case GDT_UInt32:
    case GDT_Int32:{
        // Thirty two bit unsigned integer
        // Thirty two bit signed integer
        pDataRead = new unsigned int [nReadSize];
        break;
    }
    case GDT_Float32:{
        // Thirty two bit floating point
        pDataRead = new float[nReadSize];
        break;
    }
    case GDT_Float64:{
        // Sixty four bit floating point
        pDataRead = new double[nReadSize];
        break;
    }
    case GDT_CInt16:{
        // Complex Int16  16bit复整型
        break;
    }
    case GDT_CInt32:{
        // Complex Int32 32bit复整型
        return pDataRead;
        break;
    }
    case GDT_CFloat32:{
        // Complex Float32 32bit复浮点型
        return pDataRead;
        break;
    }
    case GDT_CFloat64:{
        // Complex Float64 64bit复浮点型
        return pDataRead;
        break;
    }
    case GDT_TypeCount:
    default:{
        // maximum type # + 1
        return pDataRead;
        break;
    }
    }


    GDALRasterBand* pRasterBand = this->m_pDataset->GetRasterBand(nBandIndx);
    pRasterBand->RasterIO(GF_Read, nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, pDataRead, nReadWidth, nReadHeight, nDataType, 0, 0);

    return pDataRead;
}

void  CReadRaster::RestoreAlphaPixData(unsigned char* pPixData, unsigned long long nOneReadBandSize){
    unsigned char* pPixAlphaData = new unsigned char[nOneReadBandSize];
    memset(pPixAlphaData, 0, sizeof(unsigned char) * nOneReadBandSize);
    unsigned long long i = 0, nPtr = 0;
    int j = 0;
    int nFind = 0;
    if (this->m_nShowNoData == 0 ){
        if(this->m_nBandCnt == 1)
        for(i = 0; i<nOneReadBandSize;i++) {
            nPtr = i*4 + 3;
            pPixAlphaData[i] = pPixData[nPtr];
            nFind = 0;
            if(pPixData[nPtr-3] == this->m_arrNoDataValue[0]) {
                pPixData[nPtr] = 0;
            }
            pPixData[nPtr] = 0xFF;
            //for(j = 0 ; j < this->m_nBandCnt; j++) {
            //    if(this->m_arrNoDataValue[j] ==  pPixData[nPtr-3] ||
            //        this->m_arrNoDataValue[j] == pPixData[nPtr-2] ||
            //        this->m_arrNoDataValue[j] == pPixData[nPtr-1]
            //        ){
            //        pPixData[nPtr] = 0x0;
            //        nFind++;
            //        break;
            //    }
            //}
            //if(nFind) {
            //    continue;
            //}
        } else {
            for(i = 0; i<nOneReadBandSize;i++) {
                nPtr = i*4 + 3;
                pPixAlphaData[i] = pPixData[nPtr];
                nFind = 0;
                if(pPixData[nPtr-3] == this->m_arrNoDataValue[0] ||
                        pPixData[nPtr-2] == this->m_arrNoDataValue[1]||
                        pPixData[nPtr-1] == this->m_arrNoDataValue[2]) {
                    pPixData[nPtr] = 0;
                    continue;
                }
                pPixData[nPtr] = 0xFF;
                //for(j = 0 ; j < this->m_nBandCnt; j++) {
                //    if(this->m_arrNoDataValue[j] ==  pPixData[nPtr-3] ||
                //        this->m_arrNoDataValue[j] == pPixData[nPtr-2] ||
                //        this->m_arrNoDataValue[j] == pPixData[nPtr-1]
                //        ){
                //        pPixData[nPtr] = 0x0;
                //        nFind++;
                //        break;
                //    }
                //}
                //if(nFind) {
                //    continue;
                //}
            }
        }
    }else{
        for(i = 0; i<nOneReadBandSize;i++){
            nPtr = i*4 + 3;
            pPixAlphaData[i] = pPixData[nPtr];
            pPixData[nPtr] = 0xFF;
        }
    }


    if(this->m_pPixAlphaData){
        delete[] this->m_pPixAlphaData;
        this->m_pPixAlphaData = nullptr;
    }
    this->m_nOneReadBandSize = nOneReadBandSize;
    this->m_pPixAlphaData = pPixAlphaData;
}
void* CReadRaster::ReadPixDataFull(int nSrcLT_X, int nSrcLT_Y, int nSrcWidth, int nSrcHeight, int nReadBandCnt, int nReadWidth, int nReadHeight) {

    if(nReadWidth < 0 || nReadHeight < 0)
        return nullptr;
    void* pDataRead = nullptr;
    GDALDataType nDataType = static_cast<GDALDataType>(this->m_nDataType);

    unsigned long long nReadSize = (unsigned long long )nReadWidth * (unsigned long long )nReadHeight * 4;

    switch (nDataType) {
    case GDT_Unknown:{
        // Unknown or unspecified type
        return pDataRead;
        break;
    }
    case GDT_Byte:{
        // Eight bit unsigned integer
        pDataRead = new unsigned char[nReadSize];
        break;
    }
    case GDT_UInt16:
    case GDT_Int16:{
        // Sixteen bit unsigned integer3
        // Sixteen bit signed integer
        pDataRead = new unsigned short[nReadSize];
        break;
    }
    case GDT_UInt32:
    case GDT_Int32:{
        // Thirty two bit unsigned integer
        // Thirty two bit signed integer
        pDataRead = new unsigned int [nReadSize];
        break;
    }
    case GDT_Float32:{
        // Thirty two bit floating point
        pDataRead = new float[nReadSize];
        break;
    }
    case GDT_Float64:{
        // Sixty four bit floating point
        pDataRead = new double[nReadSize];
        break;
    }
    case GDT_CInt16:{
        // Complex Int16  16bit复整型
        break;
    }
    case GDT_CInt32:{
        // Complex Int32 32bit复整型
        return pDataRead;
        break;
    }
    case GDT_CFloat32:{
        // Complex Float32 32bit复浮点型
        return pDataRead;
        break;
    }
    case GDT_CFloat64:{
        // Complex Float64 64bit复浮点型
        return pDataRead;
        break;
    }
    case GDT_TypeCount:
    default:{
        // maximum type # + 1
        return pDataRead;
        break;
    }
    }

    //GDALRasterBand* pRasterBand = this->m_pDataset->GetRasterBand(nBandIndx);
    //pRasterBand->RasterIO(GF_Read, nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, pDataRead, nReadWidth, nReadHeight, nDataType, 0, 0);
    int arrBandIndx[4]={1,2,3,4};
    memset(pDataRead, 0xFF, nReadSize);
    if(this->m_nBandCnt <= 3 && nReadBandCnt > 3){
        nReadBandCnt = 3;
    }
    void* ptt = pDataRead;
    unsigned long long i = 0, nOneBandSize = (unsigned long long )nReadWidth * (unsigned long long )nReadHeight;
    if(this->m_nBandCnt == 1){
        nReadBandCnt = 1;
        pDataRead = new unsigned char[nOneBandSize];
    }
    if(nReadBandCnt == 1) {
        this->m_pDataset->RasterIO(GF_Read, nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, pDataRead,
            nReadWidth, nReadHeight, nDataType, nReadBandCnt, arrBandIndx, 0, 0, 0);

        void* ptt1 = ptt;
        ptt = pDataRead;
        pDataRead = ptt1;
        for(i = 0; i<nOneBandSize;i++){
            ((unsigned char*)pDataRead)[i*4+0] = ((unsigned char*)ptt)[i];
            ((unsigned char*)pDataRead)[i*4+1] = ((unsigned char*)ptt)[i];
            ((unsigned char*)pDataRead)[i*4+2] = ((unsigned char*)ptt)[i];
        }
        delete[] ((unsigned char*)ptt);
    }
    else{
        this->m_pDataset->RasterIO(GF_Read, nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, pDataRead,
            nReadWidth, nReadHeight, nDataType, nReadBandCnt, arrBandIndx, GDT_Byte * 4, GDT_Byte * nReadWidth * 4, GDT_Byte);
    }
    if(this->m_nShowAlpha == 0) {
        this->RestoreAlphaPixData((unsigned char*)pDataRead, nOneBandSize);
    }
    return pDataRead;
}

void CReadRaster::GeoPos2PixPos(double fLng, double fLat, double* pPixX, double* pPixY){

    *pPixX = static_cast<double>((fLng - this->m_fMinLng) / this->m_pAdfTransform[1]);
    *pPixY = static_cast<double>((fLat - this->m_fMaxLat) / this->m_pAdfTransform[5]);
}
void CReadRaster::GeoPos2PixPos(double fMinLng, double fMinLat, double fMaxLng, double fMaxLat, double* pMinX, double* pMinY, double* pMaxX, double* pMaxY){
    *pMinX = static_cast<double>((fMinLng - this->m_fMinLng) / this->m_pAdfTransform[1]);
    *pMinY = static_cast<double>((fMaxLat - this->m_fMaxLat) / this->m_pAdfTransform[5]);

    *pMaxX = static_cast<double>((fMaxLng - this->m_fMinLng) / this->m_pAdfTransform[1]);
    *pMaxY = static_cast<double>((fMinLat - this->m_fMaxLat) / this->m_pAdfTransform[5]);
}

void CReadRaster::PixPos2GeoPos(double fPixX, double fPixY, double* pGeoX, double* pGeoY){
    *pGeoX = static_cast<double>((fPixX) * this->m_pAdfTransform[1] + this->m_fMinLng);
    *pGeoY = static_cast<double>((fPixY) * this->m_pAdfTransform[5] + this->m_fMaxLat);
}
void* CReadRaster::ReadPixData(int nBandIndx, double fMinLng, double fMinLat, double fMaxLng, double fMaxLat, int nReadWidth, int nReadHeight) {
    double nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight;
    this->GeoPos2PixPos(fMinLng, fMinLat, fMaxLng, fMaxLat, &nSrcLT_X, &nSrcLT_Y, &nSrcWidth, &nSrcHeight);
    nSrcWidth = nSrcWidth - nSrcLT_X;
    nSrcHeight = nSrcHeight - nSrcLT_Y;
    return this->ReadPixData(nBandIndx, (int)nSrcLT_X, (int)nSrcLT_Y, (int)nSrcWidth, (int)nSrcHeight, nReadWidth, nReadHeight);
    //return this->ReadPixData(nBandIndx, 0, 0, this->m_nPixWidth, this->m_nPixHeight, nReadWidth, nReadHeight);
}

int CReadRaster::GetValidBound(double** _ppGeoPoints) {

    int nGeoPointCnt = 0;
    if(this->m_pSxTifAvailBound == nullptr) {
        //unsigned long long nBufSize = this->m_nPixWidth * this->m_nPixHeight;
        //unsigned char* pBuf = new unsigned char[nBufSize];
        unsigned char* pBufBand = (unsigned char*)this->ReadPixData(1,
            0, 0, (int)this->m_nPixWidth, (int)this->m_nPixHeight,
            (int)this->m_nPixWidth, (int)this->m_nPixHeight);
        unsigned char* ppBufBand[1] = { pBufBand };
        this->m_pSxTifAvailBound = new CTifAvailBound(ppBufBand, 1, (int)this->m_nPixWidth, (int)this->m_nPixHeight);
        this->m_pSxTifAvailBound->computePointsByMultiBand();
        nGeoPointCnt = this->m_pSxTifAvailBound->ConvertPix2Geo(this->m_pAdfTransform, _ppGeoPoints);
        if (pBufBand)
            delete[] pBufBand;
        pBufBand = nullptr;
    } else {
        nGeoPointCnt = this->m_pSxTifAvailBound->m_pointCount;
        double* pGeoPoints = new double[nGeoPointCnt * 3];
        for (int i = 0; i<nGeoPointCnt; i++){
            pGeoPoints[i * 3] =     this->m_pSxTifAvailBound->m_pGeoX[i];
            pGeoPoints[i * 3 + 1] = this->m_pSxTifAvailBound->m_pGeoY[i];
            pGeoPoints[i * 3 + 2] = 0.0;
        }
        *_ppGeoPoints = pGeoPoints;
    }

    return nGeoPointCnt;
}

/*
    switch (static_cast<GDALDataType>(this->m_nDataType)) {
    case GDT_Unknown:{
        // Unknown or unspecified type
        break;
    }
    case GDT_Byte:{
        // Eight bit unsigned integer
        break;
    }
    case GDT_UInt16:{
        // Sixteen bit unsigned integer
        break;
    }
    case GDT_Int16:{
        // Sixteen bit signed integer
        break;
    }
    case GDT_UInt32:{
        // Thirty two bit unsigned integer
        break;
    }
    case GDT_Int32:{
        // Thirty two bit signed integer
        break;
    }
    case GDT_Float32:{
        // Thirty two bit floating point
        break;
    }
    case GDT_Float64:{
        // Sixty four bit floating point
        break;
    }
    case GDT_CInt16:{
        // Complex Int16  16bit复整型
        break;
    }
    case GDT_CInt32:{
        // Complex Int32 32bit复整型
        break;
    }
    case GDT_CFloat32:{
        // Complex Float32 32bit复浮点型
        break;
    }
    case GDT_CFloat64:{
        // Complex Float64 64bit复浮点型
        break;
    }
    case GDT_TypeCount:{
        // maximum type # + 1
        break;
    }
    }
*/

