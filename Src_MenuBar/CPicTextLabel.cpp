#include "CPicTextLabel.h"
#include "ui_CPicTextLabel.h"
#include <QtGui/QPixmap>
#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>

CPicTextLabel::CPicTextLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPicTextLabel)
{
    ui->setupUi(this);
    this->ui->label_Text->adjustSize();
}

CPicTextLabel::~CPicTextLabel()
{
    delete ui;
}

void CPicTextLabel::initImg(QPixmap* pImg, const QString& sToolName){
    //this->ui->label_Pic->setPixmap(QPixmap("D:/QT_CODE_PRJ/testCtrl/rc/Img_MenuTop/undo_01.png"));
    //this->ui->label_Text->setText(QString::fromLocal8Bit("撤销撤销撤销"));

    this->ui->label_Pic->setPixmap(*pImg);
    this->ui->label_Text->setText(sToolName);
    //QRect rc = this->ui->label_Text->rect();
    this->ui->label_Text->adjustSize();
    QRect rc1 = this->ui->label_Text->rect();
    this->setMinimumWidth(rc1.width());

    //connect(this->ui->label_Pic, &CPicTextLabel::clicked, this, &CItemWidget::Clicked_Preview);
}

void CPicTextLabel::enterEvent(QEvent *e)
{
    if(e->type() == QEvent::Enter) {
        //setEnterFlag(true);
        //qDebug() << "PixTextLable ---enter";
        //palette.setColor(QPalette::Background, QColor(255,255,255));
        //this->ui->label_Pic->setBackgroundRole(QPalette::ColorRole::Dark);
        //this->ui->label_Text->setBackgroundRole(QPalette::ColorRole::Dark);
        //emit enter();


        QPalette palette;
        palette.setColor(QPalette::Background, QColor(235, 235, 235));
        this->ui->label_Pic->setAutoFillBackground(true);
        this->ui->label_Pic->setPalette(palette);

        this->ui->label_Text->setAutoFillBackground(true);
        this->ui->label_Text->setPalette(palette);
    }

}

void CPicTextLabel::leaveEvent(QEvent *e)
{
    if(e->type() == QEvent::Leave) {
        //setEnterFlag(false);
        //qDebug() << "PixTextLable ---leave";
        //this->setPixmap(*m_pLeaveImg);
        //this->ui->label_Pic->setBackgroundRole(QPalette::ColorRole::Highlight);
        //this->ui->label_Text->setBackgroundRole(QPalette::ColorRole::Highlight);

        QPalette palette;
        palette.setColor(QPalette::Background, QColor(255,255,255));
        this->ui->label_Pic->setAutoFillBackground(true);
        this->ui->label_Pic->setPalette(palette);

        this->ui->label_Text->setAutoFillBackground(true);
        this->ui->label_Text->setPalette(palette);
    }
}


void CPicTextLabel::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
}

void CPicTextLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton) {
        emit this->clicked();
    }
}

void CPicTextLabel::mouseMoveEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
}
