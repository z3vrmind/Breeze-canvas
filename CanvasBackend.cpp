#include "CanvasBackend.h"
#include <QPainterPath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QBuffer>
#include <QQueue>
#include <QtMath>
#include <algorithm>

// =============================================================================
// BREEZE VECTOR ICON ENGINE (100% Native QPainter - Zero Font Dependencies)
// =============================================================================
BreezeIcon::BreezeIcon(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setImplicitWidth(22);
    setImplicitHeight(22);
}

void BreezeIcon::setName(const QString &n)
{
    if (m_name != n) {
        m_name = n;
        emit nameChanged();
        update();
    }
}

void BreezeIcon::setColor(const QColor &c)
{
    if (m_color != c) {
        m_color = c;
        emit colorChanged();
        update();
    }
}

void BreezeIcon::paint(QPainter *p)
{
    p->setRenderHint(QPainter::Antialiasing, true);
    const qreal sx = width() / 24.0;
    const qreal sy = height() / 24.0;
    p->scale(sx, sy);

    QPen pen(m_color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);

    if (m_name == "new") {
        QPainterPath path;
        path.moveTo(6, 3);
        path.lineTo(14, 3);
        path.lineTo(19, 8);
        path.lineTo(19, 21);
        path.lineTo(6, 21);
        path.closeSubpath();
        p->drawPath(path);
        p->drawLine(12, 11, 12, 17);
        p->drawLine(9, 14, 15, 14);
    } else if (m_name == "open") {
        QPainterPath folder;
        folder.moveTo(3, 6);
        folder.lineTo(9, 6);
        folder.lineTo(11, 8);
        folder.lineTo(21, 8);
        folder.lineTo(21, 19);
        folder.lineTo(3, 19);
        folder.closeSubpath();
        p->drawPath(folder);
    } else if (m_name == "undo") {
        p->drawLine(9, 7, 4, 12);
        p->drawLine(4, 12, 9, 17);
        QPainterPath arc;
        arc.moveTo(4, 12);
        arc.cubicTo(14, 12, 20, 13, 20, 19);
        p->drawPath(arc);
    } else if (m_name == "redo") {
        p->drawLine(15, 7, 20, 12);
        p->drawLine(20, 12, 15, 17);
        QPainterPath arc;
        arc.moveTo(20, 12);
        arc.cubicTo(10, 12, 4, 13, 4, 19);
        p->drawPath(arc);
    } else if (m_name == "sun") {
        p->drawEllipse(QPointF(12, 12), 4.5, 4.5);
        for (int i = 0; i < 8; ++i) {
            qreal a = i * M_PI / 4.0;
            p->drawLine(QPointF(12 + std::cos(a) * 7.0, 12 + std::sin(a) * 7.0),
                        QPointF(12 + std::cos(a) * 9.5, 12 + std::sin(a) * 9.5));
        }
    } else if (m_name == "moon") {
        QPainterPath moon;
        moon.moveTo(15, 4);
        moon.cubicTo(9, 5, 6, 11, 9, 17);
        moon.cubicTo(12, 21, 18, 20, 20, 16);
        moon.cubicTo(15, 17, 12, 13, 15, 4);
        p->drawPath(moon);
    } else if (m_name == "adjust") {
        p->drawLine(4, 7, 20, 7);
        p->drawLine(4, 12, 20, 12);
        p->drawLine(4, 17, 20, 17);
        p->setBrush(m_color);
        p->drawEllipse(QPointF(9, 7), 2.5, 2.5);
        p->drawEllipse(QPointF(15, 12), 2.5, 2.5);
        p->drawEllipse(QPointF(11, 17), 2.5, 2.5);
    } else if (m_name == "export") {
        p->drawLine(12, 4, 12, 15);
        p->drawLine(8, 11, 12, 15);
        p->drawLine(16, 11, 12, 15);
        p->drawLine(5, 15, 5, 20);
        p->drawLine(5, 20, 19, 20);
        p->drawLine(19, 20, 19, 15);
    } else if (m_name == "move") {
        p->drawLine(12, 3, 12, 21);
        p->drawLine(3, 12, 21, 12);
        p->drawLine(9, 6, 12, 3); p->drawLine(15, 6, 12, 3);
        p->drawLine(9, 18, 12, 21); p->drawLine(15, 18, 12, 21);
        p->drawLine(6, 9, 3, 12); p->drawLine(6, 15, 3, 12);
        p->drawLine(18, 9, 21, 12); p->drawLine(18, 15, 21, 12);
    } else if (m_name == "pan") {
        p->drawRoundedRect(QRectF(7, 10, 10, 10), 3, 3);
        p->drawLine(8, 10, 8, 5);
        p->drawLine(11, 10, 11, 3);
        p->drawLine(14, 10, 14, 4);
        p->drawLine(17, 12, 17, 7);
    } else if (m_name == "brush") {
        p->drawLine(18, 4, 9, 13);
        p->drawLine(20, 6, 11, 15);
        p->drawLine(18, 4, 20, 6);
        QPainterPath tip;
        tip.moveTo(9, 13);
        tip.cubicTo(6, 14, 5, 17, 4, 20);
        tip.cubicTo(8, 19, 10, 18, 11, 15);
        tip.closeSubpath();
        p->setBrush(m_color);
        p->drawPath(tip);
    } else if (m_name == "eraser") {
        QPainterPath er;
        er.moveTo(14, 4);
        er.lineTo(20, 10);
        er.lineTo(11, 19);
        er.lineTo(5, 19);
        er.lineTo(4, 14);
        er.closeSubpath();
        p->drawPath(er);
        p->drawLine(8, 10, 14, 16);
        p->drawLine(4, 19, 20, 19);
    } else if (m_name == "bucket") {
        QPainterPath b;
        b.moveTo(6, 10);
        b.lineTo(13, 3);
        b.lineTo(20, 10);
        b.lineTo(13, 17);
        b.closeSubpath();
        p->drawPath(b);
        p->setBrush(m_color);
        p->drawEllipse(QPointF(19, 18), 2.2, 3.0);
    } else if (m_name == "shapes") {
        p->drawRect(QRectF(4, 4, 9, 9));
        p->drawEllipse(QPointF(15, 15), 5.0, 5.0);
    } else if (m_name == "crop") {
        p->drawLine(7, 3, 7, 17);
        p->drawLine(7, 17, 21, 17);
        p->drawLine(3, 7, 17, 7);
        p->drawLine(17, 7, 17, 21);
    } else if (m_name == "layers") {
        QPainterPath top;
        top.moveTo(12, 4); top.lineTo(20, 8); top.lineTo(12, 12); top.lineTo(4, 8); top.closeSubpath();
        p->drawPath(top);
        p->drawLine(4, 13, 12, 17); p->drawLine(12, 17, 20, 13);
        p->drawLine(4, 17, 12, 21); p->drawLine(12, 21, 20, 17);
    } else if (m_name == "eye") {
        QPainterPath eye;
        eye.moveTo(2, 12);
        eye.cubicTo(6, 6, 18, 6, 22, 12);
        eye.cubicTo(18, 18, 6, 18, 2, 12);
        p->drawPath(eye);
        p->setBrush(m_color);
        p->drawEllipse(QPointF(12, 12), 2.5, 2.5);
    } else if (m_name == "eye-off") {
        QPainterPath eye;
        eye.moveTo(2, 12);
        eye.cubicTo(6, 6, 18, 6, 22, 12);
        eye.cubicTo(18, 18, 6, 18, 2, 12);
        p->drawPath(eye);
        p->drawLine(4, 4, 20, 20);
    } else if (m_name == "up") {
        p->drawLine(12, 19, 12, 5);
        p->drawLine(7, 10, 12, 5);
        p->drawLine(17, 10, 12, 5);
    } else if (m_name == "down") {
        p->drawLine(12, 5, 12, 19);
        p->drawLine(7, 14, 12, 19);
        p->drawLine(17, 14, 12, 19);
    } else if (m_name == "plus") {
        p->drawLine(12, 5, 12, 19);
        p->drawLine(5, 12, 19, 12);
    } else if (m_name == "minus") {
        p->drawLine(5, 12, 19, 12);
    } else if (m_name == "check") {
        p->drawLine(4, 12, 9, 17);
        p->drawLine(9, 17, 20, 6);
    } else if (m_name == "close") {
        p->drawLine(6, 6, 18, 18);
        p->drawLine(18, 6, 6, 18);
    } else if (m_name == "copy") {
        p->drawRect(QRectF(8, 8, 11, 11));
        p->drawLine(5, 15, 5, 5);
        p->drawLine(5, 5, 15, 5);
    } else if (m_name == "trash") {
        p->drawLine(5, 7, 19, 7);
        p->drawLine(9, 7, 9, 4);
        p->drawLine(9, 4, 15, 4);
        p->drawLine(15, 4, 15, 7);
        p->drawRect(QRectF(7, 7, 10, 13));
    } else if (m_name == "bake") {
        p->drawLine(12, 4, 12, 14);
        p->drawLine(8, 10, 12, 14);
        p->drawLine(16, 10, 12, 14);
        p->drawLine(4, 19, 20, 19);
    } else if (m_name == "text") {
        p->drawLine(5, 6, 19, 6);
        p->drawLine(12, 6, 12, 19);
        p->drawLine(9, 19, 15, 19);
    } else if (m_name == "shadow") {
        p->drawRect(QRectF(5, 5, 11, 11));
        p->drawLine(9, 19, 19, 19);
        p->drawLine(19, 9, 19, 19);
    } else if (m_name == "reset") {
        p->drawArc(QRectF(5, 5, 14, 14), 30 * 16, 300 * 16);
        p->drawLine(16, 4, 19, 8);
        p->drawLine(14, 8, 19, 8);
    } else if (m_name == "arrow") {
        p->drawLine(5, 19, 19, 5);
        p->drawLine(11, 5, 19, 5);
        p->drawLine(19, 5, 19, 13);
    } else if (m_name == "double-arrow") {
        p->drawLine(5, 19, 19, 5);
        p->drawLine(13, 5, 19, 5); p->drawLine(19, 5, 19, 11);
        p->drawLine(5, 13, 5, 19); p->drawLine(5, 19, 11, 19);
    } else if (m_name == "curved-arrow") {
        QPainterPath c;
        c.moveTo(5, 18);
        c.quadTo(5, 6, 19, 6);
        p->drawPath(c);
        p->drawLine(14, 3, 19, 6);
        p->drawLine(14, 9, 19, 6);
    } else if (m_name == "line") {
        p->drawLine(5, 19, 19, 5);
        p->setBrush(m_color);
        p->drawEllipse(QPointF(5, 19), 2, 2);
        p->drawEllipse(QPointF(19, 5), 2, 2);
    } else if (m_name == "rect") {
        p->drawRect(QRectF(4, 6, 16, 12));
    } else if (m_name == "round-rect") {
        p->drawRoundedRect(QRectF(4, 6, 16, 12), 4, 4);
    } else if (m_name == "circle") {
        p->drawEllipse(QPointF(12, 12), 7, 7);
    } else if (m_name == "triangle") {
        QPainterPath t;
        t.moveTo(12, 4); t.lineTo(20, 19); t.lineTo(4, 19); t.closeSubpath();
        p->drawPath(t);
    } else if (m_name == "star") {
        QPainterPath s;
        for (int i = 0; i < 10; ++i) {
            qreal a = i * M_PI / 5.0 - M_PI / 2.0;
            qreal r = (i % 2 == 0) ? 8.5 : 3.6;
            QPointF pt(12 + std::cos(a) * r, 12 + std::sin(a) * r);
            if (i == 0) s.moveTo(pt); else s.lineTo(pt);
        }
        s.closeSubpath();
        p->drawPath(s);
    } else if (m_name == "burst") {
        QPainterPath b;
        for (int i = 0; i < 16; ++i) {
            qreal a = i * M_PI / 8.0;
            qreal r = (i % 2 == 0) ? 9.0 : 5.5;
            QPointF pt(12 + std::cos(a) * r, 12 + std::sin(a) * r);
            if (i == 0) b.moveTo(pt); else b.lineTo(pt);
        }
        b.closeSubpath();
        p->drawPath(b);
    } else if (m_name == "bubble") {
        QPainterPath bb;
        bb.addRoundedRect(QRectF(4, 4, 16, 12), 3, 3);
        bb.moveTo(8, 16); bb.lineTo(6, 20); bb.lineTo(12, 16);
        p->drawPath(bb);
    } else if (m_name == "picker") {
        p->drawLine(14, 6, 18, 10);
        p->drawLine(16, 4, 20, 8);
        p->drawLine(15, 7, 7, 15);
        p->drawLine(17, 9, 9, 17);
        p->drawLine(7, 15, 5, 19);
        p->drawLine(9, 17, 5, 19);
    }
}

// =============================================================================
// CANVAS BACKEND IMPLEMENTATION
// =============================================================================
CanvasBackend::CanvasBackend(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setRenderTarget(QQuickPaintedItem::Image);
    setOpaquePainting(true);
    setAntialiasing(true);

    m_checkerTile = QPixmap(48, 48);
    m_checkerTile.fill(QColor("#262a2e"));
    QPainter cp(&m_checkerTile);
    cp.fillRect(0, 0, 24, 24, QColor("#32373c"));
    cp.fillRect(24, 24, 24, 24, QColor("#32373c"));
    cp.end();

    createNewCanvas(1280, 720, QColor("#1e2227"));
}

void CanvasBackend::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    if (newGeometry.width() > 50 && newGeometry.height() > 50) {
        if (!m_initialFitDone || qAbs(newGeometry.width() - oldGeometry.width()) > 40) {
            fitToView();
            m_initialFitDone = true;
        }
    }
}

void CanvasBackend::fitToView()
{
    if (width() <= 10 || height() <= 10) return;
    qreal availW = width() - 28.0;
    qreal availH = height() - 28.0;
    m_zoom = qBound(0.08, qMin(availW / m_canvasWidth, availH / m_canvasHeight), 3.0);
    m_panX = (width() - (m_canvasWidth * m_zoom)) * 0.5;
    m_panY = (height() - (m_canvasHeight * m_zoom)) * 0.5;
    emit viewTransformChanged();
    update();
}

void CanvasBackend::zoomStep(qreal factor)
{
    qreal oldZoom = m_zoom;
    m_zoom = qBound(0.08, m_zoom * factor, 4.0);
    qreal cx = width() * 0.5;
    qreal cy = height() * 0.5;
    m_panX = cx - (cx - m_panX) * (m_zoom / oldZoom);
    m_panY = cy - (cy - m_panY) * (m_zoom / oldZoom);
    emit viewTransformChanged();
    update();
}

void CanvasBackend::pinchUpdate(qreal scaleFactor, qreal centerX, qreal centerY, qreal panDx, qreal panDy)
{
    qreal oldZoom = m_zoom;
    m_zoom = qBound(0.08, m_zoom * scaleFactor, 4.0);
    m_panX = centerX - (centerX - m_panX) * (m_zoom / oldZoom) + panDx;
    m_panY = centerY - (centerY - m_panY) * (m_zoom / oldZoom) + panDy;
    emit viewTransformChanged();
    update();
}

QPointF CanvasBackend::screenToCanvas(qreal sx, qreal sy) const
{
    return QPointF((sx - m_panX) / m_zoom, (sy - m_panY) / m_zoom);
}

QPointF CanvasBackend::canvasToLayerImage(const Layer &l, const QPointF &canvasPt) const
{
    QRectF r = l.rect.normalized();
    if (r.width() < 1.0 || r.height() < 1.0) return canvasPt;
    qreal lx = (canvasPt.x() - r.left()) * (l.raster.width() / r.width());
    qreal ly = (canvasPt.y() - r.top()) * (l.raster.height() / r.height());
    return QPointF(lx, ly);
}

void CanvasBackend::ensureLayerCoversCanvas(Layer &l)
{
    QRectF fullRect(0, 0, m_canvasWidth, m_canvasHeight);
    if (l.rect == fullRect && l.raster.size() == QSize(m_canvasWidth, m_canvasHeight))
        return;

    // Rebases a moved/scaled layer onto a full-canvas buffer when drawing with Brush/Bucket outside its bounds
    QImage rebased(m_canvasWidth, m_canvasHeight, QImage::Format_ARGB32_Premultiplied);
    rebased.fill(Qt::transparent);
    QPainter p(&rebased);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.drawImage(l.rect.normalized(), l.raster);
    p.end();
    l.raster = rebased;
    l.rect = fullRect;
}

void CanvasBackend::createNewCanvas(int w, int h, const QColor &bgColor)
{
    m_canvasWidth = qMax(64, w);
    m_canvasHeight = qMax(64, h);

    m_layers.clear();
    m_undoStack.clear();
    m_redoStack.clear();

    Layer bg;
    bg.name = "Background";
    bg.rect = QRectF(0, 0, m_canvasWidth, m_canvasHeight);
    bg.raster = QImage(m_canvasWidth, m_canvasHeight, QImage::Format_ARGB32_Premultiplied);
    bg.raster.fill(bgColor);
    m_layers.append(bg);

    Layer drawLayer;
    drawLayer.name = "Layer 1";
    drawLayer.rect = QRectF(0, 0, m_canvasWidth, m_canvasHeight);
    drawLayer.raster = QImage(m_canvasWidth, m_canvasHeight, QImage::Format_ARGB32_Premultiplied);
    drawLayer.raster.fill(Qt::transparent);
    m_layers.append(drawLayer);

    m_activeLayer = 1;
    m_selectedShape = -1;
    m_tool = "select";
    m_cropRect = QRectF(m_canvasWidth * 0.1, m_canvasHeight * 0.1, m_canvasWidth * 0.8, m_canvasHeight * 0.8);

    fitToView();
    emit canvasSizeChanged();
    emit layersChanged();
    emit selectionChanged();
    emit toolChanged();
    emit undoChanged();
    update();
    emit statusMessage(QString("Canvas %1x%2 ready").arg(m_canvasWidth).arg(m_canvasHeight));
}

QString CanvasBackend::activeLayerName() const
{
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size())
        return m_layers[m_activeLayer].name;
    return QStringLiteral("Layer");
}

QVariantList CanvasBackend::layerModel() const
{
    QVariantList list;
    for (int i = m_layers.size() - 1; i >= 0; --i) {
        QVariantMap map;
        map["index"] = i;
        map["name"] = m_layers[i].name;
        map["visible"] = m_layers[i].visible;
        map["opacity"] = m_layers[i].opacity;
        map["shapeCount"] = m_layers[i].shapes.size();
        list.append(map);
    }
    return list;
}

int CanvasBackend::selectedShapeType() const
{
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        return int(m_layers[m_activeLayer].shapes[m_selectedShape].type);
    }
    return -1;
}

QString CanvasBackend::selectedShapeText() const
{
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        return m_layers[m_activeLayer].shapes[m_selectedShape].text;
    }
    return QString();
}

qreal CanvasBackend::selectedShapeRotation() const
{
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        return m_layers[m_activeLayer].shapes[m_selectedShape].rotation;
    }
    return 0.0;
}

void CanvasBackend::setSelectedShapeRotation(qreal rot)
{
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        m_layers[m_activeLayer].shapes[m_selectedShape].rotation = rot;
        emit selectionChanged();
        update();
    }
}

void CanvasBackend::setActiveLayerIndex(int idx)
{
    if (idx >= 0 && idx < m_layers.size() && m_activeLayer != idx) {
        m_activeLayer = idx;
        m_selectedShape = -1;
        emit layersChanged();
        emit selectionChanged();
        update();
        emit statusMessage("Selected " + m_layers[idx].name);
    }
}

void CanvasBackend::setCurrentTool(const QString &t)
{
    if (m_tool != t) {
        m_tool = t;
        if (m_tool != "select") {
            m_selectedShape = -1;
            emit selectionChanged();
        }
        if (m_tool == "crop") {
            m_cropRect = QRectF(m_canvasWidth * 0.1, m_canvasHeight * 0.1, m_canvasWidth * 0.8, m_canvasHeight * 0.8);
        }
        emit toolChanged();
        update();
    }
}

void CanvasBackend::setPrimaryColor(const QColor &c)
{
    m_primaryColor = c;
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        auto &s = m_layers[m_activeLayer].shapes[m_selectedShape];
        s.strokeColor = c;
        s.fillColor = QColor(c.red(), c.green(), c.blue(), 110);
        update();
    }
    emit styleChanged();
}

void CanvasBackend::setStrokeWidth(qreal w)
{
    m_strokeWidth = qBound(1.0, w, 80.0);
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        m_layers[m_activeLayer].shapes[m_selectedShape].strokeWidth = m_strokeWidth;
        update();
    }
    emit styleChanged();
}

void CanvasBackend::setFillShapes(bool f)
{
    m_fillShapes = f;
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        m_layers[m_activeLayer].shapes[m_selectedShape].filled = f;
        update();
    }
    emit styleChanged();
}

void CanvasBackend::setShapeShadow(bool s)
{
    m_shapeShadow = s;
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        m_layers[m_activeLayer].shapes[m_selectedShape].shadow = s;
        update();
    }
    emit styleChanged();
}

void CanvasBackend::saveUndoState()
{
    if (m_undoStack.size() >= 10)
        m_undoStack.removeFirst();
    m_undoStack.append(m_layers);
    m_redoStack.clear();
    emit undoChanged();
}

void CanvasBackend::undo()
{
    if (m_undoStack.isEmpty()) {
        emit statusMessage("Nothing to undo");
        return;
    }
    m_redoStack.append(m_layers);
    m_layers = m_undoStack.takeLast();
    m_activeLayer = qBound(0, m_activeLayer, int(m_layers.size()) - 1);
    m_selectedShape = -1;
    emit canvasSizeChanged();
    emit layersChanged();
    emit selectionChanged();
    emit undoChanged();
    update();
    emit statusMessage("Undo applied");
}

void CanvasBackend::redo()
{
    if (m_redoStack.isEmpty()) {
        emit statusMessage("Nothing to redo");
        return;
    }
    m_undoStack.append(m_layers);
    m_layers = m_redoStack.takeLast();
    m_activeLayer = qBound(0, m_activeLayer, int(m_layers.size()) - 1);
    m_selectedShape = -1;
    emit canvasSizeChanged();
    emit layersChanged();
    emit selectionChanged();
    emit undoChanged();
    update();
    emit statusMessage("Redo applied");
}

void CanvasBackend::cancelCurrentStroke()
{
    if (m_isDrawingStroke && !m_undoStack.isEmpty()) {
        m_layers = m_undoStack.takeLast();
        m_isDrawingStroke = false;
        m_isInteracting = false;
        m_dragMode = DragNone;
        emit undoChanged();
        update();
    }
}

void CanvasBackend::addLayer(const QString &name)
{
    saveUndoState();
    Layer l;
    l.name = name.isEmpty() ? QString("Layer %1").arg(m_layers.size()) : name;
    l.rect = QRectF(0, 0, m_canvasWidth, m_canvasHeight);
    l.raster = QImage(m_canvasWidth, m_canvasHeight, QImage::Format_ARGB32_Premultiplied);
    l.raster.fill(Qt::transparent);
    m_layers.append(l);
    m_activeLayer = m_layers.size() - 1;
    m_selectedShape = -1;
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Added " + l.name);
}

void CanvasBackend::duplicateActiveLayer()
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size()) return;
    saveUndoState();
    Layer copy = m_layers[m_activeLayer];
    copy.name += " Copy";
    copy.rect.translate(24, 24);
    m_layers.insert(m_activeLayer + 1, copy);
    m_activeLayer++;
    emit layersChanged();
    update();
    emit statusMessage("Layer duplicated");
}

void CanvasBackend::clearActiveLayer()
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size()) return;
    saveUndoState();
    m_layers[m_activeLayer].raster.fill(Qt::transparent);
    m_layers[m_activeLayer].shapes.clear();
    m_selectedShape = -1;
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Layer cleared");
}

void CanvasBackend::deleteActiveLayer()
{
    if (m_layers.size() <= 1) {
        emit statusMessage("Cannot delete the last remaining layer");
        return;
    }
    saveUndoState();
    m_layers.removeAt(m_activeLayer);
    m_activeLayer = qBound(0, m_activeLayer - 1, int(m_layers.size()) - 1);
    m_selectedShape = -1;
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Layer deleted");
}

void CanvasBackend::moveLayer(int fromIdx, int toIdx)
{
    if (fromIdx < 0 || fromIdx >= m_layers.size() || toIdx < 0 || toIdx >= m_layers.size())
        return;
    saveUndoState();
    m_layers.move(fromIdx, toIdx);
    m_activeLayer = toIdx;
    emit layersChanged();
    update();
}

void CanvasBackend::setLayerVisibility(int idx, bool visible)
{
    if (idx >= 0 && idx < m_layers.size()) {
        m_layers[idx].visible = visible;
        emit layersChanged();
        update();
    }
}

void CanvasBackend::setLayerOpacity(int idx, qreal opacity)
{
    if (idx >= 0 && idx < m_layers.size()) {
        m_layers[idx].opacity = qBound(0.0, opacity, 1.0);
        emit layersChanged();
        update();
    }
}

void CanvasBackend::mergeLayerDown(int idx)
{
    if (idx <= 0 || idx >= m_layers.size()) {
        emit statusMessage("No layer below to merge into");
        return;
    }
    saveUndoState();
    Layer &top = m_layers[idx];
    Layer &bottom = m_layers[idx - 1];
    ensureLayerCoversCanvas(bottom);

    QPainter p(&bottom.raster);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setOpacity(top.opacity);
    p.drawImage(top.rect.normalized(), top.raster);
    for (const auto &s : top.shapes)
        drawShapeObject(&p, s, false);
    p.end();

    m_layers.removeAt(idx);
    m_activeLayer = idx - 1;
    m_selectedShape = -1;
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Merged layer down");
}

void CanvasBackend::centerActiveLayer()
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size()) return;
    saveUndoState();
    Layer &l = m_layers[m_activeLayer];
    QPointF oldCenter = l.rect.center();
    QPointF newCenter(m_canvasWidth / 2.0, m_canvasHeight / 2.0);
    QPointF delta = newCenter - oldCenter;
    l.rect.translate(delta);
    for (auto &s : l.shapes) {
        s.rect.translate(delta);
        s.startPt += delta;
        s.endPt += delta;
    }
    update();
    emit statusMessage("Centered " + l.name);
}

void CanvasBackend::scaleActiveLayer(qreal factor)
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size()) return;
    saveUndoState();
    Layer &l = m_layers[m_activeLayer];
    QRectF r = l.rect.normalized();
    QPointF c = r.center();
    qreal nw = qBound(32.0, r.width() * factor, m_canvasWidth * 4.0);
    qreal nh = qBound(32.0, r.height() * factor, m_canvasHeight * 4.0);
    l.rect = QRectF(c.x() - nw * 0.5, c.y() - nh * 0.5, nw, nh);
    update();
}

void CanvasBackend::resetActiveLayerTransform()
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size()) return;
    saveUndoState();
    Layer &l = m_layers[m_activeLayer];
    QSizeF imgSz = l.raster.size();
    if (imgSz.width() > 0 && imgSz.height() > 0) {
        QSizeF scaled = imgSz.scaled(m_canvasWidth, m_canvasHeight, Qt::KeepAspectRatio);
        l.rect = QRectF((m_canvasWidth - scaled.width()) * 0.5,
                        (m_canvasHeight - scaled.height()) * 0.5,
                        scaled.width(), scaled.height());
    } else {
        l.rect = QRectF(0, 0, m_canvasWidth, m_canvasHeight);
    }
    update();
    emit statusMessage("Reset layer size & position");
}

void CanvasBackend::addShape(int shapeType, const QString &customText)
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size())
        return;
    saveUndoState();

    EditableShape s;
    s.type = static_cast<EditableShape::Type>(shapeType);
    qreal cx = m_canvasWidth / 2.0;
    qreal cy = m_canvasHeight / 2.0;
    qreal boxW = qMin(m_canvasWidth * 0.42, 420.0);
    qreal boxH = qMin(m_canvasHeight * 0.32, 240.0);
    s.rect = QRectF(cx - boxW / 2, cy - boxH / 2, boxW, boxH);
    s.startPt = QPointF(cx - boxW / 2, cy + boxH / 3);
    s.endPt = QPointF(cx + boxW / 2, cy - boxH / 3);
    s.strokeColor = m_primaryColor;
    s.fillColor = QColor(m_primaryColor.red(), m_primaryColor.green(), m_primaryColor.blue(), 110);
    s.strokeWidth = m_strokeWidth;
    s.filled = m_fillShapes;
    s.shadow = m_shapeShadow;
    if (!customText.isEmpty())
        s.text = customText;

    m_layers[m_activeLayer].shapes.append(s);
    m_selectedShape = m_layers[m_activeLayer].shapes.size() - 1;
    m_tool = "select";

    emit toolChanged();
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Shape added - drag handles to resize");
}

void CanvasBackend::duplicateSelectedShape()
{
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        saveUndoState();
        EditableShape copy = m_layers[m_activeLayer].shapes[m_selectedShape];
        copy.rect.translate(30, 30);
        copy.startPt += QPointF(30, 30);
        copy.endPt += QPointF(30, 30);
        m_layers[m_activeLayer].shapes.append(copy);
        m_selectedShape = m_layers[m_activeLayer].shapes.size() - 1;
        emit layersChanged();
        emit selectionChanged();
        update();
        emit statusMessage("Shape duplicated");
    }
}

void CanvasBackend::deleteSelectedShape()
{
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size() &&
        m_selectedShape >= 0 && m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        saveUndoState();
        m_layers[m_activeLayer].shapes.removeAt(m_selectedShape);
        m_selectedShape = -1;
        emit layersChanged();
        emit selectionChanged();
        update();
        emit statusMessage("Shape deleted");
    }
}

void CanvasBackend::rasterizeSelectedShape()
{
    if (m_activeLayer < 0 || m_selectedShape < 0 ||
        m_selectedShape >= m_layers[m_activeLayer].shapes.size())
        return;
    saveUndoState();
    auto &layer = m_layers[m_activeLayer];
    ensureLayerCoversCanvas(layer);
    QPainter p(&layer.raster);
    p.setRenderHint(QPainter::Antialiasing);
    drawShapeObject(&p, layer.shapes[m_selectedShape], false);
    p.end();

    layer.shapes.removeAt(m_selectedShape);
    m_selectedShape = -1;
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Shape baked into layer");
}

void CanvasBackend::updateSelectedShapeText(const QString &newText)
{
    if (m_activeLayer >= 0 && m_selectedShape >= 0 &&
        m_selectedShape < m_layers[m_activeLayer].shapes.size()) {
        saveUndoState();
        m_layers[m_activeLayer].shapes[m_selectedShape].text = newText;
        emit selectionChanged();
        update();
    }
}

void CanvasBackend::importImage(const QUrl &fileUrl)
{
    QString path = fileUrl.isLocalFile() ? fileUrl.toLocalFile() : fileUrl.toString();
    QImage img;

    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        img.loadFromData(file.readAll());
        file.close();
    }
    if (img.isNull()) {
        img.load(path);
    }
    if (img.isNull()) {
        emit statusMessage("Failed to load image");
        return;
    }

    saveUndoState();
    Layer photoLayer;
    photoLayer.name = QString("Photo %1").arg(m_layers.size());
    photoLayer.raster = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    QSizeF scaled = QSizeF(img.size()).scaled(m_canvasWidth * 0.85, m_canvasHeight * 0.85, Qt::KeepAspectRatio);
    qreal ox = (m_canvasWidth - scaled.width()) * 0.5;
    qreal oy = (m_canvasHeight - scaled.height()) * 0.5;
    photoLayer.rect = QRectF(ox, oy, scaled.width(), scaled.height());

    m_layers.append(photoLayer);
    m_activeLayer = m_layers.size() - 1;
    m_selectedShape = -1;
    m_tool = "select";
    emit toolChanged();
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Photo layer added - drag to move or corners to resize!");
}

void CanvasBackend::applyCrop()
{
    QRect r = m_cropRect.normalized().toRect().intersected(QRect(0, 0, m_canvasWidth, m_canvasHeight));
    if (r.width() < 32 || r.height() < 32) {
        emit statusMessage("Crop area too small");
        return;
    }
    saveUndoState();
    for (auto &layer : m_layers) {
        layer.rect.translate(-r.x(), -r.y());
        for (auto &s : layer.shapes) {
            s.rect.translate(-r.x(), -r.y());
            s.startPt -= QPointF(r.x(), r.y());
            s.endPt -= QPointF(r.x(), r.y());
        }
    }
    m_canvasWidth = r.width();
    m_canvasHeight = r.height();
    m_cropRect = QRectF(m_canvasWidth * 0.1, m_canvasHeight * 0.1, m_canvasWidth * 0.8, m_canvasHeight * 0.8);
    m_tool = "select";

    fitToView();
    emit canvasSizeChanged();
    emit toolChanged();
    update();
    emit statusMessage(QString("Cropped to %1x%2").arg(m_canvasWidth).arg(m_canvasHeight));
}

void CanvasBackend::cancelCrop()
{
    m_tool = "select";
    emit toolChanged();
    update();
}

void CanvasBackend::adjustImage(int brightness, int contrast, int saturation)
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size())
        return;
    saveUndoState();
    QImage &img = m_layers[m_activeLayer].raster;
    qreal cFactor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
    qreal sFactor = 1.0 + (saturation / 100.0);

    for (int y = 0; y < img.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            int a = qAlpha(line[x]);
            if (a == 0) continue;
            int r = qRed(line[x]) + brightness;
            int g = qGreen(line[x]) + brightness;
            int b = qBlue(line[x]) + brightness;

            r = qBound(0, int(cFactor * (r - 128) + 128), 255);
            g = qBound(0, int(cFactor * (g - 128) + 128), 255);
            b = qBound(0, int(cFactor * (b - 128) + 128), 255);

            if (saturation != 0) {
                qreal gray = 0.299 * r + 0.587 * g + 0.114 * b;
                r = qBound(0, int(gray + (r - gray) * sFactor), 255);
                g = qBound(0, int(gray + (g - gray) * sFactor), 255);
                b = qBound(0, int(gray + (b - gray) * sFactor), 255);
            }
            line[x] = qRgba(r, g, b, a);
        }
    }
    update();
    emit statusMessage("Adjustments applied");
}

void CanvasBackend::applyFilter(const QString &filterName)
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size())
        return;
    saveUndoState();
    QImage &img = m_layers[m_activeLayer].raster;
    const int w = img.width();
    const int h = img.height();

    if (filterName == "grayscale") {
        for (int y = 0; y < h; ++y) {
            QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
            for (int x = 0; x < w; ++x) {
                int a = qAlpha(line[x]);
                if (a == 0) continue;
                int gray = qGray(line[x]);
                line[x] = qRgba(gray, gray, gray, a);
            }
        }
    } else if (filterName == "invert") {
        for (int y = 0; y < h; ++y) {
            QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
            for (int x = 0; x < w; ++x) {
                int a = qAlpha(line[x]);
                if (a == 0) continue;
                line[x] = qRgba(255 - qRed(line[x]), 255 - qGreen(line[x]), 255 - qBlue(line[x]), a);
            }
        }
    } else if (filterName == "vignette") {
        QPainter p(&img);
        QRadialGradient grad(QPointF(w / 2.0, h / 2.0), qMax(w, h) * 0.65);
        grad.setColorAt(0.0, QColor(0, 0, 0, 0));
        grad.setColorAt(0.65, QColor(0, 0, 0, 20));
        grad.setColorAt(1.0, QColor(0, 0, 0, 185));
        p.fillRect(0, 0, w, h, grad);
        p.end();
    }
    update();
    emit statusMessage("Filter applied: " + filterName);
}

void CanvasBackend::flipLayer(bool horizontal)
{
    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size())
        return;
    saveUndoState();
    m_layers[m_activeLayer].raster = m_layers[m_activeLayer].raster.mirrored(horizontal, !horizontal);
    update();
    emit statusMessage(horizontal ? "Flipped layer horizontally" : "Flipped layer vertically");
}

void CanvasBackend::rotateCanvas90()
{
    saveUndoState();
    QTransform trans;
    trans.rotate(90);
    for (auto &l : m_layers) {
        l.raster = l.raster.transformed(trans, Qt::SmoothTransformation);
        l.rect = QRectF(l.rect.y(), l.rect.x(), l.rect.height(), l.rect.width());
    }
    std::swap(m_canvasWidth, m_canvasHeight);
    fitToView();
    emit canvasSizeChanged();
    update();
    emit statusMessage("Rotated canvas 90 deg");
}

QImage CanvasBackend::renderCompositeImage(int targetW, int targetH)
{
    QImage finalImg(m_canvasWidth, m_canvasHeight, QImage::Format_ARGB32_Premultiplied);
    finalImg.fill(Qt::transparent);

    QPainter p(&finalImg);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    for (const auto &layer : m_layers) {
        if (!layer.visible) continue;
        p.setOpacity(layer.opacity);
        p.drawImage(layer.rect.normalized(), layer.raster);
        for (const auto &s : layer.shapes)
            drawShapeObject(&p, s, false);
    }
    p.end();

    if (targetW > 0 && targetH > 0 && (targetW != m_canvasWidth || targetH != m_canvasHeight)) {
        finalImg = finalImg.scaled(targetW, targetH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    return finalImg;
}

bool CanvasBackend::exportImage(const QUrl &fileUrl, const QString &format, int quality, int targetW, int targetH)
{
    QImage finalImg = renderCompositeImage(targetW, targetH);
    QString path = fileUrl.isLocalFile() ? fileUrl.toLocalFile() : fileUrl.toString();

    QFile outFile(path);
    bool ok = false;
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        ok = finalImg.save(&outFile, format.toUtf8().constData(), quality);
        outFile.close();
    }
    if (!ok) {
        ok = finalImg.save(path, format.toUtf8().constData(), quality);
    }
    emit statusMessage(ok ? "Saved image!" : "Failed to save image");
    return ok;
}

QString CanvasBackend::quickExportToDevice(const QString &format, int quality, int targetW, int targetH)
{
    QImage finalImg = renderCompositeImage(targetW, targetH);
    QString ext = format.toLower();
    if (ext == "jpeg") ext = "jpg";

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString fileName = QString("BreezeCanvas_%1.%2").arg(timestamp, ext);

    QStringList candidateDirs = {
        "/storage/emulated/0/Download",
        "/storage/emulated/0/Pictures/BreezeCanvas",
        "/storage/emulated/0/Pictures",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    };

    for (const QString &dirPath : candidateDirs) {
        if (dirPath.isEmpty()) continue;
        QDir dir(dirPath);
        if (!dir.exists()) dir.mkpath(".");
        QString fullPath = dir.filePath(fileName);
        QFile f(fullPath);
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (finalImg.save(&f, format.toUtf8().constData(), quality)) {
                f.close();
                emit statusMessage("Saved to: " + fullPath);
                return fullPath;
            }
            f.close();
        }
    }

    emit statusMessage("Quick export failed - use Custom Location");
    return QString();
}

bool CanvasBackend::saveProject(const QUrl &fileUrl)
{
    QJsonObject root;
    root["width"] = m_canvasWidth;
    root["height"] = m_canvasHeight;

    QJsonArray layersArr;
    for (const auto &l : m_layers) {
        QJsonObject lObj;
        lObj["name"] = l.name;
        lObj["visible"] = l.visible;
        lObj["opacity"] = l.opacity;
        lObj["lx"] = l.rect.x();
        lObj["ly"] = l.rect.y();
        lObj["lw"] = l.rect.width();
        lObj["lh"] = l.rect.height();

        QByteArray bytes;
        QBuffer buf(&bytes);
        buf.open(QIODevice::WriteOnly);
        l.raster.save(&buf, "PNG");
        lObj["rasterBase64"] = QString::fromLatin1(bytes.toBase64());

        QJsonArray shapesArr;
        for (const auto &s : l.shapes) {
            QJsonObject sObj;
            sObj["type"] = int(s.type);
            sObj["x"] = s.rect.x();
            sObj["y"] = s.rect.y();
            sObj["w"] = s.rect.width();
            sObj["h"] = s.rect.height();
            sObj["sx"] = s.startPt.x();
            sObj["sy"] = s.startPt.y();
            sObj["ex"] = s.endPt.x();
            sObj["ey"] = s.endPt.y();
            sObj["color"] = s.strokeColor.name(QColor::HexArgb);
            sObj["strokeWidth"] = s.strokeWidth;
            sObj["rotation"] = s.rotation;
            sObj["filled"] = s.filled;
            sObj["shadow"] = s.shadow;
            sObj["text"] = s.text;
            shapesArr.append(sObj);
        }
        lObj["shapes"] = shapesArr;
        layersArr.append(lObj);
    }
    root["layers"] = layersArr;

    QString path = fileUrl.isLocalFile() ? fileUrl.toLocalFile() : fileUrl.toString();
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit statusMessage("Failed to save project");
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    f.close();
    emit statusMessage("Project saved!");
    return true;
}

bool CanvasBackend::loadProject(const QUrl &fileUrl)
{
    QString path = fileUrl.isLocalFile() ? fileUrl.toLocalFile() : fileUrl.toString();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        emit statusMessage("Failed to open project");
        return false;
    }

    QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
    if (root.isEmpty()) return false;

    saveUndoState();
    m_canvasWidth = root["width"].toInt(1280);
    m_canvasHeight = root["height"].toInt(720);

    m_layers.clear();
    QJsonArray layersArr = root["layers"].toArray();
    for (const auto &val : layersArr) {
        QJsonObject lObj = val.toObject();
        Layer l;
        l.name = lObj["name"].toString();
        l.visible = lObj["visible"].toBool(true);
        l.opacity = lObj["opacity"].toDouble(1.0);
        QByteArray raw = QByteArray::fromBase64(lObj["rasterBase64"].toString().toLatin1());
        l.raster.loadFromData(raw, "PNG");
        l.rect = QRectF(
            lObj["lx"].toDouble(0),
            lObj["ly"].toDouble(0),
            lObj["lw"].toDouble(m_canvasWidth),
            lObj["lh"].toDouble(m_canvasHeight)
        );

        for (const auto &sVal : lObj["shapes"].toArray()) {
            QJsonObject sObj = sVal.toObject();
            EditableShape s;
            s.type = static_cast<EditableShape::Type>(sObj["type"].toInt());
            s.rect = QRectF(sObj["x"].toDouble(), sObj["y"].toDouble(), sObj["w"].toDouble(), sObj["h"].toDouble());
            s.startPt = QPointF(sObj["sx"].toDouble(), sObj["sy"].toDouble());
            s.endPt = QPointF(sObj["ex"].toDouble(), sObj["ey"].toDouble());
            s.strokeColor = QColor(sObj["color"].toString());
            s.fillColor = QColor(s.strokeColor.red(), s.strokeColor.green(), s.strokeColor.blue(), 110);
            s.strokeWidth = sObj["strokeWidth"].toDouble(8.0);
            s.rotation = sObj["rotation"].toDouble(0.0);
            s.filled = sObj["filled"].toBool(false);
            s.shadow = sObj["shadow"].toBool(true);
            s.text = sObj["text"].toString();
            l.shapes.append(s);
        }
        m_layers.append(l);
    }
    if (m_layers.isEmpty()) {
        createNewCanvas(1280, 720);
        return false;
    }
    m_activeLayer = m_layers.size() - 1;
    m_selectedShape = -1;
    fitToView();
    emit canvasSizeChanged();
    emit layersChanged();
    emit selectionChanged();
    update();
    emit statusMessage("Project loaded!");
    return true;
}

void CanvasBackend::paint(QPainter *painter)
{
    painter->fillRect(boundingRect(), QColor("#181a1d"));

    painter->save();
    painter->translate(m_panX, m_panY);
    painter->scale(m_zoom, m_zoom);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, !m_isInteracting);

    const QRectF canvasRect(0, 0, m_canvasWidth, m_canvasHeight);

    // Clip layer contents to the canvas frame so layers moved near edges look like a real frame
    painter->save();
    painter->setClipRect(canvasRect);
    painter->drawTiledPixmap(canvasRect, m_checkerTile);

    for (int i = 0; i < m_layers.size(); ++i) {
        const auto &layer = m_layers[i];
        if (!layer.visible) continue;

        painter->save();
        painter->setOpacity(layer.opacity);
        painter->drawImage(layer.rect.normalized(), layer.raster);

        for (int sIdx = 0; sIdx < layer.shapes.size(); ++sIdx) {
            bool isSelected = (i == m_activeLayer && sIdx == m_selectedShape && m_tool == "select");
            drawShapeObject(painter, layer.shapes[sIdx], isSelected);
        }
        painter->restore();
    }
    painter->restore(); // End canvas clip so selection handles remain visible even outside canvas edges

    const qreal invZoom = 1.0 / qMax(0.08, m_zoom);

    // Outer Canvas Frame Border
    painter->setPen(QPen(QColor("#4d5257"), 2.0 * invZoom));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(canvasRect);

    // Active Layer Bounding Box & 4 Resize Handles (when in Move mode and no shape is selected)
    if (m_tool == "select" && m_selectedShape == -1 && m_activeLayer >= 0 && m_activeLayer < m_layers.size()) {
        const QRectF lr = m_layers[m_activeLayer].rect.normalized();
        const qreal handleRad = 12.0 * invZoom;

        painter->setPen(QPen(QColor("#3daee9"), 2.0 * invZoom, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(lr);

        painter->setBrush(QColor("#3daee9"));
        painter->setPen(QPen(Qt::white, 2.0 * invZoom));
        painter->drawEllipse(lr.topLeft(), handleRad, handleRad);
        painter->drawEllipse(lr.topRight(), handleRad, handleRad);
        painter->drawEllipse(lr.bottomLeft(), handleRad, handleRad);
        painter->drawEllipse(lr.bottomRight(), handleRad, handleRad);
    }

    // Interactive Crop Overlay
    if (m_tool == "crop") {
        const qreal handleRad = 14.0 * invZoom;

        QPainterPath outside;
        outside.addRect(canvasRect);
        outside.addRect(m_cropRect.normalized());
        painter->fillPath(outside, QColor(0, 0, 0, 165));

        QRectF cr = m_cropRect.normalized();
        painter->setPen(QPen(QColor("#3daee9"), 2.5 * invZoom, Qt::DashLine));
        painter->drawRect(cr);

        painter->setBrush(QColor("#3daee9"));
        painter->setPen(QPen(Qt::white, 2.0 * invZoom));
        painter->drawEllipse(cr.topLeft(), handleRad, handleRad);
        painter->drawEllipse(cr.topRight(), handleRad, handleRad);
        painter->drawEllipse(cr.bottomLeft(), handleRad, handleRad);
        painter->drawEllipse(cr.bottomRight(), handleRad, handleRad);
    }

    painter->restore();
}

void CanvasBackend::drawArrowHead(QPainter *p, const QPointF &from, const QPointF &to, qreal size, const QColor &col)
{
    QLineF line(from, to);
    if (line.length() < 1.0) return;
    double angle = std::atan2(-line.dy(), line.dx());

    QPointF p1 = to - QPointF(std::sin(angle + M_PI / 3.0) * size, std::cos(angle + M_PI / 3.0) * size);
    QPointF p2 = to - QPointF(std::sin(angle + M_PI - M_PI / 3.0) * size, std::cos(angle + M_PI - M_PI / 3.0) * size);

    QPainterPath path;
    path.moveTo(to);
    path.lineTo(p1);
    path.lineTo(p2);
    path.closeSubpath();
    p->fillPath(path, col);
}

void CanvasBackend::drawShapeObject(QPainter *p, const EditableShape &s, bool drawHandles)
{
    p->save();
    QRectF r = s.rect.normalized();
    bool isLineLike = (s.type == EditableShape::ArrowShape ||
                       s.type == EditableShape::DoubleArrowShape ||
                       s.type == EditableShape::LineShape ||
                       s.type == EditableShape::CurvedArrowShape);

    if (!isLineLike && !qFuzzyIsNull(s.rotation)) {
        p->translate(r.center());
        p->rotate(s.rotation);
        p->translate(-r.center());
    }

    auto renderGeometry = [&](QPainter *painter, const QColor &strokeCol, const QColor &fillCol, qreal widthAdd, QPointF offset) {
        painter->save();
        painter->translate(offset);
        QPen pen(strokeCol, s.strokeWidth + widthAdd, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(s.filled ? QBrush(fillCol) : Qt::NoBrush);

        switch (s.type) {
        case EditableShape::RectShape:
            painter->drawRect(r);
            break;
        case EditableShape::RoundedRectShape:
            painter->drawRoundedRect(r, 28, 28);
            break;
        case EditableShape::EllipseShape:
            painter->drawEllipse(r);
            break;
        case EditableShape::TriangleShape: {
            QPainterPath tri;
            tri.moveTo(r.center().x(), r.top());
            tri.lineTo(r.right(), r.bottom());
            tri.lineTo(r.left(), r.bottom());
            tri.closeSubpath();
            painter->drawPath(tri);
            break;
        }
        case EditableShape::LineShape:
            painter->drawLine(s.startPt, s.endPt);
            break;
        case EditableShape::ArrowShape: {
            painter->drawLine(s.startPt, s.endPt);
            drawArrowHead(painter, s.startPt, s.endPt, qMax(28.0, (s.strokeWidth + widthAdd) * 3.6), strokeCol);
            break;
        }
        case EditableShape::DoubleArrowShape: {
            painter->drawLine(s.startPt, s.endPt);
            qreal hs = qMax(28.0, (s.strokeWidth + widthAdd) * 3.6);
            drawArrowHead(painter, s.startPt, s.endPt, hs, strokeCol);
            drawArrowHead(painter, s.endPt, s.startPt, hs, strokeCol);
            break;
        }
        case EditableShape::CurvedArrowShape: {
            QPainterPath curve;
            curve.moveTo(s.startPt);
            QPointF ctrl(s.startPt.x(), s.endPt.y());
            curve.quadTo(ctrl, s.endPt);
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(curve);
            drawArrowHead(painter, ctrl, s.endPt, qMax(28.0, (s.strokeWidth + widthAdd) * 3.6), strokeCol);
            break;
        }
        case EditableShape::StarShape: {
            QPainterPath star;
            QPointF center = r.center();
            qreal rx = r.width() / 2.0;
            qreal ry = r.height() / 2.0;
            for (int i = 0; i < 10; ++i) {
                qreal angle = i * M_PI / 5.0 - M_PI / 2.0;
                qreal radX = (i % 2 == 0) ? rx : rx * 0.42;
                qreal radY = (i % 2 == 0) ? ry : ry * 0.42;
                QPointF pt(center.x() + std::cos(angle) * radX, center.y() + std::sin(angle) * radY);
                if (i == 0) star.moveTo(pt);
                else star.lineTo(pt);
            }
            star.closeSubpath();
            painter->drawPath(star);
            break;
        }
        case EditableShape::BurstShape: {
            QPainterPath burst;
            QPointF center = r.center();
            qreal rx = r.width() / 2.0;
            qreal ry = r.height() / 2.0;
            const int pts = 24;
            for (int i = 0; i < pts; ++i) {
                qreal angle = i * 2.0 * M_PI / pts;
                qreal factor = (i % 2 == 0) ? 1.0 : 0.68;
                QPointF pt(center.x() + std::cos(angle) * rx * factor, center.y() + std::sin(angle) * ry * factor);
                if (i == 0) burst.moveTo(pt);
                else burst.lineTo(pt);
            }
            burst.closeSubpath();
            painter->drawPath(burst);
            break;
        }
        case EditableShape::BubbleShape: {
            QPainterPath bubble;
            QRectF body(r.left(), r.top(), r.width(), r.height() * 0.78);
            bubble.addRoundedRect(body, 22, 22);
            bubble.moveTo(r.left() + r.width() * 0.25, body.bottom());
            bubble.lineTo(r.left() + r.width() * 0.15, r.bottom());
            bubble.lineTo(r.left() + r.width() * 0.42, body.bottom());
            painter->drawPath(bubble);
            break;
        }
        case EditableShape::TextShape: {
            QFont font("Sans Serif", qMax(18, int(r.height() * 0.45)), QFont::Black);
            QPainterPath textPath;
            textPath.addText(r.left() + 8, r.center().y() + (r.height() * 0.18), font, s.text);
            painter->setPen(QPen(Qt::black, qMax(6.0, s.strokeWidth + widthAdd), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setBrush(strokeCol);
            painter->drawPath(textPath);
            break;
        }
        }
        painter->restore();
    };

    if (s.shadow) {
        renderGeometry(p, QColor(0, 0, 0, 170), QColor(0, 0, 0, 90), 4.0, QPointF(5, 6));
    }
    renderGeometry(p, s.strokeColor, s.fillColor, 0.0, QPointF(0, 0));

    if (drawHandles) {
        const qreal invZoom = 1.0 / qMax(0.08, m_zoom);
        const qreal handleRad = 13.0 * invZoom;

        p->setPen(QPen(QColor("#3daee9"), 2.2 * invZoom, Qt::DashLine));
        p->setBrush(QColor("#3daee9"));

        if (isLineLike) {
            p->drawLine(s.startPt, s.endPt);
            p->setPen(QPen(Qt::white, 2.0 * invZoom));
            p->drawEllipse(s.startPt, handleRad, handleRad);
            p->drawEllipse(s.endPt, handleRad, handleRad);
        } else {
            p->setBrush(Qt::NoBrush);
            p->drawRect(r);
            p->setBrush(QColor("#3daee9"));
            p->setPen(QPen(Qt::white, 2.0 * invZoom));
            p->drawEllipse(r.topLeft(), handleRad, handleRad);
            p->drawEllipse(r.topRight(), handleRad, handleRad);
            p->drawEllipse(r.bottomLeft(), handleRad, handleRad);
            p->drawEllipse(r.bottomRight(), handleRad, handleRad);
        }
    }
    p->restore();
}

int CanvasBackend::hitTestHandle(const QPointF &pt, const EditableShape &s) const
{
    const qreal invZoom = 1.0 / qMax(0.08, m_zoom);
    const qreal touchRadius = 28.0 * invZoom;

    if (s.type == EditableShape::ArrowShape ||
        s.type == EditableShape::DoubleArrowShape ||
        s.type == EditableShape::LineShape ||
        s.type == EditableShape::CurvedArrowShape) {
        if (QLineF(pt, s.startPt).length() <= touchRadius) return DragArrowStart;
        if (QLineF(pt, s.endPt).length() <= touchRadius) return DragArrowEnd;
        if (QRectF(s.startPt, s.endPt).normalized().adjusted(-touchRadius, -touchRadius, touchRadius, touchRadius).contains(pt))
            return DragMoveShape;
        return DragNone;
    }

    QRectF r = s.rect.normalized();
    if (QLineF(pt, r.topLeft()).length() <= touchRadius) return DragResizeTL;
    if (QLineF(pt, r.topRight()).length() <= touchRadius) return DragResizeTR;
    if (QLineF(pt, r.bottomLeft()).length() <= touchRadius) return DragResizeBL;
    if (QLineF(pt, r.bottomRight()).length() <= touchRadius) return DragResizeBR;
    if (r.adjusted(-12 * invZoom, -12 * invZoom, 12 * invZoom, 12 * invZoom).contains(pt)) return DragMoveShape;
    return DragNone;
}

int CanvasBackend::hitTestLayerHandle(const QPointF &pt, const Layer &l) const
{
    const qreal invZoom = 1.0 / qMax(0.08, m_zoom);
    const qreal touchRadius = 30.0 * invZoom;
    QRectF r = l.rect.normalized();

    if (QLineF(pt, r.topLeft()).length() <= touchRadius) return DragLayerTL;
    if (QLineF(pt, r.topRight()).length() <= touchRadius) return DragLayerTR;
    if (QLineF(pt, r.bottomLeft()).length() <= touchRadius) return DragLayerBL;
    if (QLineF(pt, r.bottomRight()).length() <= touchRadius) return DragLayerBR;
    return DragMoveLayer;
}

void CanvasBackend::floodFillImage(QImage &img, QPoint startPt, const QColor &newColor)
{
    if (!img.rect().contains(startPt)) return;
    QRgb targetRgba = img.pixel(startPt);
    QRgb replacementRgba = newColor.rgba();
    if (targetRgba == replacementRgba) return;

    const int w = img.width();
    const int h = img.height();
    QVector<uchar> visited(w * h, 0);
    QQueue<QPoint> queue;
    queue.enqueue(startPt);
    visited[startPt.y() * w + startPt.x()] = 1;

    while (!queue.isEmpty()) {
        QPoint p = queue.dequeue();
        img.setPixel(p, replacementRgba);

        const QPoint neighbors[4] = {
            QPoint(p.x() + 1, p.y()),
            QPoint(p.x() - 1, p.y()),
            QPoint(p.x(), p.y() + 1),
            QPoint(p.x(), p.y() - 1)
        };
        for (const QPoint &n : neighbors) {
            if (n.x() >= 0 && n.x() < w && n.y() >= 0 && n.y() < h) {
                int idx = n.y() * w + n.x();
                if (!visited[idx] && img.pixel(n) == targetRgba) {
                    visited[idx] = 1;
                    queue.enqueue(n);
                }
            }
        }
    }
}

void CanvasBackend::pointerDown(qreal screenX, qreal screenY)
{
    m_lastScreenPt = QPointF(screenX, screenY);
    QPointF pt = screenToCanvas(screenX, screenY);
    m_lastCanvasPt = pt;
    m_dragMode = DragNone;
    m_isDrawingStroke = false;
    m_isInteracting = true;

    if (m_tool == "pan") {
        return;
    }

    if (m_activeLayer < 0 || m_activeLayer >= m_layers.size())
        return;

    const qreal invZoom = 1.0 / qMax(0.08, m_zoom);
    const qreal cropTouchRad = 32.0 * invZoom;

    if (m_tool == "crop") {
        QRectF cr = m_cropRect.normalized();
        m_cropRect = cr;
        if (QLineF(pt, cr.topLeft()).length() <= cropTouchRad) m_dragMode = DragCropTL;
        else if (QLineF(pt, cr.topRight()).length() <= cropTouchRad) m_dragMode = DragCropTR;
        else if (QLineF(pt, cr.bottomLeft()).length() <= cropTouchRad) m_dragMode = DragCropBL;
        else if (QLineF(pt, cr.bottomRight()).length() <= cropTouchRad) m_dragMode = DragCropBR;
        else if (cr.contains(pt)) m_dragMode = DragCropMove;
        return;
    }

    if (m_tool == "picker") {
        QImage comp = renderCompositeImage(m_canvasWidth, m_canvasHeight);
        QPoint ip = pt.toPoint();
        if (comp.rect().contains(ip)) {
            setPrimaryColor(comp.pixelColor(ip));
            setCurrentTool("brush");
            emit statusMessage("Picked color " + m_primaryColor.name());
        }
        m_isInteracting = false;
        return;
    }

    if (m_tool == "bucket") {
        auto &layer = m_layers[m_activeLayer];
        if (!layer.rect.normalized().contains(pt)) {
            ensureLayerCoversCanvas(layer);
        }
        QPoint ip = canvasToLayerImage(layer, pt).toPoint();
        if (layer.raster.rect().contains(ip)) {
            saveUndoState();
            floodFillImage(layer.raster, ip, m_primaryColor);
            update();
            emit statusMessage("Filled area on " + layer.name);
        }
        m_isInteracting = false;
        return;
    }

    if (m_tool == "select") {
        auto &layer = m_layers[m_activeLayer];
        auto &shapes = layer.shapes;

        // 1. Check currently selected shape handles
        if (m_selectedShape >= 0 && m_selectedShape < shapes.size()) {
            int hit = hitTestHandle(pt, shapes[m_selectedShape]);
            if (hit != DragNone) {
                saveUndoState();
                m_dragMode = static_cast<DragMode>(hit);
                return;
            }
        }
        // 2. Check all shapes on active layer
        for (int i = shapes.size() - 1; i >= 0; --i) {
            int hit = hitTestHandle(pt, shapes[i]);
            if (hit != DragNone) {
                saveUndoState();
                m_selectedShape = i;
                m_dragMode = static_cast<DragMode>(hit);
                emit selectionChanged();
                update();
                return;
            }
        }
        // 3. No shape hit -> Deselect shape and Move / Resize the Active Layer itself!
        if (m_selectedShape != -1) {
            m_selectedShape = -1;
            emit selectionChanged();
        }
        saveUndoState();
        layer.rect = layer.rect.normalized();
        m_dragMode = static_cast<DragMode>(hitTestLayerHandle(pt, layer));
        update();
        return;
    }

    if (m_tool == "brush" || m_tool == "eraser") {
        saveUndoState();
        m_isDrawingStroke = true;
        auto &layer = m_layers[m_activeLayer];
        if (!layer.rect.normalized().contains(pt)) {
            ensureLayerCoversCanvas(layer);
        }
        QPointF lpt = canvasToLayerImage(layer, pt);
        qreal scaleRatio = layer.raster.width() / qMax(1.0, layer.rect.normalized().width());

        QPainter p(&layer.raster);
        p.setRenderHint(QPainter::Antialiasing);
        if (m_tool == "eraser")
            p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.setPen(QPen(m_primaryColor, m_strokeWidth * scaleRatio, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPoint(lpt);
        update();
    }
}

void CanvasBackend::pointerMove(qreal screenX, qreal screenY)
{
    if (m_tool == "pan") {
        m_panX += (screenX - m_lastScreenPt.x());
        m_panY += (screenY - m_lastScreenPt.y());
        m_lastScreenPt = QPointF(screenX, screenY);
        update();
        return;
    }

    QPointF pt = screenToCanvas(screenX, screenY);
    QPointF delta = pt - m_lastCanvasPt;

    if (m_tool == "crop" && m_dragMode != DragNone) {
        switch (m_dragMode) {
        case DragCropTL: m_cropRect.setTopLeft(pt); break;
        case DragCropTR: m_cropRect.setTopRight(pt); break;
        case DragCropBL: m_cropRect.setBottomLeft(pt); break;
        case DragCropBR: m_cropRect.setBottomRight(pt); break;
        case DragCropMove: m_cropRect.translate(delta); break;
        default: break;
        }
        m_lastCanvasPt = pt;
        update();
        return;
    }

    if (m_tool == "select" && m_dragMode != DragNone && m_activeLayer >= 0 && m_activeLayer < m_layers.size()) {
        auto &layer = m_layers[m_activeLayer];

        // Moving or Resizing the Active Layer inside the Canvas Frame
        if (m_dragMode == DragMoveLayer) {
            layer.rect.translate(delta);
            for (auto &s : layer.shapes) {
                s.rect.translate(delta);
                s.startPt += delta;
                s.endPt += delta;
            }
            m_lastCanvasPt = pt;
            update();
            return;
        }
        if (m_dragMode == DragLayerTL || m_dragMode == DragLayerTR ||
            m_dragMode == DragLayerBL || m_dragMode == DragLayerBR) {
            switch (m_dragMode) {
            case DragLayerTL: layer.rect.setTopLeft(pt); break;
            case DragLayerTR: layer.rect.setTopRight(pt); break;
            case DragLayerBL: layer.rect.setBottomLeft(pt); break;
            case DragLayerBR: layer.rect.setBottomRight(pt); break;
            default: break;
            }
            m_lastCanvasPt = pt;
            update();
            return;
        }

        // Moving or Resizing a Selected Vector Shape
        if (m_selectedShape >= 0 && m_selectedShape < layer.shapes.size()) {
            auto &s = layer.shapes[m_selectedShape];
            switch (m_dragMode) {
            case DragMoveShape:
                s.rect.translate(delta);
                s.startPt += delta;
                s.endPt += delta;
                break;
            case DragResizeTL: s.rect.setTopLeft(pt); break;
            case DragResizeTR: s.rect.setTopRight(pt); break;
            case DragResizeBL: s.rect.setBottomLeft(pt); break;
            case DragResizeBR: s.rect.setBottomRight(pt); break;
            case DragArrowStart: s.startPt = pt; break;
            case DragArrowEnd: s.endPt = pt; break;
            default: break;
            }
            m_lastCanvasPt = pt;
            update();
            return;
        }
    }

    if ((m_tool == "brush" || m_tool == "eraser") && m_isDrawingStroke) {
        auto &layer = m_layers[m_activeLayer];
        QPointF lFrom = canvasToLayerImage(layer, m_lastCanvasPt);
        QPointF lTo = canvasToLayerImage(layer, pt);
        qreal scaleRatio = layer.raster.width() / qMax(1.0, layer.rect.normalized().width());

        QPainter p(&layer.raster);
        p.setRenderHint(QPainter::Antialiasing);
        if (m_tool == "eraser")
            p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.setPen(QPen(m_primaryColor, m_strokeWidth * scaleRatio, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(lFrom, lTo);
        m_lastCanvasPt = pt;
        update();
    }
}

void CanvasBackend::pointerUp(qreal screenX, qreal screenY)
{
    Q_UNUSED(screenX)
    Q_UNUSED(screenY)
    if (m_activeLayer >= 0 && m_activeLayer < m_layers.size()) {
        auto &layer = m_layers[m_activeLayer];
        layer.rect = layer.rect.normalized();
        if (m_selectedShape >= 0 && m_selectedShape < layer.shapes.size()) {
            layer.shapes[m_selectedShape].rect = layer.shapes[m_selectedShape].rect.normalized();
        }
    }
    if (m_tool == "crop") {
        m_cropRect = m_cropRect.normalized();
    }
    m_isDrawingStroke = false;
    m_isInteracting = false;
    m_dragMode = DragNone;
    update();
}
