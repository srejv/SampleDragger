/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "DataModel.h"
#include "ScaleComponent.h"
#include "AudioFileLoader.h"
#include "Sample.h"

class MainContentComponent   : public AudioAppComponent, public Button::Listener, private Timer, public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent()
    {
		addAndMakeVisible(addSample = new TextButton("Add Sample"));
		addSample->addListener(this);

		addAndMakeVisible(saveGenerated = new TextButton("Save Generated"));
		saveGenerated->addListener(this);

		addAndMakeVisible(pixelsToSeconds);
		pixelsToSeconds.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, true, 80, 20);
		pixelsToSeconds.setRange(2.0f, 12000.0f);
		pixelsToSeconds.setTextValueSuffix("px/s");
		pixelsToSeconds.setValue(10.0f);
		pixelsToSeconds.addListener(this);

		addAndMakeVisible(scaleComponent);

		addAndMakeVisible(generateWaveform = new TextButton("Generate"));
		generateWaveform->addListener(this);

		addAndMakeVisible(samplePositionLabel);

        setSize (800, 600);
        setAudioChannels (2, 2);
		startTimer(2);

    }

    ~MainContentComponent()
    {
		stopTimer();
        shutdownAudio();
		specialBufferThumbnail = nullptr;
		
		removeAllChildren();
		samples.clear(true);
    }

	double sr = 48000.0f;
	int samplesPerBlock = 0;
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
		samplesPerBlock = samplesPerBlockExpected;
		sr = sampleRate;
    }

	Label samplePositionLabel;
	uint64 samplePosition = 0U;

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // For more details, see the help for AudioProcessor::getNextAudioBlock()

        bufferToFill.clearActiveBufferRegion();
		samplePosition += bufferToFill.numSamples;
		
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colour(30,30,30));

		g.setColour(Colour(120, 120, 120));
		auto area(getLocalBounds().removeFromRight(200));
		for (auto b : sampleBuffers) {
			g.drawText(b->fileName, area.removeFromTop(20), Justification::centredLeft);
		}

		if (specialBufferThumbnail != nullptr) {
			double width = (mySpecialBuffer->getNumSamples() / sr) * pixelsToSeconds.getValue();
			drawWaveform(g, getLocalBounds().removeFromBottom(100).withTrimmedBottom(20).withWidth(width));
		}
    }
	void drawWaveform(Graphics& g, const Rectangle<int>& thumbnailBounds)
	{
		g.setColour(Colour(50, 50, 50));
		g.fillRect(thumbnailBounds);
		double startTime = 0.0f;
		double endTime = mySpecialBuffer->getNumSamples() / sr;
		float verticalZoom = 1.0f;
		g.setColour(Colours::red);
		specialBufferThumbnail->drawChannels(g, thumbnailBounds, startTime, endTime, verticalZoom);
	}

	void sliderValueChanged(Slider* slider) override
	{
		for (auto s : samples) {
			s->setPixelScale(slider->getValue());
		}
		scaleComponent.setPixelToSeconds(slider->getValue());
	};


    void resized() override
    {
		auto row(getLocalBounds().removeFromBottom(20));
		addSample->setBounds(row.removeFromLeft(100));
		generateWaveform->setBounds(row.removeFromRight(100));
		samplePositionLabel.setBounds(row.removeFromRight(80));
		pixelsToSeconds.setBounds(row);

		saveGenerated->setBounds(getLocalBounds().removeFromTop(20).removeFromRight(100).translated(0,20));

		scaleComponent.setBounds(getLocalBounds().removeFromTop(20));
    }

	void buttonClicked(Button* btn) override
	{
		if (btn == addSample) {
			openButtonClicked();
		}
		if(btn == generateWaveform) {
			generateFinalBuffer();
		}
		if (btn == saveGenerated) {
			saveButtonClicked();
		}
	}
    
    void generateFinalBuffer() {
		int min = INT32_MAX;
		int max = 0;
		for (auto s : samples) {
			auto start = s->getSampleStartPosition();
			auto len = s->getSampleLength();
			if (start < min) { min = start; }
			if ((start + len) > max) { max = (start + len); }
		}
		int length = max - min;
		
		specialBufferThumbnail = loader.createThumbnail();
		specialBufferThumbnail->reset(2, sr);

		mySpecialBuffer = new AudioSampleBuffer(2, length);
		mySpecialBuffer->clear();

		for (auto s : samples) {
			auto startPos = static_cast<int>(s->getSampleStartPosition() - min);
			for (int i = 0; i < s->getNumChannels(); ++i) {
				mySpecialBuffer->addFrom(i, startPos,
					s->getSource(), i, 0,
					s->getSampleLength());
			}
			
		}
		specialBufferThumbnail->addBlock(0, *mySpecialBuffer, 0, mySpecialBuffer->getNumSamples());
	}

	void openButtonClicked()
	{
		ScopedPointer<MySample> mySample = loader.loadAudioFile();
		if (mySample == nullptr) return;
		addAndMakeVisible(samples.add(new Sample(mySample.release())));
		samples.getLast()->setPixelScale(pixelsToSeconds.getValue());
	}

	void saveButtonClicked()
	{
		FileChooser myChooser("Saving as wav...",
			File::getSpecialLocation(File::userHomeDirectory),
			"*.wav");
		if (myChooser.browseForFileToSave(true))
		{
			StringArray metadata;
			WavAudioFormat wav;
			File wavFile(myChooser.getResult());
			FileOutputStream *outputStream = wavFile.createOutputStream();
			
			ScopedPointer<AudioFormatWriter> writer = wav.createWriterFor(outputStream, sr, 2, 16, StringPairArray(), 0);
			if (writer != nullptr)
			{
				if (writer->writeFromAudioSampleBuffer(*mySpecialBuffer, 0, mySpecialBuffer->getNumSamples()))
				{
					std::cout << "Saving .wav succeeded" << std::endl;
				}
				else
				{
					std::cout << "Saving .wav failed" << std::endl;
				}
			}
		}
	}


private:
	void timerCallback() override
	{
		samplePositionLabel.setText(String(samplePosition), dontSendNotification);
		repaint();
	}

    //==============================================================================
	ScopedPointer<TextButton> addSample, generateWaveform, saveGenerated;
	OwnedArray<Sample> samples;
	OwnedArray<MySample> sampleBuffers;



	ScopedPointer<AudioSampleBuffer> mySpecialBuffer;
	ScopedPointer<AudioThumbnail> specialBufferThumbnail;
	
	
	AudioFileLoader loader;
	
	Slider pixelsToSeconds;
	ScaleComponent scaleComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }
