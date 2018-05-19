import QtQuick 2.0

Item
{
	property int blockSize: 4
	property int spacing: 2
	property int blocks: width / (blockSize + spacing)
	property double treshold: 0.01
	property variant values: []

	// ---------------------------------------------------------
	// - Insert data into the chart sliding the older data
	// ---------------------------------------------------------
	function pushValue(value)
	{
		values.push(value);

		if (values.length > blocks)
			values.splice(0, 1);

		canvas.requestPaint();
	}

	Canvas
	{
		id: canvas

		anchors.fill: parent

		onPaint:
		{
			var ctx = getContext("2d");

			ctx.reset();

			for (var i = 0; i < values.length; i++)
			{
				if (values[i] < 0)
				{
					ctx.fillStyle = Qt.rgba(1, 0, 0, 1);
					ctx.fillRect((blocks - i) * (blockSize + spacing), height / 2, blockSize, height / 2);
				}
				else if (values[i] > -treshold && values[i] < treshold)
				{
					ctx.fillStyle = Qt.rgba(0.8, 0.8, 0.8, 1);
					ctx.fillRect((blocks - i) * (blockSize + spacing), height / 2 - 1, 4, 2);
				}
				else
				{
					ctx.fillStyle = Qt.rgba(0, 1, 0, 1);
					ctx.fillRect((blocks - i) * (blockSize + spacing), 0, blockSize, height / 2);
				}
			}

		}
	}
}
