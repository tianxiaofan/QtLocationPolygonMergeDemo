/**************************************************************************
 *   文件名	：projconvert.h
 *   =======================================================================
 *   创 建 者	：田小帆
 *   创建日期	：2023-7-19
 *   邮   箱	：499131808@qq.com
 *   Q Q		：499131808
 *   公   司      ：
 *   功能描述      ：
 *   使用说明 ：
 *   ======================================================================
 *   修改者	：
 *   修改日期	：
 *   修改内容	：
 *   ======================================================================
 *
 ***************************************************************************/
#pragma once

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/operation/union/CascadedPolygonUnion.h>
#include <geos/operation/union/UnaryUnionOp.h>
#include <proj.h>

#include <QGeoCoordinate>
#include <QObject>

// 封装一个proj转换类，单例类,相互转换"EPSG:4326"和"EPSG:3857"
// 不支持高度
class ProjConvert {
public:
    static ProjConvert* get()
    {
        static ProjConvert p;
        return &p;
    }

    bool geoCoordinate2Coordinate(const QGeoCoordinate &coord, geos::geom::Coordinate &point);
    bool coordinate2GeoCoordinate(const geos::geom::Coordinate &point, QGeoCoordinate &coord);

    QList<QList<QGeoCoordinate> > polygonUnion(const QList<QList<QGeoCoordinate> > &polygons,
                                               const QList<QGeoCoordinate> &newPolygon);

private:
    ProjConvert();
    ~ProjConvert();

    PJ_CONTEXT* m_context = nullptr;
    PJ *m_proj = nullptr;
};
