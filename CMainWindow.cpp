#include "CMainWindow.h"
#include "ui_CMainWindow.h"

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDialog>

#include "Src_Core/CGlobal.h"
#include "Src_MenuBar/CMenuBar.h"
#include "Src_GlWnd/CMainViewWidget.h"
#include "Src_GlWnd/CMainViewWidgetSub.h"
#include "Src_LayerManager/CLayerManagerWidget.h"
#include "Src_Attribute/CLayerAttribute.h"
#include "Src_Attribute/CChunkAttribute.h"

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow)
{
    ui->setupUi(this);

    CGlobal* pGlobal = GetGlobalPtr();

    pGlobal->m_pMainWindow = this;

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    this->setPalette(palette);

    resize(1740, 955);
    QWidget* pCenterWidget = takeCentralWidget();
    if (pCenterWidget) {
        delete pCenterWidget;
        pCenterWidget = nullptr;
    }
    if(this->ui->mainToolBar) {
        delete this->ui->mainToolBar;
        this->ui->mainToolBar = nullptr;
    }
    this->InitMainWidgets();

    this->setWindowTitle(u8"SuperXView v1.0.0");
}

CMainWindow::~CMainWindow()
{
    delete ui;
}


void CMainWindow::InitMainWidgets(){
    //this->setStyleSheet(
    //"QMainWindow::separator{\n"
    //"background:rgb(255,255,255);\n"
    //"width:2px;\n"
    ////"height:1px;\n"
    //"margin: 4px;\n"
    //"padding: 4px;\n"
    //"}\n");

    CGlobal* pGlobal = GetGlobalPtr();

    this->m_pSxDlgLayerManager = new CLayerManagerWidget(this);
    this->m_pMainViewWidget = new CMainViewWidget(this);
    CMainViewWidgetSub* pSubWnd0 = new CMainViewWidgetSub(this);
    CMainViewWidgetSub* pSubWnd1 = new CMainViewWidgetSub(this);
    this->m_pLayerAttribute = new CLayerAttribute(this);
    this->m_pChunkAttribute = new CChunkAttribute;
    QDialog* pOtherProperty1 = new QDialog;

    this->m_pMenuBar = new CMenuBar(this);
    this->m_pMenuBar->m_pMainViewWidget = this->m_pMainViewWidget;

    QWidget* pTitle = nullptr;
    QWidget* lEmptyWidget = nullptr;

    int nDockArea = 11;//Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea;

    QDockWidget* pDockToolBar = new QDockWidget(this);
    pTitle = pDockToolBar->titleBarWidget();
    lEmptyWidget = new QWidget();
    pDockToolBar->setTitleBarWidget(lEmptyWidget);
    delete pTitle;
    pDockToolBar->setFeatures(QDockWidget::NoDockWidgetFeatures/* | QDockWidget::DockWidgetClosable | QDockWidget::AllDockWidgetFeatures*/);
    pDockToolBar->setAllowedAreas((Qt::DockWidgetAreas)Qt::TopDockWidgetArea /* | Qt::AllDockWidgetAreas*/);
    pDockToolBar->setWidget(this->m_pMenuBar);
    this->m_docks.append(pDockToolBar);

    QDockWidget *pDockLayManager = new QDockWidget(u8"图层管理", this);
    pTitle = pDockLayManager->titleBarWidget();
    lEmptyWidget = new QWidget();
    pDockLayManager->setTitleBarWidget(lEmptyWidget);
    delete pTitle;
    pDockLayManager->setFeatures(QDockWidget::NoDockWidgetFeatures/* | QDockWidget::DockWidgetClosable | QDockWidget::AllDockWidgetFeatures*/);
    pDockLayManager->setAllowedAreas((Qt::DockWidgetAreas)nDockArea);
    pDockLayManager->setWidget(this->m_pSxDlgLayerManager);
    pDockLayManager->setMinimumWidth(230);
    pDockLayManager->setMinimumHeight(600);
    this->m_docks.append(pDockLayManager);

    QDockWidget* pDockMainView111 = nullptr;
    QDockWidget* pDockMainView = new QDockWidget(u8"视图", this);
    pTitle = pDockMainView->titleBarWidget();
    lEmptyWidget = new QWidget();
    pDockMainView->setTitleBarWidget(lEmptyWidget);
    delete pTitle;
    pDockMainView->setFeatures(QDockWidget::NoDockWidgetFeatures/* | QDockWidget::DockWidgetClosable | QDockWidget::AllDockWidgetFeatures*/);
    pDockMainView->setAllowedAreas((Qt::DockWidgetAreas)nDockArea);
    pDockMainView->setWidget(this->m_pMainViewWidget);
    pDockMainView->setMinimumWidth(667);
    pDockMainView->setMinimumHeight(300);
    this->m_docks.append(pDockMainView);
    pDockMainView111 = pDockMainView;

    //if (0) {
        QDockWidget *pDockOutput01 = new QDockWidget(tr("Output1"), this);
        pTitle = pDockOutput01->titleBarWidget();
        lEmptyWidget = new QWidget();
        pDockOutput01->setTitleBarWidget(lEmptyWidget);
        delete pTitle;
        //pDockOutput01->setFeatures(QDockWidget::DockWidgetFloatable);
        //pDockOutput->setAllowedAreas(Qt::BottomDockWidgetArea);
        //this->m_pOutputInfo = new CStatusInfo;
        //pDockOutput->setWidget(this->m_pOutputInfo);
        pDockOutput01->setFeatures(QDockWidget::NoDockWidgetFeatures);
        pDockOutput01->setWidget(pSubWnd0);
        pDockOutput01->setMinimumWidth(595);
        pDockOutput01->setMinimumHeight(300);
        //pDockOutput01->setFixedWidth(595);
        //pDockOutput01->setFixedHeight(300);
        //addDockWidget(Qt::RightDockWidgetArea, pDockOutput);
        this->m_docks.append(pDockOutput01);
        pGlobal->m_vecDockSub.push_back(pDockOutput01);

        QDockWidget *pDockOutput02 = new QDockWidget(tr("Output2"), this);
        pTitle = pDockOutput02->titleBarWidget();
        lEmptyWidget = new QWidget();
        pDockOutput02->setTitleBarWidget(lEmptyWidget);
        delete pTitle;
        //pDockOutput02->setFeatures(QDockWidget::DockWidgetFloatable);
        //pDockOutput->setAllowedAreas(Qt::BottomDockWidgetArea);
        //this->m_pOutputInfo = new CStatusInfo;
        //pDockOutput->setWidget(this->m_pOutputInfo);
        //CMainViewWidgetSub* pTest02 = new CMainViewWidgetSub(this);
        pDockOutput02->setFeatures(QDockWidget::NoDockWidgetFeatures);
        pDockOutput02->setWidget(pSubWnd1);
        pDockOutput02->setMinimumWidth(595);
        pDockOutput02->setMinimumHeight(300);
        //pDockOutput02->setFixedWidth(595);
        //pDockOutput02->setFixedHeight(300);
        //addDockWidget(Qt::RightDockWidgetArea, pDockOutput);

        this->m_docks.append(pDockOutput02);
        pGlobal->m_vecDockSub.push_back(pDockOutput02);
    //}

    QDockWidget *pDockLayerProperty = new QDockWidget(u8"图层属性 ", this);
    pDockLayerProperty->setAllowedAreas((Qt::DockWidgetAreas)nDockArea);
    pDockLayerProperty->setWidget(this->m_pLayerAttribute);
    pDockLayerProperty->setMinimumWidth(100);
    pDockLayerProperty->setMinimumHeight(200);
    this->m_docks.append(pDockLayerProperty);

    this->m_pSxDlgLayerManager->m_pPropertyInfo = this->m_pLayerAttribute;

    this->SetDockerWidgetStyleSheet(pDockLayerProperty);

    QDockWidget *pDockChunkProperty = new QDockWidget(u8"图块属性", this);
    pDockChunkProperty->setAllowedAreas((Qt::DockWidgetAreas)nDockArea);
    pDockChunkProperty->setWidget(this->m_pChunkAttribute);
    pDockChunkProperty->setMinimumWidth(100);
    pDockChunkProperty->setMinimumHeight(200);
    this->m_docks.append(pDockChunkProperty);
    this->SetDockerWidgetStyleSheet(pDockChunkProperty);

    QDockWidget *pDockOther1 = new QDockWidget(u8"其它", this);
    pDockOther1->setAllowedAreas((Qt::DockWidgetAreas)nDockArea);
    pDockOther1->setWidget(pOtherProperty1);
    pDockOther1->setMinimumWidth(100);
    pDockOther1->setMinimumHeight(200);
    this->m_docks.append(pDockOther1);
    this->SetDockerWidgetStyleSheet(pDockOther1);

    this->RemoveAllDock();
    addDockWidget(Qt::TopDockWidgetArea, pDockToolBar);

    //splitDockWidget(pDockToolBar, pDockLayTop, Qt::Vertical);
    splitDockWidget(pDockToolBar, pDockLayManager, Qt::Vertical);
    splitDockWidget(pDockLayManager, pDockMainView, Qt::Horizontal);
    //splitDockWidget(pDockMainView, pDockOutput01, Qt::Vertical);
    //splitDockWidget(pDockOutput01, pDockFill0, Qt::Horizontal);
    splitDockWidget(pDockMainView, pDockOutput01, Qt::Horizontal);
    splitDockWidget(pDockOutput01, pDockOutput02, Qt::Vertical);
    //splitDockWidget(pDockOutput01, pDockLayerProperty, Qt::Horizontal);
    tabifyDockWidget(pDockLayManager, pDockLayerProperty);
    tabifyDockWidget(pDockLayerProperty, pDockChunkProperty);
    tabifyDockWidget(pDockChunkProperty, pDockOther1);
    ShowDock(QList<int>()<<0<<1<<2<<3<<4<<5<<6<<7);//<<6<<7

    //pSubWnd0->hide();
    //pSubWnd1->hide();

    this->m_pMainViewWidget->setFocus();

    pGlobal->m_vecDockSub[0]->hide();
    pGlobal->m_vecDockSub[1]->hide();
}

void CMainWindow::RemoveAllDock()
{
    int nVecDocksCnt = this->m_docks.size();
    for(int i=0;i<nVecDocksCnt;++i) {
        removeDockWidget(m_docks[i]);
    }
}
///
/// \brief 显示指定序号的dock
/// \param index 指定序号，如果不指定，则会显示所有
///
void CMainWindow::ShowDock(const QList<int> &index)
{
    int nVecDocksCnt = this->m_docks.size();
    if (index.isEmpty()) {
        for(int i=0;i<nVecDocksCnt;++i) {
            m_docks[i]->show();
        }
    } else {
        foreach (int i, index) {
            if(i<nVecDocksCnt){
                m_docks[i]->show();
            }
        }
    }
}

void CMainWindow::SetDockerWidgetStyleSheet(QDockWidget* pDockWidget){

    pDockWidget->setStyleSheet(
                //"QDockWidget:title::separator {                                                   \n"
                //" width: 1px;                                                               \n"
                //" height: 1px;                                                              \n"
                //" margin: 1px;                                                              \n"
                //" padding: 1px;                                                             \n"
                //" }                                                                         \n"
                //"                                                                           \n"
                "QDockWidget {                                                              \n"
                "    border: 1px solid red;                                                 \n"
                "}                                                                          \n"
                "/* 标题设置 */                                                              \n"
                "QDockWidget::title {                                                       \n"
                "    text-align: left;                                                      \n"
                "    background: rgb(250, 253, 254);                                                     \n"
                //"    background: rgb(234, 248, 251);                                                     \n"
                "    padding-left: 5px;                                                    \n"
                //"    height: 19px;                                                              \n"
                "    font: 8pt;                                                              \n"
                "}                                                                          \n"
                "                                                                           \n"
                "/* 关闭和显示按钮 */                                                        \n"
                "QDockWidget::close-button, QDockWidget::float-button {                     \n"
                //"    border: /*2px solid transparent;*/                                     \n"
                //"    background: HotPink;                                                   \n"
                //"    padding: 2px;                                                          \n"
                "    icon-size: 14px;  /*maximum icon size */                               \n"
                "}                                                                          \n"
                );
}
