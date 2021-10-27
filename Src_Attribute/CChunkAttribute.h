#ifndef CCHUNKATTRIBUTE_H
#define CCHUNKATTRIBUTE_H

#include <QWidget>
#include <vector>


namespace Ui {
class CChunkAttribute;
}

class CChunk;
class CTreeWidgetItem;
class QStandardItemModel;

class CChunkAttribute : public QWidget
{
    Q_OBJECT

public:
    explicit CChunkAttribute(QWidget *parent = nullptr);
    ~CChunkAttribute();

    int InitAttributeView(CChunk* pChunk);
    void InitChunkTree();
    int FreshViewByChunks();

    QStandardItemModel* m_pChunkOidTreeModel = nullptr;

    CTreeWidgetItem* m_pNodeOrignalInfo = nullptr;
    CTreeWidgetItem* m_pNodeExternInfo = nullptr;
    CTreeWidgetItem* m_pNodeSpecialInfo = nullptr;

    int m_nShowHide = 0;
    int m_nHitSelectChunkIndx = -1;
    std::vector<CChunk*> m_vecSelectChunks;
public slots:
    void itemTreeClickedSlot(const QModelIndex &modelIndex);
    void itemTreeDbClickedSlot(const QModelIndex &modelIndex);

private:
    Ui::CChunkAttribute *ui;
};

#endif // CCHUNKATTRIBUTE_H
