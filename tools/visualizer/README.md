Geometry visualizer
====

平面幾何用ビジュアライザ

## 入力ファイルフォーマット
- JSONで記述
- 1つのSceneのみからなる

### Scene
- 0個以上のLayerからなる配列

### Layer
- name: レイヤー名
- shapes: 0個以上のShapeからなる配列

### Shape
- kind: 図形の種類 ("point", "line", "segment", "circle", "polygon")
- coords: 図形を構成する座標列 ( [ x0, y0, x1, y1, ... xn, yn ] の順)
- radius: 円の半径 (kind = "circle" の場合のみ)
- stroke_color:
- stroke_opacity:
- stroke_style: solid, dashed, dotted
- stroke_width:
- fill_color:
- fill_opacity:


## 依存している外部ライブラリ
- jQuery
  - http://jquery.com
- Bootstrap
  - http://getbootstrap.com
- Bootstrap File Input
  - http://plugins.krajee.com/file-input
- Snap.svg
  - http://snapsvg.io