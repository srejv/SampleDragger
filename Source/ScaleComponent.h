/*
  ==============================================================================

    ScaleComponent.h
    Created: 3 Nov 2017 3:18:19pm
    Author:  srejv

  ==============================================================================
*/

#pragma once

class ScaleComponent : public Component {
public:
	ScaleComponent() {}
	
	void setPixelToSeconds(double ps) {
		pixelToSeconds = ps;
		repaint();
	}

private:

	void paint(Graphics& g) override {
		// Mark every second?
		auto area(getLocalBounds());

		double w = static_cast<double>(area.getWidth());
		
		g.setColour(Colours::red);
		// Seconds!
		double c = 0.0;
		while (c < w) {
			g.drawVerticalLine(roundToInt(c), 0, getHeight());
			c += pixelToSeconds;
		}
		
		
		// Half a second!
		g.setColour(Colours::green);
		c = 0.0;
		while (c < w) {
			g.drawVerticalLine(roundToInt(c), 0, getHeight() - 8);
			c += pixelToSeconds * 0.5f;
		}
		
		// Quarter of a second!
		g.setColour(Colours::blue);
		c = 0.0;
		while (c < w) {
			g.drawVerticalLine(roundToInt(c), 0, getHeight() - 16);
			c += pixelToSeconds * 0.25f;
		}
		
	}

	double pixelToSeconds;
};