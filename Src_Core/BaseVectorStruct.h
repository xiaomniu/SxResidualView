#ifndef BASESTRUCT_H
#define BASESTRUCT_H

#include <math.h>

struct t_Point2D {
    double x, y;
    t_Point2D(){}
    t_Point2D(double _x, double _y) {
        this->x = _x;
        this->y = _y;
    }
    inline void SetPoint(double _x, double _y) {
        this->x = _x;
        this->y = _y;
    }

    inline double CalcDistance(double _x, double _y){
        return sqrt((this->x - _x) *(this->x - _x) + (this->y - _y)*(this->y - _y));
    }

    inline double CalcDistance(t_Point2D* _pPoint2D){
        return sqrt((this->x - _pPoint2D->x) *(this->x - _pPoint2D->x) + (this->y - _pPoint2D->y)*(this->y - _pPoint2D->y));
    }
};


struct t_Line2D {
    double x1;
    double y1;
    double x2;
    double y2;
    t_Line2D(){}
    t_Line2D(double _x1, double _y1, double _x2, double _y2){
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
    }
    void SetPoints(double _x1, double _y1, double _x2, double _y2){
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
    }
};

typedef enum _ELAYERTYPE{
    e_Unknow = 0,
    e_Tiff,
    e_Tif,
    e_Img,
    e_shape,
    e_shapePoint,
    e_shapePolyline,
    e_shapePolygon,
    e_MapGisWT,
    e_MapGisWL,
    e_MapGisWP,
    e_LayerTypeCount,
} ELAYERTYPE;

typedef enum _ECHUNK_TYPE{
    e_Chunk_Unknow = 0,
    e_Chunk_Point = 1,
    e_Chunk_Polyline = 2,
    e_Chunk_Polygon = 3,
    e_Chunk_MosaicPolyline = 4,
} ECHUNK_TYPE;

typedef enum _ECHUNK_OPERATE_TYPE{
    e_Chunk_UnKnowState = -1,
    e_Chunk_UnSelected = 0,
    e_Chunk_Selected = 1,
} ECHUNK_OPERATE_TYPE;

typedef enum _ESHOWORHIDE{
    e_hide = 0,
    e_show = 1,
}ESHOWORHIDE;

typedef enum _ECHUNK_ATTRIBUTE_TYPE{
    e_unknown = -1,
    e_char = 0,
    e_int,
    e_long,
    e_long_long,

    e_float,
    e_double,

    e_unsigned_char,
    e_unsigned_int,
    e_unsigned_long,
    e_unsigned_long_long,

    e_void_ptr,
    e_char_ptr,
    e_wchar_ptr,
}ECHUNK_ATTRIBUTE_TYPE;

#endif // BASESTRUCT_H
