#ifndef SXTREEVIEW_H
#define SXTREEVIEW_H
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QList>

class CTreeWidgetItem;
class CTreeWidget  : public QTreeWidget
{
    Q_OBJECT

public:
    explicit CTreeWidget (QWidget *parent = 0);
    ~CTreeWidget ();

    static int compare_Xiao_To_Da( const void *pVal1, const void *pVal2 );

    void SetItemCheckState(QTreeWidgetItem* pItem, Qt::CheckState nChecked);
    void SetParentItemCheckState(QTreeWidgetItem* pItem);

signals:
    void widgetChanged();
    void ItemIndxResort(QTreeWidgetItem*, QTreeWidgetItem*, int);
    void ItemCheckStateChanged(QTreeWidgetItem*, Qt::CheckState);
    void itemCheckStateChangedIn(QTreeWidgetItem*, Qt::CheckState);//内部使用

public slots:
    //应用
    void apply(){
        //QTreeWidget::apply();
    }
    void itemCheckStateChangedSlot(QTreeWidgetItem* item, Qt::CheckState nChecked);

private:
    //初始化
    void init();

    //重新排序
    void reSort();

protected:
    //drag操作在窗体移动事件
    void dragMoveEvent(QDragMoveEvent* event) override;

    //drag操作进入窗体
    void dragEnterEvent(QDragEnterEvent* event) override;

    //drop释放事件
    void dropEvent(QDropEvent* event) override;

private:
    QTreeWidgetItem * m_TarItem = NULL;//拖动的目标点
    QList<QTreeWidgetItem*> m_dragItem;//拖动的节点
    QList<QTreeWidgetItem*> m_parentItem;//所有父节点

};
#endif
