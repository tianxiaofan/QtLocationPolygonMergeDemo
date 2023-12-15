/**************************************************************************
 *   文件名	：eot.cpp
 *   =======================================================================
 *   创 建 者	：田小帆
 *   创建日期	：2023-10-8
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
#include "fogview.h"
#include <QtMath>


FogView::FogView(QObject *parent)
    : QObject(parent)
{}

void FogView::addCoordinate(const QGeoCoordinate &coordinate)
{
    m_geoPolygon.setPerimeter(createPolygonPoints(coordinate, 80000));
    //    m_geoPolygon.addCoordinate(coordinate);
    emit geoPolygonChanged();
    connect(&m_timer, &QTimer::timeout, this, [=]() {
        auto start = clock();

#if 1
        QList<QList<QGeoCoordinate> > holes;
        for (int i = m_geoPolygon.holesCount() - 1; i >= 0; --i) {
            holes.append(m_geoPolygon.holePath(i));
            m_geoPolygon.removeHole(i);
        }

        auto unioned_start = clock();
        holes = ProjConvert::get()->polygonUnion(holes, randomPoints(coordinate, 30000));
        auto unioned_end = clock() - unioned_start;
        for (int i = 0; i < holes.count(); i++) {
            m_geoPolygon.addHole(holes.at(i));
        }
#else
        m_geoPolygon.addHole(randomPoints(coordinate, 30000));
#endif
        static int st = clock();
        if (clock() - st > 1000) {
            emit geoPolygonChanged();
            st = clock();
        }
        qDebug() << m_holes.count() << m_geoPolygon.holesCount() << clock() - start << unioned_end;
    });
    m_timer.start(40);
}

void FogView::addHole(const QGeoCoordinate &coordinate)
{
    if (m_tempCoordinates.count() < 3) {
        m_tempCoordinates.append(coordinate);
        return;
    } else {
        m_tempCoordinates.append(coordinate);
        if (m_geoPolygon.holesCount() == 0) {
            m_geoPolygon.addHole(m_tempCoordinates);
        } else {
            QList<QList<QGeoCoordinate> > holes;
            for (int i = m_geoPolygon.holesCount() - 1; i >= 0; --i) {
                holes.append(m_geoPolygon.holePath(i));
                m_geoPolygon.removeHole(i);
            }
            holes.append(m_tempCoordinates);

            //            ProjConvert::get()->polygonUnion(holes);
            for (int i = 0; i < holes.count(); i++) {
                m_geoPolygon.addHole(holes.at(i));
            }
        }

        m_tempCoordinates.clear();
        emit geoPolygonChanged();
    }
    qDebug() << coordinate;
}

QGeoCoordinate FogView::center() const
{
    return m_center;
}

void FogView::setCenter(const QGeoCoordinate &newCenter)
{
    if (m_center == newCenter)
        return;
    m_center = newCenter;
    emit centerChanged();
}

void FogView::calcPolygonPoints(const QGeoCoordinate &center)
{
    // 根据中收点坐标计算一个矩形的四个点，半径为50000米
    double rectR = 50000;
    auto lp = center.atDistanceAndAzimuth(rectR, 270);
    auto rp = center.atDistanceAndAzimuth(rectR, 90);
    auto tp = center.atDistanceAndAzimuth(rectR, 0);
    auto bp = center.atDistanceAndAzimuth(rectR, 180);
    m_polygonPath.clear();
    // 取lp的经度和tp的纬度，即左上角点
    m_polygonPath.append(QVariant::fromValue(QGeoCoordinate(lp.longitude(), tp.latitude())));
    m_polygonPath.append(QVariant::fromValue(QGeoCoordinate(rp.longitude(), tp.latitude())));
    m_polygonPath.append(QVariant::fromValue(QGeoCoordinate(rp.longitude(), bp.latitude())));
    m_polygonPath.append(QVariant::fromValue(QGeoCoordinate(lp.longitude(), bp.latitude())));
    emit polygonPathChanged();
}

QList<QGeoCoordinate> FogView::createPolygonPoints(const QGeoCoordinate &center, double radius)
{
    // 生成一个以center为中心，半径为radius的多边形的点集
    QList<QGeoCoordinate> points;
    // 生成一个正多边形，边数为8
    int n = 4;
    double angle = 360.0 / n;
    for (int i = 0; i < n; i++) {
        auto p = center.atDistanceAndAzimuth(radius, i * angle);
        points.append(p);
    }
    return points;
}

QList<QGeoCoordinate> FogView::randomPoints(const QGeoCoordinate &center, double radius)
{
    // 生成一个以center为中心，半径为radius的矩形的点集，所有点必须在m_geoPolygon内
    QList<QGeoCoordinate> points;
    double minLat = center.atDistanceAndAzimuth(radius, 180).latitude();
    double maxLat = center.atDistanceAndAzimuth(radius, 0).latitude();
    double minLon = center.atDistanceAndAzimuth(radius, 270).longitude();
    double maxLon = center.atDistanceAndAzimuth(radius, 90).longitude();
    // q: 经度0.005，纬度0.005 是多少米？
    // a: 0.005纬度约等于555米，0.005经度约等于555米

    double lat = minLat + (maxLat - minLat) * rand() / RAND_MAX;
    double lon = minLon + (maxLon - minLon) * rand() / RAND_MAX;
    points.append(createPolygonPoints(QGeoCoordinate(lat, lon), 900));
    return points;
}

QVariantList FogView::polygonPath() const
{
    return m_polygonPath;
}

void FogView::setPolygonPath(const QVariantList &newPolygonPath)
{
    if (m_polygonPath == newPolygonPath)
        return;
    m_polygonPath = newPolygonPath;
    emit polygonPathChanged();
}

QGeoPolygon FogView::geoPolygon() const
{
    return m_geoPolygon;
}

void FogView::setGeoPolygon(const QGeoPolygon &newGeoPolygon)
{
    if (m_geoPolygon == newGeoPolygon)
        return;
    m_geoPolygon = newGeoPolygon;
    emit geoPolygonChanged();
}
