import QtQuick 2.0
import QtGraphicalEffects 1.0

import replay.fonts 1.0

Item
{
    id: button

    property bool checked: false
	property color color: "white"
	property string icon

    // - Accessibility
    Accessible.role: Accessible.Button
    Accessible.onPressAction: button.clicked()

    // - Event click
    signal clicked
	signal hovered

    Text
    {
		id: buttonIcon

        // - Position
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter

        // - Text style
		font.family: Fonts.solidIcons
		font.pixelSize: parent.height + (mouseArea.containsMouse ? 4 : 0)
		opacity: mouseArea.pressed ? 0.5 : 1
		color: parent.color

		// - Animations
		Behavior on font.pixelSize { NumberAnimation { duration: 80 } }

        // - Current time hovered
		text: button.icon
	}

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: parent.clicked()
    }
}
