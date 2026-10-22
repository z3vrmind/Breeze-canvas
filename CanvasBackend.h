#pragma once

#include <QQuickPaintedItem>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QVariantList>
#include <QVariantMap>
#include <QUrl>
#include <QVector>

// Native C++ Vector Icon Renderer (Zero Font/Emoji Dependencies)
class BreezeIcon : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit BreezeIcon(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;

    QString name() const { return m_name; }
    QColor color() const { return m_color; }
    void setName(const QString &n);
    void setColor(const QColor &c);

signals:
    void nameChanged();
    void colorChanged();

private:
    QString m_name = "move";
    QColor m_color = QColor("#eff0f1");
};

struct EditableShape {
    enum Type {
        RectShape = 0,
        RoundedRectShape = 1,
        EllipseShape = 2,
        ArrowShape = 3,
        DoubleArrowShape = 4,
        LineShape = 5,
        StarShape = 6,
        BubbleShape = 7,
        TextShape = 8,
        TriangleShape = 9,
        CurvedArrowShape = 10,
        BurstShape = 11
    };

    Type type = RectShape;
    QRectF rect = QRectF(100, 100, 320, 200);
    QPointF startPt = QPointF(100, 200);
    QPointF endPt = QPointF(420, 120);
    QColor strokeColor = QColor("#3daee9");
    QColor fillColor = QColor(61, 174, 233, 110);
    qreal strokeWidth = 8.0;
    qreal rotation = 0.0;
    bool filled = false;
    bool shadow = true;
    QString text = "THUMBNAIL";
};

struct Layer {
    QString name;
    bool visible = true;
    qreal opacity = 1.0;
    QRectF rect; // Independent position & size of this layer inside the canvas frame
    QImage raster;
    QVector<EditableShape> shapes;
};

class CanvasBackend : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int canvasWidth READ canvasWidth NOTIFY canvasSizeChanged)
    Q_PROPERTY(int canvasHeight READ canvasHeight NOTIFY canvasSizeChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel NOTIFY viewTransformChanged)
    Q_PROPERTY(int activeLayerIndex READ activeLayerIndex WRITE setActiveLayerIndex NOTIFY layersChanged)
    Q_PROPERTY(QString activeLayerName READ activeLayerName NOTIFY layersChanged)
    Q_PROPERTY(int selectedShapeIndex READ selectedShapeIndex NOTIFY selectionChanged)
    Q_PROPERTY(int selectedShapeType READ selectedShapeType NOTIFY selectionChanged)
    Q_PROPERTY(QString selectedShapeText READ selectedShapeText NOTIFY selectionChanged)
    Q_PROPERTY(qreal selectedShapeRotation READ selectedShapeRotation WRITE setSelectedShapeRotation NOTIFY selectionChanged)
    Q_PROPERTY(QString currentTool READ currentTool WRITE setCurrentTool NOTIFY toolChanged)
    Q_PROPERTY(QColor primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY styleChanged)
    Q_PROPERTY(qreal strokeWidth READ strokeWidth WRITE setStrokeWidth NOTIFY styleChanged)
    Q_PROPERTY(bool fillShapes READ fillShapes WRITE setFillShapes NOTIFY styleChanged)
    Q_PROPERTY(bool shapeShadow READ shapeShadow WRITE setShapeShadow NOTIFY styleChanged)
    Q_PROPERTY(QVariantList layerModel READ layerModel NOTIFY layersChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY undoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY undoChanged)

public:
    explicit CanvasBackend(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

    int canvasWidth() const { return m_canvasWidth; }
    int canvasHeight() const { return m_canvasHeight; }
    qreal zoomLevel() const { return m_zoom; }
    int activeLayerIndex() const { return m_activeLayer; }
    QString activeLayerName() const;
    int selectedShapeIndex() const { return m_selectedShape; }
    int selectedShapeType() const;
    QString selectedShapeText() const;
    qreal selectedShapeRotation() const;
    QString currentTool() const { return m_tool; }
    QColor primaryColor() const { return m_primaryColor; }
    qreal strokeWidth() const { return m_strokeWidth; }
    bool fillShapes() const { return m_fillShapes; }
    bool shapeShadow() const { return m_shapeShadow; }
    QVariantList layerModel() const;
    bool canUndo() const { return !m_undoStack.isEmpty(); }
    bool canRedo() const { return !m_redoStack.isEmpty(); }

    void setActiveLayerIndex(int idx);
    void setCurrentTool(const QString &t);
    void setPrimaryColor(const QColor &c);
    void setStrokeWidth(qreal w);
    void setFillShapes(bool f);
    void setShapeShadow(bool s);
    void setSelectedShapeRotation(qreal rot);

    // Viewport Navigation
    Q_INVOKABLE void fitToView();
    Q_INVOKABLE void zoomStep(qreal factor);
    Q_INVOKABLE void pinchUpdate(qreal scaleFactor, qreal centerX, qreal centerY, qreal panDx, qreal panDy);

    // Canvas & File Operations
    Q_INVOKABLE void createNewCanvas(int w, int h, const QColor &bgColor = QColor("#1e2227"));
    Q_INVOKABLE void importImage(const QUrl &fileUrl);
    Q_INVOKABLE bool exportImage(const QUrl &fileUrl, const QString &format, int quality, int targetW, int targetH);
    Q_INVOKABLE QString quickExportToDevice(const QString &format, int quality, int targetW, int targetH);
    Q_INVOKABLE bool saveProject(const QUrl &fileUrl);
    Q_INVOKABLE bool loadProject(const QUrl &fileUrl);

    // Layer Operations (Including Per-Layer Position & Scaling Inside Frame)
    Q_INVOKABLE void addLayer(const QString &name = "");
    Q_INVOKABLE void duplicateActiveLayer();
    Q_INVOKABLE void clearActiveLayer();
    Q_INVOKABLE void deleteActiveLayer();
    Q_INVOKABLE void moveLayer(int fromIdx, int toIdx);
    Q_INVOKABLE void setLayerVisibility(int idx, bool visible);
    Q_INVOKABLE void setLayerOpacity(int idx, qreal opacity);
    Q_INVOKABLE void mergeLayerDown(int idx);
    Q_INVOKABLE void centerActiveLayer();
    Q_INVOKABLE void scaleActiveLayer(qreal factor);
    Q_INVOKABLE void resetActiveLayerTransform();

    // Shape & Text Operations
    Q_INVOKABLE void addShape(int shapeType, const QString &customText = "");
    Q_INVOKABLE void duplicateSelectedShape();
    Q_INVOKABLE void deleteSelectedShape();
    Q_INVOKABLE void rasterizeSelectedShape();
    Q_INVOKABLE void updateSelectedShapeText(const QString &newText);

    // Filters, Crop & History
    Q_INVOKABLE void applyCrop();
    Q_INVOKABLE void cancelCrop();
    Q_INVOKABLE void adjustImage(int brightness, int contrast, int saturation);
    Q_INVOKABLE void applyFilter(const QString &filterName);
    Q_INVOKABLE void flipLayer(bool horizontal);
    Q_INVOKABLE void rotateCanvas90();
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void cancelCurrentStroke();

    // Screen-Space Touch Input
    Q_INVOKABLE void pointerDown(qreal screenX, qreal screenY);
    Q_INVOKABLE void pointerMove(qreal screenX, qreal screenY);
    Q_INVOKABLE void pointerUp(qreal screenX, qreal screenY);

protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:
    void canvasSizeChanged();
    void viewTransformChanged();
    void layersChanged();
    void selectionChanged();
    void toolChanged();
    void styleChanged();
    void undoChanged();
    void statusMessage(const QString &msg);

private:
    enum DragMode {
        DragNone = 0,
        DragMoveShape,
        DragResizeTL,
        DragResizeTR,
        DragResizeBL,
        DragResizeBR,
        DragArrowStart,
        DragArrowEnd,
        DragMoveLayer,
        DragLayerTL,
        DragLayerTR,
        DragLayerBL,
        DragLayerBR,
        DragCropTL,
        DragCropTR,
        DragCropBL,
        DragCropBR,
        DragCropMove
    };

    QPointF screenToCanvas(qreal sx, qreal sy) const;
    QPointF canvasToLayerImage(const Layer &l, const QPointF &canvasPt) const;
    void ensureLayerCoversCanvas(Layer &l);
    void saveUndoState();
    QImage renderCompositeImage(int targetW, int targetH);
    void drawShapeObject(QPainter *p, const EditableShape &s, bool drawHandles);
    void drawArrowHead(QPainter *p, const QPointF &from, const QPointF &to, qreal size, const QColor &col);
    int hitTestHandle(const QPointF &pt, const EditableShape &s) const;
    int hitTestLayerHandle(const QPointF &pt, const Layer &l) const;
    void floodFillImage(QImage &img, QPoint startPt, const QColor &newColor);

    int m_canvasWidth = 1280;
    int m_canvasHeight = 720;
    qreal m_zoom = 0.28;
    qreal m_panX = 20.0;
    qreal m_panY = 40.0;
    bool m_initialFitDone = false;
    bool m_isInteracting = false;

    int m_activeLayer = 0;
    int m_selectedShape = -1;
    QString m_tool = "select";
    QColor m_primaryColor = QColor("#3daee9");
    qreal m_strokeWidth = 10.0;
    bool m_fillShapes = false;
    bool m_shapeShadow = true;

    QPixmap m_checkerTile;
    QVector<Layer> m_layers;
    QVector<QVector<Layer>> m_undoStack;
    QVector<QVector<Layer>> m_redoStack;

    QPointF m_lastCanvasPt;
    QPointF m_lastScreenPt;
    bool m_isDrawingStroke = false;
    DragMode m_dragMode = DragNone;
    QRectF m_cropRect = QRectF(128, 72, 1024, 576);
};
