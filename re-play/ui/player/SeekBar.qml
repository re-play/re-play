import QtQuick 2.0
import QtQuick.Layouts 1.3

import replay.fonts 1.0

Item
{
	id: seekBar

	// - Video length informations
	property double currentTime: 0
	property double videoLenght: 0
	property double hoverTime: Math.max(Math.min(mouse.mouseX / dragBar.width, 1), 0) * videoLenght

	signal seekRequest(double t)

	height: 8

	RowLayout
	{
		anchors.leftMargin: 10
		anchors.rightMargin: 10
		anchors.fill: parent
		spacing: 6



		// ---------------------------------------------------------------
		// - Current time string
		// ---------------------------------------------------------------
		Text
		{
			// - Position & size
			anchors.verticalCenter: parent.verticalCenter
			height: 7; width: 30

			// - Text and style
			text: new Date(0, 0, 0, 0, 0, currentTime).toTimeString("hh:mm:ss")
			color: "white"
		}

		Rectangle
		{
			id: dragBar

			// - Position
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			Layout.fillWidth: true

			// - Container style
			color: "#777777"
			radius: 5

			// - More thicccc when mouse over
			height: mouse.containsMouse || mouse.pressed ? 8 : 5
			Behavior on height { NumberAnimation { duration: 200 } }


			// ---------------------------------------------------------------
			// - Bar current time
			// ---------------------------------------------------------------
			Rectangle
			{
				// - Style
				height: parent.height
				radius: parent.radius
				color: "#81F0E5"

				// - Position
				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter

				// - Current bar value
				width: parent.width * ((mouse.pressed ? hoverTime : currentTime) / videoLenght)
			}



			// ---------------------------------------------------------------
			// - Time hover marker
			// ---------------------------------------------------------------
			Item
			{
				// - Position & size
				anchors.bottom: parent.top
				width: 80; height: 25

				// - Marker visibility
				opacity: mouse.containsMouse || mouse.pressed ? 1 : 0
				Behavior on opacity { NumberAnimation { duration: 200 } }

				// - Current marker position
				x: -(width / 2) + ((hoverTime / videoLenght) * parent.width)
				Behavior on x { SmoothedAnimation { velocity: 700 } }


				// ---------------------------------------------------------------
				// - Marker time string
				// ---------------------------------------------------------------
				Text
				{
					// - Position
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.bottom: parent.bottom
					anchors.bottomMargin: -5

					// - Text style
					font.family: Fonts.solidIcons
					font.pointSize: 15
					color: "white"

					// - Current time hovered
					text: Icons.faCaretDown
				}

				// ---------------------------------------------------------------
				// - Down arrow in the marker
				// ---------------------------------------------------------------
				Text
				{
					// - Position
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.top: parent.top

					// - Font style
					color: "white"

					// - Current time hovered
					text: new Date(0, 0, 0, 0, 0, hoverTime).toTimeString("hh:mm:ss")
				}
			}



			// ---------------------------------------------------------------
			// - Mouse management area
			// ---------------------------------------------------------------
			MouseArea
			{
				id: mouse

				// - Position
				anchors.fill: parent

				// - Enables hover check
				hoverEnabled : true

				onPositionChanged:
				{
					if (pressed)
					{
						// - If mouse is pressed and we are moving cursor -> update time
						seekRequest(Math.min(Math.max(mouseX / parent.width, 0), 1) * videoLenght);
					}
				}

				onPressed:
				{
					// - Update time instantanely on mouse press on the bar
					seekRequest(Math.min(Math.max(mouseX / parent.width, 0), 1) * videoLenght);
				}
			}

		}



		// ---------------------------------------------------------------
		// - Total time string
		// ---------------------------------------------------------------
		Text
		{
			// - Position & size
			anchors.verticalCenter: parent.verticalCenter
			height: 7

			// - Text and style
			text: new Date(0, 0, 0, 0, 0, videoLenght).toTimeString("hh:mm:ss")
			color: "white"
		}

	}
}
