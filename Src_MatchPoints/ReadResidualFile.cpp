#include "ReadResidualFile.h"

#include <stdio.h>
#include <algorithm>
#include <fstream>

#include "Src_Core/Misc.h"
#include "Src_MatchPoints/Class_RPCs.h"
#include "Src_Geometry/CReadRaster.h"

////////////////////////////////////////////////////////
void CResidualInfo::GetRasterQuad(const std::string& sRpcFileFullPath, double* pQuadCoord, unsigned long long _width, unsigned long long _height,  double& _resolution)
{
    Class_RPCs t_rpc;
    t_rpc.initial_RPCs(sRpcFileFullPath);
    double doutopleftlatitude = t_rpc.m_LAT_OFF;
    double doutopleftlongitude = t_rpc.m_LONG_OFF;
    double doutoprightlatitude = t_rpc.m_LAT_OFF;
    double doutoprightlongitude = t_rpc.m_LONG_OFF;
    double doubottomrightlatitude = t_rpc.m_LAT_OFF;
    double doubottomrightlongitude = t_rpc.m_LONG_OFF;
    double doubottomleftlatitude = t_rpc.m_LAT_OFF;
    double doubottomleftlongitude = t_rpc.m_LONG_OFF;


    double t_height = t_rpc.m_HEIGHT_OFF;

    t_rpc.Calculate_Ground_XYZ(0, 0, doutopleftlatitude, doutopleftlongitude, t_height);
    t_rpc.Calculate_Ground_XYZ(_width, 0, doutoprightlatitude, doutoprightlongitude, t_height);
    t_rpc.Calculate_Ground_XYZ(_width, _height, doubottomrightlatitude, doubottomrightlongitude, t_height);
    t_rpc.Calculate_Ground_XYZ(0, _height, doubottomleftlatitude, doubottomleftlongitude, t_height);


    double _xresolution = ((doutoprightlongitude - doutopleftlongitude)/_width + (doubottomrightlongitude - doubottomleftlongitude)/_width)/2.0;
    double _yresolution = ((doutopleftlatitude - doubottomleftlatitude)/_height + (doutoprightlatitude - doubottomrightlatitude)/_height)/2.0;
    _resolution = (_xresolution + _yresolution)/2.0;

    int nIndx = 0;
    pQuadCoord[nIndx++] = doubottomleftlongitude;
    pQuadCoord[nIndx++] = doubottomleftlatitude;

    pQuadCoord[nIndx++] = doutopleftlongitude;
    pQuadCoord[nIndx++] = doutopleftlatitude;

    pQuadCoord[nIndx++] = doutoprightlongitude;
    pQuadCoord[nIndx++] = doutoprightlatitude;

    pQuadCoord[nIndx++] = doubottomrightlongitude;
    pQuadCoord[nIndx++] = doubottomrightlatitude;

    //FILE* fpresult = fopen(_extPathName.c_str(), ("w"));
    //if(fpresult == nullptr)
    //    return;
    //
    //fprintf(fpresult, "  %12.8f", doutopleftlongitude);
    //fprintf(fpresult, "  %12.8f\n", doutopleftlatitude);
    //
    //fprintf(fpresult, "  %12.8f", doutoprightlongitude);
    //fprintf(fpresult, "  %12.8f\n", doutoprightlatitude);
    //
    //fprintf(fpresult, "  %12.8f", doubottomrightlongitude);
    //fprintf(fpresult, "  %12.8f\n", doubottomrightlatitude);
    //
    //fprintf(fpresult, "  %12.8f", doubottomleftlongitude);
    //fprintf(fpresult, "  %12.8f\n", doubottomleftlatitude);
    //fclose(fpresult);
}

////////////////////////////////////////////////////////
CResidualInfo::CResidualInfo() {

}

CResidualInfo::~CResidualInfo() {
    this->ClearFileInfo();
    this->Clear();
}

void CResidualInfo::ClearFileInfo(){
    for(auto item : this->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor){
        delete item;
        item = nullptr;
    }
    this->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor.clear();
    this->m_RpcAdjustFactorBlock.m_mapRpcAdjustFactor2VecIndx.clear();
}

void CResidualInfo::Clear(){
    int i = 0, nVecSize = 0;
    nVecSize = (int)this->m_LianJieDianZuoBiaoBlock.m_vecLianJieDianZuoBiao.size();
    for(auto item : this->m_LianJieDianZuoBiaoBlock.m_vecLianJieDianZuoBiao){
        delete item;
        item = nullptr;
    }
    this->m_LianJieDianZuoBiaoBlock.m_vecLianJieDianZuoBiao.clear();
    this->m_LianJieDianZuoBiaoBlock.m_mapLianJieDianZuoBiao2VecIndx.clear();

    for(auto item : this->m_LianJieDianXiangFangWuChaBlock.m_vecLianJieDianXiangFangWuCha){
        delete item;
        item = nullptr;
    }
    this->m_LianJieDianXiangFangWuChaBlock.m_vecLianJieDianXiangFangWuCha.clear();
    this->m_LianJieDianXiangFangWuChaBlock.m_mapLianJieDianXiangFangWuCha2VecIndx.clear();

    //////////////////////////////////////
    for(auto item : this->m_ControlDianPrecisionBlock.m_vecControlDianPrecision){
        delete item;
        item = nullptr;
    }
    this->m_ControlDianPrecisionBlock.m_vecControlDianPrecision.clear();
    this->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx.clear();

    for(auto item : this->m_ControlDianXiangFangWuChaBlock.m_vecControlDianXiangFangWuCha){
        delete item;
        item = nullptr;
    }
    this->m_ControlDianXiangFangWuChaBlock.m_vecControlDianXiangFangWuCha.clear();
    this->m_ControlDianXiangFangWuChaBlock.m_mapControlDianXiangFangWuCha2VecIndx.clear();

    //////////////////////////////////////
    for(auto item : this->m_CheckDianPrecisionBlock.m_vecCheckDianPrecision){
        delete item;
        item = nullptr;
    }
    this->m_CheckDianPrecisionBlock.m_vecCheckDianPrecision.clear();
    this->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx.clear();

    for(auto item : this->m_CheckDianXiangFangWuChaBlock.m_vecCheckDianXiangFangWuCha){
        delete item;
        item = nullptr;
    }
    this->m_CheckDianXiangFangWuChaBlock.m_vecCheckDianXiangFangWuCha.clear();
    this->m_CheckDianXiangFangWuChaBlock.m_mapCheckDianXiangFangWuCha2VecIndx.clear();

}

std::string& CResidualInfo::Replace_str(std::string& str, const std::string& to_replaced, const std::string& newchars)
{
    for (std::string::size_type pos(0); pos != std::string::npos; pos += newchars.length())
    {
        pos = str.find(to_replaced, pos);
        if (pos != std::string::npos)
            str.replace(pos, to_replaced.length(), newchars);
        else
            break;
    }
    return str;
}

int CResidualInfo::SplitString(std::vector<std::string>& resultStringVector, const std::string& srcStrIn, const std::string& delimStr, bool repeatedCharIgnored)
{
    resultStringVector.clear();
    std::string srcStr = srcStrIn;

    std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {
        if (delimStr.find(c) != std::string::npos) {
            return true;
        }
        else {
            return false;
        } }/*pred*/
        , delimStr.at(0));//将出现的所有分隔符都替换成为一个相同的字符（分隔符字符串的第一个）
    size_t pos = srcStr.find(delimStr.at(0));
    std::string addedString = "";
    while (pos != std::string::npos) {
        addedString = srcStr.substr(0, pos);
        if (!addedString.empty() || !repeatedCharIgnored) {
            resultStringVector.push_back(addedString);
        }
        srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
        pos = srcStr.find(delimStr.at(0));
    }
    addedString = srcStr;
    if (!addedString.empty() || !repeatedCharIgnored) {
        resultStringVector.push_back(addedString);
    }
    return (int)resultStringVector.size();
}

int CResidualInfo::LoadResidualFile(const std::string& sResidualFileFullPath) {    
    std::fstream fReadFile;

    std::string sReadLine;
    std::string sTifName, sTifFileFullPath, sRpcFileFullPath, sPxyFileFullPath, sGcpFileFullPath;
    std::vector<std::string> vecTifFile;
    std::string sTifFileInfoFullPath = sResidualFileFullPath;
    sTifFileInfoFullPath = sResidualFileFullPath.substr(0, sResidualFileFullPath.rfind('.'));
    sTifFileInfoFullPath = sTifFileInfoFullPath + "_TifInfo.txt";
    fReadFile.open(sTifFileInfoFullPath.c_str(), std::ios::in);
    CRpcAdjustFactor* pRpcAdjustFactor = nullptr;
    while (std::getline(fReadFile, sReadLine)) {
        if(sReadLine == "")
            continue;
        if( 0 == CMisc::CheckFileExists(sReadLine.c_str())){
            continue;
        }
        sTifName = sReadLine;
        sTifName = sTifName.substr(sTifName.rfind('/') + 1);
        sTifName = sTifName.substr(0, sTifName.rfind('.'));

        sTifFileFullPath = sReadLine;

        sRpcFileFullPath = sReadLine;
        sRpcFileFullPath = sRpcFileFullPath.substr(0, sRpcFileFullPath.rfind('.'))+"_rpc.txt";

        sPxyFileFullPath = sReadLine;
        sPxyFileFullPath = sPxyFileFullPath.substr(0, sPxyFileFullPath.rfind('.'))+".pxy";

        sGcpFileFullPath = sReadLine;
        sGcpFileFullPath = sGcpFileFullPath.substr(0, sGcpFileFullPath.rfind('.'))+".gcp";

        CReadRaster readTif(sTifFileFullPath);
        pRpcAdjustFactor = new CRpcAdjustFactor;
        pRpcAdjustFactor->m_nPixWidth = readTif.m_nPixWidth;
        pRpcAdjustFactor->m_nPixHeight = readTif.m_nPixHeight;
        this->GetRasterQuad(sRpcFileFullPath
                               , pRpcAdjustFactor->m_arrQuadCoord
                               , pRpcAdjustFactor->m_nPixWidth
                               , pRpcAdjustFactor->m_nPixHeight
                               , pRpcAdjustFactor->m_fPixResolution);

        pRpcAdjustFactor->m_sTifName = sTifName;
        pRpcAdjustFactor->m_sTifFileFullPath = sTifFileFullPath;
        pRpcAdjustFactor->m_sRpcFileFullPath = sRpcFileFullPath;
        pRpcAdjustFactor->m_sPxyFileFullPath = sPxyFileFullPath;
        pRpcAdjustFactor->m_sGcpFileFullPath = sGcpFileFullPath;

        this->m_RpcAdjustFactorBlock.m_vecRpcAdjustFactor.push_back(pRpcAdjustFactor);
        this->m_RpcAdjustFactorBlock.m_mapRpcAdjustFactor2VecIndx.insert(
            std::pair<std::string, CRpcAdjustFactor*>(sTifFileFullPath, pRpcAdjustFactor));
        this->m_RpcAdjustFactorBlock.m_mapRpcAdjustFactorName2VecIndx.insert(
            std::pair<std::string, CRpcAdjustFactor*>(sTifName, pRpcAdjustFactor));
    }
    fReadFile.close();
    //////////////////////////////////////////////////////////////////////////////////

    fReadFile.open(sResidualFileFullPath, std::ios::in);
    if(fReadFile.is_open() == false){
        printf("该文件打开失败！", sResidualFileFullPath.c_str());
        return 0;
    }
    std::string sFlag01 = "RPC改正数";
    //>>>>>>>>>>>>>>>>>>>>>>>
    std::string sFlag02 = "连接点坐标";
    std::string sFlag03 = "连接点的像方误差（pixel）";
    std::string sFlag04 = "连接点的像方中误差（pixel）";
    std::string sFlag05 = "连接点最大像方误差（pixel）";
    //>>>>>>>>>>>>>>>>>>>>>>>
    std::string sFlag06 = "控制点的精度";
    std::string sFlag07 = "控制点物方精度（m）";
    std::string sFlag08 = "控制点物方最大误差（m）";
    std::string sFlag09 = "控制点的像方误差（pixel）";
    std::string sFlag10 = "控制点的像方中误差（pixel）";
    //>>>>>>>>>>>>>>>>>>>>>>>
    std::string sFlag11 = "检查点的精度";
    std::string sFlag12 = "检查点物方精度（m）";
    std::string sFlag13 = "检查点物方最大误差（m）";
    std::string sFlag14 = "检查点的像方误差（pixel）";
    std::string sFlag15 = "检查点的像方中误差（pixel）";
    //>>>>>>>>>>>>>>>>>>>>>>>

    int nFind = 1;
    int nFileReadLineCount = 0;
    std::string sDianHao;
    int nVecSplitCnt = 0, nItem = 0;
    std::vector<std::string> vecSplitStr;
    CLianJieDianZuoBiao* pLianJieDianZuoBiao = nullptr;
    auto ittEndLianJieDian = this->m_LianJieDianZuoBiaoBlock.m_mapLianJieDianZuoBiao2VecIndx.end();
    auto ittEndControlDian = this->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx.end();
    auto ittEndCheckDian = this->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx.end();
    while (std::getline(fReadFile, sReadLine)) {
        nFileReadLineCount++;
        if (sReadLine == "")
            continue;
        if (sReadLine.find(sFlag01) != std::string::npos) {
            printf("%02d %s\n\n", nFind++, sFlag01.c_str());
        }
        else if (sReadLine.find(sFlag02) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag02.c_str());

            while (std::getline(fReadFile, sReadLine)) {
                nFileReadLineCount++;
                if (sReadLine == "")
                    break;
                nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
                if (nVecSplitCnt < 10) {
                    printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                    continue;
                }

                pLianJieDianZuoBiao = new CLianJieDianZuoBiao;

                nItem = 0;
                pLianJieDianZuoBiao->m_sDianHao = vecSplitStr[nItem++];
                pLianJieDianZuoBiao->m_nDianHao = strtoull(pLianJieDianZuoBiao->m_sDianHao.c_str(), 0, 10);

                pLianJieDianZuoBiao->m_fLng = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fLngDu = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fLngFen = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fLngMiao = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fLat = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fLatDu = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fLatFen = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fLatMiao = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianZuoBiao->m_fHeight = atof(vecSplitStr[nItem++].c_str());

                this->m_LianJieDianZuoBiaoBlock.m_vecLianJieDianZuoBiao.push_back(pLianJieDianZuoBiao);
                this->m_LianJieDianZuoBiaoBlock.m_mapLianJieDianZuoBiao2VecIndx.insert(
                    std::pair<unsigned long long, CLianJieDianZuoBiao*>(pLianJieDianZuoBiao->m_nDianHao, pLianJieDianZuoBiao));
            }

            ittEndLianJieDian = this->m_LianJieDianZuoBiaoBlock.m_mapLianJieDianZuoBiao2VecIndx.end();
            printf("   %s itemCount:%d   mapCount:%d\n\n", sFlag02.c_str()
                , (int)this->m_LianJieDianZuoBiaoBlock.m_vecLianJieDianZuoBiao.size()
                , (int)this->m_LianJieDianZuoBiaoBlock.m_mapLianJieDianZuoBiao2VecIndx.size());

        }
        else if (sReadLine.find(sFlag03) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag03.c_str());

            std::vector<CLianJieDianXiangFangWuCha*>* pVecDianHao2Indx = NULL;
            CLianJieDianXiangFangWuCha* pLianJieDianXiangFangWuCha = nullptr;
            while (std::getline(fReadFile, sReadLine)) {
                nFileReadLineCount++;
                if (sReadLine == "")
                    break;
                nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
                if (nVecSplitCnt < 8) {
                    printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                    continue;
                }

                pLianJieDianXiangFangWuCha = new CLianJieDianXiangFangWuCha;

                nItem = 0;
                pLianJieDianXiangFangWuCha->m_sDianHao = vecSplitStr[nItem++];
                pLianJieDianXiangFangWuCha->m_nDianHao = strtoull(pLianJieDianXiangFangWuCha->m_sDianHao.c_str(), 0, 10);
                pLianJieDianXiangFangWuCha->m_sTifName = vecSplitStr[nItem++];
                pLianJieDianXiangFangWuCha->m_fPix_X = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianXiangFangWuCha->m_fPix_Y = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianXiangFangWuCha->m_fPix_Residual_X = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianXiangFangWuCha->m_fPix_Residual_Y = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianXiangFangWuCha->m_fPix_Residual_Plane = atof(vecSplitStr[nItem++].c_str());
                pLianJieDianXiangFangWuCha->m_fWeight = atof(vecSplitStr[nItem++].c_str());

                auto itt = this->m_LianJieDianXiangFangWuChaBlock.m_mapLianJieDianXiangFangWuCha2VecIndx.find(pLianJieDianXiangFangWuCha->m_nDianHao);
                if (itt == this->m_LianJieDianXiangFangWuChaBlock.m_mapLianJieDianXiangFangWuCha2VecIndx.end()) {
                    std::vector<CLianJieDianXiangFangWuCha*> vecIndx;
                    this->m_LianJieDianXiangFangWuChaBlock.m_mapLianJieDianXiangFangWuCha2VecIndx.insert(
                        std::pair<unsigned long long, std::vector<CLianJieDianXiangFangWuCha*>>(pLianJieDianXiangFangWuCha->m_nDianHao, vecIndx));

                    pVecDianHao2Indx = &(this->m_LianJieDianXiangFangWuChaBlock.m_mapLianJieDianXiangFangWuCha2VecIndx[pLianJieDianXiangFangWuCha->m_nDianHao]);
                }
                else {
                    pVecDianHao2Indx = &(itt->second);
                }
                pVecDianHao2Indx->push_back(pLianJieDianXiangFangWuCha);

                auto itt1 = this->m_LianJieDianZuoBiaoBlock.m_mapLianJieDianZuoBiao2VecIndx.find(pLianJieDianXiangFangWuCha->m_nDianHao);
                if(ittEndLianJieDian != itt1){
                    pLianJieDianZuoBiao = itt1->second;
                    if(pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1 == nullptr){
                        pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha1 = pLianJieDianXiangFangWuCha;
                    }else{
                        pLianJieDianZuoBiao->m_pLianJieDianXiangFangWuCha2 = pLianJieDianXiangFangWuCha;
                    }
                }

                this->m_LianJieDianXiangFangWuChaBlock.m_vecLianJieDianXiangFangWuCha.push_back(pLianJieDianXiangFangWuCha);
            }
            printf("   %s itemCount:%d  mapCount:%d\n\n", sFlag03.c_str()
                , (int)this->m_LianJieDianXiangFangWuChaBlock.m_vecLianJieDianXiangFangWuCha.size()
                , (int)this->m_LianJieDianXiangFangWuChaBlock.m_mapLianJieDianXiangFangWuCha2VecIndx.size());
        }
        else if (sReadLine.find(sFlag04) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag04.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 3) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_LianJieDianXiangFangZhongWuCha.m_fPix_Residual_X = atof(vecSplitStr[nItem++].c_str());
            m_LianJieDianXiangFangZhongWuCha.m_fPix_Residual_Y = atof(vecSplitStr[nItem++].c_str());
            m_LianJieDianXiangFangZhongWuCha.m_fPix_Residual_Plane = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag04.c_str());
        }
        else if (sReadLine.find(sFlag05) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag05.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 3) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_LianJieDianXiangFangMaxWuCha.m_fMax_Pix_Residual_X = atof(vecSplitStr[nItem++].c_str());
            m_LianJieDianXiangFangMaxWuCha.m_fMax_Pix_Residual_Y = atof(vecSplitStr[nItem++].c_str());
            m_LianJieDianXiangFangMaxWuCha.m_fMax_Pix_Residual_Plane = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag05.c_str());
        }
        else if (sReadLine.find(sFlag06) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag06.c_str());

            CControlDianPrecision* pControlDianPrecision = nullptr;
            while (std::getline(fReadFile, sReadLine)) {
                nFileReadLineCount++;
                if (sReadLine == "")
                    break;
                nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
                if (nVecSplitCnt < 9) {
                    printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                    continue;
                }

                pControlDianPrecision = new CControlDianPrecision;

                nItem = 0;
                pControlDianPrecision->m_sDianHao = vecSplitStr[nItem++];
                sDianHao = pControlDianPrecision->m_sDianHao;
                this->Replace_str(sDianHao, "GCP", "");
                pControlDianPrecision->m_nDianHao = strtoull(sDianHao.c_str(), 0, 10);
                pControlDianPrecision->m_fLng = atof(vecSplitStr[nItem++].c_str());
                pControlDianPrecision->m_fLat = atof(vecSplitStr[nItem++].c_str());
                pControlDianPrecision->m_fHeight = atof(vecSplitStr[nItem++].c_str());
                pControlDianPrecision->m_fResidual_X = atof(vecSplitStr[nItem++].c_str());
                pControlDianPrecision->m_fResidual_Y = atof(vecSplitStr[nItem++].c_str());
                pControlDianPrecision->m_fResidual_Plane = atof(vecSplitStr[nItem++].c_str());
                pControlDianPrecision->m_fResidual_Height = atof(vecSplitStr[nItem++].c_str());
                pControlDianPrecision->m_fAngle = atof(vecSplitStr[nItem++].c_str());

                this->m_ControlDianPrecisionBlock.m_vecControlDianPrecision.push_back(pControlDianPrecision);
                this->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx.insert(
                    std::pair<unsigned long long, CControlDianPrecision*>(pControlDianPrecision->m_nDianHao, pControlDianPrecision));
            }
            ittEndControlDian = this->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx.end();
            printf("   %s itemCount:%d  mapCount:%d\n\n", sFlag06.c_str()
                , (int)this->m_ControlDianPrecisionBlock.m_vecControlDianPrecision.size()
                , (int)this->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx.size());

        }
        else if (sReadLine.find(sFlag07) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag07.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 4) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_ControlDianGroundPrecision.m_fResidual_X = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianGroundPrecision.m_fResidual_Y = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianGroundPrecision.m_fResidual_Plane = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianGroundPrecision.m_fResidual_Height = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag07.c_str());
        }
        else if (sReadLine.find(sFlag08) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag08.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 4) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_ControlDianGroundMaxWuCha.m_fMax_Residual_X = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianGroundMaxWuCha.m_fMax_Residual_Y = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianGroundMaxWuCha.m_fMax_Residual_Plane = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianGroundMaxWuCha.m_fMax_Residual_Height = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag08.c_str());
        }
        else if (sReadLine.find(sFlag09) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag09.c_str());

            CControlDianXiangFangWuCha* pControlDianXiangFangWuCha = nullptr;
            while (std::getline(fReadFile, sReadLine)) {
                nFileReadLineCount++;
                if (sReadLine == "")
                    break;
                nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
                if (nVecSplitCnt < 7) {
                    printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                    continue;
                }

                pControlDianXiangFangWuCha = new CControlDianXiangFangWuCha;

                nItem = 0;

                pControlDianXiangFangWuCha->m_sDianHao = vecSplitStr[nItem++];
                sDianHao = pControlDianXiangFangWuCha->m_sDianHao;
                this->Replace_str(sDianHao, "GCP", "");
                pControlDianXiangFangWuCha->m_nDianHao = strtoull(sDianHao.c_str(), 0, 10);
                pControlDianXiangFangWuCha->m_sTifName = vecSplitStr[nItem++];
                pControlDianXiangFangWuCha->m_fPix_X = atof(vecSplitStr[nItem++].c_str());
                pControlDianXiangFangWuCha->m_fPix_Y = atof(vecSplitStr[nItem++].c_str());
                pControlDianXiangFangWuCha->m_fPix_Residual_X = atof(vecSplitStr[nItem++].c_str());
                pControlDianXiangFangWuCha->m_fPix_Residual_Y = atof(vecSplitStr[nItem++].c_str());
                pControlDianXiangFangWuCha->m_fWeight = atof(vecSplitStr[nItem++].c_str());

                this->m_ControlDianXiangFangWuChaBlock.m_vecControlDianXiangFangWuCha.push_back(pControlDianXiangFangWuCha);

                this->m_ControlDianXiangFangWuChaBlock.m_mapControlDianXiangFangWuCha2VecIndx.insert(
                    std::pair<unsigned long long, CControlDianXiangFangWuCha*>(pControlDianXiangFangWuCha->m_nDianHao, pControlDianXiangFangWuCha));


                auto itt2 = this->m_ControlDianPrecisionBlock.m_mapControlDianPrecision2VecIndx.find(pControlDianXiangFangWuCha->m_nDianHao);
                if(itt2 != ittEndControlDian){
                    itt2->second->m_pControlDianXiangFangWuCha = pControlDianXiangFangWuCha;
                }
            }
            printf("   %s itemCount:%d  mapCount:%d\n\n", sFlag09.c_str()
                , (int)this->m_ControlDianXiangFangWuChaBlock.m_vecControlDianXiangFangWuCha.size()
                , (int)this->m_ControlDianXiangFangWuChaBlock.m_mapControlDianXiangFangWuCha2VecIndx.size());
        }
        else if (sReadLine.find(sFlag10) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag10.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 3) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_ControlDianXiangFangZhongWuCha.m_fPix_Residual_X = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianXiangFangZhongWuCha.m_fPix_Residual_Y = atof(vecSplitStr[nItem++].c_str());
            m_ControlDianXiangFangZhongWuCha.m_fPix_Residual_Plane = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag10.c_str());
        }
        else if (sReadLine.find(sFlag11) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag11.c_str());

            CCheckDianPrecision* pCheckDianPrecision = nullptr;
            while (std::getline(fReadFile, sReadLine)) {
                nFileReadLineCount++;
                if (sReadLine == "")
                    break;
                nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
                if (nVecSplitCnt < 9) {
                    printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                    continue;
                }

                pCheckDianPrecision = new CCheckDianPrecision;

                nItem = 0;
                pCheckDianPrecision->m_sDianHao = vecSplitStr[nItem++];
                sDianHao = pCheckDianPrecision->m_sDianHao;
                this->Replace_str(sDianHao, "GCP", "");
                pCheckDianPrecision->m_nDianHao = strtoull(sDianHao.c_str(), 0, 10);
                pCheckDianPrecision->m_fLng = atof(vecSplitStr[nItem++].c_str());
                pCheckDianPrecision->m_fLat = atof(vecSplitStr[nItem++].c_str());
                pCheckDianPrecision->m_fHeight = atof(vecSplitStr[nItem++].c_str());
                pCheckDianPrecision->m_fResidual_X = atof(vecSplitStr[nItem++].c_str());
                pCheckDianPrecision->m_fResidual_Y = atof(vecSplitStr[nItem++].c_str());
                pCheckDianPrecision->m_fResidual_Plane = atof(vecSplitStr[nItem++].c_str());
                pCheckDianPrecision->m_fResidual_Height = atof(vecSplitStr[nItem++].c_str());
                pCheckDianPrecision->m_fAngle = atof(vecSplitStr[nItem++].c_str());

                this->m_CheckDianPrecisionBlock.m_vecCheckDianPrecision.push_back(pCheckDianPrecision);

                this->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx.insert(
                    std::pair<unsigned long long, CCheckDianPrecision*>(pCheckDianPrecision->m_nDianHao, pCheckDianPrecision));
            }
            ittEndCheckDian = this->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx.end();
            printf("  %s itemCount:%d  mapCount:%d\n\n", sFlag11.c_str()
                , (int)this->m_CheckDianPrecisionBlock.m_vecCheckDianPrecision.size()
                , (int)this->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx.size());

        }
        else if (sReadLine.find(sFlag12) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag12.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 4) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_CheckDianGroundPrecision.m_fResidual_X = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianGroundPrecision.m_fResidual_Y = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianGroundPrecision.m_fResidual_Plane = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianGroundPrecision.m_fResidual_Height = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag12.c_str());
        }
        else if (sReadLine.find(sFlag13) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag13.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 4) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_CheckDianGroundMaxWuCha.m_fMax_Residual_X = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianGroundMaxWuCha.m_fMax_Residual_Y = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianGroundMaxWuCha.m_fMax_Residual_Plane = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianGroundMaxWuCha.m_fMax_Residual_Height = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag13.c_str());
        }
        else if (sReadLine.find(sFlag14) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag14.c_str());

            CCheckDianXiangFangWuCha* pCheckDianXiangFangWuCha = nullptr;
            while (std::getline(fReadFile, sReadLine)) {
                nFileReadLineCount++;
                if (sReadLine == "")
                    break;
                nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
                if (nVecSplitCnt < 7) {
                    printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                    continue;
                }

                pCheckDianXiangFangWuCha = new CCheckDianXiangFangWuCha;

                nItem = 0;
                pCheckDianXiangFangWuCha->m_sDianHao = vecSplitStr[nItem++];
                sDianHao = pCheckDianXiangFangWuCha->m_sDianHao;
                this->Replace_str(sDianHao, "GCP", "");
                pCheckDianXiangFangWuCha->m_nDianHao = strtoull(sDianHao.c_str(), 0, 10);
                pCheckDianXiangFangWuCha->m_sTifName = vecSplitStr[nItem++];
                pCheckDianXiangFangWuCha->m_fPix_X = atof(vecSplitStr[nItem++].c_str());
                pCheckDianXiangFangWuCha->m_fPix_Y = atof(vecSplitStr[nItem++].c_str());
                pCheckDianXiangFangWuCha->m_fPix_Residual_X = atof(vecSplitStr[nItem++].c_str());
                pCheckDianXiangFangWuCha->m_fPix_Residual_Y = atof(vecSplitStr[nItem++].c_str());
                pCheckDianXiangFangWuCha->m_fWeight = atof(vecSplitStr[nItem++].c_str());

                this->m_CheckDianXiangFangWuChaBlock.m_vecCheckDianXiangFangWuCha.push_back(pCheckDianXiangFangWuCha);
                this->m_CheckDianXiangFangWuChaBlock.m_mapCheckDianXiangFangWuCha2VecIndx.insert(
                    std::pair<unsigned long long, CCheckDianXiangFangWuCha*>(pCheckDianXiangFangWuCha->m_nDianHao, pCheckDianXiangFangWuCha));

                auto itt3 = this->m_CheckDianPrecisionBlock.m_mapCheckDianPrecision2VecIndx.find(pCheckDianXiangFangWuCha->m_nDianHao);
                if(itt3 != ittEndCheckDian){
                    itt3->second->m_pCheckDianXiangFangWuCha = pCheckDianXiangFangWuCha;
                }
            }
            printf("   %s itemCount:%d  mapCount:%d\n\n", sFlag14.c_str()
                , (int)this->m_CheckDianXiangFangWuChaBlock.m_vecCheckDianXiangFangWuCha.size()
                , (int)this->m_CheckDianXiangFangWuChaBlock.m_mapCheckDianXiangFangWuCha2VecIndx.size());
        }
        else if (sReadLine.find(sFlag15) != std::string::npos) {
            printf("%02d %s\n", nFind++, sFlag15.c_str());
            std::getline(fReadFile, sReadLine);
            nFileReadLineCount++;
            if (sReadLine == "")
                continue;
            nVecSplitCnt = this->SplitString(vecSplitStr, sReadLine, " ", 1);
            if (nVecSplitCnt < 3) {
                printf("[ERROR] : nLineCnt:%d %s,  vecSplitStr.size = %d\n", nFileReadLineCount, sReadLine.c_str(), nVecSplitCnt);
                continue;
            }
            nItem = 0;
            m_CheckDianXiangFangZhongWuCha.m_fPix_Residual_X = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianXiangFangZhongWuCha.m_fPix_Residual_Y = atof(vecSplitStr[nItem++].c_str());
            m_CheckDianXiangFangZhongWuCha.m_fPix_Residual_Plane = atof(vecSplitStr[nItem++].c_str());

            for (nItem = 0; nItem < nVecSplitCnt; nItem++) {
                printf("%s\n", vecSplitStr[nItem].c_str());
            }
            printf("   %s\n\n", sFlag15.c_str());
        }
    }

    fReadFile.close();
}
