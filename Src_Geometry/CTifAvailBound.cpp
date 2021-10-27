#include "CTifAvailBound.h"
#include <deque>

#include "Src_Core/Misc.h"
#include <unordered_map>

CTifAvailBound::~CTifAvailBound() {
    if (this->m_pixX)
        delete[] this->m_pixX;
    if (this->m_pixY)
        delete[] this->m_pixY;
    if (this->m_pixReverseX)
        delete[] this->m_pixReverseX;
    if (this->m_pixReverseY)
        delete[] this->m_pixReverseY;
    this->m_pixX = this->m_pixY = this->m_pixReverseX = this->m_pixReverseY = nullptr;

    if (this->m_pGeoX)
        delete this->m_pGeoX;
    if (this->m_pGeoY)
        delete this->m_pGeoY;
    if (this->m_pGeoReverseX)
        delete this->m_pGeoReverseX;
    if (this->m_pGeoReverseY)
        delete this->m_pGeoReverseY;
    this->m_pGeoX = this->m_pGeoY = m_pGeoReverseX = m_pGeoReverseY = nullptr;

    if (this->m_pAdfTransForm)
        delete this->m_pAdfTransForm;
    this->m_pAdfTransForm = nullptr;
}
//输入一个波段的数据
CTifAvailBound::CTifAvailBound() {}
CTifAvailBound::CTifAvailBound(unsigned char* buffer, int w, int h) {
    this->m_buf = buffer;
    this->m_bufW = w;
    this->m_bufH = h;
    this->m_pixX = new int[w * 2];//存储找到的边界点 以X方向遍历，从最左边开始顺时针存储
    memset(this->m_pixX, -1, w * 2 * sizeof(int));//赋值-1
    this->m_pixY = new int[w * 2];
    memset(this->m_pixY, -1, w * 2 * sizeof(int));//赋值-1

    this->m_pixReverseX = new int[w * 2];//存储找到的边界点 以X方向遍历，从最左边开始顺时针存储
    memset(this->m_pixReverseX, -1, w * 2 * sizeof(int));//赋值-1
    this->m_pixReverseY = new int[w * 2];
    memset(this->m_pixReverseY, -1, w * 2 * sizeof(int));//赋值-1

    this->m_pointCount = 0;//点数

}
CTifAvailBound::CTifAvailBound(unsigned char** ppBuffer, int nBandCount, int w, int h, int bAllocMm/* = true*/, double* pAdfTransForm/* = nullptr*/) {
    this->m_nBandCount = nBandCount;
    this->m_ppBuf = ppBuffer;
    this->m_bufW = w;
    this->m_bufH = h;

    this->m_pGeoX = nullptr;
    this->m_pGeoY = nullptr;

    this->m_pGeoReverseX = nullptr;
    this->m_pGeoReverseY = nullptr;

    if (bAllocMm) {
        this->m_pixX = new int[w * 2];//存储找到的边界点 以X方向遍历，从最左边开始顺时针存储
        memset(this->m_pixX, -1, w * 2 * sizeof(int));//赋值-1
        this->m_pixY = new int[w * 2];
        memset(this->m_pixY, -1, w * 2 * sizeof(int));//赋值-1

        //this->m_pGeoX = new double[w * 2];//存储找到的边界点 以X方向遍历，从最左边开始顺时针存储
        //memset(this->m_pGeoX, -1, w * 2 * sizeof(double));//赋值-1
        //this->m_pGeoY = new double[w * 2];
        //memset(this->m_pGeoY, -1, w * 2 * sizeof(double));//赋值-1

        this->m_pixReverseX = new int[h * 2];//存储找到的边界点 以X方向遍历，从最左边开始顺时针存储
        memset(this->m_pixReverseX, -1, h * 2 * sizeof(int));//赋值-1
        this->m_pixReverseY = new int[h * 2];
        memset(this->m_pixReverseY, -1, h * 2 * sizeof(int));//赋值-1

    }
    if (pAdfTransForm) {
        this->m_pAdfTransForm = new double[6]{ 0 };
        memcpy(this->m_pAdfTransForm, pAdfTransForm, sizeof(double) * 6);
    }
    this->m_pointCount = 0;//点数
    //this->m_nX0 = this->m_nY0 = this->m_nX1 = this->m_nY1 = this->m_nX2 = this->m_nY2 = this->m_nX3 = this->m_nY3 = 0;

    memset(this->m_nArrLeftTopX, 0, sizeof(int) * 8);
    memset(this->m_nArrRightBottomX, 0, sizeof(int) * 8);
}

void CTifAvailBound::computePoints() {
    int ptr;
    int bufValue;
    this->m_pointCount = 0;//点数

    int nPointCount = 0;//点数

    //找上部点成线
    for (int i = 0; i < this->m_bufW; i++) {
        for (int j = 0; j < this->m_bufH; j++) {
            ptr = (j * this->m_bufW + i);//buf数据的指针位置
            bufValue = this->m_buf[ptr];//buf数据灰度值
            if (bufValue != 0) {
                this->m_pixX[this->m_pointCount] = i;//保存有效边界位置
                this->m_pixY[this->m_pointCount] = j;
                this->m_pointCount++;
                this->m_LeftPointCount++;
                break;
            }
        }
    }
    for (int j = 0; j < this->m_bufH; j++) {
        for (int i = 0; i < this->m_bufW; i++) {
            ptr = (i * this->m_bufH + j);//buf数据的指针位置
            bufValue = this->m_buf[ptr];//buf数据灰度值
            if (bufValue != 0) {
                this->m_pixReverseX[nPointCount] = i;//保存有效边界位置
                this->m_pixReverseY[nPointCount] = j;
                nPointCount++;
                break;
            }
        }
    }
    //反向再找一遍（找下部点）
    int dataPos = this->m_bufW;
    for (int i = this->m_bufW - 1; i > 0; i--) {
        for (int j = this->m_bufH - 1; j > 0; j--) {
            ptr = (j * this->m_bufW + i);
            bufValue = this->m_buf[ptr];//该像素的灰度值
            if (bufValue != 0) {
                this->m_pixX[this->m_pointCount] = i;
                this->m_pixY[this->m_pointCount] = j;
                this->m_pointCount++;
                this->m_RightPointCount++;
                break;
            }
        }
    }

    for (int j = this->m_bufH - 1; j > 0; j--) {
        for (int i = this->m_bufW - 1; i > 0; i--) {
            ptr = (i * this->m_bufH + j);
            bufValue = this->m_buf[ptr];//该像素的灰度值
            if (bufValue != 0) {
                this->m_pixReverseX[nPointCount] = i;
                this->m_pixReverseY[nPointCount] = j;
                nPointCount++;
                break;
            }
        }
    }
}
int  CTifAvailBound::getGeoPointByPixPoint(int nPixX, int nPixY, double* _pGeoX, double* _pGeoY) {
    if (this->m_pAdfTransForm == nullptr) {
        *_pGeoX = 0.0;
        *_pGeoY = 0.0;
        return 0;
    }

    *_pGeoX = this->m_pAdfTransForm[0] + nPixX * this->m_pAdfTransForm[1] + nPixY * this->m_pAdfTransForm[2];
    *_pGeoY = this->m_pAdfTransForm[3] + nPixX * this->m_pAdfTransForm[4] + nPixY * this->m_pAdfTransForm[5];

    return 1;
}
void CTifAvailBound::computePointsByMultiBand(int nCalcGeoPoint/* = 0*/) {
    int ptr;
    int bufValue;
    bool bFind = false;
    this->m_pointCount = 0;//点数
    unsigned char* pBuf = nullptr;
    int nPointCount = 0;

    //找上部点成线
    for (int i = 0; i < this->m_bufW; i++) {
        for (int j = 0; j < this->m_bufH; j++) {
            ptr = (j * this->m_bufW + i);//buf数据的指针位置

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    this->m_pixX[this->m_pointCount] = i;//保存有效边界位置
                    this->m_pixY[this->m_pointCount] = j;

                    if (1 == nCalcGeoPoint) {
                        getGeoPointByPixPoint(i, j, &this->m_pGeoX[this->m_pointCount], &this->m_pGeoY[this->m_pointCount]);
                    }

                    this->m_pointCount++;
                    this->m_LeftPointCount++;

                    m_mapTopX[i] = j;

                    if (this->m_pointCount == 1)
                        this->m_nPixMaxX = this->m_nPixMinX = i;
                    else
                        this->m_nPixMaxX = i;
                    //if (this->m_nPixMinX > i)
                    //	this->m_nPixMinX = i;
                    //else if (this->m_nPixMaxX < i)
                    //	this->m_nPixMaxX = i;
                    //
                    //if (this->m_nPixMinY > j)
                    //	this->m_nPixMinY = j;
                    //else if (this->m_nPixMaxY < j)
                    //	this->m_nPixMaxY = j;

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
    }

    this->m_nArrLeftTopX[0] = this->m_pixX[0];	                    this->m_nArrLeftTopX[1] = this->m_pixY[0];
    this->m_nArrLeftTopX[4] = this->m_pixX[this->m_pointCount - 1];	this->m_nArrLeftTopX[5] = this->m_pixY[this->m_pointCount - 1];

    for (int j = 0; j < this->m_bufH; j++) {
        for (int i = 0; i < this->m_bufW; i++) {
            ptr = (j * this->m_bufW + i);//buf数据的指针位置

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    this->m_pixReverseX[nPointCount] = i;//保存有效边界位置
                    this->m_pixReverseY[nPointCount] = j;

                    nPointCount++;
                    this->m_ReverseLeftPointCount++;
                    bFind = true;

                    m_mapLeftY[j] = i;

                    if (nPointCount == 1)
                        this->m_nPixMaxY = this->m_nPixMinY = j;
                    else
                        this->m_nPixMaxY = j;

                    break;
                }
            }
            if (bFind)
                break;
        }
    }
    this->m_nArrLeftTopX[2] = this->m_pixReverseX[0];	            this->m_nArrLeftTopX[3] = this->m_pixReverseY[0];
    this->m_nArrLeftTopX[6] = this->m_pixReverseX[nPointCount - 1];	this->m_nArrLeftTopX[7] = this->m_pixReverseY[nPointCount - 1];

    //反向再找一遍（找下部点）
    int dataPos = this->m_bufW;
    for (int i = this->m_bufW - 1; i >= 0; i--) {
        for (int j = this->m_bufH - 1; j >= 0; j--) {
            ptr = (j * this->m_bufW + i);

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    this->m_pixX[this->m_pointCount] = i;//保存有效边界位置
                    this->m_pixY[this->m_pointCount] = j;

                    if (1 == nCalcGeoPoint) {
                        getGeoPointByPixPoint(i, j, &this->m_pGeoX[this->m_pointCount], &this->m_pGeoY[this->m_pointCount]);
                    }

                    this->m_pointCount++;
                    this->m_RightPointCount++;

                    m_mapBottomX[i] = j;

                    //if (this->m_nPixMinX > i)
                    //	this->m_nPixMinX = i;
                    //else if (this->m_nPixMaxX < i)
                    //	this->m_nPixMaxX = i;
                    //
                    //if (this->m_nPixMinY > j)
                    //	this->m_nPixMinY = j;
                    //else if (this->m_nPixMaxY < j)
                    //	this->m_nPixMaxY = j;

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
    }

    this->m_nArrRightBottomX[0] = this->m_pixX[m_LeftPointCount];	    this->m_nArrRightBottomX[1] = this->m_pixY[m_LeftPointCount];
    this->m_nArrRightBottomX[4] = this->m_pixX[this->m_pointCount - 1];	this->m_nArrRightBottomX[5] = this->m_pixY[this->m_pointCount - 1];

    for (int j = this->m_bufH - 1; j >= 0; j--) {
        for (int i = this->m_bufW - 1; i >= 0; i--) {
            ptr = (j * this->m_bufW + i);

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    this->m_pixReverseX[nPointCount] = i;//保存有效边界位置
                    this->m_pixReverseY[nPointCount] = j;
                    nPointCount++;
                    bFind = true;

                    m_mapRightY[j] = i;
                    break;
                }
            }
            if (bFind)
                break;
        }
    }
    this->m_nArrRightBottomX[2] = this->m_pixReverseX[m_ReverseLeftPointCount];	this->m_nArrRightBottomX[3] = this->m_pixReverseY[m_ReverseLeftPointCount];
    this->m_nArrRightBottomX[6] = this->m_pixReverseX[nPointCount - 1];	        this->m_nArrRightBottomX[7] = this->m_pixReverseY[nPointCount - 1];
}


void CTifAvailBound::computeCornerPointsByMultiBand() {
    int i = 0, j = 0, ptr = 0;
    int bufValue = 0, bufValue1 = 0;
    bool bFind = false;
    this->m_pointCount = 0;//点数
    unsigned char* pBuf = nullptr;
    int nPointCount = 0;


    // 最左边开始  从上到下扫描
    for (i = 0; i < this->m_bufW; i++) {
        for (j = 0; j < this->m_bufH; j++) {
            ptr = (j * this->m_bufW + i);//buf数据的指针位置

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Left_Up_Down.x = i;
                    m_st_Left_Up_Down.y = j;

                    //继续向下找另一端的有效像素点
                    j++;
                    for (; j < this->m_bufH; j++) {
                        ptr = (j * this->m_bufW + i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {
                            ptr = ((j - 1) * this->m_bufW + i);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值
                            m_st_Left_Up_Down1.x = i;
                            m_st_Left_Up_Down1.y = j - 1;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Left_Up_Down1.x = i;
                        m_st_Left_Up_Down1.y = this->m_bufH - 1;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }

    // 最上边开始  从左到右扫描
    for (j = 0; j < this->m_bufH; j++) {
        for (i = 0; i < this->m_bufW; i++) {
            ptr = (j * this->m_bufW + i);//buf数据的指针位置

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Up_Left_Right.x = i;
                    m_st_Up_Left_Right.y = j;

                    //继续向右找另一端的有效像素点
                    i++;
                    for (; i < this->m_bufW; i++) {
                        ptr = (j * this->m_bufW + i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {
                            ptr = (j * this->m_bufW + i - 1);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值
                            m_st_Up_Left_Right1.x = i - 1;
                            m_st_Up_Left_Right1.y = j;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Up_Left_Right1.x = this->m_bufW - 1;
                        m_st_Up_Left_Right1.y = j;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }

    // 最右边开始  从下到上扫描
    for (i = this->m_bufW - 1; i >= 0; i--) {
        for (j = this->m_bufH - 1; j >= 0; j--) {
            ptr = (j * this->m_bufW + i);
            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Right_Down_Up.x = i;
                    m_st_Right_Down_Up.y = j;

                    //继续向上找另一端的有效像素点
                    j--;
                    for (; j >= 0; j--) {
                        ptr = (j * this->m_bufW + i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {

                            ptr = ((j + 1) * this->m_bufW + i);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值

                            m_st_Right_Down_Up1.x = i;
                            m_st_Right_Down_Up1.y = j + 1;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Right_Down_Up1.x = i;
                        m_st_Right_Down_Up1.y = 0;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }

    // 最下边开始  从右到左扫描
    for (j = this->m_bufH - 1; j >= 0; j--) {
        for (i = this->m_bufW - 1; i >= 0; i--) {
            ptr = (j * this->m_bufW + i);

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Down_Right_Left.x = i;
                    m_st_Down_Right_Left.y = j;

                    //继续向上找另一端的有效像素点
                    i--;
                    for (; i >= 0; i--) {
                        ptr = (j * this->m_bufW + i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {

                            ptr = (j * this->m_bufW + i + 1);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值

                            m_st_Down_Right_Left1.x = i + 1;
                            m_st_Down_Right_Left1.y = j;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Down_Right_Left1.x = 0;
                        m_st_Down_Right_Left1.y = j;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }
}

void CTifAvailBound::computeCornerPointsByMultiBand_16Bit() {
    long long i = 0, j = 0;
    unsigned long long ptr = 0;
    int bufValue = 0, bufValue1 = 0;
    bool bFind = false;
    this->m_pointCount = 0;//点数
    unsigned short* pBuf = nullptr;
    int nPointCount = 0;


    // 最左边开始  从上到下扫描
    for (i = 0; i < this->m_bufW; i++) {
        for (j = 0; j < this->m_bufH; j++) {
            //ptr = (j * this->m_bufW + i);//buf数据的指针位置
            ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = (unsigned short*)this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Left_Up_Down.x = i;
                    m_st_Left_Up_Down.y = j;

                    //继续向下找另一端的有效像素点
                    j++;
                    for (; j < this->m_bufH; j++) {
                        ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {
                            ptr = ((j-1) * this->m_bufW + i);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值
                            m_st_Left_Up_Down1.x = i;
                            m_st_Left_Up_Down1.y = j - 1;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Left_Up_Down1.x = i;
                        m_st_Left_Up_Down1.y = this->m_bufH - 1;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }

    // 最上边开始  从左到右扫描
    for (j = 0; j < this->m_bufH; j++) {
        for (i = 0; i < this->m_bufW; i++) {
            //ptr = (j * this->m_bufW + i);//buf数据的指针位置
            ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = (unsigned short*)this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Up_Left_Right.x = i;
                    m_st_Up_Left_Right.y = j;

                    //继续向右找另一端的有效像素点
                    i++;
                    for (; i < this->m_bufW; i++) {
                        //ptr = (j * this->m_bufW + i);//buf数据的指针位置
                        ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {
                            ptr = (j * this->m_bufW + i-1);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值
                            m_st_Up_Left_Right1.x = i - 1;
                            m_st_Up_Left_Right1.y = j;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Up_Left_Right1.x = this->m_bufW - 1;
                        m_st_Up_Left_Right1.y = j;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }

    // 最右边开始  从下到上扫描
    for (i = this->m_bufW - 1; i >= 0; i--) {
        for (j = this->m_bufH - 1; j >= 0; j--) {
            //ptr = (j * this->m_bufW + i);//buf数据的指针位置
            ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置
            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = (unsigned short*)this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Right_Down_Up.x = i;
                    m_st_Right_Down_Up.y = j;

                    //继续向上找另一端的有效像素点
                    j--;
                    for (; j >= 0; j--) {
                        //ptr = (j * this->m_bufW + i);//buf数据的指针位置
                        ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {

                            ptr = ((j+1) * this->m_bufW + i);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值

                            m_st_Right_Down_Up1.x = i;
                            m_st_Right_Down_Up1.y = j + 1;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Right_Down_Up1.x = i;
                        m_st_Right_Down_Up1.y = 0;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }

    // 最下边开始  从右到左扫描
    for (j = this->m_bufH - 1; j >= 0; j--) {
        for (i = this->m_bufW - 1; i >= 0; i--) {
            //ptr = (j * this->m_bufW + i);//buf数据的指针位置
            ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置

            bFind = false;
            for (int nBandIndx = 0; nBandIndx < this->m_nBandCount; nBandIndx++) {
                pBuf = (unsigned short*)this->m_ppBuf[nBandIndx];
                if (pBuf == nullptr)
                    continue;
                bufValue = pBuf[ptr];//buf数据灰度值
                if (bufValue != 0) {
                    m_st_Down_Right_Left.x = i;
                    m_st_Down_Right_Left.y = j;

                    //继续向上找另一端的有效像素点
                    i--;
                    for (; i >= 0; i--) {
                        //ptr = (j * this->m_bufW + i);//buf数据的指针位置
                        ptr = ((unsigned long long)j * (unsigned long long)this->m_bufW + (unsigned long long)i);//buf数据的指针位置
                        bufValue = pBuf[ptr];//buf数据灰度值
                        if (bufValue == 0) {

                            ptr = (j * this->m_bufW + i + 1);//buf数据的指针位置
                            bufValue1 = pBuf[ptr];//buf数据灰度值

                            m_st_Down_Right_Left1.x = i + 1;
                            m_st_Down_Right_Left1.y = j;
                            break;
                        }
                    }
                    if (bufValue != 0) {
                        m_st_Down_Right_Left1.x = 0;
                        m_st_Down_Right_Left1.y = j;
                    }

                    bFind = true;
                    break;
                }
            }
            if (bFind)
                break;
        }
        if (bFind)
            break;
    }
}

int  CTifAvailBound::initByCacheFile(const char* szCacheFile) {
    if (false == CMisc::CheckFileExists(szCacheFile))
        return 0;
    FILE* pf = fopen(szCacheFile, "rb");
    if (nullptr == pf)
        return 0;
    fseek(pf, 0, SEEK_END);
    int nFileLen = ftell(pf);
    if (nFileLen <= 0) {
        fclose(pf);
        return 0;
    }

    rewind(pf);

    //char* szCacheBuf = new char[nFileLen];
    //fread(szCacheBuf, sizeof(char) * nFileLen, 1, pf);
    //int nFilePtr = 0;
    //int nStep = 0;
    //this->m_nBandCount = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_bufW = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_bufH = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_pointCount = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_LeftPointCount = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_RightPointCount = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_ReverseLeftPointCount = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_ReverseRightPointCount = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_nPixMinX = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_nPixMinY = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_nPixMaxX = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //this->m_nPixMaxY = *(int*)((char*)szCacheBuf + nStep); nStep+=4;
    //
    //memcpy(this->m_nArrLeftTopX, ((char*)szCacheBuf + nStep), sizeof(int) * 8); nStep += 32;
    //memcpy(this->m_nArrRightBottomX, ((char*)szCacheBuf + nStep), sizeof(int) * 8); nStep += 32;
    //
    //this->m_pixX = new int[this->m_pointCount];
    //this->m_pixY = new int[this->m_pointCount];
    //this->m_pixReverseX = new int[this->m_pointCount];
    //this->m_pixReverseY = new int[this->m_pointCount];
    //
    //memcpy(this->m_pixX, ((char*)szCacheBuf + nStep), sizeof(int) * this->m_pointCount); nStep += (sizeof(int) * this->m_pointCount);
    //memcpy(this->m_pixY, ((char*)szCacheBuf + nStep), sizeof(int) * this->m_pointCount); nStep += (sizeof(int) * this->m_pointCount);
    //memcpy(this->m_pixReverseX, ((char*)szCacheBuf + nStep), sizeof(int) * this->m_pointCount); nStep += (sizeof(int) * this->m_pointCount);
    //memcpy(this->m_pixReverseY, ((char*)szCacheBuf + nStep), sizeof(int) * this->m_pointCount); nStep += (sizeof(int) * this->m_pointCount);



    fread(&this->m_nBandCount, sizeof(int), 1, pf);
    fread(&this->m_bufW, sizeof(int), 1, pf);
    fread(&this->m_bufH, sizeof(int), 1, pf);

    fread(&this->m_pointCount, sizeof(int), 1, pf);
    fread(&this->m_LeftPointCount, sizeof(int), 1, pf);
    fread(&this->m_RightPointCount, sizeof(int), 1, pf);
    fread(&this->m_ReverseLeftPointCount, sizeof(int), 1, pf);
    fread(&this->m_ReverseRightPointCount, sizeof(int), 1, pf);

    fread(&this->m_nPixMinX, sizeof(int), 1, pf);
    fread(&this->m_nPixMinY, sizeof(int), 1, pf);
    fread(&this->m_nPixMaxX, sizeof(int), 1, pf);
    fread(&this->m_nPixMaxY, sizeof(int), 1, pf);

    fread(this->m_nArrLeftTopX, sizeof(int) * 8, 1, pf);
    fread(this->m_nArrRightBottomX, sizeof(int) * 8, 1, pf);
    this->m_pixX = new int[this->m_pointCount];
    this->m_pixY = new int[this->m_pointCount];
    this->m_pixReverseX = new int[this->m_pointCount];
    this->m_pixReverseY = new int[this->m_pointCount];
    fread(this->m_pixX, sizeof(int) * this->m_pointCount, 1, pf);
    fread(this->m_pixY, sizeof(int) * this->m_pointCount, 1, pf);
    fread(this->m_pixReverseX, sizeof(int) * this->m_pointCount, 1, pf);
    fread(this->m_pixReverseY, sizeof(int) * this->m_pointCount, 1, pf);

    int nGeoFlag = 0;
    char szGeoPtFlat[8] = { 0 };
    //memcpy(szGeoPtFlat, ((char*)szCacheBuf + nStep), 8);
    fread(szGeoPtFlat, 8, 1, pf);
    //fread(&nGeoFlag, 8, 1, pf);
    if (strcmp("EXGEOPT", szGeoPtFlat) == 0/*nGeoFlag == 1*/) {
        this->m_pGeoX = new double[this->m_pointCount];
        this->m_pGeoY = new double[this->m_pointCount];
        fread(this->m_pGeoX, sizeof(double) * this->m_pointCount, 1, pf);
        fread(this->m_pGeoY, sizeof(double) * this->m_pointCount, 1, pf);
    }
    fclose(pf);

    int i = 0;
    this->m_mapTopX.clear();
    for (i = 0; i < this->m_LeftPointCount; i++) {
        this->m_mapTopX[this->m_pixX[i]] = this->m_pixY[i];
    }
    this->m_mapBottomX.clear();
    for (i = this->m_LeftPointCount; i < this->m_pointCount; i++) {
        this->m_mapBottomX[this->m_pixX[i]] = this->m_pixY[i];
    }
    for (i = 0; i < this->m_ReverseLeftPointCount; i++) {
        this->m_mapLeftY[this->m_pixReverseY[i]] = this->m_pixReverseX[i];
    }
    this->m_mapBottomX.clear();
    for (i = this->m_ReverseLeftPointCount; i < this->m_pointCount; i++) {
        this->m_mapRightY[this->m_pixReverseY[i]] = this->m_pixReverseX[i];
    }
    return 1;
}
int  CTifAvailBound::saveCacheFile(const char* szCacheFile) {
    FILE* pf = fopen(szCacheFile, "wb");
    if (nullptr == pf)
        return 0;
    fwrite(&this->m_nBandCount, sizeof(int), 1, pf);
    fwrite(&this->m_bufW, sizeof(int), 1, pf);
    fwrite(&this->m_bufH, sizeof(int), 1, pf);
    fwrite(&this->m_pointCount, sizeof(int), 1, pf);
    fwrite(&this->m_LeftPointCount, sizeof(int), 1, pf);
    fwrite(&this->m_RightPointCount, sizeof(int), 1, pf);
    fwrite(&this->m_ReverseLeftPointCount, sizeof(int), 1, pf);
    fwrite(&this->m_ReverseRightPointCount, sizeof(int), 1, pf);
    fwrite(&this->m_nPixMinX, sizeof(int), 1, pf);
    fwrite(&this->m_nPixMinY, sizeof(int), 1, pf);
    fwrite(&this->m_nPixMaxX, sizeof(int), 1, pf);
    fwrite(&this->m_nPixMaxY, sizeof(int), 1, pf);

    fwrite(this->m_nArrLeftTopX, sizeof(int) * 8, 1, pf);
    fwrite(this->m_nArrRightBottomX, sizeof(int) * 8, 1, pf);
    fwrite(this->m_pixX, sizeof(int) * this->m_pointCount, 1, pf);
    fwrite(this->m_pixY, sizeof(int) * this->m_pointCount, 1, pf);
    fwrite(this->m_pixReverseX, sizeof(int) * this->m_pointCount, 1, pf);
    fwrite(this->m_pixReverseY, sizeof(int) * this->m_pointCount, 1, pf);

    int nGeoFlag = 0;
    if (this->m_pGeoX && this->m_pGeoY) {
        nGeoFlag = 1;
        char szGeoPtFlat[] = "EXGEOPT\0\0\0";
        fwrite(szGeoPtFlat, 8, 1, pf);
        //fwrite(&nGeoFlag, sizeof(int), 1, pf);
        fwrite(this->m_pGeoX, sizeof(double) * this->m_pointCount, 1, pf);
        fwrite(this->m_pGeoY, sizeof(double) * this->m_pointCount, 1, pf);
    }
    else {
        nGeoFlag = 0;
        char szGeoPtFlat[] = "NOGEOPT";
        fwrite(szGeoPtFlat, 8, 1, pf);
        //fwrite(&nGeoFlag, sizeof(int), 1, pf);
    }
    fclose(pf);
    return 1;
}

//结果返回
int* CTifAvailBound::xArray() {//x数组
    return this->m_pixX;
}
//结果返回
int* CTifAvailBound::yArray() {//y数组
    return this->m_pixY;
}
//结果返回
int* CTifAvailBound::xArrayReverse() {//x数组
    return this->m_pixReverseX;
}
//结果返回
int* CTifAvailBound::yArrayReverse() {//y数组
    return this->m_pixReverseY;
}
//结果返回
int CTifAvailBound::size() {//点数
    return this->m_pointCount;
}
int CTifAvailBound::leftPointCount() {
    //左半部分有效点个数
    return this->m_LeftPointCount;
}
int CTifAvailBound::rightPointCount() {
    //左半部分有效点个数
    return this->m_RightPointCount;
}

int CTifAvailBound::ReverseleftPointCount() {
    //左半部分有效点个数
    return this->m_ReverseLeftPointCount;
}
int CTifAvailBound::ReverserightPointCount() {
    //左半部分有效点个数
    return this->m_ReverseRightPointCount;
}

int CTifAvailBound::ConvertPix2Geo(CGeoTiff* pGeoTiff) {
    if (this->m_pGeoX == nullptr || this->m_pGeoY == nullptr) {
        if (this->m_pGeoX)
            delete[] this->m_pGeoX;
        if (this->m_pGeoY)
            delete[] this->m_pGeoY;
        this->m_pGeoX = new double[this->m_pointCount];
        this->m_pGeoY = new double[this->m_pointCount];
        for (int i = 0; i < this->m_pointCount; i++) {
            //pGeoTiff->transPos(GeoTiffSrcPix2Geo, this->m_pixX[i], this->m_pixY[i], this->m_pGeoX[i], this->m_pGeoY[i]);
        }
    }
    return this->m_pointCount;
}

int CTifAvailBound::ConvertPix2Geo(double* pAdfTransForm) {
    if (this->m_pAdfTransForm == nullptr) {
        this->m_pAdfTransForm = new double[6]{ 0 };
        memcpy(this->m_pAdfTransForm, pAdfTransForm, sizeof(double) * 6);
    }
    if (this->m_pGeoX == nullptr || this->m_pGeoY == nullptr) {
        if (this->m_pGeoX)
            delete[] this->m_pGeoX;
        if (this->m_pGeoY)
            delete[] this->m_pGeoY;
        this->m_pGeoX = new double[this->m_pointCount];
        this->m_pGeoY = new double[this->m_pointCount];
        for (int i = 0; i < this->m_pointCount; i++) {
            this->m_pGeoX[i] = this->m_pAdfTransForm[0] + this->m_pixX[i] * this->m_pAdfTransForm[1] + this->m_pixY[i] * this->m_pAdfTransForm[2];
            this->m_pGeoY[i] = this->m_pAdfTransForm[3] + this->m_pixX[i] * this->m_pAdfTransForm[4] + this->m_pixY[i] * this->m_pAdfTransForm[5];
        }
    }
    return this->m_pointCount;
}

int CTifAvailBound::ConvertPix2Geo(double* pAdfTransForm, double** _ppGeoPoints){
    if (this->m_pAdfTransForm == nullptr) {
        this->m_pAdfTransForm = new double[6]{ 0 };
        memcpy(this->m_pAdfTransForm, pAdfTransForm, sizeof(double) * 6);
    }
    if (this->m_pGeoX == nullptr || this->m_pGeoY == nullptr) {
        if (this->m_pGeoX)
            delete[] this->m_pGeoX;
        if (this->m_pGeoY)
            delete[] this->m_pGeoY;
        this->m_pGeoX = new double[this->m_pointCount];
        this->m_pGeoY = new double[this->m_pointCount];

        double* pGeoPoints = new double[this->m_pointCount * 3];
        for (int i = 0; i < this->m_pointCount; i++) {
            this->m_pGeoX[i] = this->m_pAdfTransForm[0] + this->m_pixX[i] * this->m_pAdfTransForm[1] + this->m_pixY[i] * this->m_pAdfTransForm[2];
            this->m_pGeoY[i] = this->m_pAdfTransForm[3] + this->m_pixX[i] * this->m_pAdfTransForm[4] + this->m_pixY[i] * this->m_pAdfTransForm[5];
            pGeoPoints[i * 3] = this->m_pGeoX[i];
            pGeoPoints[i * 3 + 1] = this->m_pGeoY[i];
            pGeoPoints[i * 3 + 2] = 0.0;
        }
        *_ppGeoPoints = pGeoPoints;
    }
    return this->m_pointCount;
}
