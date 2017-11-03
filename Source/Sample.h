/*
  ==============================================================================

    Sample.h
    Created: 3 Nov 2017 3:19:12pm
    Author:  srejv

  ==============================================================================
*/

#pragma once

class FadeOverlayComponent : public Component {

	double rampInTime = 0.0;
	double rampOutTime = 0.0;
};

class Sample : public Component {
public:
	Sample() { setSize(140, 80); }
	Sample(MySample* sample) : mySample(sample) {
		setName(mySample->fileName);

		auto nsamples = sample->fileBuffer.getNumSamples();
		double sampleRate = 44100.0;
		double pixelsPerSecond = 10.0f;
		
		double width = (nsamples / sampleRate) * pixelsPerSecond;
		setSize(roundToInt(width), 80);
	}

	void setPixelScale(double newPixelScale) { 
		pixelsToSeconds = newPixelScale; 
		auto nsamples = mySample->fileBuffer.getNumSamples();
		double sampleRate = 44100.0;

		double width = (nsamples / sampleRate) * pixelsToSeconds;
		setSize(roundToInt(width), 80);

		setTopLeftPosition(mySample->position * pixelsToSeconds, getY());

		repaint(); 
	}

	void paint(Graphics& g) override {
		g.setColour(Colour(240, 240, 240));
		g.fillRect(getLocalBounds());

		
		g.setColour(Colour(30, 30, 30));
		g.drawText(getName(), getLocalBounds(), Justification::centred);


		if (mySample != nullptr) {
			paintIfFileLoaded(g, getLocalBounds());

			g.setColour(Colour(30, 30, 30));
			auto nsamples = mySample->fileBuffer.getNumSamples();
			g.drawText(String(), getLocalBounds().removeFromBottom(20), Justification::centred);
		}
	}

	void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
	{
		const double startTime = 0.0f;
		const double endTime = mySample->thumbnail->getTotalLength();
		float verticalZoom = 1.0f;
		g.setColour(Colours::red);
		mySample->thumbnail->drawChannels(g, 
			thumbnailBounds, startTime, endTime, verticalZoom);
	}

	int getIndex() const { return sampleIndex; }
	void setIndex(int index) {
		sampleIndex = index;
	}

	void setContraint(const Rectangle<int>& r) {
		constrainer.setBoundsForComponent(this, r, false, false, false, false);
	}

	int getSampleStartPosition() const {
		const double sampleRate = 48000.0;
		return roundToInt(mySample->position * sampleRate);
	}

	int getSampleLength() const {
		return mySample->fileBuffer.getNumSamples();
	}

	AudioBuffer<float>& getSource() {
		return mySample->fileBuffer;
	}

	int getNumChannels() const {
		return mySample->numChannels;
	 }

private:
	ComponentDragger myDragger;
	ComponentBoundsConstrainer constrainer;

	void mouseDown(const MouseEvent& e) override
	{
		myDragger.startDraggingComponent(this, e);
	}
	void mouseDrag(const MouseEvent& e) override
	{
		myDragger.dragComponent(this, e, &constrainer);
		mySample->position = getX() / pixelsToSeconds;
	}

	int sampleIndex = 0;
	double pixelsToSeconds = 10.0f;

	ScopedPointer<MySample> mySample;
};