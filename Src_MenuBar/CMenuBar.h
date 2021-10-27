#ifndef CMENUBAR_H
#define CMENUBAR_H

#include <QWidget>
#include <string>
#include <vector>

namespace Ui {
class CMenuBar;
}

class CPicTextLabel;
class CToolMenuHandler;
class CMainViewWidget;
class CMenuBar : public QWidget
{
    Q_OBJECT

public:
    explicit CMenuBar(QWidget *parent = nullptr);
    ~CMenuBar();


    void resizeEvent(QResizeEvent* event);

    void initToolBar(CPicTextLabel* pPicTextLabel, const QString& sImgFile, const QString& sToolName);

    void InitTabPage00_File();
    void InitTabPage01_Start();
    void InitTabPage02_Edit();
    void InitTabPage03_Tools();

    std::vector<QPixmap*> m_vecImg;
    int m_nMaxTabPagesIndx = 0;
    std::vector<QWidget*> m_vecTabPages;
    CToolMenuHandler* m_pToolMenuHandler = nullptr;

    CMainViewWidget* m_pMainViewWidget = nullptr;

    std::string m_sExeDir;

public slots:
    void hideOrShowTabWidget(int nTabIndx);

public:
    Ui::CMenuBar *ui;
};

#endif // CMENUBAR_H
