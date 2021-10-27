#ifndef CLAYERATTRIBUTE_H
#define CLAYERATTRIBUTE_H

#include <QWidget>
#include <unordered_map>

namespace Ui {
class CLayerAttribute;
}

class CLayerDraw;
class CTreeWidgetItem;
class QTreeWidgetItem;
class CLayerAttribute : public QWidget
{
    Q_OBJECT

public:
    explicit CLayerAttribute(QWidget *parent = nullptr);
    ~CLayerAttribute();

    void InitTreeView(CLayerDraw* pLayerDraw);

    std::unordered_map <std::string, QVariant> m_mapAttrValue;

    CTreeWidgetItem* m_pNodeRasterInfo = nullptr;
    CTreeWidgetItem* m_pNodeRangeInfo = nullptr;
    CTreeWidgetItem* m_pNodeProjectRef = nullptr;
    CTreeWidgetItem* m_pNodeStatistics = nullptr;

    CTreeWidgetItem* m_pNodeShapeInfoShp = nullptr;

    CTreeWidgetItem* m_pNodeFileInfo = nullptr;
public slots:
    void itemDbPressedSlot(QTreeWidgetItem* pItem, int nColumn);
private:
    Ui::CLayerAttribute *ui;
};

#endif // CLAYERATTRIBUTE_H
