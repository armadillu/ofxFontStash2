#pragma once

#include "ofxiOS.h"
#include "ofxFontStash2.h"

class ofApp : public ofxiOSApp{
	
    public:
        void setup();
        void update();
        void draw();
        void exit();
	
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);

        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);


	void testDraw(float x, float y);
	void testDrawColumn(float x, float y);
	void testDrawFormatted(float x, float y);
	void testDrawFormattedColumn(float x, float y);
	void testDrawTabs(float x, float y);

	ofxFontStash2 fonts;
	bool debug = true;

	//helpers
	void drawInsertionPoint(float x, float y, float w);
	ofAlignHorz getCyclingAlignment();

	bool isSetup = false;

};


