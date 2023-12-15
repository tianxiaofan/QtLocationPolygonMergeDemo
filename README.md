# QtLocationPolygonMergeDemo
在QtLocation中实现一个多边形合并，类似于游戏中的战争阴影，只是个demo

编译环境： Qt 6.5.3

运行时：先拷贝 vcpkg/installed/你的编译平台/share/proj/proj.db 到运行目录

主要使用了geos库和proj库，进行多边形的合并，主要思路是将gps坐标转为UTM坐标，再使用geos库进行多边形合并，目前性能只能说够用，还有优化的空间
可以找同样能实现此功能的库，boots库中也有同样的合并功能来代替geos。

