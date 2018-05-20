import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3

import replay.core.audio 1.0;
import replay.core.video 1.0;
import replay.core 1.0;

import "ui/player/diagnostic"
import "ui/player"


Item
{
	anchors.fill: parent


	DropArea {
			id: drop
			anchors.fill: parent

			enabled: true

			onEntered:
				console.log("entered");

			onExited:
				console.log("exited")

			onDropped: function(event)
			{
				// remove prefixed "file:///"
				var path= event.urls[0].replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");

				player.loadVideo(path);
			}
		}

	StandardPlayer
	{
		id: player

		anchors.fill: parent
	}
}
