#include "CLayerManagerWidget.h"
#include "ui_CLayerManagerWidget.h"


#include <QtCore/QPoint>
#include <QtCore/QDebug>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>

#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QPalette>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QFileDialog>
#include <QtCore/QTextCodec>
#include <QtWidgets/QColorDialog>
#include <algorithm>

#include "Src_Geometry/CLayerDraw.h"
#include "Src_Geometry/CReadRaster.h"
#include "Src_Geometry/CRasterDraw.h"
#include "Src_Geometry/CLayerGeoDraw.h"
#include "Src_Geometry/CLayerVector.h"
#include "Src_Geometry/CLayerResidual.h"

#include "Src_Core/Misc.h"
#include "Src_Core/CGlobal.h"
#include "Src_Core/BaseVectorStruct.h"
#include "Src_LayerManager/CTreeWidgetItem.h"
#include "Src_Attribute/CLayerAttribute.h"

#ifndef FLAG_USER_ROLE
static int INDX_FLAG_USER_ROLE = 0x1001;
const int FLAG_ITEM_GROUP_0 = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
const int FLAG_ITEM_GROUP_1 = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
const int FLAG_ITEM_GROUP_2 = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
const int FLAG_ITEM_GROUP_3 = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
const int FLAG_ITEM_GROUP_4 = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;

const int FLAG_ITEM_LAYER_ID = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
const int FLAG_ITEM_GROUP_ID = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
const int FLAG_ITEM_LAYER_TYPE = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
const int FLAG_IMTE_EXT_DATA = Qt::ItemDataRole::UserRole + INDX_FLAG_USER_ROLE++;
#endif

CLayerManagerWidget::CLayerManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLayerManagerWidget)
{
    ui->setupUi(this);


    GetGlobalPtr()->m_pDlgLayerManager = this;

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    //palette.setBrush(QPalette::Background, QBrush(QPixmap(":/background.png")));
    this->setPalette(palette);

    this->InitTreeView();


    QAction* pActCtrl = nullptr;
    pActCtrl = new QAction(QStringLiteral("移除"), this);
    this->m_pActCtrlRemoveLayer = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuRemoveLayerSlot);

    pActCtrl = new QAction(QStringLiteral("显示NoData"), this);
    pActCtrl->setCheckable(true);
    pActCtrl->setChecked(true);
    this->m_pActCtrlShowNoData = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuShowNoDataSlot);

    pActCtrl = new QAction(QStringLiteral("显示Alpha"), this);
    pActCtrl->setCheckable(true);
    pActCtrl->setChecked(false);
    this->m_pActCtrlShowAlpha = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuShowAlphaSlot);

    pActCtrl = new QAction(QStringLiteral("卷帘"), this);
    pActCtrl->setCheckable(true);
    pActCtrl->setChecked(false);
    this->m_pActCtrlRollerBlind = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuRollerBlindSlot);

    pActCtrl = new QAction(QStringLiteral("编辑"), this);
    pActCtrl->setCheckable(true);
    pActCtrl->setChecked(false);
    this->m_pActCtrlEditor = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuEditorSlot);

    pActCtrl = new QAction(QStringLiteral("生成镶嵌线"), this);
    pActCtrl->setCheckable(true);
    pActCtrl->setChecked(false);
    this->m_pActCtrlCreateMosaicLine = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuCreateMosaicLineSlot);

    pActCtrl = new QAction(QStringLiteral("查看匹配点"), this);
    //pActCtrl->setCheckable(true);
    //pActCtrl->setChecked(false);
    this->m_pActCtrlTravelGcp = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuTravelGcpSlot);

    pActCtrl = new QAction(QStringLiteral("图层颜色"), this);
    //pActCtrl->setCheckable(true);
    //pActCtrl->setChecked(false);
    this->m_pActCtrlSelectLayerColor = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuSelectColorSlot);

    pActCtrl = new QAction(QStringLiteral("图层属性"), this);
    //pActCtrl->setCheckable(true);
    //pActCtrl->setChecked(false);
    this->m_pActCtrlShowLayerAttribute = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuShowLayerAttributeSlot);

    pActCtrl = new QAction(QStringLiteral("清除图块"), this);
    //pActCtrl->setCheckable(true);
    //pActCtrl->setChecked(false);
    this->m_pActCtrlDeleteAllChunks = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuDeleteAllChunksSlot);

    pActCtrl = new QAction(QStringLiteral("残差图"), this);
    //pActCtrl->setCheckable(true);
    //pActCtrl->setChecked(false);
    this->m_pActCtrlShowResidual = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuShowResidualSlot);

    pActCtrl = new QAction(QStringLiteral("显示箭头"), this);
    //pActCtrl->setCheckable(true);
    //pActCtrl->setChecked(false);
    this->m_pActCtrlShowResidualWithOutArrow = pActCtrl;
    connect(pActCtrl, &QAction::triggered, this, &CLayerManagerWidget::itemTreeMenuShowResidualWithoutArrowSlot);
}

CLayerManagerWidget::~CLayerManagerWidget()
{
    delete ui;
}

bool CLayerManagerWidget::eventFilter(QObject *watched, QEvent *event)
{
    return QWidget::eventFilter(watched,event);
}

int CLayerManagerWidget::InitTreeView() {

    this->m_publicIconMap["Layer_Group"] =QIcon((":/img/LayManager/Res/ImgLayerManager/01Icon.ico"));
    this->m_publicIconMap["Layer_Item"] =QIcon((":/img/LayManager/Res/ImgLayerManager/02Icon.ico"));
    this->m_publicIconMap["Layer_Info"] =QIcon((":/img/LayManager/Res/ImgLayerManager/03Icon.ico"));
    this->m_publicIconMap["treeItem_group"] =QIcon((":/img/LayManager/Res/ImgLayerManager/04Icon.ico"));
    this->m_publicIconMap["treeItem_channel"] =QIcon((":/img/LayManager/Res/ImgLayerManager/05Icon.ico"));

    this->m_publicIconMap["treeItem_Img_raster"] =QIcon((":/img/LayManager/Res/ImgLayerManager/img_raster.ico"));
    this->m_publicIconMap["treeItem_shp_point"] =QIcon((":/img/LayManager/Res/ImgLayerManager/shp_point.ico"));
    this->m_publicIconMap["treeItem_shp_polyline"] =QIcon((":/img/LayManager/Res/ImgLayerManager/shp_polyline.ico"));
    this->m_publicIconMap["treeItem_shp_polygon"] =QIcon((":/img/LayManager/Res/ImgLayerManager/shp_polygon.ico"));
    this->m_publicIconMap["treeItem_layer_group"] =QIcon((":/img/LayManager/Res/ImgLayerManager/img_layerGroup.ico"));

    ui->m_pCtlTreeView->setDragEnabled(true); //允许拖拽
    ui->m_pCtlTreeView->setDragDropMode(QAbstractItemView::InternalMove); //拖放模式为移动
    ui->m_pCtlTreeView->setDropIndicatorShown(true);  //显示拖放位置

    QTreeWidgetItem* pHeader = ui->m_pCtlTreeView->headerItem();
    pHeader->setText(0, u8"图层管理");
    this->m_pLayerGroup0 = new CTreeWidgetItem(QStringList()<<u8"图层组0");   //顶层节点
    this->m_pLayerGroup0->setData(0, FLAG_ITEM_GROUP_ID, (int)FLAG_ITEM_GROUP_0);

    this->m_pLayerGroup0->setCheckState(0, Qt::CheckState::Checked);
    this->m_pLayerGroup0->setIcon(0, this->m_publicIconMap["treeItem_layer_group"]);
    //Global_item->setFont(0,QFont("Calibri",10,QFont::Bold));     //节点字体样式
    ui->m_pCtlTreeView->addTopLevelItem(this->m_pLayerGroup0);      //顶层节点添加到QTreeWidget中

    connect(ui->m_pCtlTreeView ,&QAbstractItemView::clicked, this, &CLayerManagerWidget::itemTreeClickedSlot);
    connect(ui->m_pCtlTreeView ,&QAbstractItemView::doubleClicked, this, &CLayerManagerWidget::itemTreeDBClickedSlot);

    void (CTreeWidget::*pfnItemCheckStateChangedSig)(QTreeWidgetItem*, Qt::CheckState) = &CTreeWidget::ItemCheckStateChanged;
    void (CLayerManagerWidget::*pfnTreeItemCheckStateChangedSlot)(QTreeWidgetItem*, Qt::CheckState) = &CLayerManagerWidget::TreeItemCheckStateChangedSlot;
    connect(ui->m_pCtlTreeView, pfnItemCheckStateChangedSig, this, pfnTreeItemCheckStateChangedSlot);

    void (CTreeWidget::*pfnItemIndxResortSig)(QTreeWidgetItem*, QTreeWidgetItem*, int) = &CTreeWidget::ItemIndxResort;
    void (CLayerManagerWidget::*pfnTreeItemIndxResortSlot)(QTreeWidgetItem*, QTreeWidgetItem*, int) = &CLayerManagerWidget::TreeItemIndxResortSlot;
    connect(ui->m_pCtlTreeView, pfnItemIndxResortSig, this, pfnTreeItemIndxResortSlot);

    ui->m_pCtlTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_pCtlTreeView, &QTreeView::customContextMenuRequested, this, &CLayerManagerWidget::itemTreeMenuSlot);

    return 1;
}

void CLayerManagerWidget::AddItem(const std::string& sLayerName, int nLayerType, void* pExtData, QTreeWidgetItem* pLayerGroup/* = nullptr*/){
    QTreeWidgetItem* pItemLayer = nullptr, *pItemBand = nullptr;
    if(pLayerGroup == nullptr) {
        pLayerGroup = this->m_pLayerGroup0;
    }
    std::string sItemIconName;
    switch (nLayerType) {
    case ELAYERTYPE::e_Tiff:
    case ELAYERTYPE::e_Tif:
    case ELAYERTYPE::e_Img:{
        sItemIconName = "treeItem_Img_raster";
        break;
    }
    case ELAYERTYPE::e_shapePoint:{
        sItemIconName = "treeItem_shp_point";
        break;
    }
    case ELAYERTYPE::e_shapePolyline:{
        sItemIconName = "treeItem_shp_polyline";
        break;
    }
    case ELAYERTYPE::e_shapePolygon:{
        sItemIconName = "treeItem_shp_polygon";
        break;
    }
    default:{
        return;
    }
    }

    pItemLayer = new CTreeWidgetItem(QStringList() << CMisc::AnsiToUtf8(sLayerName.c_str()).c_str());
    pItemLayer->setIcon(0, m_publicIconMap[sItemIconName]);
    pItemLayer->setCheckState(0, Qt::CheckState::Checked);
    //unsigned long long nLayerIndex = pItemLayer->data(0, FLAG_ITEM_LAYER_ID).toULongLong();//this->GetLayerID(pItemLayer);
    //this->SetLayerType(pItemLayer, nLayerType);
    //this->SetItemExtData(pItemLayer, pExtData);
    pItemLayer->setData(0, FLAG_ITEM_LAYER_TYPE, (int)nLayerType);
    pItemLayer->setData(0, FLAG_IMTE_EXT_DATA, (unsigned long long)pExtData);
    //unsigned long long nLayerIndex = (pItemLayer->data(FLAG_ITEM_LAYER_TYPE)).toULongLong() + 1;
    //pItemLayer->setData((unsigned long long)nLayerIndex, FLAG_ITEM_LAYER_TYPE);
    //pItemLayer->setData((unsigned long long)pExtData, FLAG_IMTE_RASTER_EXT_DATA);
    pLayerGroup->addChild(pItemLayer);

    //this->ui->m_pCtlTreeView->expand(pLayerGroup->index());
    pLayerGroup->setExpanded(1);
}

void CLayerManagerWidget::Clicked_AddTif_Slot(){

    if(this->m_pGLCore == nullptr) {

        this->m_pGLCore = GetGlobalPtr()->m_pGLCore;
        this->m_pSxLayerVector = this->m_pGLCore->m_pLayerVector;
    }
    //std::string sVectorFile = "D:/tifData/Shp/宅基地界址线.shp";//
    //std::string sVectorFile = "D:/tifData/Shp/Export_Output - 副本.shp";
    std::string sVectorFile = "D:/tifData/Shp/20190605114343.shp";
    //std::string sVectorFile = "E:/JunMingXingShp/shp/shp008.shp";
    //this->m_pSxLayerVector->AddGeometryItem(sVectorFile.c_str());
    QStringList listFileNames;
    listFileNames << sVectorFile.c_str();
    if(1){
        listFileNames.clear();
        listFileNames = QFileDialog::getOpenFileNames(this,
            tr("File Dialog"),
            "",
            tr("All")+"(*tif *tiff *img *shp *wt *wl *wp *gcp);;" +
            tr("Raster")+"(*tif *tiff *img);;" +
            tr("Shape")+"(*shp);;" +
            tr("Gcp")+"(*gcp);;" +
            tr("Mapgis")+"(*wt *wl *wp);;" +
            tr("Text")+"(*txt);;" +
            tr("All")+"(*.*);;"
            );
    }
    int i = 0, nListFileNamesCnt = listFileNames.size();
    if(nListFileNamesCnt <= 0)
        return;
    QString sFileName;
    std::string sGisFileFullPath;
    std::string sGisFileName;
    std::string sFileExtName;
    void* pExtData = nullptr;
    int nLayerType = 0;
    //for(int jjj = 0; jjj < 100; jjj++) {
    for(i = 0; i< nListFileNamesCnt; i++){
        QString sFileName = listFileNames[i];
        sGisFileFullPath = (const char*)sFileName.toLocal8Bit().data();
        sGisFileName = sGisFileFullPath.substr(sGisFileFullPath.rfind('/')+1);
        sFileExtName = sGisFileName.substr(sGisFileName.rfind('.') + 1);
        std::transform(sFileExtName.begin(), sFileExtName.end(), sFileExtName.begin(), ::toupper);
        if (sFileExtName.compare("TIF")==0 || sFileExtName.compare("TIFF")==0 || sFileExtName.compare("IMG")==0){
            QFileInfo fileInfo(sFileName);
            if(false == fileInfo.exists()) {
                QMessageBox::information(this, tr("Notion"), tr("Can't find file : ")+sFileName);
                return ;
            }

            CRasterDraw* pRasterDraw = this->m_pSxLayerVector->AddRasterItem(sGisFileFullPath);
            if(pRasterDraw == nullptr){
                printf("Err:[%s] 加载失败!!!\n", sGisFileFullPath.c_str());
                return;
            }

            nLayerType = pRasterDraw->m_nLayerType;
            pExtData = (void*)pRasterDraw;
            //int nBandCnt = pRasterDraw->m_pReadRaster->m_nBandCnt;
        } else if (sFileExtName.compare("SHP")==0) {
            CLayerGeoDraw* pLayerGeoDra = this->m_pSxLayerVector->AddGeometryItem(sGisFileFullPath);
            if(pLayerGeoDra == nullptr){
                printf("Err:[%s] 加载失败!!!\n", sGisFileFullPath.c_str());
                return;
            }
            nLayerType = pLayerGeoDra->m_nLayerType;
            pExtData = (void*)pLayerGeoDra;
        } else if (sFileExtName.compare("GCP")==0) {
            CLayerGeoDraw* pLayerGeoDra = this->m_pSxLayerVector->AddGeometryItemGcp(sGisFileFullPath);
            if(pLayerGeoDra == nullptr){
                printf("Err:[%s] 加载失败!!!\n", sGisFileFullPath.c_str());
                return;
            }
            nLayerType = pLayerGeoDra->m_nLayerType;
            //nLayerType = ELAYERTYPE::e_shape;
            pExtData = (void*)pLayerGeoDra;
        }
        this->AddItem(sGisFileName.c_str(), nLayerType, pExtData);
    }
    //}
    this->m_pGLCore->UpdateWidgets();
}


void CLayerManagerWidget::Clicked_AddResidualFile_Slot(){
    if(this->m_pGLCore == nullptr) {
        this->m_pGLCore = GetGlobalPtr()->m_pGLCore;
        this->m_pSxLayerVector = this->m_pGLCore->m_pLayerVector;
    }
    std::string sVectorFile;// = "E:/BlockAdjustmentView/data_result/result.txt";

    QString sQVectorFile;
    if(sVectorFile.empty()) {
        sQVectorFile = QFileDialog::getOpenFileName(this,
            tr("File Dialog"),
            "",
            tr("Text")+"(*txt);;"
            );
        sVectorFile = sQVectorFile.toLocal8Bit().data();
    }
    if(sVectorFile == "")
        return;
    if(0 == CMisc::CheckFileExists(sVectorFile.c_str())){
        QMessageBox::information(this, u8"友情提示", sQVectorFile+u8":文件不存在！");
        return;
    }

    std::vector<CLayerGeoDraw*> vecLayerGeoDra = this->m_pSxLayerVector->AddGeometryItemResidualInfo(sVectorFile);
    if(vecLayerGeoDra.size() < 3){
        printf("Err:[%s] 加载失败!!!\n", sVectorFile.c_str());
        return;
    }
    if(this->m_pLayerGroup1 != nullptr) {
        QList<QTreeWidgetItem*> pItemList = this->m_pLayerGroup1->takeChildren();
        for(auto pItem : pItemList){
            if(pItem == nullptr)
                return;
            CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
            if(pRasterDraw == nullptr)
                return;
            if(pRasterDraw->m_nLayerType == 3
                    ||pRasterDraw->m_nLayerType == 1
                    ||pRasterDraw->m_nLayerType == 2) {
                pRasterDraw->m_nShowNoData = !pRasterDraw->m_nShowNoData;
            }
            this->m_pGLCore->m_pLayerVector->RemoveOneLayer((CLayerDraw*)pRasterDraw);
            //this->m_pLayerGroup0->removeChild(curIndex.row(), 1);
            this->m_pLayerGroup0->removeChild(pItem);
        }
        int nItemIndx = this->ui->m_pCtlTreeView->indexOfTopLevelItem(this->m_pLayerGroup1);
        this->ui->m_pCtlTreeView->takeTopLevelItem(nItemIndx);
    }
    this->m_pLayerGroup1 = new CTreeWidgetItem(QStringList()<<sVectorFile.c_str());   //顶层节点
    this->m_pLayerGroup1->setData(0, FLAG_ITEM_GROUP_ID, (int)FLAG_ITEM_GROUP_0);

    this->m_pLayerGroup1->setCheckState(0, Qt::CheckState::Checked);
    this->m_pLayerGroup1->setIcon(0, this->m_publicIconMap["treeItem_layer_group"]);
    //Global_item->setFont(0,QFont("Calibri",10,QFont::Bold));     //节点字体样式
    ui->m_pCtlTreeView->addTopLevelItem(this->m_pLayerGroup1);      //顶层节点添加到QTreeWidget中

    CLayerGeoDraw* pLayerGeo = nullptr;
    for(int i = 0; i<4; i++) {
        pLayerGeo = vecLayerGeoDra[i];
        this->AddItem(pLayerGeo->m_sLayerName, pLayerGeo->m_nLayerType, pLayerGeo, this->m_pLayerGroup1);
    }
    this->m_pGLCore->UpdateWidgets();
}

void CLayerManagerWidget::itemTreeClickedSlot(const QModelIndex &modelIndex)
{
    //QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    //QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index
    //if(index.isValid())
    //{
    //    ui->m_pCtlTreeView->collapse(index);
    //}
    QString str;
    str += QStringLiteral("当前选中：%1\nrow:%2,column:%3\n").arg(modelIndex.data().toString())
                        .arg(modelIndex.row()).arg(modelIndex.column());
    str += QStringLiteral("父级：%1\n").arg(modelIndex.parent().data().toString() );
    QString name,info;
    if(modelIndex.column() == 0)
    {
        name = modelIndex.data().toString();
        info = modelIndex.sibling(modelIndex.row(),1).data().toString();
    }
    else
    {
        name = modelIndex.sibling(modelIndex.row(),0).data().toString();
        info = modelIndex.data().toString();
    }
    str += QStringLiteral("名称：%1\n信息：%2").arg(name).arg(info);
    std::string sMsg = str.toLocal8Bit().data();
    printf((sMsg+" \n").c_str());
}

void CLayerManagerWidget::itemTreeDBClickedSlot(const QModelIndex &modelIndex) {
    QTreeWidgetItem* pItem = ui->m_pCtlTreeView->itemAt(modelIndex.row(), 0);
    QString sItemText = pItem->text(0);
    pItem = ui->m_pCtlTreeView->currentItem();
    sItemText = pItem->text(0);
    if(pItem == nullptr) {
        return;
    }

    COpenGLCore* pGLCore = nullptr;
    double fGeometryMinLng = 0.0, fGeometryMinLat = 0.0, fGeometryMaxLng = 0.0, fGeometryMaxLat = 0.0;
    int nLayerType = (int)pItem->data(0, FLAG_ITEM_LAYER_TYPE).toInt();
    if(nLayerType == ELAYERTYPE::e_Tiff ||
        nLayerType == ELAYERTYPE::e_Tif ||
        nLayerType == ELAYERTYPE::e_Img)
    {
        CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pRasterDraw == nullptr){
            return;
        }
        CReadRaster* pReadRaster = pRasterDraw->m_pReadRaster;
        if(pReadRaster == nullptr){
            return;
        }
        pGLCore = pRasterDraw->m_pGLCore;
        fGeometryMinLng = pReadRaster->m_fMinLng;
        fGeometryMinLat = pReadRaster->m_fMinLat;
        fGeometryMaxLng = pReadRaster->m_fMaxLng;
        fGeometryMaxLat = pReadRaster->m_fMaxLat;
    } else if (nLayerType == ELAYERTYPE::e_shapePoint
               || nLayerType == ELAYERTYPE::e_shapePolyline
               || nLayerType == ELAYERTYPE::e_shapePolygon) {

        CLayerGeoDraw* pLayerGeoDraw = (CLayerGeoDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pLayerGeoDraw == nullptr){
            return;
        }
        if(pLayerGeoDraw->m_vecChunkDatas.size() <= 0){
            return;
        }
        pGLCore = pLayerGeoDraw->m_pGLCore;
        fGeometryMinLng = pLayerGeoDraw->m_fMinLng;
        fGeometryMinLat = pLayerGeoDraw->m_fMinLat;
        fGeometryMaxLng = pLayerGeoDraw->m_fMaxLng;
        fGeometryMaxLat = pLayerGeoDraw->m_fMaxLat;
    }
    if(pGLCore == nullptr){
        return;
    }
    double fRasterGeoWidth = fGeometryMaxLng - fGeometryMinLng;
    double fRasterGeoHeight = fGeometryMaxLat - fGeometryMinLat;

    double fPercent20W = fRasterGeoWidth / 20.0;
    double fPercent20H = fRasterGeoHeight / 20.0;

    //double fPercent20 = fPercent20W > fPercent20H ? fPercent20W : fPercent20H;

    double fMinLng = 0.0, fMinLat = 0.0, fMaxLng = 0.0, fMaxLat = 0.0;
    if( fPercent20W <= fPercent20H ) {
        double fNewGeoWidth = fRasterGeoWidth + 2.0 * fPercent20W;
        fMinLng = fGeometryMinLng - fPercent20W;
        fMaxLng = fGeometryMaxLng + fPercent20W;

        double fNewGeoHeight = fNewGeoWidth / pGLCore->m_fGeoWidthHeightScale;
        fNewGeoHeight = fNewGeoHeight / 2.0;
        double fRasterGeoCenterLat = (fGeometryMaxLat + fGeometryMinLat) / 2.0;

        fMinLat = fRasterGeoCenterLat - fNewGeoHeight;
        fMaxLat = fRasterGeoCenterLat + fNewGeoHeight;

    }
    else{
        double fNewGeoHeight = fRasterGeoHeight + 2.0 * fPercent20H;
        fMinLat = fGeometryMinLat - fPercent20H;
        fMaxLat = fGeometryMaxLat + fPercent20H;

        double fNewGeoWidth = fNewGeoHeight * pGLCore->m_fGeoWidthHeightScale;
        fNewGeoWidth = fNewGeoWidth / 2.0;
        double fRasterGeoCenterLng = (fGeometryMaxLng + fGeometryMinLng) / 2.0;

        fMinLng = fRasterGeoCenterLng - fNewGeoWidth;
        fMaxLng = fRasterGeoCenterLng + fNewGeoWidth;
    }
    pGLCore->m_arrGeoMinMax[0] = fMinLng;
    pGLCore->m_arrGeoMinMax[1] = fMinLat;
    pGLCore->m_arrGeoMinMax[2] = fMaxLng;
    pGLCore->m_arrGeoMinMax[3] = fMaxLat;
    pGLCore->InitGeometry();
    pGLCore->UpdateWidgets();
}

void CLayerManagerWidget::itemTreeMenuSlot(const QPoint &pos)
{
    QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";

    QMenu menu;
    menu.setStyleSheet(qss);    //可以给菜单设置样式

    QModelIndex curIndex = ui->m_pCtlTreeView->indexAt(pos);      //当前点击的元素的index
    QModelIndex index = curIndex.sibling(curIndex.row(),0); //该行的第1列元素的index
    if (index.isValid())
    {
        //可获取元素的文本、data,进行其他判断处理
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr) {
            return;
        }
        CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pRasterDraw == nullptr)
            return;

        menu.addAction(this->m_pActCtrlRemoveLayer);
        menu.addSeparator();    //添加一个分隔线
        this->m_pActCtrlShowNoData->setChecked(pRasterDraw->m_nShowNoData);
        menu.addAction(this->m_pActCtrlShowNoData);
        menu.addSeparator();    //添加一个分隔线
        this->m_pActCtrlShowAlpha->setChecked(pRasterDraw->m_nShowAlpha);
        menu.addAction(this->m_pActCtrlShowAlpha);
        if(this->m_pGLCore->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_RollerBlind){
            menu.addSeparator();    //添加一个分隔线
            this->m_pActCtrlRollerBlind->setChecked(pRasterDraw->m_nRollerBlinding);
            menu.addAction(this->m_pActCtrlRollerBlind);
        }
        menu.addSeparator();
        this->m_pActCtrlEditor->setChecked(pRasterDraw->m_nBeingEditor);
        menu.addAction(this->m_pActCtrlEditor);
        switch (pRasterDraw->m_nLayerType) {
        case ELAYERTYPE::e_Tiff:
        case ELAYERTYPE::e_Tif:
        case ELAYERTYPE::e_Img:{
            menu.addSeparator();
            this->m_pActCtrlCreateMosaicLine->setChecked(pRasterDraw->m_nHasExistMosaicLine);
            menu.addAction(this->m_pActCtrlCreateMosaicLine);
            menu.addAction(this->m_pActCtrlTravelGcp);
            break;
        }
        case ELAYERTYPE::e_shapePoint:
        case ELAYERTYPE::e_shapePolyline:
        case ELAYERTYPE::e_shapePolygon:{
            menu.addSeparator();
            menu.addAction(this->m_pActCtrlSelectLayerColor);
            menu.addAction(this->m_pActCtrlShowResidual);
            menu.addAction(this->m_pActCtrlShowResidualWithOutArrow);

            break;
        }
        }
        menu.addAction(this->m_pActCtrlShowLayerAttribute);
        menu.addAction(this->m_pActCtrlDeleteAllChunks);

        menu.exec(QCursor::pos());  //显示菜单
    }
}

void CLayerManagerWidget::itemTreeMenuRemoveLayerSlot()
{
    printf("itemTreeMenuSRemoveLayerSlot\n");
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pRasterDraw == nullptr)
            return;
        if(pRasterDraw->m_nLayerType == 3
                ||pRasterDraw->m_nLayerType == 1
                ||pRasterDraw->m_nLayerType == 2) {
            pRasterDraw->m_nShowNoData = !pRasterDraw->m_nShowNoData;
        }
        this->m_pGLCore->m_pLayerVector->RemoveOneLayer((CLayerDraw*)pRasterDraw);
        //this->m_pLayerGroup0->removeChild(curIndex.row(), 1);
        this->m_pLayerGroup0->removeChild(pItem);
        this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::itemTreeMenuShowNoDataSlot(bool checked)
{
    printf("slotTreeMenuShowNoData:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pRasterDraw == nullptr)
            return;
        pRasterDraw->m_nShowNoData = !pRasterDraw->m_nShowNoData;
        this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::itemTreeMenuShowAlphaSlot(bool checked)
{
    printf("slotTreeMenuShowAlpha:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pRasterDraw == nullptr)
            return;
        pRasterDraw->m_nShowAlpha = !pRasterDraw->m_nShowAlpha;

        this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::itemTreeMenuRollerBlindSlot(bool checked)
{
    printf("itemTreeMenuRollerBlindSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pRasterDraw == nullptr)
            return;

        this->m_pSxLayerVector->UpdateLayerIndxRollerBlind(pRasterDraw, checked);
        //pRasterDraw->m_nRollerBlinding = !pRasterDraw->m_nRollerBlinding;
        //this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::itemTreeMenuEditorSlot(bool checked)
{
    printf("itemTreeMenuEditorSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CLayerDraw* pLayerDraw = (CLayerDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pLayerDraw == nullptr)
            return;
        //if(pLayerDraw->m_nEnableEdit == 0)
        //    return;
        pLayerDraw->m_nBeingEditor = !pLayerDraw->m_nBeingEditor;
        this->m_pSxLayerVector->m_nBeingEditor = pLayerDraw->m_nBeingEditor;
        pLayerDraw->m_nShowMosaicLine = pLayerDraw->m_nBeingEditor;
        if(pLayerDraw->m_nBeingEditor == 1){
            pLayerDraw->m_nEnableEdit = 1;
            pLayerDraw->BeingEditor();
        }
        this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::itemTreeMenuCreateMosaicLineSlot(bool checked) {
    printf("itemTreeMenuCreateMosaicLineSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pRasterDraw == nullptr)
            return;
        if(pRasterDraw->m_nHasExistMosaicLine == 1)
            return;
        pRasterDraw->m_nShowMosaicLine = !pRasterDraw->m_nShowMosaicLine;
        pRasterDraw->m_nBeingEditor = pRasterDraw->m_nShowMosaicLine;

        if(pRasterDraw->m_nShowMosaicLine == 1){
            pRasterDraw->CreateMosaicLines();
            this->m_pGLCore->UpdateWidgets();
            pRasterDraw->m_nEnableEdit = 1;
        }
    }
    return;
}

void CLayerManagerWidget::itemTreeMenuTravelGcpSlot(bool checked){
    printf("itemTreeMenuTravelGcpSlot:%d\n", checked);
    //if(this->m_pPropertyInfo == nullptr)
    {
        return;
    }
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CLayerDraw* pLayerDraw = (CLayerDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pLayerDraw == nullptr)
            return;

        //this->m_pPropertyInfo->RefreshPointTableViewByGcp((SxLayerPoints*)pLayerDraw);
    }
}

void CLayerManagerWidget::itemTreeMenuSelectColorSlot(bool checked){
    printf("itemTreeMenuSelectColorSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CLayerGeoDraw* pLayerDraw = (CLayerGeoDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(!(pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePoint
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolyline
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolygon))
        {
            return;
        }
        QColor colorSelect = QColorDialog::getColor(Qt::white, this);
        int nR = 0, nG = 0, nB = 0, nA = 0;
        colorSelect.getRgb(&nR, &nG, &nB, &nA);
        unsigned int nLayerColor = nR | (nG << 8) | (nB << 16) | (((unsigned int)0xFF) << 24);
        if(pLayerDraw->m_nLayerColor == nLayerColor){
            return;
        }
        pLayerDraw->m_nLayerColor = nLayerColor;
        this->m_pGLCore->UpdateWidgets();

    }
}

void CLayerManagerWidget::itemTreeMenuShowLayerAttributeSlot(bool checked){
    printf("itemTreeMenuShowLayerAttributeSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CLayerDraw* pLayerDraw = (CLayerDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        this->m_pPropertyInfo->InitTreeView(pLayerDraw);
    }
}

void CLayerManagerWidget::itemTreeMenuDeleteAllChunksSlot(bool checked){
    printf("itemTreeMenuDeleteAllChunksSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CLayerGeoDraw* pLayerDraw = (CLayerGeoDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(!(pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePoint
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolyline
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolygon))
        {
            return;
        }
        pLayerDraw->RemoveAllChunks(true);
        this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::itemTreeMenuShowResidualSlot(bool checked){
    printf("itemTreeMenuShowResidualSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CLayerGeoDraw* pLayerDraw = (CLayerGeoDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(!(pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePoint
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolyline
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolygon))
        {
            return;
        }

        COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
        CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
        CLayerResidual* pLayerResidual = nullptr;
        std::string sLayerName;
        int nResidualType = 0;

        if(pLayerDraw->m_sLayerName.compare("Residual_LianJie_Points") == 0){
            sLayerName = "Residual_LianJie_Points_show";
            nResidualType = 1;
        }else if(pLayerDraw->m_sLayerName.compare("Residual_Control_Points") == 0){
            nResidualType = 2;
            sLayerName = "Residual_Control_Points_show";
        }else if(pLayerDraw->m_sLayerName.compare("Residual_Check_Points") == 0){
            nResidualType = 3;
            sLayerName = "Residual_Check_Points_show";
        }
        pLayerResidual = (CLayerResidual*)pLayerVector->FindLayerByName(sLayerName);
        if(pLayerResidual){
            pLayerResidual->ShowEnable(!pLayerResidual->m_nShowOrHide);
            this->m_pGLCore->UpdateWidgets();
            return;
        }
        pLayerResidual = (CLayerResidual*)pLayerVector->CreateEmptyGeoLayer(4);
        pLayerResidual->m_sLayerName = sLayerName;
        pLayerResidual->m_pExtData = pLayerDraw->m_pExtData;
        pLayerResidual->InitChunksByResidualInfo(nResidualType);
        pLayerDraw->m_pRelationLayer = pLayerResidual;
        this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::itemTreeMenuShowResidualWithoutArrowSlot(bool checked){
    printf("itemTreeMenuShowResidualWithoutArrowSlot:%d\n", checked);
    QModelIndex curIndex = ui->m_pCtlTreeView->currentIndex();
    if (curIndex.isValid()) {
        QTreeWidgetItem* pItem = ui->m_pCtlTreeView->currentItem();
        if(pItem == nullptr)
            return;
        CLayerGeoDraw* pLayerDraw = (CLayerGeoDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(!(pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePoint
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolyline
                || pLayerDraw->m_nLayerType == ELAYERTYPE::e_shapePolygon))
        {
            return;
        }

        COpenGLCore* pGLCore = GetGlobalPtr()->m_pGLCore;
        CLayerVector* pLayerVector = pGLCore->m_pLayerVector;
        CLayerResidual* pLayerResidual = nullptr;
        std::string sLayerName;

        if(pLayerDraw->m_sLayerName.compare("Residual_LianJie_Points") == 0){
            sLayerName = "Residual_LianJie_Points_show";
        }else if(pLayerDraw->m_sLayerName.compare("Residual_Control_Points") == 0){
            sLayerName = "Residual_Control_Points_show";
        }else if(pLayerDraw->m_sLayerName.compare("Residual_Check_Points") == 0){
            sLayerName = "Residual_Check_Points_show";
        }else{
            return;
        }
        pLayerResidual = (CLayerResidual*)pLayerVector->FindLayerByName(sLayerName);
        if(pLayerResidual == nullptr){
            return;
        }
        pLayerResidual->ShowArrow(!pLayerResidual->m_bShowArrow);
        this->m_pGLCore->UpdateWidgets();
    }
}

void CLayerManagerWidget::TreeItemCheckStateChangedSlot(QTreeWidgetItem* pItem, Qt::CheckState nCheckState){
    if(pItem == nullptr)
        return;
    //int nLayerGroupID = pItem->data(0, FLAG_ITEM_GROUP_ID).toInt();
    //if(nLayerGroupID >= FLAG_ITEM_GROUP_0 && nLayerGroupID <= FLAG_ITEM_GROUP_4){
    int nShowHide = nCheckState == Qt::CheckState::Checked ? 1 : 0;
    if(this->m_pLayerGroup0 == pItem) {
        int i = 0, nLayerType = 0, nChildCount = pItem->childCount();
        QTreeWidgetItem* pItemChild = nullptr;
        CLayerDraw* pLayerDraw = nullptr;
        for(i = 0; i<nChildCount; i++){
            pItemChild = pItem->child(i);
            nLayerType = pItemChild->data(0, FLAG_ITEM_LAYER_TYPE).toInt();
            if(nLayerType <= 0 || nLayerType >= ELAYERTYPE::e_LayerTypeCount){
                continue;
            }
            pLayerDraw = (CLayerDraw*)(pItemChild->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
            if(pLayerDraw == nullptr)
                continue;
            pLayerDraw->ShowEnable(nShowHide);
            //this->
        }
        return;
    }

    CRasterDraw* pRasterDraw = (CRasterDraw*)(pItem->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
    if(pRasterDraw == nullptr)
        return;
    pRasterDraw->ShowEnable(nShowHide);
    //if(nCheckState == Qt::CheckState::Checked) {
    //    pRasterDraw->ShowEnable(1);
    //} else {
    //    pRasterDraw->ShowEnable(0);
    //}
    pRasterDraw->m_pGLCore->UpdateWidgets();
}

void CLayerManagerWidget::GetSubLayerChildItem(QTreeWidgetItem* pItem, std::vector<CLayerDraw*> &vecNewSortLayer) {
    int i = 0, nLayerType = 0, nChildCount = pItem->childCount();
    QTreeWidgetItem* pItemChild = nullptr;
    CLayerDraw* pLayerDraw = nullptr;
    for(i = 0; i<nChildCount; i++){
        pItemChild = pItem->child(i);
        nLayerType = pItemChild->data(0, FLAG_ITEM_LAYER_TYPE).toInt();
        if(nLayerType <= 0 || nLayerType >= ELAYERTYPE::e_LayerTypeCount){
            continue;
        }
        pLayerDraw = (CLayerDraw*)(pItemChild->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
        if(pLayerDraw == nullptr)
            continue;
        vecNewSortLayer.push_back(pLayerDraw);
        //this->
    }
}

void CLayerManagerWidget::GetNewTreeItemSort(std::vector<CLayerDraw*> &vecNewSortLayer) {
    int i = 0, j = 0, nTopItemCount = this->ui->m_pCtlTreeView->topLevelItemCount();
    QTreeWidgetItem* pItem = nullptr;
    for(i = 0; i<nTopItemCount; i++){
        pItem = this->ui->m_pCtlTreeView->topLevelItem(i);
        if(pItem) {
            GetSubLayerChildItem(pItem, vecNewSortLayer);
        }
    }
}

void CLayerManagerWidget::TreeItemIndxResortSlot(QTreeWidgetItem* pTarget, QTreeWidgetItem* pFroms, int nItemCount){
    if(pTarget == nullptr || pFroms == nullptr || nItemCount <= 0){
        return;
    }
    if(this->m_pGLCore == nullptr) {
        this->m_pGLCore = GetGlobalPtr()->m_pGLCore;
        this->m_pSxLayerVector = this->m_pGLCore->m_pLayerVector;
    }
    CRasterDraw* pRasterDrawTarget = (CRasterDraw*)(pTarget->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
    CRasterDraw* pRasterDrawFroms = (CRasterDraw*)(pFroms->data(0, FLAG_IMTE_EXT_DATA).toULongLong());
    if(pRasterDrawTarget == nullptr || pRasterDrawFroms == nullptr){
        return;
    }
    auto ittEnd = this->m_pSxLayerVector->m_mapLayerItem2Indx.end();
    auto ittTarg = this->m_pSxLayerVector->m_mapLayerItem2Indx.find(pRasterDrawTarget);
    auto ittFrom = this->m_pSxLayerVector->m_mapLayerItem2Indx.find(pRasterDrawFroms);
    if(ittEnd == ittTarg || ittEnd == ittFrom){
        return;
    }

    std::vector<CLayerDraw*> vecNewSortLayer;
    this->GetNewTreeItemSort(vecNewSortLayer);

    int nLayerDrawCount = (int)this->m_pSxLayerVector->m_vecLayerItem.size();
    int nVecNewSortLayerCount = (int)vecNewSortLayer.size();
    if(nLayerDrawCount != nVecNewSortLayerCount){
        return;
    }
    this->m_pSxLayerVector->m_vecLayerItem.clear();
    this->m_pSxLayerVector->m_mapLayerItem2Indx.clear();
    this->m_pSxLayerVector->m_vecLayerItem.insert(this->m_pSxLayerVector->m_vecLayerItem.begin(), vecNewSortLayer.begin(), vecNewSortLayer.end());
    int i = 0;
    for (i = 0;i<nLayerDrawCount;i++) {
        this->m_pSxLayerVector->m_mapLayerItem2Indx.insert(
                    std::pair<CLayerDraw*, int>(this->m_pSxLayerVector->m_vecLayerItem[i], i));
    }
    this->m_pGLCore->UpdateWidgets();
}
