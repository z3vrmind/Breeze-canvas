import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import BreezeCanvas.Backend 1.0

Window {
    id: root
    visible: true
    width: 412
    height: 892
    title: "BreezeCanvas"

    // KDE Breeze Dark & Light Palette
    property bool darkBreeze: true
    readonly property color breezeBg: darkBreeze ? "#232629" : "#eff0f1"
    readonly property color breezeSurface: darkBreeze ? "#31363b" : "#fcfcfc"
    readonly property color breezeCard: darkBreeze ? "#2a2e32" : "#e3e5e7"
    readonly property color breezeBorder: darkBreeze ? "#4d5257" : "#bdc3c7"
    readonly property color breezeText: darkBreeze ? "#eff0f1" : "#232629"
    readonly property color breezeAccent: "#3daee9"
    readonly property color breezeSuccess: "#27ae60"
    readonly property color breezeDanger: "#da4453"

    color: breezeBg

    // Vector-Icon + Text Breeze Touch Button (Zero Font/Emoji Bugs)
    component BreezeButton : Rectangle {
        id: btn
        property string text: ""
        property string iconName: ""
        property bool verticalStack: false
        property bool activeState: false
        property bool enabledBtn: true
        property color customAccent: root.breezeAccent
        signal clicked()

        readonly property color fgColor: btn.activeState ? "#ffffff" : root.breezeText

        implicitHeight: verticalStack ? 50 : 40
        implicitWidth: verticalStack ? 46 : Math.max(40, (iconName !== "" ? 24 : 0) + (text !== "" ? hLabel.implicitWidth + 14 : 14))
        radius: 6
        opacity: enabledBtn ? 1.0 : 0.38
        color: btn.activeState ? btn.customAccent
             : (hitArea.pressed ? Qt.darker(root.breezeSurface, 1.3) : root.breezeCard)
        border.width: 1
        border.color: btn.activeState ? btn.customAccent : root.breezeBorder

        // Horizontal Layout (Icon + Text or Icon-Only)
        Row {
            visible: !btn.verticalStack
            anchors.centerIn: parent
            spacing: 5

            BreezeIcon {
                visible: btn.iconName !== ""
                width: 18
                height: 18
                anchors.verticalCenter: parent.verticalCenter
                name: btn.iconName
                color: btn.fgColor
            }
            Text {
                id: hLabel
                visible: btn.text !== ""
                anchors.verticalCenter: parent.verticalCenter
                text: btn.text
                color: btn.fgColor
                font.pixelSize: 12
                font.bold: true
            }
        }

        // Vertical Dock Layout (Vector Icon on top + Label below)
        Column {
            visible: btn.verticalStack
            anchors.centerIn: parent
            spacing: 2

            BreezeIcon {
                visible: btn.iconName !== ""
                width: 20
                height: 20
                anchors.horizontalCenter: parent.horizontalCenter
                name: btn.iconName
                color: btn.fgColor
            }
            Text {
                visible: btn.text !== ""
                anchors.horizontalCenter: parent.horizontalCenter
                text: btn.text
                color: btn.fgColor
                font.pixelSize: 10
                font.bold: true
            }
        }

        MouseArea {
            id: hitArea
            anchors.fill: parent
            anchors.margins: -3
            enabled: btn.enabledBtn
            preventStealing: true
            onClicked: btn.clicked()
        }
    }

    // Fullscreen Modal Overlay
    component BreezeModal : Item {
        id: modalRoot
        property string modalTitle: ""
        default property alias bodyContent: innerCol.data
        anchors.fill: parent
        visible: false
        z: 500

        function open() { visible = true }
        function close() { visible = false }

        Rectangle {
            anchors.fill: parent
            color: "#b0000000"
            MouseArea {
                anchors.fill: parent
                preventStealing: true
                onClicked: modalRoot.close()
            }
        }

        Rectangle {
            width: Math.min(root.width - 24, 380)
            height: Math.min(root.height - 80, innerCol.implicitHeight + 76)
            anchors.centerIn: parent
            radius: 10
            color: root.breezeSurface
            border.color: root.breezeAccent
            border.width: 2

            MouseArea { anchors.fill: parent; preventStealing: true }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: modalRoot.modalTitle
                        color: root.breezeText
                        font.pixelSize: 16
                        font.bold: true
                        Layout.fillWidth: true
                    }
                    BreezeButton {
                        iconName: "close"
                        implicitWidth: 38
                        implicitHeight: 38
                        customAccent: root.breezeDanger
                        activeState: true
                        onClicked: modalRoot.close()
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: root.breezeBorder
                }

                Flickable {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentHeight: innerCol.implicitHeight
                    clip: true

                    ColumnLayout {
                        id: innerCol
                        width: parent.width
                        spacing: 10
                    }
                }
            }
        }
    }

    // =========================================================================
    // MAIN VERTICAL APP LAYOUT
    // =========================================================================
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 0. ANDROID STATUS BAR SAFE SPACER
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            color: "#181a1d"
            z: 50
        }

        // 1. TOP HEADER BAR
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 52
            color: root.breezeSurface
            z: 50

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                spacing: 4

                BreezeButton {
                    Layout.fillWidth: true
                    iconName: "new"
                    text: "New"
                    onClicked: newCanvasModal.open()
                }
                BreezeButton {
                    Layout.fillWidth: true
                    iconName: "open"
                    text: "Open"
                    onClicked: importFileDialog.open()
                }
                BreezeButton {
                    implicitWidth: 40
                    iconName: "undo"
                    enabledBtn: canvas.canUndo
                    onClicked: canvas.undo()
                }
                BreezeButton {
                    implicitWidth: 40
                    iconName: "redo"
                    enabledBtn: canvas.canRedo
                    onClicked: canvas.redo()
                }
                BreezeButton {
                    implicitWidth: 40
                    iconName: root.darkBreeze ? "sun" : "moon"
                    onClicked: root.darkBreeze = !root.darkBreeze
                }
                BreezeButton {
                    Layout.fillWidth: true
                    iconName: "adjust"
                    text: "Adjust"
                    onClicked: adjustModal.open()
                }
                BreezeButton {
                    Layout.fillWidth: true
                    iconName: "export"
                    text: "Export"
                    activeState: true
                    onClicked: exportModal.open()
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: root.breezeBorder
            }
        }

        // 2. CONTEXT ACTION BAR (Crop Controls / Shape Controls / Active Layer Move & Scale Controls)
        Rectangle {
            id: contextBar
            readonly property bool showCropBar: canvas.currentTool === "crop"
            readonly property bool showShapeBar: canvas.currentTool === "select" && canvas.selectedShapeIndex >= 0
            readonly property bool showLayerBar: canvas.currentTool === "select" && canvas.selectedShapeIndex < 0
            visible: showCropBar || showShapeBar || showLayerBar
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? 46 : 0
            color: root.breezeCard
            z: 49

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                spacing: 5

                // A. CROP MODE ACTIONS
                BreezeButton {
                    visible: contextBar.showCropBar
                    Layout.fillWidth: true
                    implicitHeight: 36
                    iconName: "check"
                    text: "Apply Crop"
                    activeState: true
                    customAccent: root.breezeSuccess
                    onClicked: canvas.applyCrop()
                }
                BreezeButton {
                    visible: contextBar.showCropBar
                    Layout.fillWidth: true
                    implicitHeight: 36
                    iconName: "close"
                    text: "Cancel"
                    activeState: true
                    customAccent: root.breezeDanger
                    onClicked: canvas.cancelCrop()
                }

                // B. SELECTED SHAPE ACTIONS
                BreezeButton {
                    visible: contextBar.showShapeBar && canvas.selectedShapeType === 8
                    Layout.fillWidth: true
                    implicitHeight: 36
                    iconName: "text"
                    text: "Text"
                    activeState: true
                    onClicked: {
                        shapeTextInput.text = canvas.selectedShapeText
                        textEditModal.open()
                    }
                }
                BreezeButton {
                    visible: contextBar.showShapeBar
                    Layout.fillWidth: true
                    implicitHeight: 36
                    iconName: "copy"
                    text: "Copy"
                    onClicked: canvas.duplicateSelectedShape()
                }
                BreezeButton {
                    visible: contextBar.showShapeBar
                    Layout.fillWidth: true
                    implicitHeight: 36
                    iconName: "bake"
                    text: "Bake"
                    onClicked: canvas.rasterizeSelectedShape()
                }
                BreezeButton {
                    visible: contextBar.showShapeBar
                    Layout.fillWidth: true
                    implicitHeight: 36
                    iconName: "shadow"
                    text: "Shadow"
                    activeState: canvas.shapeShadow
                    onClicked: canvas.shapeShadow = !canvas.shapeShadow
                }
                BreezeButton {
                    visible: contextBar.showShapeBar
                    Layout.fillWidth: true
                    implicitHeight: 36
                    iconName: "trash"
                    text: "Del"
                    activeState: true
                    customAccent: root.breezeDanger
                    onClicked: canvas.deleteSelectedShape()
                }

                // C. ACTIVE LAYER TRANSFORM ACTIONS (Shown in Move Mode)
                Text {
                    visible: contextBar.showLayerBar
                    text: canvas.activeLayerName + ":"
                    color: root.breezeText
                    font.pixelSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.maximumWidth: 90
                }
                BreezeButton {
                    visible: contextBar.showLayerBar
                    Layout.fillWidth: true
                    implicitHeight: 34
                    iconName: "move"
                    text: "Center"
                    onClicked: canvas.centerActiveLayer()
                }
                BreezeButton {
                    visible: contextBar.showLayerBar
                    implicitWidth: 38
                    implicitHeight: 34
                    iconName: "minus"
                    onClicked: canvas.scaleActiveLayer(0.88)
                }
                BreezeButton {
                    visible: contextBar.showLayerBar
                    implicitWidth: 38
                    implicitHeight: 34
                    iconName: "plus"
                    onClicked: canvas.scaleActiveLayer(1.14)
                }
                BreezeButton {
                    visible: contextBar.showLayerBar
                    Layout.fillWidth: true
                    implicitHeight: 34
                    iconName: "reset"
                    text: "Reset"
                    onClicked: canvas.resetActiveLayerTransform()
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: root.breezeAccent
            }
        }

        // 3. VIEWPORT-SIZED C++ CANVAS
        Item {
            id: viewportContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            z: 1

            CanvasBackend {
                id: canvas
                anchors.fill: parent
                onStatusMessage: (msg) => {
                    toastText.text = msg
                    toastBanner.opacity = 1.0
                    toastTimer.restart()
                }
            }

            PinchArea {
                anchors.fill: parent
                property real lastPinchScale: 1.0
                property point lastPinchCenter: Qt.point(0, 0)

                onPinchStarted: (pinch) => {
                    canvas.cancelCurrentStroke()
                    lastPinchScale = 1.0
                    lastPinchCenter = pinch.center
                }
                onPinchUpdated: (pinch) => {
                    var stepScale = pinch.scale / lastPinchScale
                    var dx = pinch.center.x - lastPinchCenter.x
                    var dy = pinch.center.y - lastPinchCenter.y
                    canvas.pinchUpdate(stepScale, pinch.center.x, pinch.center.y, dx, dy)
                    lastPinchScale = pinch.scale
                    lastPinchCenter = pinch.center
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: (mouse) => canvas.pointerDown(mouse.x, mouse.y)
                    onPositionChanged: (mouse) => canvas.pointerMove(mouse.x, mouse.y)
                    onReleased: (mouse) => canvas.pointerUp(mouse.x, mouse.y)
                }
            }

            // Zoom Controls (Bottom Right of Viewport)
            RowLayout {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10
                spacing: 4
                z: 10

                BreezeButton {
                    iconName: "minus"
                    implicitWidth: 38
                    implicitHeight: 36
                    onClicked: canvas.zoomStep(0.8)
                }
                BreezeButton {
                    iconName: "reset"
                    text: Math.round(canvas.zoomLevel * 100) + "%"
                    implicitHeight: 36
                    onClicked: canvas.fitToView()
                }
                BreezeButton {
                    iconName: "plus"
                    implicitWidth: 38
                    implicitHeight: 36
                    onClicked: canvas.zoomStep(1.25)
                }
            }

            // Toast Notification Banner
            Rectangle {
                id: toastBanner
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(viewportContainer.width - 24, toastText.implicitWidth + 28)
                height: 36
                radius: 18
                color: "#e6232629"
                border.color: root.breezeAccent
                border.width: 1
                opacity: 0.0
                z: 20
                Behavior on opacity { NumberAnimation { duration: 150 } }

                Text {
                    id: toastText
                    anchors.centerIn: parent
                    width: parent.width - 20
                    elide: Text.ElideMiddle
                    horizontalAlignment: Text.AlignHCenter
                    color: "#eff0f1"
                    font.pixelSize: 12
                }
                Timer {
                    id: toastTimer
                    interval: 2200
                    onTriggered: toastBanner.opacity = 0.0
                }
            }
        }

        // 4. COLOR & STROKE STYLE BAR
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: root.breezeSurface
            z: 50

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: root.breezeBorder
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 6

                Repeater {
                    model: ["#3daee9", "#da4453", "#27ae60", "#fdbc4b", "#9b59b6", "#ff6600", "#ffffff", "#000000"]
                    delegate: Rectangle {
                        width: 26
                        height: 26
                        radius: 13
                        color: modelData
                        border.width: canvas.primaryColor === Qt.color(modelData) ? 3 : 1
                        border.color: canvas.primaryColor === Qt.color(modelData) ? root.breezeAccent : root.breezeBorder

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -3
                            onClicked: canvas.primaryColor = modelData
                        }
                    }
                }

                Slider {
                    Layout.fillWidth: true
                    from: 2
                    to: 54
                    value: canvas.strokeWidth
                    onMoved: canvas.strokeWidth = value
                }

                BreezeButton {
                    text: canvas.fillShapes ? "Fill ON" : "Fill OFF"
                    activeState: canvas.fillShapes
                    implicitHeight: 34
                    onClicked: canvas.fillShapes = !canvas.fillShapes
                }
            }
        }

        // 5. BOTTOM TOOL DOCK (Crisp Vector Icons + Labels)
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: root.breezeSurface
            z: 50

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: root.breezeBorder
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                spacing: 3

                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "move"
                    text: "Move"
                    activeState: canvas.currentTool === "select"
                    onClicked: canvas.currentTool = "select"
                }
                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "pan"
                    text: "Pan"
                    activeState: canvas.currentTool === "pan"
                    onClicked: canvas.currentTool = "pan"
                }
                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "brush"
                    text: "Brush"
                    activeState: canvas.currentTool === "brush"
                    onClicked: canvas.currentTool = "brush"
                }
                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "eraser"
                    text: "Erase"
                    activeState: canvas.currentTool === "eraser"
                    onClicked: canvas.currentTool = "eraser"
                }
                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "bucket"
                    text: "Fill"
                    activeState: canvas.currentTool === "bucket"
                    onClicked: canvas.currentTool = "bucket"
                }
                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "shapes"
                    text: "Shapes"
                    onClicked: shapesModal.open()
                }
                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "crop"
                    text: "Crop"
                    activeState: canvas.currentTool === "crop"
                    onClicked: canvas.currentTool = "crop"
                }
                BreezeButton {
                    Layout.fillWidth: true
                    verticalStack: true
                    iconName: "layers"
                    text: "Layers(" + canvas.layerModel.length + ")"
                    onClicked: layersModal.open()
                }
            }
        }
    }

    // =========================================================================
    // MODALS (All Using Native Vector Icons)
    // =========================================================================

    BreezeModal {
        id: shapesModal
        modalTitle: "Insert Shape, Arrow or Text"

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 8
            columnSpacing: 8

            BreezeButton { Layout.fillWidth: true; iconName: "arrow"; text: "Single Arrow"; onClicked: { canvas.addShape(3); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "double-arrow"; text: "Double Arrow"; onClicked: { canvas.addShape(4); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "curved-arrow"; text: "Curved Arrow"; onClicked: { canvas.addShape(10); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "line"; text: "Straight Line"; onClicked: { canvas.addShape(5); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "rect"; text: "Rectangle"; onClicked: { canvas.addShape(0); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "round-rect"; text: "Rounded Box"; onClicked: { canvas.addShape(1); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "circle"; text: "Circle / Oval"; onClicked: { canvas.addShape(2); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "triangle"; text: "Triangle"; onClicked: { canvas.addShape(9); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "star"; text: "5-Pt Star"; onClicked: { canvas.addShape(6); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "burst"; text: "Impact Burst"; onClicked: { canvas.addShape(11); shapesModal.close() } }
            BreezeButton { Layout.fillWidth: true; iconName: "bubble"; text: "Speech Bubble"; onClicked: { canvas.addShape(7); shapesModal.close() } }
            BreezeButton {
                Layout.fillWidth: true
                iconName: "text"
                text: "Thumbnail Text"
                activeState: true
                onClicked: { canvas.addShape(8, "EPIC!"); shapesModal.close() }
            }
        }

        BreezeButton {
            Layout.fillWidth: true
            iconName: "picker"
            text: "Eyedropper (Pick Color from Canvas)"
            onClicked: {
                canvas.currentTool = "picker"
                shapesModal.close()
            }
        }
    }

    BreezeModal {
        id: layersModal
        modalTitle: "Layers Manager"

        RowLayout {
            Layout.fillWidth: true
            spacing: 5
            BreezeButton { Layout.fillWidth: true; iconName: "plus"; text: "New"; activeState: true; onClicked: canvas.addLayer() }
            BreezeButton { Layout.fillWidth: true; iconName: "copy"; text: "Copy"; onClicked: canvas.duplicateActiveLayer() }
            BreezeButton { Layout.fillWidth: true; iconName: "bake"; text: "Merge"; onClicked: canvas.mergeLayerDown(canvas.activeLayerIndex) }
            BreezeButton { Layout.fillWidth: true; iconName: "eraser"; text: "Clear"; onClicked: canvas.clearActiveLayer() }
            BreezeButton { Layout.fillWidth: true; iconName: "trash"; text: "Del"; customAccent: root.breezeDanger; activeState: true; onClicked: canvas.deleteActiveLayer() }
        }

        Repeater {
            model: canvas.layerModel
            delegate: Rectangle {
                Layout.fillWidth: true
                height: 68
                radius: 8
                color: modelData.index === canvas.activeLayerIndex ? root.breezeCard : root.breezeBg
                border.width: modelData.index === canvas.activeLayerIndex ? 2 : 1
                border.color: modelData.index === canvas.activeLayerIndex ? root.breezeAccent : root.breezeBorder

                MouseArea {
                    anchors.fill: parent
                    onClicked: canvas.activeLayerIndex = modelData.index
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    BreezeButton {
                        iconName: modelData.visible ? "eye" : "eye-off"
                        implicitWidth: 38
                        activeState: modelData.visible
                        onClicked: canvas.setLayerVisibility(modelData.index, !modelData.visible)
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            text: modelData.name + " (" + modelData.shapeCount + " shapes)"
                            color: root.breezeText
                            font.bold: true
                            font.pixelSize: 13
                        }
                        Slider {
                            Layout.fillWidth: true
                            from: 0.0
                            to: 1.0
                            value: modelData.opacity
                            onMoved: canvas.setLayerOpacity(modelData.index, value)
                        }
                    }

                    BreezeButton { iconName: "up"; implicitWidth: 36; onClicked: canvas.moveLayer(modelData.index, modelData.index + 1) }
                    BreezeButton { iconName: "down"; implicitWidth: 36; onClicked: canvas.moveLayer(modelData.index, modelData.index - 1) }
                }
            }
        }
    }

    BreezeModal {
        id: adjustModal
        modalTitle: "Adjustments & Filters"

        Text { text: "Brightness: " + Math.round(brightSlider.value); color: root.breezeText; font.bold: true }
        Slider { id: brightSlider; Layout.fillWidth: true; from: -100; to: 100; value: 0 }

        Text { text: "Contrast: " + Math.round(contrastSlider.value); color: root.breezeText; font.bold: true }
        Slider { id: contrastSlider; Layout.fillWidth: true; from: -100; to: 100; value: 0 }

        Text { text: "Saturation: " + Math.round(satSlider.value); color: root.breezeText; font.bold: true }
        Slider { id: satSlider; Layout.fillWidth: true; from: -100; to: 100; value: 0 }

        BreezeButton {
            Layout.fillWidth: true
            iconName: "check"
            text: "Apply Sliders to Layer"
            activeState: true
            onClicked: {
                canvas.adjustImage(Math.round(brightSlider.value), Math.round(contrastSlider.value), Math.round(satSlider.value))
                brightSlider.value = 0
                contrastSlider.value = 0
                satSlider.value = 0
                adjustModal.close()
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 8
            columnSpacing: 8

            BreezeButton { Layout.fillWidth: true; text: "Flip Horiz"; onClicked: { canvas.flipLayer(true); adjustModal.close() } }
            BreezeButton { Layout.fillWidth: true; text: "Flip Vert"; onClicked: { canvas.flipLayer(false); adjustModal.close() } }
            BreezeButton { Layout.fillWidth: true; text: "Rotate 90 deg"; onClicked: { canvas.rotateCanvas90(); adjustModal.close() } }
            BreezeButton { Layout.fillWidth: true; text: "Vignette"; onClicked: { canvas.applyFilter("vignette"); adjustModal.close() } }
            BreezeButton { Layout.fillWidth: true; text: "Grayscale"; onClicked: { canvas.applyFilter("grayscale"); adjustModal.close() } }
            BreezeButton { Layout.fillWidth: true; text: "Invert Colors"; onClicked: { canvas.applyFilter("invert"); adjustModal.close() } }
        }
    }

    BreezeModal {
        id: newCanvasModal
        modalTitle: "New Canvas Preset"

        BreezeButton {
            Layout.fillWidth: true
            text: "YouTube Thumbnail HD (1280 x 720)"
            activeState: true
            onClicked: { canvas.createNewCanvas(1280, 720); newCanvasModal.close() }
        }
        BreezeButton {
            Layout.fillWidth: true
            text: "Full HD Landscape (1920 x 1080)"
            onClicked: { canvas.createNewCanvas(1920, 1080); newCanvasModal.close() }
        }
        BreezeButton {
            Layout.fillWidth: true
            text: "Shorts / Reels Vertical (1080 x 1920)"
            onClicked: { canvas.createNewCanvas(1080, 1920); newCanvasModal.close() }
        }
        BreezeButton {
            Layout.fillWidth: true
            text: "Square Post / Icon (1080 x 1080)"
            onClicked: { canvas.createNewCanvas(1080, 1080); newCanvasModal.close() }
        }
    }

    BreezeModal {
        id: exportModal
        modalTitle: "Export & Save"

        property string selectedFormat: "PNG"
        property int exportW: canvas.canvasWidth
        property int exportH: canvas.canvasHeight

        onVisibleChanged: {
            if (visible) {
                exportW = canvas.canvasWidth
                exportH = canvas.canvasHeight
            }
        }

        Text { text: "Format"; color: root.breezeText; font.bold: true }
        RowLayout {
            Layout.fillWidth: true
            Repeater {
                model: ["PNG", "JPG", "WEBP"]
                BreezeButton {
                    Layout.fillWidth: true
                    text: modelData
                    activeState: exportModal.selectedFormat === modelData
                    onClicked: exportModal.selectedFormat = modelData
                }
            }
        }

        Text { text: "Resolution Preset"; color: root.breezeText; font.bold: true }
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 6
            columnSpacing: 6

            BreezeButton {
                Layout.fillWidth: true
                text: "Original (" + canvas.canvasWidth + "x" + canvas.canvasHeight + ")"
                activeState: exportModal.exportW === canvas.canvasWidth && exportModal.exportH === canvas.canvasHeight
                onClicked: { exportModal.exportW = canvas.canvasWidth; exportModal.exportH = canvas.canvasHeight }
            }
            BreezeButton {
                Layout.fillWidth: true
                text: "720p (1280x720)"
                activeState: exportModal.exportW === 1280 && exportModal.exportH === 720
                onClicked: { exportModal.exportW = 1280; exportModal.exportH = 720 }
            }
            BreezeButton {
                Layout.fillWidth: true
                text: "1080p (1920x1080)"
                activeState: exportModal.exportW === 1920 && exportModal.exportH === 1080
                onClicked: { exportModal.exportW = 1920; exportModal.exportH = 1080 }
            }
            BreezeButton {
                Layout.fillWidth: true
                text: "Vertical (1080x1920)"
                activeState: exportModal.exportW === 1080 && exportModal.exportH === 1920
                onClicked: { exportModal.exportW = 1080; exportModal.exportH = 1920 }
            }
        }

        Text { text: "Quality: " + Math.round(qualitySlider.value) + "%"; color: root.breezeText; font.bold: true }
        Slider { id: qualitySlider; Layout.fillWidth: true; from: 10; to: 100; value: 95 }

        BreezeButton {
            Layout.fillWidth: true
            implicitHeight: 44
            iconName: "export"
            text: "Quick Save to Phone (Download/Pictures)"
            activeState: true
            customAccent: root.breezeSuccess
            onClicked: {
                canvas.quickExportToDevice(exportModal.selectedFormat, Math.round(qualitySlider.value), exportModal.exportW, exportModal.exportH)
                exportModal.close()
            }
        }

        BreezeButton {
            Layout.fillWidth: true
            iconName: "open"
            text: "Pick Custom Save Location..."
            activeState: true
            onClicked: { exportModal.close(); saveImageFileDialog.open() }
        }

        RowLayout {
            Layout.fillWidth: true
            BreezeButton { Layout.fillWidth: true; iconName: "new"; text: "Save Project"; onClicked: { exportModal.close(); saveProjectDialog.open() } }
            BreezeButton { Layout.fillWidth: true; iconName: "open"; text: "Load Project"; onClicked: { exportModal.close(); loadProjectDialog.open() } }
        }
    }

    BreezeModal {
        id: textEditModal
        modalTitle: "Edit Thumbnail Text"

        TextField {
            id: shapeTextInput
            Layout.fillWidth: true
            placeholderText: "Enter text..."
            color: root.breezeText
            font.pixelSize: 16
            background: Rectangle { color: root.breezeBg; radius: 6; border.color: root.breezeAccent; border.width: 1 }
        }

        Text { text: "Tilt / Rotation: " + Math.round(rotSlider.value) + " deg"; color: root.breezeText; font.bold: true }
        Slider {
            id: rotSlider
            Layout.fillWidth: true
            from: -45
            to: 45
            value: canvas.selectedShapeRotation
            onMoved: canvas.selectedShapeRotation = value
        }

        BreezeButton {
            Layout.fillWidth: true
            iconName: "check"
            text: "Update Text"
            activeState: true
            onClicked: {
                canvas.updateSelectedShapeText(shapeTextInput.text)
                textEditModal.close()
            }
        }
    }

    FileDialog {
        id: importFileDialog
        title: "Select Photo"
        fileMode: FileDialog.OpenFile
        nameFilters: ["Images (*.png *.jpg *.jpeg *.webp *.bmp)"]
        onAccepted: canvas.importImage(selectedFile)
    }
    FileDialog {
        id: saveImageFileDialog
        title: "Save Exported Image"
        fileMode: FileDialog.SaveFile
        onAccepted: canvas.exportImage(selectedFile, exportModal.selectedFormat, Math.round(qualitySlider.value), exportModal.exportW, exportModal.exportH)
    }
    FileDialog {
        id: saveProjectDialog
        title: "Save Project"
        fileMode: FileDialog.SaveFile
        onAccepted: canvas.saveProject(selectedFile)
    }
    FileDialog {
        id: loadProjectDialog
        title: "Load Project"
        fileMode: FileDialog.OpenFile
        onAccepted: canvas.loadProject(selectedFile)
    }
}
