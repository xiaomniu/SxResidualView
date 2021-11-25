#ifndef CLAYERMANAGERWIDGET_H
#define CLAYERMANAGERWIDGET_H

#include <QWidget>

namespace Ui {
class CLayerManagerWidget;
}

class CLayerDraw;
class QTreeWidgetItem;
class CTreeWidgetItem;

class CLayerDraw;
class COpenGLCore;
class CLayerVector;
class QVBoxLayout;
class QTextEdit;
class QListWidget;
class QMouseEvent;
class QPixmap;
class CLayerAttribute;

class CLayerManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CLayerManagerWidget(QWidget *parent = nullptr);
    ~CLayerManagerWidget();


    QAction* m_pActCtrlRemoveLayer = nullptr;
    QAction* m_pActCtrlShowNoData = nullptr;
    QAction* m_pActCtrlShowAlpha = nullptr;
    QAction* m_pActCtrlRollerBlind = nullptr;
    QAction* m_pActCtrlEditor = nullptr;
    QAction* m_pActCtrlCreateMosaicLine = nullptr;
    QAction* m_pActCtrlTravelGcp = nullptr;
    QAction* m_pActCtrlSelectLayerColor = nullptr;
    QAction* m_pActCtrlShowLayerAttribute = nullptr;
    QAction* m_pActCtrlDeleteAllChunks = nullptr;
    QAction* m_pActCtrlShowResidual = nullptr;
    QAction* m_pActCtrlShowResidualWithOutArrow = nullptr;

    COpenGLCore* m_pGLCore = nullptr;
    CLayerVector* m_pSxLayerVector = nullptr;

    CTreeWidgetItem* m_pLayerGroup0 = nullptr;
    CTreeWidgetItem* m_pLayerGroup1 = nullptr;
    std::map<std::string, QIcon> m_publicIconMap;

    CLayerAttribute* m_pPropertyInfo = nullptr;

public:
    void AddItem(const std::string& sLayerName, int nLayerType, void* pExtData, QTreeWidgetItem* pLayerGroup = nullptr);

    int InitTreeView();

    void GetSubLayerChildItem(QTreeWidgetItem* pItem, std::vector<CLayerDraw*> &vecNewSortLayer);
    void GetNewTreeItemSort(std::vector<CLayerDraw*> &vecNewSortLayer);

    bool eventFilter(QObject *,QEvent *);    //注意这里

public slots:
    void Clicked_AddTif_Slot();
    void Clicked_AddResidualFile_Slot();
    //void treeItemChanged(QStandardItem* item);
    void itemTreeClickedSlot(const QModelIndex &index);
    void itemTreeDBClickedSlot(const QModelIndex &index);
    void itemTreeMenuSlot(const QPoint &pos);
    void itemTreeMenuRemoveLayerSlot();
    void itemTreeMenuShowNoDataSlot(bool checked);
    void itemTreeMenuShowAlphaSlot(bool checked);
    void itemTreeMenuRollerBlindSlot(bool checked);
    void itemTreeMenuEditorSlot(bool checked);
    void itemTreeMenuCreateMosaicLineSlot(bool checked);
    void itemTreeMenuTravelGcpSlot(bool checked);
    void itemTreeMenuSelectColorSlot(bool checked);
    void itemTreeMenuShowLayerAttributeSlot(bool checked);
    void itemTreeMenuDeleteAllChunksSlot(bool checked);
    void itemTreeMenuShowResidualSlot(bool checked);
    void itemTreeMenuShowResidualWithoutArrowSlot(bool checked);

    void TreeItemCheckStateChangedSlot(QTreeWidgetItem* pItem, Qt::CheckState nCheckState);
    void TreeItemIndxResortSlot(QTreeWidgetItem* pTarget, QTreeWidgetItem* pFroms, int nItemCount);

private:
    Ui::CLayerManagerWidget *ui;
};

#endif // CLAYERMANAGERWIDGET_H
