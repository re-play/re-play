import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import replay.fonts 1.0

import "../common"

Item
{
	anchors.bottom: parent.bottom
	width: parent.width
	height: 100

	property double videoDuration: 0
	property bool paused: false
	property var masterClock: null

	signal timeSeek(double t)
	signal pause

	LinearGradient
	{
		anchors.fill: parent
		cached: true

		start: Qt.point(0, 0)
		end: Qt.point(0, 100)

		gradient: Gradient
		{
			GradientStop { position: 0.0; color: "transparent" }
			GradientStop { position: 1.0; color: "black" }
		}
	}

	SeekBar
	{
		// - Positioning & size
		anchors.topMargin: 50
		anchors.top: parent.top
		width: parent.width

		currentTime: masterClock.time
		videoLenght: videoDuration

		onSeekRequest: function(time)
		{
			timeSeek(time)
		}
	}

	RowLayout
	{
		// - Positioning & size
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottomMargin: 8
		anchors.leftMargin: 20
		anchors.rightMargin: 10

		height: 25

		RowLayout
		{
			anchors.left: parent.left
			height: parent.height

			spacing: 10

			FaButton
			{
				icon: paused ? Icons.faPlay : Icons.faPause

				anchors.verticalCenter: parent.verticalCenter

				height: 16
				width: 16

				onClicked:
				{
					pause();
				}
			}

			FaButton
			{
				icon: Icons.faStepForward

				anchors.verticalCenter: parent.verticalCenter

				height: 15
				width: 15
			}
		}

	}


}
