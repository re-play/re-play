import QtQuick 2.0
import QtCharts 2.2

Rectangle
{
	property color chartColor: "#51E0E2"
	property int historySize: 60
	property int maxValue: 60

	// ---------------------------------------------------------
	// - Insert data into the chart sliding the older data
	// ---------------------------------------------------------
	function pushValue(value)
    {
        var newX = areaSeries.upperSeries.at(areaSeries.upperSeries.count - 1).x + 1;
        var minX = areaSeries.upperSeries.at(0).x;
        valueAxisX.max = newX;
        valueAxisX.min = minX;

		// - Add the new value
        areaSeries.upperSeries.append(newX , value);

		// - If chart is full remove last value
		if (areaSeries.upperSeries.count > historySize)
			areaSeries.upperSeries.remove(0);
	}

	// - Container style
	color: "transparent"
	clip: true;

	ChartView
	{
		// - Positioning and size
		width: parent.width + 60
		height: parent.height + 60
		x: -30; y: -30

		// - Style
		antialiasing: true
		backgroundColor: "transparent"
		legend.visible: false

		// - Set chart axis
        ValueAxis { id: valueAxisX; visible: false }
		ValueAxis { id: valueAxisY; max: maxValue; visible: false }

		AreaSeries
		{
			id: areaSeries

			// - Serie axis
			axisX: valueAxisX
			axisY: valueAxisY

			// - Serie colors
			borderColor: Qt.lighter(chartColor, 1.5)
			color: chartColor

			// - Serie data
			upperSeries: LineSeries {}
		}
	}
}
