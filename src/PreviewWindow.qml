import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window

Window {
    id: previewWindow
    objectName: "previewWindow"
    width: 1280
    height: 820
    minimumWidth: 720
    minimumHeight: 520
    visible: false
    title: "Preview — " + backend.fileName + " - Omawrite"
    color: backend.themeBackground

    Material.theme: backend.darkMode ? Material.Dark : Material.Light
    Material.accent: backend.themeAccent

    readonly property real textScale: backend.textScale
    readonly property int editorFontPixelSize: Math.max(1, Math.round(20 * textScale))
    readonly property int previewWidth: Math.min(
        Math.round(previewFontMetrics.averageCharacterWidth * 65),
        Math.max(360, width - Math.round(previewFontMetrics.averageCharacterWidth * 20)))
    readonly property color textColor: backend.themeForeground
    readonly property color mutedColor: backend.darkMode ? "#909191" : "#aeb1b5"
    readonly property color selectionFill: backend.themeSelection

    function scaledSize(pixels) {
        return Math.max(1, Math.round(pixels * previewWindow.textScale));
    }

    function showPreview() {
        visible = true;
        raise();
    }

    function hidePreview() {
        visible = false;
    }

    onClosing: function(close) {
        close.accepted = false;
        visible = false;
    }

    FontMetrics {
        id: previewFontMetrics
        font.family: "iA Writer Mono S"
        font.pixelSize: previewWindow.editorFontPixelSize
    }

    Flickable {
        id: previewFlick
        anchors.fill: parent
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        clip: true
        contentWidth: width
        contentHeight: Math.max(height, preview.y + preview.implicitHeight + 220)
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            bottomPadding: previewWindow.scaledSize(32)
            bottomInset: previewWindow.scaledSize(32)
        }

        function clampContentY(y) {
            return Math.max(0, Math.min(Math.max(0, contentHeight - height), y));
        }

        function snapToPixel(y) {
            return Math.round(y * Screen.devicePixelRatio) / Screen.devicePixelRatio;
        }

        function scrollTo(y) {
            contentY = snapToPixel(y);
        }

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: function(wheel) {
                if (wheel.pixelDelta.y !== 0)
                    previewFlick.scrollTo(previewFlick.clampContentY(
                        previewFlick.contentY - wheel.pixelDelta.y));
                else {
                    var notches = wheel.angleDelta.y / 120;
                    if (notches !== 0)
                        previewFlick.scrollTo(previewFlick.clampContentY(
                            previewFlick.contentY - notches * previewWindow.scaledSize(120)));
                }
                wheel.accepted = true;
            }
        }

        TextEdit {
            id: preview
            objectName: "renderedPreview"
            x: Math.round((previewFlick.width - width) / 2)
            y: Math.max(42, Math.round(previewWindow.height * 0.05))
            width: previewWindow.previewWidth
            height: Math.max(previewFlick.height - y - 96, implicitHeight + 20)
            readOnly: true
            selectByMouse: true
            wrapMode: TextEdit.Wrap
            color: previewWindow.textColor
            selectedTextColor: previewWindow.textColor
            selectionColor: previewWindow.selectionFill
            font.family: "iA Writer Mono S"
            font.pixelSize: previewWindow.editorFontPixelSize
            font.weight: Font.Normal
            renderType: Screen.devicePixelRatio % 1 === 0
                ? TextEdit.NativeRendering : TextEdit.QtRendering
            onWidthChanged: backend.setPreviewWidth(width)
            onLinkActivated: function(link) { backend.openExternalUrl(link) }
            Component.onCompleted: {
                backend.attachPreviewDocument(textDocument);
                backend.setPreviewWidth(width);
            }
        }
    }

    Label {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 12
        anchors.bottomMargin: 10
        text: "Preview"
        color: previewWindow.mutedColor
        opacity: 0.55
        font.family: "iA Writer Mono S"
        font.pixelSize: previewWindow.scaledSize(11)
    }
}
