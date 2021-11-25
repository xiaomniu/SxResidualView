#ifndef SXGLOBAL_H
#define SXGLOBAL_H

//#define MY_DEBUG1 1

#define SX_DEBUG 1
#if SX_DEBUG
  #define SX_DBG(fmt, ...) printf(fmt, __VA_ARGS__)
#else
  #define SX_DBG//(fmt, ...) DbgPrintEx(DPFLTR_FASTFAT_ID, DPFLTR_ERROR_LEVEL, fmt, __VA_ARGS__)
#endif
#include <string>
#include <vector>
#include <unordered_map>

class QFont;
class QApplication;
class QWidget;
class QDockWidget;

class CMenuBar;

class COpenGLCore;
class COpenGLCoreSub;
class CLayerVector;
class CLayerManagerWidget;
class CChunkAttribute;
class CLayerAttribute;
class CGlobal {
public:
    CGlobal();
    ~CGlobal();

    QFont* m_pFont = nullptr;
    QApplication* m_pQApplication = nullptr;

    QWidget* m_pMainWindow = nullptr;
    CMenuBar* m_pMenuBar = nullptr;

    COpenGLCore* m_pGLCore = nullptr;
    std::vector<QWidget*> m_vecWndSub;
    std::vector<QWidget*> m_vecDockSub;
    std::unordered_map<QWidget*, COpenGLCoreSub*> m_mapWnd2GLCore;
    CLayerVector* m_pLayerVector = nullptr;
    CLayerManagerWidget* m_pDlgLayerManager = nullptr;
    CChunkAttribute* m_pChunkAttribute = nullptr;
    CLayerAttribute* m_pLayerAttribute = nullptr;

    std::string m_sExeDir;
};

CGlobal* GetGlobalPtr();

#endif // !SXGLOBAL_H

