#pragma once

//#define TEST_OFX_NANOVG_COMPAT
//#define TEST_FBO

#ifdef TEST_OFX_NANOVG_COMPAT
	#include "ofxNanoVG.h"
#endif

#include "ofMain.h"
#include "ofxTimeMeasurements.h"
#include "ofxRemoteUIServer.h"
#include "ofxFontStash2.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update(){};
		void draw();

	void testDraw(float x, float y);
	void testDrawBatch(float x, float y);

	float testDrawColumn(float x, float y);
	float testDrawColumnNVG(float x, float y);

	void testDrawFormatted(float x, float y);
	float testDrawFormattedColumn(float x, float y);

	float testDiyPlainLayout(float x, float y);
	float testDiyFormattedLayout(float x, float y);

	void testDrawTabs(float x, float y);


	void keyPressed(int key);

	bool debug;

	ofxFontStash2::Fonts fonts;

	void drawInsertionPoint(float x, float y, float w);

	ofAlignHorz getCurrentAlignment();
	void drawID(int ID, int x, int y);

	ofFbo fbo;
	float columnVariationSpeed = 1;
};
