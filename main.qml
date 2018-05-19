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

				videoReader.unloadVideo();
				videoDecoder.clearQueue();
				audioDecoder.clearQueue();
				videoReader.loadVideo(decodeURIComponent(path));
				videoDecoder.load(videoReader.videoStreams[0]);
                audioDecoder.load(videoReader.audioStreams[0]);
                speakers.openAudioDevice(audioDecoder.getSampleRate(),audioDecoder.getChannelCount(), audioDecoder.getBitDepth(), audioDecoder.bufferSize);
			}
		}

	VideoReader
	{
		id: videoReader
	}

	VideoDecoder
	{
		id: videoDecoder
		queueLimit: 60
        Component.onCompleted:
        {
            newFrame.connect(canvas.setFrame);
        }
	}

	AudioDecoder
	{
		id: audioDecoder
		queueLimit: 60
	}

	Speaker
	{
		id: speakers

        Component.onCompleted:
        {
            frameRequest.connect(audioDecoder.processData);
        }
	}

	Component.onCompleted:
	{
		videoReader.registerDecoder(videoDecoder);
        videoReader.registerDecoder(audioDecoder);
        audioDecoder.masterClock = videoDecoder;
	}

	VideoCanvas
	{
		id: canvas
		anchors.fill: parent
	}

	VideoBar
	{
		id: videoBar

		masterClock: videoDecoder.masterClock
		videoDuration: videoReader.length / 1000000

		paused: videoDecoder.paused
		onPause:
		{
			if (videoDecoder.paused)
				videoDecoder.start();
			else
				videoDecoder.pause();
		}

		onTimeSeek: function(time)
		{
			videoReader.seek(time)
		}
	}

	DebugInformations
	{
		id: debugInfo
		x: 20
		y: 20

		height: 200

		decoders: videoReader.decoders
	}
}
