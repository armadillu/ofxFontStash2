#pragma once

#include "ofMain.h"
#include "ofxFontStash2.h"
#include "ofxTimeMeasurements.h"
#include "ofxRemoteUIServer.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void testDraw(float x, float y);
		void testDrawColumn(float x, float y);
		void testDrawFormatted(float x, float y);
		void testDrawFormattedColumn(float x, float y);
		void testDrawTabs(float x, float y);

		void keyPressed(int key);

		ofxFontStash2 fonts;
		bool debug;

	//helpers
	void drawInsertionPoint(float x, float y, float w);
	FONSalign getCyclingAlignment();

};
