#include "Misc.h"

#include <QtGui/QImage>
#include <QtGui/QFont>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtCore/QFileInfo>

#include <io.h>
#include <time.h>

QTextCodec* CMisc::m_pCodecGBK = QTextCodec::codecForName("GBK");
QTextCodec* CMisc::m_pCodecUTF8 = QTextCodec::codecForName("UTF-8");

CMisc::CMisc() {
}
CMisc::~CMisc() {

}

QImage* CMisc::CreateFontImage(int_32 width, int_32 height, const QString &text, int_32 textPixelSize, const QColor &textColor){
    QImage* pImg = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);//img是图片，模式为QImage::Format_ARGB32_Premultiplied是因为按官方的说法，这个模式快。
    pImg->fill(QColor(255, 255, 255, 255));	//填充为透明
    QPainter painter;
    QFont font;
    painter.begin(pImg);
    font.setPixelSize(textPixelSize);
    painter.setFont(font);
    QPen pen;
    pen.setColor(textColor);
    painter.setPen(pen);
    QTextOption option(Qt::AlignLeft | Qt::AlignTop);//这里使文字绘制时自动换行
    option.setWrapMode(QTextOption::WordWrap);
    QRectF rect(0, 0, width, height);
    painter.drawText(rect, text, option);//绘制到图片
    painter.end();
    return pImg;
}

//////////////////////////////////////////////////////////


int_32 CMisc::CheckFileExists(const int_8* szFileFullPath){
    //QString szzz = QString::fromUtf8(szFileFullPath);
    //QFileInfo fileInfo(szzz);
    //if(true == fileInfo.exists())
    //    return 1;
    //return 0;
    if( (_access(szFileFullPath, 0 )) != -1 ){
        return 1;
    }
    return 0;
}

int_32 CMisc::CheckFileExists(const std::string& sFileFullPath){
    //QString szzz = QString::fromUtf8(szFileFullPath);
    //QFileInfo fileInfo(szzz);
    //if(true == fileInfo.exists())
    //    return 1;
    //return 0;
    if( (_access(sFileFullPath.c_str(), 0 )) != -1 ){
        return 1;
    }
    return 0;
}


std::string CMisc::GetCurrTime() {
    time_t t;
    time(&t);//获取Unix时间戳。
    struct tm* lt = localtime(&t);//转为时间结构。
    int_8 szBuf[64] = { 0 };
    sprintf(szBuf, "%04d_%02d_%02d_%02d_%02d_%02d", lt->tm_year + 1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果
    return std::string(szBuf);
}

std::string CMisc::GetCurrTimeString() {
    time_t t;
    time(&t);//获取Unix时间戳。
    struct tm* lt = localtime(&t);//转为时间结构。
    int_8 szBuf[64] = { 0 };
    sprintf(szBuf, "%04d%02d%02d%02d%02d%02d", lt->tm_year + 1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果
    return std::string(szBuf);
}

std::string CMisc::Utf8ToAnsi(QString sUTF8Buf){
    QString strUnicode = CMisc::m_pCodecUTF8->toUnicode(sUTF8Buf.toLocal8Bit().data());
    return std::string((const int_8*)(CMisc::m_pCodecGBK->fromUnicode(strUnicode)).data());
}

std::string CMisc::Utf8ToAnsi(const int_8* szUTF8Buf) {
    QString strUnicode = CMisc::m_pCodecUTF8->toUnicode(szUTF8Buf);
    return std::string((const int_8*)(CMisc::m_pCodecGBK->fromUnicode(strUnicode)).data());
}

std::string CMisc::AnsiToUtf8(const int_8* szAnsiBuf) {
    QString strUnicode = CMisc::m_pCodecGBK->toUnicode(szAnsiBuf);
    return std::string((const int_8*)(CMisc::m_pCodecUTF8->fromUnicode(strUnicode)).data());
}

//////////////////////////////////////////////////////////
class Triangulate
{
public:

  // triangulate a contour/polygon, places results in STL vector
  // as series of triangles.
  static bool Process(const std::vector< glm::dvec2> &contour,
                      std::vector< glm::dvec2> &result);

  // compute area of a contour/polygon
  static double Area(const std::vector< glm::dvec2> &contour);

  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bool InsideTriangle(double Ax, double Ay,
                      double Bx, double By,
                      double Cx, double Cy,
                      double Px, double Py);


private:
  static bool Snip(const std::vector< glm::dvec2> &contour,int_32 u,int_32 v,int_32 w,int_32 n,int_32 *V);

};
#include "Misc.h"
void CMisc::SnipTriangles(std::vector< glm::dvec2>& vecPolygonPoints, std::vector< glm::dvec2>& vecTrianglePoints){
    Triangulate::Process(vecPolygonPoints, vecTrianglePoints);
}


// COTD Entry submitted by John W. Ratcliff [jratcliff@verant.com]

// ** THIS IS A CODE SNIPPET WHICH WILL EFFICIEINTLY TRIANGULATE ANY
// ** POLYGON/CONTOUR (without holes) AS A STATIC CLASS.  THIS SNIPPET
// ** IS COMPRISED OF 3 FILES, TRIANGULATE.H, THE HEADER FILE FOR THE
// ** TRIANGULATE BASE CLASS, TRIANGULATE.CPP, THE IMPLEMENTATION OF
// ** THE TRIANGULATE BASE CLASS, AND TEST.CPP, A SMALL TEST PROGRAM
// ** DEMONSTRATING THE USAGE OF THE TRIANGULATOR.  THE TRIANGULATE
// ** BASE CLASS ALSO PROVIDES TWO USEFUL HELPER METHODS, ONE WHICH
// ** COMPUTES THE AREA OF A POLYGON, AND ANOTHER WHICH DOES AN EFFICENT
// ** POINT IN A TRIANGLE TEST.
// ** SUBMITTED BY JOHN W. RATCLIFF (jratcliff@verant.com) July 22, 2000

/**********************************************************************/
/************ HEADER FILE FOR TRIANGULATE.H ***************************/
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Misc.h"

#ifndef EPSILON
#define EPSILON 0.000000000001
#endif

double Triangulate::Area(const std::vector< glm::dvec2> &contour)
{

    int_32 n = contour.size();

    double A=0.0;

    for(int_32 p=n-1,q=0; q<n; p=q++)
    {
        A+= contour[p].x*contour[q].y - contour[q].x*contour[p].y;
    }
    return A*0.5;
}

/*
     InsideTriangle decides if a point P is Inside of the triangle
     defined by A, B, C.
   */
bool Triangulate::InsideTriangle(double Ax, double Ay,
                                 double Bx, double By,
                                 double Cx, double Cy,
                                 double Px, double Py)

{
    double ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    double cCROSSap, bCROSScp, aCROSSbp;

    ax = Cx - Bx;  ay = Cy - By;
    bx = Ax - Cx;  by = Ay - Cy;
    cx = Bx - Ax;  cy = By - Ay;
    apx= Px - Ax;  apy= Py - Ay;
    bpx= Px - Bx;  bpy= Py - By;
    cpx= Px - Cx;  cpy= Py - Cy;

    aCROSSbp = ax*bpy - ay*bpx;
    cCROSSap = cx*apy - cy*apx;
    bCROSScp = bx*cpy - by*cpx;

    return ((aCROSSbp >= 0.0) && (bCROSScp >= 0.0) && (cCROSSap >= 0.0));
};

bool Triangulate::Snip(const std::vector< glm::dvec2> &contour,int_32 u,int_32 v,int_32 w,int_32 n,int_32 *V)
{
    int_32 p;
    double Ax, Ay, Bx, By, Cx, Cy, Px, Py;

    Ax = contour[V[u]].x;
    Ay = contour[V[u]].y;

    Bx = contour[V[v]].x;
    By = contour[V[v]].y;

    Cx = contour[V[w]].x;
    Cy = contour[V[w]].y;

    if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ){

        return false;
    }
    for (p=0;p<n;p++)
    {
        if( (p == u) || (p == v) || (p == w) ) continue;
        Px = contour[V[p]].x;
        Py = contour[V[p]].y;
        if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
    }

    return true;
}

bool Triangulate::Process(const std::vector< glm::dvec2> &contour,std::vector< glm::dvec2> &result)
{
    /* allocate and initialize list of Vertices in polygon */

    int_32 n = contour.size();
    if ( n < 3 ) return false;

    int_32 *V = new int_32[n];

    /* we want a counter-clockwise polygon in V */

    if ( 0.0 < Area(contour) )
        for (int_32 v=0; v<n; v++) V[v] = v;
    else
        for(int_32 v=0; v<n; v++) V[v] = (n-1)-v;

    int_32 nv = n;

    /*  remove nv-2 Vertices, creating 1 triangle every time */
    int_32 count = 2*nv;   /* error detection */

    for(int_32 m=0, v=nv-1; nv>2; )
    {
        /* if we loop, it is probably a non-simple polygon */
        if (0 >= (count--))
        {
            //** Triangulate: ERROR - probable bad polygon!
            return false;
        }

        /* three consecutive vertices in current polygon, <u,v,w> */
        int_32 u = v  ; if (nv <= u) u = 0;     /* previous */
        v = u+1; if (nv <= v) v = 0;     /* new v    */
        int_32 w = v+1; if (nv <= w) w = 0;     /* next     */

        if ( Snip(contour,u,v,w,nv,V) )
        {
            int_32 a,b,c,s,t;

            /* true names of the vertices */
            a = V[u]; b = V[v]; c = V[w];

            /* output Triangle */
            result.push_back( contour[a] );
            result.push_back( contour[b] );
            result.push_back( contour[c] );

            m++;

            /* remove v from remaining polygon */
            for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

            /* resest error detection counter */
            count = 2*nv;
        }
    }



    delete[] V;

    return true;
}


/************************************************************************/
/*** END OF CODE SECTION TRIANGULATE.CPP BEGINNING OF TEST.CPP A SMALL **/
/*** TEST APPLICATION TO DEMONSTRATE THE USAGE OF THE TRIANGULATOR     **/
/************************************************************************/

void SnipTriangles_Test(std::vector< glm::dvec2>& a, std::vector< glm::dvec2>& result)
{
    a.push_back(glm::dvec2(0,6));
    a.push_back(glm::dvec2(0,0));
    a.push_back(glm::dvec2(3,0));
    a.push_back(glm::dvec2(4,1));
    a.push_back(glm::dvec2(6,1));
    a.push_back(glm::dvec2(8,0));
    a.push_back(glm::dvec2(12,0));
    a.push_back(glm::dvec2(13,2));
    a.push_back(glm::dvec2(8,2));
    a.push_back(glm::dvec2(8,4));
    a.push_back(glm::dvec2(11,4));
    a.push_back(glm::dvec2(11,6));
    a.push_back(glm::dvec2(6,6));
    a.push_back(glm::dvec2(4,3));
    a.push_back(glm::dvec2(2,6));

    //  Invoke the triangulator to triangulate this polygon.
    Triangulate::Process(a,result);

    // print out the results.
    int_32 tcount = result.size()/3;

    for (int_32 i=0; i<tcount; i++)
    {
        const glm::dvec2 &p1 = result[i*3+0];
        const glm::dvec2 &p2 = result[i*3+1];
        const glm::dvec2 &p3 = result[i*3+2];
        printf("Triangle %d => (%0.0f,%0.0f) (%0.0f,%0.0f) (%0.0f,%0.0f)\n",i+1,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);
    }

}
