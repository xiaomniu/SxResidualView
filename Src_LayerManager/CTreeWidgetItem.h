#ifndef SXTREEWIDGETSITEM_H
#define SXTREEWIDGETSITEM_H
#include <QtWidgets/QTreeWidgetItem>

class CTreeWidgetItem : public QTreeWidgetItem
{
public:
    CTreeWidgetItem ();

    CTreeWidgetItem (int type = Type) :QTreeWidgetItem(type){ }
    CTreeWidgetItem (const QStringList & strings, int type = Type) : QTreeWidgetItem(strings, type){ }
    CTreeWidgetItem (QTreeWidget * parent, int type = Type) : QTreeWidgetItem(parent, type){}
    CTreeWidgetItem (QTreeWidget * parent, const QStringList & strings, int type = Type)
        :QTreeWidgetItem(parent, strings, type){}
    CTreeWidgetItem (QTreeWidget * parent, QTreeWidgetItem * preceding, int type = Type)
        : QTreeWidgetItem(parent, preceding, type){}
    CTreeWidgetItem (QTreeWidgetItem * parent, int type = Type) :QTreeWidgetItem(parent, type){}
    CTreeWidgetItem (QTreeWidgetItem * parent, const QStringList & strings, int type = Type)
        :QTreeWidgetItem(parent, strings, type){}
    CTreeWidgetItem (QTreeWidgetItem * parent, QTreeWidgetItem * preceding, int type = Type)
        :QTreeWidgetItem(parent, preceding, type){}

    ~CTreeWidgetItem (){}

    void setData(int column, int role, const QVariant& value);
    void removeAllChildren();
};

#endif
