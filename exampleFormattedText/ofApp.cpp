#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);
	#ifdef TIME_SAMPLE_OFX_FONTSTASH2
	TIME_SAMPLE_ENABLE();
	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);
	TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_BOTTOM_RIGHT);
	TIME_SAMPLE_SET_PRECISION(4);
	#endif

	fonts.setup(false);

	fonts.addFont("roboBlack", "fonts/Roboto-Black.ttf");
	fonts.addFont("roboBold", "fonts/Roboto-Bold.ttf");
	fonts.addFont("robo", "fonts/Roboto-Regular.ttf");

	fonts.addStyle("header", ofxFontStashStyle("roboBlack", 44, ofColor::white));
	fonts.addStyle("body", ofxFontStashStyle("robo", 18, ofColor::white));
	fonts.addStyle("bodyBold", ofxFontStashStyle("roboBold", 18, ofColor::white));
	fonts.addStyle("bodyRed", ofxFontStashStyle("robo", 18, ofColor::red));

	ofDisableAntiAliasing(); //to get precise lines
	fonts.pixelDensity = 2.0;

}


void ofApp::draw(){

	//test global multipliers
	//fonts.setLineHeightMult(1.5 + 0.5 * sin(ofGetElapsedTimef()));
	//fonts.fontScale = 1.5 + 0.5 * sin(ofGetElapsedTimef());

	float x = 100;
	float y = 100;



	drawInsertionPoint(x, y, 100);

	float colW = MIN(MAX(120, ofGetMouseX() - x), ofGetWidth()/2 - x);
	string styledText = "<header>ofxFontStash2</header>"
						"<body><br/><br/>ofxFontStash2 allows you to draw text. It can draw individual "
						"lines of text, or longer texts constrained to a column (to create "
						"paragraph, automatically flowing the text for you). "
						"It offers text alignment (left, center, right).<br/><br/> "
						"It also allows you to mix and match different text styles in the same "
						"paragraph. You can do so by creating <bodyRed>styles</bodyRed> that can be applied at an "
 						"individual word level. You can also inline styles;</body>";

	ofRectangle bbox;
	TSGL_START("draw formatted GL");
	TS_START("draw formatted");
	bbox = fonts.drawFormattedColumn(styledText, x, y, colW, OF_ALIGN_HORZ_LEFT, debug);
	TS_STOP("draw formatted");
	TSGL_STOP("draw formatted GL");

	//draw left & right column limits
	ofSetColor(255,32);
	ofDrawLine(x , 0, x, ofGetHeight());
	ofDrawLine(x + colW, 0, x + colW, ofGetHeight());

	//draw bbox on top
	ofSetColor(255,16);
	ofDrawRectangle(bbox);


	//now draw with straight nanovg renderer
	x = ofGetWidth() * 0.5 + x;
	TSGL_START("draw NVG GL");
	TS_START("draw NVG");
	fonts.drawColumnNVG(styledText, fonts.getStyle("body"), x, y, colW, OF_ALIGN_HORZ_LEFT);
	TS_STOP("draw NVG");
	TSGL_STOP("draw NVG GL");
	ofSetColor(255,32);
	ofDrawLine(x , 0, x, ofGetHeight());
	ofDrawLine(x + colW, 0, x + colW, ofGetHeight());



}
void ofApp::keyPressed(int key){

	if(key == 'd'){
		debug ^= true;
	}
}



void ofApp::drawInsertionPoint(float x, float y, float w){
	ofSetColor((ofGetFrameNum() * 20)%255,200);
	ofDrawCircle(x,y, 1.5);
	ofSetColor(255,64);
	ofDrawLine(x - 10, y, x + w, y);
	ofSetColor(255);
}
