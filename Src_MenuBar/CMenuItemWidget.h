#ifndef MENUITEMWIDGET_H
#define MENUITEMWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStyleOption>
#include <QtGui/QPainter>
#include <QtCore/QFile>
class CMenuItemWidget : public QWidget
{
    Q_OBJECT

public:
    CMenuItemWidget(const QPixmap& icon, const QString& text, QWidget *parent = 0)
    {
        QLabel* label_Icon = new QLabel(this);
        label_Icon->setFixedSize(24, 24);
        label_Icon->setScaledContents(true);
        label_Icon->setPixmap(icon);

        QLabel* label_Text = new QLabel(text,this);

        QHBoxLayout* layout = new QHBoxLayout;

        layout->setContentsMargins(20, 6, 50, 6);
        layout->setSpacing(10);
        layout->addWidget(label_Icon);
        layout->addWidget(label_Text);

        setLayout(layout);

        setFixedWidth(200);

        //QFile qss("StyleSheet.qss");
        //qss.open(QFile::ReadOnly);
        //this->setStyleSheet(qss.readAll());
        //qss.close();
        const char* szMenuStyleSheet = {
            "QMenu {\n"
            "    background-color: #FCFCFC;\n"
            "    border: 1px solid #8492A6;\n"
            "}\n"
            "QMenu::item {\n"
            "    background-color: transparent;\n"
            "}\n"
            "MenuItemWidget::hover{\n"
            "    border: 1px solid #FFB700;\n"
            "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FEF9F4, stop:0.38 #FDE0BD,stop:0.39 #FFCE69, stop:1 #FFFFE7);\n"
            "}\n"
            "QMenu::item:selected {\n"
            "    background-color: #654321;\n"
            "}\n"
        };
        this->setStyleSheet(QString::fromUtf8(szMenuStyleSheet));

    }
    ~CMenuItemWidget()
    {}

protected:
    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE{
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

private:
};
#endif // MENUITEMWIDGET_H
