#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"
#include "ofxRemoteUIServer.h"
#include "ofxFontStash2.h"

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

	bool debug;

	ofxFontStash2 fonts;

	void drawInsertionPoint(float x, float y, float w);

	ofAlignHorz getCyclingAlignment();
};
