#include "CChunkAttribute.h"
#include "ui_CChunkAttribute.h"

#include <QStandardItemModel>
#include <QMessageBox>

#include "Src_Core/Misc.h"
#include "Src_Core/CGlobal.h"
#include "Src_Geometry/CLayerDraw.h"
#include "Src_Geometry/CLayerGeoDraw.h"
#include "Src_Geometry/CLayerVector.h"
#include "Src_LayerManager/CTreeWidgetItem.h"

CChunkAttribute::CChunkAttribute(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CChunkAttribute)
{
    ui->setupUi(this);

    GetGlobalPtr()->m_pChunkAttribute = this;

    this->ui->m_treeView_ChunkOid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->m_pChunkOidTreeModel = new QStandardItemModel(this->ui->m_treeView_ChunkOid);
    this->ui->m_treeView_ChunkOid->setModel(this->m_pChunkOidTreeModel);

    this->ui->m_treeWidget_ChunkAtrribute->setColumnCount(2);
    this->ui->m_treeWidget_ChunkAtrribute->setColumnWidth(0, 150);
    this->ui->m_treeWidget_ChunkAtrribute->setColumnWidth(1, 1300);
    QStringList sHeaderTexts;
    sHeaderTexts<<u8"属性名"<<u8"属性值";
    this->ui->m_treeWidget_ChunkAtrribute->setHeaderLabels(sHeaderTexts);

    this->InitAttributeView(nullptr);

    connect(this->ui->m_treeView_ChunkOid ,&QAbstractItemView::clicked, this, &CChunkAttribute::itemTreeClickedSlot);
    connect(this->ui->m_treeView_ChunkOid ,&QAbstractItemView::doubleClicked, this, &CChunkAttribute::itemTreeDbClickedSlot);
}

CChunkAttribute::~CChunkAttribute()
{
    delete ui;
}

void CChunkAttribute::InitChunkTree()
{
    //1，构造Model，这里示例具有3层关系的model构造过程
    QStandardItemModel* model = new QStandardItemModel(this->ui->m_treeView_ChunkOid);
    model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("序号") << QStringLiteral("名称"));     //设置列头
    for(int i=0;i<5;i++)
    {
        //一级节点，加入mModel
        QList<QStandardItem*> items1;
        QStandardItem* item1 = new QStandardItem(QString::number(i));
        items1.append(item1);
        QStandardItem* item2 = new QStandardItem(QStringLiteral("一级节点"));
        items1.append(item2);
        model->appendRow(items1);

        for(int j=0;j<5;j++)
        {
            //二级节点,加入第1个一级节点
            QList<QStandardItem*> items2;
            QStandardItem* item3 = new QStandardItem(QString::number(j));
            items2.append(item3);
            QStandardItem* item4 = new QStandardItem(QStringLiteral("二级节点"));
            items2.append(item4);
            item1->appendRow(items2);

            for(int k=0;k<5;k++)
            {
                //三级节点,加入第1个二级节点
                QList<QStandardItem*> items3;
                QStandardItem* item5 = new QStandardItem(QString::number(k));
                items3.append(item5);
                QStandardItem* item6 = new QStandardItem(QStringLiteral("三级节点"));
                items3.append(item6);
                item3->appendRow(items3);
            }
        }
    }
    //2，给QTreeView应用model
    this->ui->m_treeView_ChunkOid->setModel(model);
}

int CChunkAttribute::InitAttributeView(CChunk* pChunk){
    if(pChunk == nullptr){
        return 1;
    }
    /////////////////////////////////////////

    /////////////////////////////////////////
    //this->ui->m_label_ChunkBaseInfo->setText(u8"图块范围：左下(-999.999, -999.999)，右上(999.999, 999.999)");

    /////////////////////////////////////////
    this->ui->m_treeWidget_ChunkAtrribute->clear();
    //if(this->m_pNodeOrignalInfo){
    //    this->m_pNodeOrignalInfo->takeChildren();
    //    delete this->m_pNodeOrignalInfo;
    //    this->m_pNodeOrignalInfo = nullptr;
    //}
    //if(this->m_pNodeExternInfo){
    //    this->m_pNodeExternInfo->takeChildren();
    //    delete this->m_pNodeExternInfo;
    //    this->m_pNodeExternInfo = nullptr;
    //}

    std::unordered_map<int, QString> mapLayerType;
    mapLayerType[ELAYERTYPE::e_Img] = u8"栅格";
    mapLayerType[ELAYERTYPE::e_Tiff] = u8"栅格";
    mapLayerType[ELAYERTYPE::e_Tif] = u8"栅格";
    mapLayerType[ELAYERTYPE::e_shape] = u8"矢量";
    mapLayerType[ELAYERTYPE::e_shapePoint] = u8"点";
    mapLayerType[ELAYERTYPE::e_shapePolygon] = u8"面";
    mapLayerType[ELAYERTYPE::e_shapePolyline] = u8"线";

    CTreeWidgetItem* pItem = nullptr;
    QString sItemText;
    char szItemText[128]={0};
    this->m_pNodeOrignalInfo = new CTreeWidgetItem(QStringList()<<u8"常规");
    //this->m_pNodeOrignalInfo->setData(0, FLAG_ITEM_GROUP_NODE, QVariant((int)1001));
    this->ui->m_treeWidget_ChunkAtrribute->addTopLevelItem(this->m_pNodeOrignalInfo);
    sItemText = mapLayerType[pChunk->m_pBelongLayer->m_nLayerType];
    pItem = new CTreeWidgetItem(QStringList()<<u8"类型"<<sItemText);
    this->m_pNodeOrignalInfo->addChild(pItem);
    sItemText = QString::number(pChunk->m_vecAttributeValues[0].GetIntValue());
    pItem = new CTreeWidgetItem(QStringList()<<u8"OID"<<sItemText);
    this->m_pNodeOrignalInfo->addChild(pItem);
    sItemText = QString::number(pChunk->m_vecChunkPoints.size());
    pItem = new CTreeWidgetItem(QStringList()<<u8"节点个数"<<sItemText);
    this->m_pNodeOrignalInfo->addChild(pItem);

    /////////////////////////////////////////////////////////////////////////////////////////
    this->m_pNodeExternInfo = new CTreeWidgetItem(QStringList()<<u8"扩展");
    this->ui->m_treeWidget_ChunkAtrribute->addTopLevelItem(this->m_pNodeExternInfo);
    //sprintf(szItemText, "%.9f, %.9f, %.9f, %.9f", pChunk->m_chunkBound.x, pChunk->m_chunkBound.y, pChunk->m_chunkBound.z, pChunk->m_chunkBound.w);
    sItemText = QString::number(pChunk->m_chunkBound.x, 'f', 9) + ", "
        + QString::number(pChunk->m_chunkBound.y, 'f', 9) + ", "
        + QString::number(pChunk->m_chunkBound.z, 'f', 9) + ", "
        + QString::number(pChunk->m_chunkBound.w, 'f', 9);

    //sItemText = QString("%.9f, %.9f, %.9f, %.9f").arg(pChunk->m_chunkBound.x).arg(pChunk->m_chunkBound.y).arg(pChunk->m_chunkBound.z).arg(pChunk->m_chunkBound.w);
    pItem = new CTreeWidgetItem(QStringList()<<u8"范围"<<sItemText);
    this->m_pNodeExternInfo->addChild(pItem);
    /////////////////////////////////////////////////////////////////////////////////////////

    this->m_pNodeSpecialInfo = new CTreeWidgetItem(QStringList()<<u8"特性");
    this->ui->m_treeWidget_ChunkAtrribute->addTopLevelItem(this->m_pNodeSpecialInfo);

    std::string sFldName, sFldValue;
    int i = 1, nFldCount = (int)pChunk->m_pBelongLayer->m_vecLayerFields.size();
    for(i = 1; i<nFldCount; i++){
        sFldName = pChunk->m_pBelongLayer->m_vecLayerFields[i].m_sFieldName;
        sFldValue = pChunk->m_vecAttributeValues[i].GetValueAsString();

        pItem = new CTreeWidgetItem(QStringList()
                                    <<CMisc::AnsiToUtf8(sFldName.c_str()).c_str()
                                    <<CMisc::AnsiToUtf8(sFldValue.c_str()).c_str());
        this->m_pNodeSpecialInfo->addChild(pItem);
    }
    /////////////////////////////////////////////////////////////////////////////////////////

    this->m_pNodeOrignalInfo->setExpanded(true);
    this->m_pNodeExternInfo->setExpanded(true);
    this->m_pNodeSpecialInfo->setExpanded(true);

    return 1;
}

int CChunkAttribute::FreshViewByChunks(){

    //if(this->m_pChunkOidTreeModel){
    //    delete this->m_pChunkOidTreeModel;
    //    this->m_pChunkOidTreeModel = nullptr;
    //}

    //int nIdx = 0, nChildCnt = this->m_pChunkOidTreeModel->rowCount();
    //for(nIdx = nChildCnt-1; nIdx >= 0; nIdx--){
    //    this->m_pChunkOidTreeModel->removeRow(nIdx);
    //}
    //this->m_pChunkOidTreeModel->removeRows(0, );

    //this->m_pChunkOidTreeModel = new QStandardItemModel(this->ui->m_treeView_ChunkOid);
    //this->ui->m_treeView_ChunkOid->setModel(this->m_pChunkOidTreeModel);

    this->m_pChunkOidTreeModel->clear();
    this->ui->m_treeWidget_ChunkAtrribute->clear();
    //this->m_pChunkOidTreeModel->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("OID"));

    if(this->m_nShowHide == 0)
        return 0;
    CLayerVector* pLayerVector = GetGlobalPtr()->m_pLayerVector;
    CLayerGeoDraw* pLayer = nullptr;
    int i = 0, nLayerCnt = (int)pLayerVector->m_vecLayerItem.size();
    CChunk* pChunk = nullptr;
    int j = 0, nChunkCnt = 0, nChunkOperateIndx = -1;
    int nTreeItemIndx = 0;
    for(i=0; i<nLayerCnt; i++){
        pLayer = (CLayerGeoDraw*)pLayerVector->m_vecLayerItem[i];
        if(pLayer->m_nShowOrHide == ESHOWORHIDE::e_hide){
            continue;
        }
        if ( !(pLayer->m_nLayerType == ELAYERTYPE::e_shapePoint
                || pLayer->m_nLayerType == ELAYERTYPE::e_shapePolyline
                || pLayer->m_nLayerType == ELAYERTYPE::e_shapePolygon))
        {
            continue;
        }
        pLayer->m_pVecPointsIndxOperateHit->clear();
        pLayer->m_pVecPointsIndxOperatePointNodeHit->clear();
        nChunkCnt = (int)pLayer->m_pVecEleChunksOperate->size();
        if (nChunkCnt <= 0)
            continue;

        QStandardItem* item1 = new QStandardItem(QString(CMisc::AnsiToUtf8(pLayer->m_sLayerName.c_str()).c_str()));
        printf("\n>>>\nitem1: %p\n", item1);
        this->m_pChunkOidTreeModel->setItem(nTreeItemIndx++, 0, item1);

        if(nChunkCnt > 5000){
            nChunkCnt = 5000;
            QMessageBox::information(this, u8"友情提示", u8"由于选中图块数量过大，此图块属性列表只显示5000个图块的信息记录！");
        }
        for(j = 0; j<nChunkCnt; j++) {
            nChunkOperateIndx = pLayer->m_pVecEleChunksOperate->at(j);
            pChunk = pLayer->m_vecChunkDatas[nChunkOperateIndx];

            QStandardItem* item3 = new QStandardItem(QString::number(nChunkOperateIndx));
            //printf("item3: %p pChunk: %p OID: %d\n", item3, pChunk, nChunkOperateIndx);
            item3->setData((uint_64)pChunk, Qt::UserRole + 1);
            item1->setChild(j, 0, item3);
        }
        this->ui->m_treeView_ChunkOid->setExpanded(item1->index(), true);
    }
    return 1;
}

void CChunkAttribute::itemTreeClickedSlot(const QModelIndex &modelIndex){
    printf("select Indx: %p\n>>>\n", modelIndex.row());
    QStandardItem* pItem = this->m_pChunkOidTreeModel->itemFromIndex(modelIndex);
    if(pItem == nullptr)
        return;
    //QStandardItem* pItem = (QStandardItem*)modelIndex.model();
    uint_64 nChunkPtr = pItem->data(Qt::UserRole + 1).toULongLong();
    CChunk* pChunk = (CChunk*)nChunkPtr;
    printf("pItem: %p nChunkPtr: %p pChunk: %p\n>>>\n", pItem, nChunkPtr, pChunk);
    this->InitAttributeView(pChunk);

    //QAbstractItemModel* pItem = (QAbstractItemModel*)modelIndex.model();
    //QModelIndex indxModel = pItem->index(modelIndex.row(), 0);
    //QString sOid= indxModel.data().toString();
}

void CChunkAttribute::itemTreeDbClickedSlot(const QModelIndex &modelIndex){
    printf("select Indx: %p\n>>>\n", modelIndex.row());
    QStandardItem* pItem = this->m_pChunkOidTreeModel->itemFromIndex(modelIndex);
    if(pItem == nullptr)
        return;
    //QStandardItem* pItem = (QStandardItem*)modelIndex.model();
    uint_64 nChunkPtr = pItem->data(Qt::UserRole + 1).toULongLong();
    CChunk* pChunk = (CChunk*)nChunkPtr;
    printf("pItem: %p nChunkPtr: %p pChunk: %p\n>>>\n", pItem, nChunkPtr, pChunk);
    if(pChunk)
        pChunk->SetFresh();
}
