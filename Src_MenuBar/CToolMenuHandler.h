#ifndef CTOOLMENUHANDLER_H
#define CTOOLMENUHANDLER_H

#include <QWidget>
#include <vector>
#include <string>

#include "Src_Core/BaseVectorStruct.h"

namespace Ui {
class CToolBar01;
}

class OGRGeometry;
class OGRLineString;

class CLayerLines;

class QPixmap;
class CPicTextLabel;
class QPushButton;
class CMenuBar;

class CToolMenuHandler : public QObject
{
    Q_OBJECT

public:
    explicit CToolMenuHandler(QObject *parent = nullptr);
    ~CToolMenuHandler(){ }

    CMenuBar* m_pMenuBar = nullptr;

    std::string m_s3DViewDemImgFullPath, m_s3DViewTifImgFullPath;

public slots:
    void Clicked_Lab_Select();
    void Clicked_Lab_CalcDistance();
    void Clicked_Lab_DelGroundPt();
    void Clicked_Lab_RollerBlind();
    void Clicked_Lab_ChunkAttribute();

    void Clicked_Btn_MouseReleaseDone(double fGeoMouseX, double fGeoMouseY);

    void Clicked_Btn_TestFilterPoint();

    void Clicked_Btn_DemImgFullPath();
    void Clicked_Lab_3DView();
    void Clicked_Lab_ResidualInfo();
    void Clicked_Lab_Residual_SaveFile();

    void Clicked_Lab_Residual_LianJie_Filter();
    void Clicked_Lab_Residual_Control_Filter();
    void Clicked_Lab_Residual_Check_Filter();

    void Clicked_Lab_TransTo_CheckPoint();
    void Clicked_Lab_TransTo_ControlPoint();

    void Clicked_Lab_Show_PAN_Tif();

    void slotMouseRelease(void *pParam);
};

#endif // CTOOLBAR01_H
