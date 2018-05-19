import QtQuick 2.0
import QtQuick.Layouts 1.3

import replay.core.audio 1.0;
import replay.core.video 1.0;
import replay.core 1.0;

import "../../common/charts"

Item
{
	property var decoders

	clip: true

	width: decoders.length * (100 + 10) + 10

    Rectangle
    {
        id: background
        anchors.fill: parent

        color: "black"
        opacity: 0.8

        radius: 5
    }

	Text
	{
		// - Positioning
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.topMargin: 10

		font.pixelSize: 15
		font.bold: true
		text: "DECODERS"
		color: "white"
	}

	ListView
	{
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.topMargin: 35
		anchors.leftMargin: 10
		anchors.rightMargin: 10

		height: 200


		model: decoders

		orientation: ListView.Horizontal
		spacing: 5

		delegate: Rectangle
		{
			width: 105

			Text
			{
				text:
				{
					if (model.modelData instanceof VideoDecoder)
						return "Video stream";
					else if (model.modelData instanceof AudioDecoder)
						return "Audio stream";
					else
						return "Stream";
				}

				color: "white"
				font.bold: true
			}

			Text
			{
				y: 12
				x: 4

				text:
				{
					if (model.modelData.stream)
						return "Stream id " + model.modelData.stream.id;
					else
						return "No stream";
				}

				color: "white"
				font.bold: true
				font.pixelSize: 8
				opacity: 0.7
			}

			Timer
			{
                interval: 250; running: true; repeat: true
				onTriggered:
                {
					queueSize.text = model.modelData.queueTime.toFixed(1) + "s (" + (model.modelData.queueSpace / 1000000).toFixed(1) + "mb)";
					clockTime.text = model.modelData.time.toFixed(3) + "s"
					//codecName.text = model.modelData.codecName;
					//resolution.text = model.modelData.width + "x" + model.modelData.height;
					timeOffset.pushValue(model.modelData.time - model.modelData.masterClock.time);
					queueLoadChart.pushValue(model.modelData.queueSize);
				}
			}

			MiniAreaChart
			{
				id: queueLoadChart
				y: 30

				width: parent.width
				height: 25

				maxValue: model.modelData.queueLimit

				chartColor: "#51E0E2"
			}

			MiniTresholdChart
			{
				id: timeOffset

				y: 30 + 25 + 5
				width: 100
				height: 10
			}


			GridLayout
			{
				y: 30 + 25 + 10 + 15
				width: parent.width

				columns: 2

				property color color: "white"
				property int fontSize: 9

				Text
				{
					text: "Queue";
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					id: queueSize
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					text: "Clock";
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					id: clockTime
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					text: "State";
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					text: "Decoding";
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					text: "Codec";
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					id: codecName
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					text: "Resolution";
					color: parent.color; font.pixelSize: parent.fontSize
				}

				Text
				{
					id: resolution
					color: parent.color; font.pixelSize: parent.fontSize
				}

			}
		}
	}


}
