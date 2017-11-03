/*
  ==============================================================================

    DataModel.h
    Created: 3 Nov 2017 3:18:34pm
    Author:  srejv

  ==============================================================================
*/

#pragma once


struct MySample {
	double position = 0.0f;
	int numChannels = 0;
	String fileName;
	AudioSampleBuffer fileBuffer;
	ScopedPointer<AudioThumbnail> thumbnail;
};