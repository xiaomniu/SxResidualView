#include "CTreeWidgetItem.h"
#include "CTreeWidget.h"

void CTreeWidgetItem ::setData(int column, int role, const QVariant& value)
{
    const bool isCheckChange = column == 0
        && role == Qt::CheckStateRole
        && data(column, role).isValid() // Don't "change" during initialization
        && checkState(0) != value;
    QTreeWidgetItem::setData(column, role, value);
    if (isCheckChange) {
        CTreeWidget *tree = static_cast<CTreeWidget *>(treeWidget());
        emit tree->itemCheckStateChangedIn((QTreeWidgetItem*)this, checkState(0));
    }
}
void CTreeWidgetItem ::removeAllChildren(){
    QTreeWidgetItem* child = nullptr;
    QList<QTreeWidgetItem*> vecChildren = this->takeChildren();
    int nChlidCount = this->childCount();
    for (int i = nChlidCount - 1; i >= 0; i--)
    {
        child = this->takeChild(i);
        delete child;
    }
}
