import QtQuick 2.0

import replay.core.timing 1.0;
import replay.core.audio 1.0;
import replay.core.video 1.0;
import replay.core 1.0;

import "diagnostic"

Item
{
	property VideoStream videoStream
	property VideoStream audioStream
	property VideoStream subsStream

	property Clock masterClock


	function loadVideo(path)
	{
		videoReader.unloadVideo();

		videoDecoder.clearQueue();
		audioDecoder.clearQueue();

		videoReader.loadVideo(decodeURIComponent(path));

		videoStream = videoReader.videoStreams[0];
		audioStream = videoReader.audioStreams[0];

		masterClock = videoDecoder;


		speakers.openAudioDevice(audioDecoder.getSampleRate(),audioDecoder.getChannelCount(), audioDecoder.getBitDepth(), audioDecoder.bufferSize);
	}

	onAudioStreamChanged: audioDecoder.load(audioStream);
	onVideoStreamChanged: videoDecoder.load(videoStream);

	VideoReader
	{
		id: videoReader
	}

	VideoDecoder
	{
		id: videoDecoder
		masterClock: parent.masterClock

		queueLimit: 40

		Component.onCompleted: newFrame.connect(canvas.setFrame);
	}

	AudioDecoder
	{
		id: audioDecoder
		masterClock: parent.masterClock

		queueLimit: 60
	}

	Speaker
	{
		id: speakers

		Component.onCompleted: frameRequest.connect(audioDecoder.processData);
	}

	VideoCanvas
	{
		id: canvas
		anchors.fill: parent
	}

	VideoBar
	{
		id: videoBar

		masterClock: parent.masterClock
		videoDuration: parent.masterClock.length / 1000000

		paused: parent.masterClock.paused
		onPause:
		{
			if (parent.masterClock.paused)
				parent.masterClock.start();
			else
				parent.masterClock.pause();
		}

		onTimeSeek: function(time)
		{
			videoReader.seek(time)
			parent.masterClock.setTime(time);
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

	Component.onCompleted:
	{
		videoReader.registerDecoder(videoDecoder);
		videoReader.registerDecoder(audioDecoder);
	}
}
