/**************************************************************************
 *   文件名	：eot.h
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
#pragma once

#include <QGeoCoordinate>
#include <QGeoPolygon>
#include <QObject>
#include <QQmlEngine>
#include <QQueue>
#include <QTimer>

#include "projconvert.h"


class FogView : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit FogView(QObject *parent = nullptr);
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged FINAL)
    Q_PROPERTY(QVariantList polygonPath READ polygonPath WRITE setPolygonPath NOTIFY
                   polygonPathChanged FINAL)
    Q_PROPERTY(
        QGeoPolygon geoPolygon READ geoPolygon WRITE setGeoPolygon NOTIFY geoPolygonChanged FINAL)

    Q_INVOKABLE void addCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void addHole(const QGeoCoordinate &coordinate);

    QGeoCoordinate center() const;
    void setCenter(const QGeoCoordinate &newCenter);

    QVariantList polygonPath() const;
    void setPolygonPath(const QVariantList &newPolygonPath);

    QGeoPolygon geoPolygon() const;
    void setGeoPolygon(const QGeoPolygon &newGeoPolygon);

signals:
    void centerChanged();

    void polygonPathChanged();

    void geoPolygonChanged();

private:
    void calcPolygonPoints(const QGeoCoordinate &center);
    QList<QGeoCoordinate> createPolygonPoints(const QGeoCoordinate &center, double radius);
    QList<QGeoCoordinate> randomPoints(const QGeoCoordinate &center, double radius);

private:
    QGeoCoordinate m_center;
    QVariantList m_polygonPath;
    QGeoPolygon m_geoPolygon;
    QList<QGeoCoordinate> m_tempCoordinates;
    QQueue<QList<QGeoCoordinate>> m_holes;
    QTimer m_timer;
};
