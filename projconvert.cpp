/**************************************************************************
 *   文件名	：projconvert.cpp
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
#include "projconvert.h"

#include <QDebug>
#include <QPointF>

bool ProjConvert::geoCoordinate2Coordinate(const QGeoCoordinate &coord,
                                           geos::geom::Coordinate &point)
{
    if (m_proj == nullptr) {
        return false;
    }
    auto c = proj_coord(coord.longitude(), coord.latitude(), 0, 0);
    auto m = proj_trans(m_proj, PJ_FWD, c);
    point.x = m.enu.e;
    point.y = m.enu.n;
    return true;
}

bool ProjConvert::coordinate2GeoCoordinate(const geos::geom::Coordinate &point,
                                           QGeoCoordinate &coord)
{
    if (m_proj == nullptr) {
        return false;
    }
    auto p_mer = proj_coord(point.x, point.y, 0, 0);
    auto p_coord = proj_trans(m_proj, PJ_INV, p_mer);
    coord.setLongitude(p_coord.lp.lam);
    coord.setLatitude(p_coord.lp.phi);
    return true;
}

// 递归函数，用于提取 GeometryCollection 中的多边形
void extractPolygons(const geos::geom::Geometry *geometry,
                     std::vector<std::unique_ptr<geos::geom::Polygon>> &polygons)
{
    if (auto collection = dynamic_cast<const geos::geom::GeometryCollection *>(geometry);
        collection != nullptr) {
        for (size_t i = 0; i < collection->getNumGeometries(); ++i) {
            extractPolygons(collection->getGeometryN(i), polygons);
        }
    } else if (auto polygon = dynamic_cast<const geos::geom::Polygon *>(geometry);
               polygon != nullptr) {
        polygons.push_back(polygon->clone());
    }
}

QList<QList<QGeoCoordinate>> ProjConvert::polygonUnion(const QList<QList<QGeoCoordinate>> &polygons,
                                                       const QList<QGeoCoordinate> &newPolygon)
{
    if (newPolygon.count() < 3) {
        return polygons;
    }

    QList<QList<QGeoCoordinate>> result;

    // 将新加入的多边形转换为geos的polygon
    std::unique_ptr<geos::geom::Geometry> newGeometry;
    {
        std::vector<geos::geom::Coordinate> pts;
        for (const auto &coord : newPolygon) {
            geos::geom::Coordinate c;
            if (geoCoordinate2Coordinate(coord, c)) {
                pts.emplace_back(c);
            }
        }
        // 如果首尾不相同，则添加首尾相同的点
        if (pts.front() != pts.back()) {
            pts.emplace_back(pts.front());
        }

        newGeometry = std::move(
            geos::geom::GeometryFactory::getDefaultInstance()->createPolygon(std::move(pts)));
    }

    // 将原来的多边形转换为geos的polygon，并查检新加入的多边形是否在原来的多边形相交
    std::vector<std::unique_ptr<geos::geom::Geometry>> geometries;
    for (const auto &polygon : polygons) {
        std::vector<geos::geom::Coordinate> pts;
        for (const auto &coord : polygon) {
            geos::geom::Coordinate c;
            if (geoCoordinate2Coordinate(coord, c))
                pts.emplace_back(c);
        }
        if (pts.size() < 3) {
            continue;
        }
        // 如果首尾不相同，则添加首尾相同的点
        if (pts.front() != pts.back()) {
            pts.emplace_back(pts.front());
        }
        auto current_polygon = geos::geom::GeometryFactory::getDefaultInstance()->createPolygon(
            std::move(pts));
        // 如果相交，则将相交的多边形添加到geometries中
        if (current_polygon->intersects(newGeometry.get())) {
            geometries.emplace_back(std::move(current_polygon));
        } else {
            result.append(polygon);
        }
        pts.clear();
    }

    // 如果没有相交的多边形，则直接返回
    if (geometries.size() == 0) {
        result.append(newPolygon);
        return result;
    }

    geometries.emplace_back(std::move(newGeometry));

    auto start = clock();
    // 合并
    auto union_geometry = geos::geom::GeometryFactory::getDefaultInstance()
                              ->createGeometryCollection(std::move(geometries))
                              ->Union();
    qDebug() << clock() - start << "ms";

    // 提取 GeometryCollection 中的多边形
    std::vector<std::unique_ptr<geos::geom::Polygon>> union_polygons;
    extractPolygons(union_geometry.get(), union_polygons);

    for (const auto &polygon : union_polygons) {
        QList<QGeoCoordinate> coords;
        for (int i = 0; i < polygon->getExteriorRing()->getCoordinates()->getSize(); ++i) {
            QGeoCoordinate c;
            if (coordinate2GeoCoordinate(polygon->getExteriorRing()->getCoordinates()->getAt(i), c))
                coords.append(c);
        }
        result.append(coords);
    }
    geometries.clear();
    return result;
}

//QList < QList<QGeoCoordinate> ProjConvert::polygonUnion(QList<QList<QGeoCoordinate>> &polygons)
//{
//    std::vector<std::unique_ptr<geos::geom::Geometry>> geometries;
//    for (const auto &polygon : polygons) {
//        std::vector<geos::geom::Coordinate> pts;
//        for (const auto &coord : polygon) {
//            geos::geom::Coordinate c;
//            if (geoCoordinate2Coordinate(coord, c))
//                pts.emplace_back(c);
//        }
//        if (pts.size() < 3) {
//            continue;
//        }
//        // 如果首尾不相同，则添加首尾相同的点
//        if (pts.front() != pts.back()) {
//            pts.emplace_back(pts.front());
//        }

//        geometries.push_back(
//            geos::geom::GeometryFactory::getDefaultInstance()->createPolygon(std::move(pts)));
//        pts.clear();
//    }

//    auto start = clock();
//    // 合并
//    auto union_geometry = geos::geom::GeometryFactory::getDefaultInstance()
//                              ->createGeometryCollection(std::move(geometries))
//                              ->Union();
//    qDebug() << clock() - start << "ms";

//    // 提取 GeometryCollection 中的多边形
//    std::vector<std::unique_ptr<geos::geom::Polygon>> union_polygons;
//    extractPolygons(union_geometry.get(), union_polygons);

//    polygons.clear();
//    for (const auto &polygon : union_polygons) {
//        QList<QGeoCoordinate> coords;
//        for (int i = 0; i < polygon->getExteriorRing()->getCoordinates()->getSize(); ++i) {
//            QGeoCoordinate c;
//            if (coordinate2GeoCoordinate(polygon->getExteriorRing()->getCoordinates()->getAt(i), c))
//                coords.append(c);
//        }
//        polygons.append(coords);
//    }
//    geometries.clear();
//}

ProjConvert::ProjConvert()
{
    // 创建一个上下文
    m_context = proj_context_create();
    if (m_context == nullptr) {
        qDebug() << "proj_context_create failed";
        return;
    }
    // 创建转换对象
    m_proj = proj_create_crs_to_crs(m_context, "EPSG:4326", "EPSG:3857", nullptr);
    if (m_proj == nullptr) {
        qDebug() << "proj_create_crs_to_crs failed";
        return;
    }
    auto norm = proj_normalize_for_visualization(m_context, m_proj);
    if (norm == nullptr) {
        qDebug() << "proj_normalize_for_visualization failed";
        return;
    }
    proj_destroy(m_proj);
    m_proj = norm;
    qDebug() << "proj_create_crs_to_crs success";
}

ProjConvert::~ProjConvert()
{
    if (m_proj) {
        proj_destroy(m_proj);
        m_proj = nullptr;
    }
    if (m_context) {
        proj_context_destroy(m_context);
        m_context = nullptr;
    }
}
