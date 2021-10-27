#ifndef CPICTEXTLABEL_H
#define CPICTEXTLABEL_H

#include <QWidget>

namespace Ui {
class CPicTextLabel;
}

class CPicTextLabel : public QWidget
{
    Q_OBJECT

public:
    explicit CPicTextLabel(QWidget *parent = nullptr);
    ~CPicTextLabel();

    void leaveEvent(QEvent *e);
    void enterEvent(QEvent *e);

    void initImg(QPixmap* pImg, const QString& sToolName);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

signals:
    void clicked();

private:
    Ui::CPicTextLabel *ui;
};

#endif // CPICTEXTLABEL_H
