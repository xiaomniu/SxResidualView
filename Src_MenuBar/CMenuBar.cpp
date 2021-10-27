#include "CMenuBar.h"
#include "ui_CMenuBar.h"
#include "Src_Core/CGlobal.h"

#include "Src_MenuBar/CToolMenuHandler.h"
#include "Src_LayerManager/CLayerManagerWidget.h"
#include "Src_GlWnd/CMainViewWidget.h"

CMenuBar::CMenuBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMenuBar)
{
    ui->setupUi(this);
    CGlobal* pGlobal = GetGlobalPtr();
    pGlobal->m_pMenuBar = this;
    this->m_sExeDir = pGlobal->m_sExeDir;

    this->setFixedHeight(130);

    this->m_pToolMenuHandler = new CToolMenuHandler(this);

    this->InitTabPage00_File();
    this->InitTabPage01_Start();
    this->InitTabPage02_Edit();
    this->InitTabPage03_Tools();

    this->m_vecTabPages.push_back(ui->tab_page00_file);
    this->m_vecTabPages.push_back(ui->tab_page01_start);
    this->m_vecTabPages.push_back(ui->tab_page02_edit);
    this->m_vecTabPages.push_back(ui->tab_page03_tool);

    ui->btn_Show_Hide->setParent(ui->tab_Tools);
    ui->btn_Show_Hide->setIcon(QIcon((this->m_sExeDir + "/Res/ImgMenuBar/MenuBar_hide.png").c_str()));
    connect(ui->btn_Show_Hide, &QPushButton::clicked, this, &CMenuBar::hideOrShowTabWidget);

    ui->tab_Tools->setTabText(ui->tab_Tools->indexOf(ui->tab_page00_file), QCoreApplication::translate("CMenuBar", u8"文件", nullptr));
    ui->tab_Tools->setTabText(ui->tab_Tools->indexOf(ui->tab_page01_start), QCoreApplication::translate("CMenuBar", u8"常规", nullptr));
    ui->tab_Tools->setTabText(ui->tab_Tools->indexOf(ui->tab_page02_edit), QCoreApplication::translate("CMenuBar", u8"编辑", nullptr));
    ui->tab_Tools->setTabText(ui->tab_Tools->indexOf(ui->tab_page03_tool), QCoreApplication::translate("CMenuBar", u8"工具", nullptr));

    this->m_nMaxTabPagesIndx = (int)this->m_vecTabPages.size();

    this->m_nMaxTabPagesIndx -= 1;

    ui->tab_page02_edit->setHidden(true);
    ui->tab_page03_tool->setHidden(true);
    this->ui->tab_Tools->removeTab(3);
    this->ui->tab_Tools->removeTab(2);

    this->ui->tab_Tools->setCurrentIndex(0);
}

void CMenuBar::hideOrShowTabWidget(int nTabIndx)
{
    //if(nTabIndx == this->m_nMaxTabPagesIndx )
    {
        printf("tabIndx : %d \n", nTabIndx);
        static bool isTabWidgetHide = false;
        this->setFixedHeight(isTabWidgetHide ? 130 : 40);
        isTabWidgetHide = !isTabWidgetHide;
        if(isTabWidgetHide) {
            ui->btn_Show_Hide->setIcon(QIcon((this->m_sExeDir + "/Res/ImgMenuBar/MenuBar_show.png").c_str()));//res_rc
        }else{
            ui->btn_Show_Hide->setIcon(QIcon((this->m_sExeDir + "/Res/ImgMenuBar/MenuBar_hide.png").c_str()));//res_rc
        }
    }
}

void CMenuBar::resizeEvent(QResizeEvent *event)
{
    ui->btn_Show_Hide->move(this->width()-50, ui->btn_Show_Hide->pos().y());
}

CMenuBar::~CMenuBar()
{
    delete ui;
}

void CMenuBar::initToolBar(CPicTextLabel* pPicTextLabel, const QString& sImgFile, const QString& sToolName){
    pPicTextLabel->adjustSize();
    QPixmap* pImg = new QPixmap(sImgFile);
    this->m_vecImg.push_back(pImg);
    pPicTextLabel->initImg(pImg, sToolName);
}

void CMenuBar::InitTabPage00_File(){
    CGlobal* pGlobal = GetGlobalPtr();

    this->initToolBar(this->ui->PixTxt_Lab_NewFile,          (this->m_sExeDir + "/Res/ImgMenuBar/newFile.png").c_str(), u8"新建");//QString::fromLocal8Bit("选取"));
    this->initToolBar(this->ui->PixTxt_Lab_OpenFile,         (this->m_sExeDir + "/Res/ImgMenuBar/openFile.png").c_str(), u8"打开");//QString::fromLocal8Bit("选取"));
    this->initToolBar(this->ui->PixTxt_Lab_SaveFile,         (this->m_sExeDir + "/Res/ImgMenuBar/saveFile.png").c_str(), u8"保存");//QString::fromLocal8Bit("选取"));
    this->initToolBar(this->ui->PixTxt_Lab_SaveFileAs,       (this->m_sExeDir + "/Res/ImgMenuBar/saveFileAs.png").c_str(), u8"另存");//QString::fromLocal8Bit("选取"));
    this->initToolBar(this->ui->PixTxt_Lab_PrintFile,        (this->m_sExeDir + "/Res/ImgMenuBar/printFile.png").c_str(), u8"打印");//QString::fromLocal8Bit("选取"));
    this->initToolBar(this->ui->PixTxt_Lab_HelpOption,       (this->m_sExeDir + "/Res/ImgMenuBar/helpOption.png").c_str(), u8"帮助");//QString::fromLocal8Bit("选取"));
    this->initToolBar(this->ui->PixTxt_Lab_ExitExe,          (this->m_sExeDir + "/Res/ImgMenuBar/exit_exe.png").c_str(), u8"退出");//QString::fromLocal8Bit("选取"));

    connect(this->ui->PixTxt_Lab_OpenFile, &CPicTextLabel::clicked, pGlobal->m_pDlgLayerManager, &CLayerManagerWidget::Clicked_AddTif_Slot);
    connect(this->ui->PixTxt_Lab_ExitExe, &CPicTextLabel::clicked, pGlobal->m_pMainWindow, &QWidget::close);
}

void CMenuBar::InitTabPage01_Start() {
    this->initToolBar(this->ui->PixTxt_Lab_Select,          (this->m_sExeDir + "/Res/ImgMenuBar/select_01.png").c_str(), u8"选取");//QString::fromLocal8Bit("选取"));
    this->initToolBar(this->ui->PixTxt_Lab_Grab,            (this->m_sExeDir + "/Res/ImgMenuBar/grab_01.png").c_str(), (u8"测距"));//QString::fromLocal8Bit("抓取"));
    this->initToolBar(this->ui->PixTxt_Lab_Del_GroundPoint, (this->m_sExeDir + "/Res/ImgMenuBar/delete_point_01.png").c_str(), u8"删除");//QString::fromLocal8Bit("删除地面点"));
    this->initToolBar(this->ui->PixTxt_Lab_MovePoint,       (this->m_sExeDir + "/Res/ImgMenuBar/move_point_01.png").c_str(), (u8"卷帘"));//QString::fromLocal8Bit("移动点"));

    this->initToolBar(this->ui->PixTxt_Lab_ResidualInfo,        (this->m_sExeDir + "/Res/ImgMenuBar/select_01.png").c_str(), u8"残差");
    this->initToolBar(this->ui->PixTxt_Lab_Residual_DelPoints,  (this->m_sExeDir + "/Res/ImgMenuBar/select_01.png").c_str(), u8"过滤");
    this->initToolBar(this->ui->PixTxt_Lab_Residual_SaveFile,   (this->m_sExeDir + "/Res/ImgMenuBar/select_01.png").c_str(), u8"保存");

    int i = 0, j = 0;
    ui->comb_Scale->setEditable(false);
    ui->comb_Scale->insertItem(i++, ("    1:1000"));
    ui->comb_Scale->insertItem(i++, ("   1:10000"));
    ui->comb_Scale->insertItem(i++, ("   1:24000"));
    ui->comb_Scale->insertItem(i++, ("  1:100000"));
    ui->comb_Scale->insertItem(i++, ("  1:250000"));
    ui->comb_Scale->insertItem(i++, ("  1:500000"));
    ui->comb_Scale->insertItem(i++, ("  1:750000"));
    ui->comb_Scale->insertItem(i++, (" 1:1000000"));
    ui->comb_Scale->insertItem(i++, (" 1:3000000"));
    ui->comb_Scale->insertItem(i++, ("1:10000000"));

    this->initToolBar(this->ui->PixTxt_Lab_Chunk_Property,  (this->m_sExeDir + "/Res/ImgMenuBar/grab_01.png").c_str(), (u8"图块属性"));//QString::fromLocal8Bit("移动点"));

    connect(this->ui->PixTxt_Lab_Select, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_Select);
    connect(this->ui->PixTxt_Lab_Grab, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_CalcDistance);
    connect(this->ui->PixTxt_Lab_Del_GroundPoint, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_DelGroundPt);
    connect(this->ui->PixTxt_Lab_MovePoint, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_RollerBlind);

    connect(this->ui->PixTxt_Lab_Chunk_Property, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_ChunkAttribute);

    connect(this->ui->PixTxt_Lab_ResidualInfo, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_ResidualInfo);
    connect(this->ui->PixTxt_Lab_Residual_DelPoints, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_Residual_DelPoints);
    connect(this->ui->PixTxt_Lab_Residual_SaveFile, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_Residual_SaveFile);

    this->ui->PixTxt_Lab_Grab->hide();
    this->ui->PixTxt_Lab_MovePoint->hide();
    this->ui->comb_Scale->hide();
    this->ui->label_Comb_Scale->hide();
}

void CMenuBar::InitTabPage02_Edit(){

    //connect(this->ui->btn_Select_Pt1, &QPushButton::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Btn_SelectPoint1);
    //connect(this->ui->btn_Select_Pt2, &QPushButton::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Btn_SelectPoint2);
    //connect(this->ui->btn_SelectPointSwitch, &QPushButton::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Btn_SelectPointSwitch);
    //connect(this->ui->btn_Select_MapFile, &QPushButton::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Btn_MapFIleFullPath);
    //connect(this->ui->bnt_FindPath, &QPushButton::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Btn_StartFindPath);


    connect(this->ui->btn_TestFilterPoint, &QPushButton::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Btn_TestFilterPoint);
}

void CMenuBar::InitTabPage03_Tools(){
    this->initToolBar(this->ui->PixTxt_Lab_3DView,              (this->m_sExeDir + "/Res/ImgMenuBar/select_01.png").c_str(), u8"三维");//QString::fromLocal8Bit("选取"));


    connect(this->ui->btn_DemImgFullPath, &QPushButton::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Btn_DemImgFullPath);
    connect(this->ui->PixTxt_Lab_3DView, &CPicTextLabel::clicked, this->m_pToolMenuHandler, &CToolMenuHandler::Clicked_Lab_3DView);

}
