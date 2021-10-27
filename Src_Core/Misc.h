#ifndef MISC_H
#define MISC_H

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Src_Core/DefType.h"

#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtCore/QTextCodec>

#include <string>
#include <vector>  // Include STL vector class.

class QImage;

class CMisc
{
public:
    CMisc();
    ~CMisc();
public:

    static QImage* CreateFontImage(int_32 width, int_32 height, const QString &text, int_32 textPixelSize, const QColor &textColor);
    static void SnipTriangles(std::vector< glm::dvec2>& vecPolygonPoints, std::vector< glm::dvec2>& vecTrianglePoints);

    static int_32 CheckFileExists(const int_8* szFileFullPath);
    static int_32 CheckFileExists(const std::string& sFileFullPath);

    static std::string GetCurrTime();
    static std::string GetCurrTimeString();

    static QTextCodec *m_pCodecGBK;
    static QTextCodec *m_pCodecUTF8;
    static std::string Utf8ToAnsi(QString sUTF8Buf);
    static std::string Utf8ToAnsi(const int_8* szUTF8Buf);
    static std::string AnsiToUtf8(const int_8* szAnsiBuf);
};

#endif // MISC_H
