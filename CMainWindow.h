#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QtWidgets/QMainWindow>

namespace Ui {
class CMainWindow;
}

class QDialog;
class CMenuBar;
class CMainViewWidget;
class CLayerManagerWidget;
class CLayerAttribute;
class CChunkAttribute;

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

public:
    //移除并隐藏所有dock
    void RemoveAllDock();
    //显示dock窗口
    void ShowDock(const QList<int>& index = QList<int>());

    void SetDockerWidgetStyleSheet(QDockWidget* pDockWidget);

    void InitMainWidgets();


    CMainViewWidget* m_pMainViewWidget = nullptr;
    CMenuBar* m_pMenuBar = nullptr;
    CLayerManagerWidget* m_pSxDlgLayerManager = nullptr;
    QDialog* m_pOutputInfo = nullptr;
    QDialog* m_pLayerManagerMenuBar = nullptr;
    CLayerAttribute* m_pLayerAttribute = nullptr;
    CChunkAttribute* m_pChunkAttribute = nullptr;

    QList<QDockWidget*> m_docks;///< 记录所有dockWidget的指针

private:
    Ui::CMainWindow *ui;
};

#endif // CMAINWINDOW_H
