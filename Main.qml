import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import Qt.labs.platform
import QtLocation
import QtPositioning
import QtQuick.Controls
import QtLocationPolygonMergeDemo

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map
        visible: true
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(34.23418, 108.873713)
        zoomLevel: 14
        property geoCoordinate startCentroid

        MapPolygon {
            id: polygon
            //            border.width: 2
            //            border.color: "red"
            color: "#800000FF"
            geoShape: fog.geoPolygon
            autoFadeIn: false
        }
        FogView {
            id: fog
        }

        PinchHandler {
            id: pinch
            target: null
            onActiveChanged: if (active) {
                                 map.startCentroid = map.toCoordinate(
                                             pinch.centroid.position, false)
                             }
            onScaleChanged: delta => {
                                map.zoomLevel += Math.log2(delta)
                                map.alignCoordinateToPoint(
                                    map.startCentroid, pinch.centroid.position)
                            }
            onRotationChanged: delta => {
                                   map.bearing -= delta
                                   map.alignCoordinateToPoint(
                                       map.startCentroid,
                                       pinch.centroid.position)
                               }
            grabPermissions: PointerHandler.TakeOverForbidden
        }
        WheelHandler {
            id: wheel
            // workaround for QTBUG-87646 / QTBUG-112394 / QTBUG-112432:
            // Magic Mouse pretends to be a trackpad but doesn't work with PinchHandler
            // and we don't yet distinguish mice and trackpads on Wayland either
            acceptedDevices: Qt.platform.pluginName === "cocoa"
                             || Qt.platform.pluginName
                             === "wayland" ? PointerDevice.Mouse
                                             | PointerDevice.TouchPad : PointerDevice.Mouse
            rotationScale: 1 / 120
            property: "zoomLevel"
        }
        DragHandler {
            id: drag
            target: null
            onTranslationChanged: delta => map.pan(-delta.x, -delta.y)
        }
        Shortcut {
            enabled: map.zoomLevel < map.maximumZoomLevel
            sequence: StandardKey.ZoomIn
            onActivated: map.zoomLevel = Math.round(map.zoomLevel + 1)
        }
        Shortcut {
            enabled: map.zoomLevel > map.minimumZoomLevel
            sequence: StandardKey.ZoomOut
            onActivated: map.zoomLevel = Math.round(map.zoomLevel - 1)
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => {//                           if (mouse.button === Qt.LeftButton) {
                           //                               fog.addCoordinate(map.toCoordinate(
                           //                                                     Qt.point(mouse.x,
                           //                                                              mouse.y)))
                           //                           } else if (mouse.button === Qt.RightButton) {
                           //                               fog.addHole(map.toCoordinate(Qt.point(mouse.x,
                           //                                                                     mouse.y)))
                           //                           }
                       }
        }
    }
    Button {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 5
        text: "XXXXXX"
        onClicked: {
            fog.addCoordinate(map.toCoordinate(map.center))
        }
    }

    function randomColor() {
        var red = Math.floor(Math.random() * 256)
        var green = Math.floor(Math.random() * 256)
        var blue = Math.floor(Math.random() * 256)
        return Qt.rgba(red / 255, green / 255, blue / 255, 1)
    }
}
