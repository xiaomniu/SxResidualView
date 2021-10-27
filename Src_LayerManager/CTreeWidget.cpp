#include "CTreeWidget.h"
#include "CTreeWidgetItem.h"

#include <vector>

CTreeWidget::CTreeWidget (QWidget *parent/* = 0*/)
    :QTreeWidget (parent)
{
    void (CTreeWidget::*pfnItemCheckStateChanged)(QTreeWidgetItem*, Qt::CheckState) = &CTreeWidget::itemCheckStateChangedIn;
    void (CTreeWidget::*pfnItemCheckStateChangedSlot)(QTreeWidgetItem*, Qt::CheckState) = &CTreeWidget::itemCheckStateChangedSlot;
    connect(this, pfnItemCheckStateChanged, this, pfnItemCheckStateChangedSlot);
}
CTreeWidget::~CTreeWidget () {

}

int CTreeWidget::compare_Xiao_To_Da( const void *pVal1, const void *pVal2 )
{
    int nVal1 = *((int*)pVal1);
    int nVal2 = *((int*)pVal2);
    if(nVal1 == nVal2)
        return 0;
    if( nVal1 > nVal2)
        return 1;
    return -1;
}

void CTreeWidget::reSort()
{
    QString sItemText;
    if (!m_TarItem || !m_TarItem->parent() ||
        m_TarItem->parent() != m_dragItem[0]->parent())
        return;
    else
    {
        QTreeWidgetItem * parent = m_dragItem[0]->parent();
        int nIdxTargItem = parent->indexOfChild(m_TarItem);
        int nIdxDragItem = parent->indexOfChild(m_dragItem[0]);
        int nIdxInsert = 1;
        if(nIdxDragItem < nIdxTargItem){
            nIdxInsert = 1;
        }else{
            nIdxInsert = 0;
        }
        std::vector<int> index;
        for (int i = 0; i < m_dragItem.size(); i++)
        {
            QTreeWidgetItem* child = m_dragItem[i];
            if (child) {
                sItemText = child->text(0);
                index.push_back(parent->indexOfChild(child));
            }
        }
        int nIndexCnt = (int)index.size();
        //qSort(index.begin(), index.end()); //按升序对范围 [ begin , end ) 中的项目进行排序
        qsort(index.data(), nIndexCnt, sizeof(int), CTreeWidget::compare_Xiao_To_Da);

        int nCount = parent->childCount();
        //将拖拽节点全部倒叙摘下
        QList<QTreeWidgetItem*> tempChild;
        for (int i = nIndexCnt - 1; i >= 0; i--)
        {
            QTreeWidgetItem* child = parent->takeChild(index[i]);
            sItemText = child->text(0);
            tempChild.insert(0, child);
        }
        int nResetIndxChildCount = tempChild.size();
        if(nResetIndxChildCount <= 0)
            return;
        nCount = parent->childCount();
        sItemText = m_TarItem->text(0);
        for (int i = 0; i <parent->childCount(); i++)
        {
            QTreeWidgetItem* child = parent->child(i);
            sItemText = child->text(0);
            if (child != m_TarItem)
                continue;
            nIdxInsert = i + nIdxInsert;  //Insert New Index
            //if(0 > nIdxTargItem){
            //    nIdxTargItem = 0;
            //}
            parent->insertChildren(nIdxInsert, tempChild);

            emit widgetChanged();
            emit this->ItemIndxResort(m_TarItem, tempChild[0], tempChild.size());
            break;
        }
    }

    int nParentSize= m_parentItem.size();
    for (int i = 0; i < nParentSize; i++)
    {
        QTreeWidgetItem *itemParent = topLevelItem(i);
        itemParent->text(0);
        int nChildSize = itemParent->childCount();
        for (int j = 0; j < nChildSize; j++)
        {
            QTreeWidgetItem * itemChild = itemParent->child(j);
            itemChild->setText(1, QString::number(j+1));
        }
    }
}

void CTreeWidget::dragEnterEvent(QDragEnterEvent * ev)
{
    printf("DragEnter 11\n");
    ev->setDropAction(Qt::MoveAction);
    QList<QTreeWidgetItem*> selectedItem = selectedItems();
    QList<QTreeWidgetItem*> childrenItem;
    for (int i = 0; i < selectedItem.size(); i++)
    {
        if (selectedItem[i] && selectedItem[i]->parent())
        {
            childrenItem.push_back(selectedItem[i]);
        }
    }
    if (childrenItem.size() < 1){
        printf("DragEnter 33\n");
        return;
    }
    QTreeWidgetItem *parent = childrenItem[0]->parent();
    for (int i = 0; i < childrenItem.size(); i++)
    {
        if (parent != childrenItem[i]->parent()) {
            printf("DragEnter 44\n");
            return;//必须是同父节点下的子节点
        }
    }

    QTreeWidget::dragEnterEvent(ev);
    m_dragItem = childrenItem;//获取拖动节点
    printf("DragEnter 22\n");
}

void CTreeWidget::dragMoveEvent(QDragMoveEvent * ev)
{
    printf("DragMove 11\n");
    ev->setDropAction(Qt::MoveAction);
    QTreeWidgetItem* itemOver = itemAt(ev->pos());
    //设置可拖拽区域
    QTreeWidgetItem *parent = m_dragItem[0]->parent();
    if (itemOver && itemOver->parent() == parent)
    {
        QTreeWidget::dragMoveEvent(ev);
        printf("DragMove 33\n");
        return;
    }
    ev->ignore();//事件忽略
    printf("DragMove 22\n");
}

void CTreeWidget::dropEvent(QDropEvent * ev)
{
    printf("DropEvent 11\n");
    m_TarItem = itemAt(ev->pos());//获得目标节点
    ev->ignore();
    reSort();//自己手动处理
    printf("DropEvent 22\n");
}

void CTreeWidget::SetItemCheckState(QTreeWidgetItem* pItem, Qt::CheckState nIsChecked){
    CTreeWidgetItem* pItemChild = nullptr;
    int i = 0, nCount = pItem->childCount();
    for(i = nCount - 1; i >= 0; i--) {
        pItemChild = (CTreeWidgetItem*)pItem->child(i);
        if(pItemChild){
            pItemChild->setCheckState(0, (Qt::CheckState)nIsChecked);
            this->SetItemCheckState(pItemChild, nIsChecked);
        }
    }
}

void CTreeWidget::SetParentItemCheckState(QTreeWidgetItem* pItem){
    QTreeWidgetItem* pParentItem = pItem->parent();
    if(pParentItem == nullptr) {
        return;
    }
    QTreeWidgetItem* pChild = nullptr;
    int nAllChecked = 1;
    int i = 0, nCount = pParentItem->childCount();
    for(i  = 0; i<nCount; i++){
        pChild = pParentItem->child(i);
        if(pChild){
            if(Qt::CheckState::Checked != pChild->checkState(0)){
                nAllChecked = 0;
                break;
            }
        }
    }
    pParentItem->setCheckState(0, nAllChecked == 1 ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    this->SetParentItemCheckState(pParentItem);
}

void CTreeWidget::itemCheckStateChangedSlot(QTreeWidgetItem* pItem, Qt::CheckState nChecked)
{
    if (!pItem){
        return;
    }

    if (pItem->isDisabled() || pItem->flags() == Qt::NoItemFlags){
        return;
    }

    void (CTreeWidget::*pfnItemCheckStateChanged)(QTreeWidgetItem*, Qt::CheckState) = &CTreeWidget::itemCheckStateChangedIn;
    void (CTreeWidget::*pfnItemCheckStateChangedSlot)(QTreeWidgetItem*, Qt::CheckState) = &CTreeWidget::itemCheckStateChangedSlot;
    disconnect(this, pfnItemCheckStateChanged, this, pfnItemCheckStateChangedSlot);

    this->SetItemCheckState(pItem, nChecked);
    this->SetParentItemCheckState(pItem);

    connect(this, pfnItemCheckStateChanged, this, pfnItemCheckStateChangedSlot);

    emit this->ItemCheckStateChanged(pItem, nChecked);
}


