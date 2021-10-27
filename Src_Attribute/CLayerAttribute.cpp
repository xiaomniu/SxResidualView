#include "CLayerAttribute.h"
#include "ui_CLayerAttribute.h"

#include <stdio.h>

#include "gdal/gdal.h"
#include "gdal/cpl_conv.h"
#include "gdal/gdal_priv.h"

#include "Src_Core/Misc.h"
#include "Src_LayerManager/CTreeWidgetItem.h"
#include "Src_Geometry/CLayerDraw.h"
#include "Src_Geometry/CLayerGeoDraw.h"
#include "Src_Geometry/CRasterDraw.h"
#include "Src_Geometry/CReadRaster.h"

#define STR_CONVERT_ANSI2UTF8(a) CMisc::AnsiToUtf8((a)).c_str()

static int INDX_FLAG_USER_ROLE_ATTRIBUTE = 0x1001;
const int FLAG_ITEM_PATH = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE_ATTRIBUTE++;
const int FLAG_ITEM_GROUP_NODE = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE_ATTRIBUTE++;

CLayerAttribute::CLayerAttribute(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLayerAttribute)
{
    ui->setupUi(this);
    GetGlobalPtr()->m_pLayerAttribute = this;

    this->ui->m_treeLayerAtrribute->setColumnCount(2);
    this->ui->m_treeLayerAtrribute->setColumnWidth(0, 150);
    this->ui->m_treeLayerAtrribute->setColumnWidth(1, 1300);
    QStringList sHeaderTexts;
    sHeaderTexts<<u8"属性名"<<u8"属性值";
    this->ui->m_treeLayerAtrribute->setHeaderLabels(sHeaderTexts);

    this->InitTreeView(nullptr);
    connect(ui->m_treeLayerAtrribute, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(itemDbPressedSlot(QTreeWidgetItem*,int)));
}

CLayerAttribute::~CLayerAttribute()
{
    delete ui;
}

void CLayerAttribute::itemDbPressedSlot(QTreeWidgetItem* pItem, int nColumn){
    if(pItem == nullptr || nColumn < 0  || nColumn > 1)
        return;
    int nExtData = pItem->data(0, FLAG_ITEM_GROUP_NODE).toInt();
    if(nExtData == 1001) {
        return;
    }
    if(nColumn == 1){
        pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    }else if (nColumn == 0) {
        pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
    }
}

void CLayerAttribute::InitTreeView(CLayerDraw* pLayerDraw){

    if(pLayerDraw == nullptr){
        return;
    }
    //m_mapAttrValue.insert(std::pair<std::string, QVariant>("LayerName", QVariant("aaaa.tiff")));
    //m_mapAttrValue.insert(std::pair<std::string, QVariant>("LayerType", QVariant("tiff")));
    //m_mapAttrValue.insert(std::pair<std::string, QVariant>("TiffName", QVariant("aaaa.tiff")));
    //m_mapAttrValue.insert(std::pair<std::string, QVariant>("TiffBands", QVariant("4")));
    //m_mapAttrValue.insert(std::pair<std::string, QVariant>("TiffDepth", QVariant("8bit")));
    //m_mapAttrValue.insert(std::pair<std::string, QVariant>("TiffScale", QVariant("0.123,0.321")));

    int i = 0;
    char szItemText[64] = {0};
    QString sItemText;
    std::string sLayerName = pLayerDraw->m_sLayerName;
    CTreeWidgetItem* pItem = nullptr;
    CTreeWidgetItem* pItem1 = nullptr;
    this->ui->m_treeLayerAtrribute->clear();
    std::vector<CTreeWidgetItem*> vecTreeItemExpand;

    std::unordered_map<int, QString> mapLayerType;
    mapLayerType[ELAYERTYPE::e_Img] = u8"栅格";
    mapLayerType[ELAYERTYPE::e_Tiff] = u8"栅格";
    mapLayerType[ELAYERTYPE::e_Tif] = u8"栅格";
    mapLayerType[ELAYERTYPE::e_shape] = u8"矢量";
    mapLayerType[ELAYERTYPE::e_shapePoint] = u8"点";
    mapLayerType[ELAYERTYPE::e_shapePolygon] = u8"面";
    mapLayerType[ELAYERTYPE::e_shapePolyline] = u8"线";

    if(pLayerDraw->m_nLayerType == ELAYERTYPE::e_Img
        || pLayerDraw->m_nLayerType == ELAYERTYPE::e_Tiff
        || pLayerDraw->m_nLayerType == ELAYERTYPE::e_Tif)
    {
        CRasterDraw* pRasterLayer = (CRasterDraw*)pLayerDraw;
        CReadRaster* pReadRaster = pRasterLayer->m_pReadRaster;

        this->m_pNodeRasterInfo = new CTreeWidgetItem(QStringList()<<u8"栅格信息");
        this->m_pNodeRasterInfo->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
        sItemText = QString("%1, %2").arg(pReadRaster->m_nPixWidth).arg(pReadRaster->m_nPixHeight);
        this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeRasterInfo);
        pItem = new CTreeWidgetItem(QStringList()<<u8"列数，行数"<<sItemText);
        this->m_pNodeRasterInfo->addChild(pItem);
        sItemText = QString("%1").arg(pReadRaster->m_nBandCnt);
        pItem = new CTreeWidgetItem(QStringList()<<u8"波段数"<<sItemText);
        this->m_pNodeRasterInfo->addChild(pItem);
        sprintf(szItemText, "%e, %e", pReadRaster->m_pAdfTransform[1], pReadRaster->m_pAdfTransform[5]);
        pItem = new CTreeWidgetItem(QStringList()<<u8"像元大小(x，y)"<< szItemText);
        this->m_pNodeRasterInfo->addChild(pItem);
        sItemText = (sLayerName.substr(sLayerName.rfind('.')+1)).c_str();
        pItem = new CTreeWidgetItem(QStringList()<<u8"格式"<<sItemText.toUpper());
        this->m_pNodeRasterInfo->addChild(pItem);
        pItem = new CTreeWidgetItem(QStringList()<<u8"像素类型"<<STR_CONVERT_ANSI2UTF8(pReadRaster->m_sDataType.c_str()));
        this->m_pNodeRasterInfo->addChild(pItem);
        sItemText = QString("%1").arg(pReadRaster->m_nDataDepth);
        pItem = new CTreeWidgetItem(QStringList()<<u8"像素深度"<<sItemText);
        this->m_pNodeRasterInfo->addChild(pItem);
        //pItem = new SxTreeWidgetItem(QStringList()<<u8"金字塔"<<STR_CONVERT_ANSI2UTF8("级别: 7, 重采样: 最邻近法"));
        //this->m_pNodeRasterInfo->addChild(pItem);

        this->m_pNodeRangeInfo = new CTreeWidgetItem(QStringList()<<u8"范围");
        this->m_pNodeRangeInfo->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
        this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeRangeInfo);
        sprintf(szItemText, "%.9f", pReadRaster->m_fMaxLat);
        pItem = new CTreeWidgetItem(QStringList()<<u8"上"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);
        sprintf(szItemText, "%.9f", pReadRaster->m_fMinLng);
        pItem = new CTreeWidgetItem(QStringList()<<u8"左"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);
        sprintf(szItemText, "%.9f", pReadRaster->m_fMinLat);
        pItem = new CTreeWidgetItem(QStringList()<<u8"下"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);
        sprintf(szItemText, "%.9f", pReadRaster->m_fMaxLng);
        pItem = new CTreeWidgetItem(QStringList()<<u8"右"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);

        this->m_pNodeProjectRef = new CTreeWidgetItem(QStringList()<<u8"空间参考");
        this->m_pNodeProjectRef->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
        this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeProjectRef);
        sItemText = pReadRaster->m_pDataset->GetProjectionRef();
        pItem = new CTreeWidgetItem(QStringList()<<u8"内容"<<sItemText);
        this->m_pNodeProjectRef->addChild(pItem);

        this->m_pNodeStatistics = new CTreeWidgetItem(QStringList()<<u8"统计值");
        this->m_pNodeStatistics->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
        this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeStatistics);

        for(i = 0; i<pReadRaster->m_nBandCnt; i++) {
            GDALRasterBand* pRasterBand = pReadRaster->m_pDataset->GetRasterBand(1);
            sItemText = QString("band_%1").arg(i+1);
            pItem = new CTreeWidgetItem(QStringList()<<sItemText);
            pItem->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
            this->m_pNodeStatistics->addChild(pItem);

            double fNoDataVal = pRasterBand->GetNoDataValue();
            sprintf(szItemText, "%.6f", fNoDataVal);
            pItem1 = new CTreeWidgetItem(QStringList()<<u8"NoData 值"<<szItemText);
            pItem->addChild(pItem1);
        }

        vecTreeItemExpand.push_back(this->m_pNodeRasterInfo);
        vecTreeItemExpand.push_back(this->m_pNodeRangeInfo);
        vecTreeItemExpand.push_back(this->m_pNodeProjectRef);
        vecTreeItemExpand.push_back(this->m_pNodeStatistics);

    }else if(pLayerDraw->m_nLayerType == ELAYERTYPE::e_shape
             || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePoint
             || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolygon
             || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolyline)
    {
        CLayerGeoDraw* pLayerGeoDraw = (CLayerGeoDraw*)pLayerDraw;

        this->m_pNodeShapeInfoShp = new CTreeWidgetItem(QStringList()<<u8"矢量信息");
        this->m_pNodeShapeInfoShp->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
        this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeShapeInfoShp);
        pItem = new CTreeWidgetItem(QStringList()<<u8"几何类型"<<mapLayerType[pLayerDraw->m_nLayerType]);
        this->m_pNodeShapeInfoShp->addChild(pItem);
        sItemText = QString("%1").arg(pLayerGeoDraw->GetChunkCount());
        pItem = new CTreeWidgetItem(QStringList()<<u8"图块个数"<<sItemText);
        this->m_pNodeShapeInfoShp->addChild(pItem);
        sItemText = pLayerGeoDraw->m_nHasZValue == 1 ? u8"是" : u8"否";
        pItem = new CTreeWidgetItem(QStringList()<<u8"坐标包含Z值"<<sItemText);
        this->m_pNodeShapeInfoShp->addChild(pItem);

        this->m_pNodeRangeInfo = new CTreeWidgetItem(QStringList()<<u8"范围");
        this->m_pNodeRangeInfo->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
        this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeRangeInfo);
        sprintf(szItemText, "%.9f", pLayerGeoDraw->m_fMaxLat);
        pItem = new CTreeWidgetItem(QStringList()<<u8"上"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);
        sprintf(szItemText, "%.9f", pLayerGeoDraw->m_fMinLng);
        pItem = new CTreeWidgetItem(QStringList()<<u8"左"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);
        sprintf(szItemText, "%.9f", pLayerGeoDraw->m_fMinLat);
        pItem = new CTreeWidgetItem(QStringList()<<u8"下"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);
        sprintf(szItemText, "%.9f", pLayerGeoDraw->m_fMaxLng);
        pItem = new CTreeWidgetItem(QStringList()<<u8"右"<<szItemText);
        this->m_pNodeRangeInfo->addChild(pItem);

        this->m_pNodeProjectRef = new CTreeWidgetItem(QStringList()<<u8"空间参考");
        this->m_pNodeProjectRef->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
        this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeProjectRef);
        pItem = new CTreeWidgetItem(QStringList()<<u8"内容"<<pLayerGeoDraw->m_sCoordProject.c_str());
        this->m_pNodeProjectRef->addChild(pItem);

        vecTreeItemExpand.push_back(this->m_pNodeShapeInfoShp);
        vecTreeItemExpand.push_back(this->m_pNodeRangeInfo);
        vecTreeItemExpand.push_back(this->m_pNodeProjectRef);
    } else {
        return ;
    }

    this->m_pNodeFileInfo = new CTreeWidgetItem(QStringList()<<u8"数据源");
    this->m_pNodeFileInfo->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
    this->ui->m_treeLayerAtrribute->addTopLevelItem(this->m_pNodeFileInfo);
    pItem = new CTreeWidgetItem(QStringList()<<u8"路径"<<STR_CONVERT_ANSI2UTF8(sLayerName.c_str()));
    pItem->setData(0, FLAG_ITEM_PATH, QVariant((unsigned int)('path')));
    //pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    //this->ui->m_treeLayerAtrribute->editItem(pItem, 0);
    this->m_pNodeFileInfo->addChild(pItem);

    vecTreeItemExpand.push_back(this->m_pNodeFileInfo);
    for(i = 0; i < (int)vecTreeItemExpand.size(); i++){
        pItem = vecTreeItemExpand[i];
        if(pItem)
            pItem->setExpanded(1);
    }

}
