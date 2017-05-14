#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);
	TIME_SAMPLE_ENABLE();
	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);
	TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_BOTTOM_RIGHT);
	TIME_SAMPLE_SET_PRECISION(4);

	RUI_SETUP();
	RUI_SHARE_PARAM(debug);

	fonts.setup();

	fonts.addFont("Arial", "fonts/Arial Unicode.ttf");
	fonts.addFont("veraMono", "fonts/VeraMono.ttf");
	fonts.addFont("veraMonoBold", "fonts/VeraMono-Bold.ttf");
	fonts.addFont("Helvetica", "fonts/HelveticaNeue.ttf");

	ofxFontStashStyle style;
	style.fontID = "Arial";
	style.fontSize = 33;
	style.color = ofColor::yellow;
	fonts.addStyle("style1", style);

	style.color = ofColor::turquoise;
	fonts.addStyle("style2", style);

	style.color = ofColor::lightCyan;
	style.fontID = "veraMono";
	style.fontSize = 55;
	fonts.addStyle("style3", style);

	ofDisableAntiAliasing(); //to get precise lines
	fonts.pixelDensity = 2.0;

	#ifdef TEST_OFX_NANOVG_COMPAT
	ofxNanoVG::one().setup(true, true);
	ofxNanoVG::one().addFont("VeraMono", "fonts/VeraMono.ttf");
	#endif

}



void ofApp::update(){

}



void ofApp::draw(){

	ofScale(0.9, 0.9);
	
	TSGL_START("d");

	float xx = 40;
	float yy = 40;

	TS_START("draw");
	testDraw(xx,yy);
	TS_STOP("draw");

	yy += 100;
	TS_START("drawColumn");
	testDrawColumn(xx,yy);
	TS_STOP("drawColumn");

	yy = 40;
	xx = 600;
	TS_START("testDrawFormatted");
	testDrawFormatted(xx, yy);
	TS_STOP("testDrawFormatted");

	yy += 100;
	TS_START("drawFormattedColumn");
	testDrawFormattedColumn(xx, yy);
	TS_STOP("drawFormattedColumn");

	xx = 40;
	yy += 450;
	TS_START("drawTabs");
	testDrawTabs(xx, yy);
	TS_STOP("drawTabs");

	TSGL_STOP("d");

	#ifdef TEST_OFX_NANOVG_COMPAT
	ofxNanoVG::one().beginFrame(ofGetWidth(), ofGetHeight(), 1.0);

	ofSetColor(255,0,0);
	ofxNanoVG::one().applyOFStyle();
	ofxNanoVG::one().rect(ofRectangle(0,0,200,200));
	ofxNanoVG::one().fillPath();

	ofSetColor(0,255,0);
	ofxNanoVG::one().applyOFStyle();
	ofxNanoVG::one().drawText("VeraMono", 20, 20, "test", 22);
	ofSetColor(0,0,255,128);
	ofDrawRectangle(100,100,200,200);
	ofSetColor(255,128);
	ofxNanoVG::one().applyOFStyle();
	ofxNanoVG::one().drawText("VeraMono", 20, 60, "test", 22);

	ofSetColor(255,255,0,128);
	ofDrawRectangle(200,200,200,200);

	ofxNanoVG::one().endFrame();
	#endif
}


void ofApp::testDraw(float x, float y){

	ofxFontStashStyle style;
	style.fontID = "Arial";
	style.fontSize = 22;
	{
		string text = "testing draw() method with one-line string. バナナのようなサル";
		ofRectangle bounds = fonts.getTextBounds(text, style, x, y);
		ofSetColor(0,255,0,33);
		ofDrawRectangle(bounds);
		drawInsertionPoint(x,y,100);
		fonts.draw(text, style, x, y);
	}

	y += 30;

	{
		string text = "testing draw() methods with\na multiline string (not supported).";
		ofRectangle bounds = fonts.getTextBounds(text, style, x, y);
		ofSetColor(0,255,0,33);
		ofDrawRectangle(bounds);
		drawInsertionPoint(x,y,100);
		fonts.draw(text, style, x, y);
	}
}


void ofApp::testDrawColumn(float x, float y){

	ofxFontStashStyle style;
	style.fontID = "Arial";
	style.fontSize = 45;
	ofAlignHorz align = getCyclingAlignment();

	string text = "Testing drawColumn() methods with a long string and no line breaks whatsoever. Also, adding some fancy unicode バナナのようなサル. And back to normal...";
	drawInsertionPoint(x,y,100);
	float colW = 300 + (0.5 + 0.5 * sin(ofGetElapsedTimef() * 0.33)) * 200;
	ofSetColor(255,33);
	float boxH = 300;
	ofDrawLine(x , y - 15, x, y + boxH);
	ofDrawLine(x + colW, y - 15, x + colW, y + boxH);
	ofRectangle bbox = fonts.drawColumn(text, style, x, y, colW, align, debug);
	ofSetColor(255,13);
	ofDrawRectangle(bbox);
}


void ofApp::testDrawFormatted(float x, float y){

	string styledText = "<style id='style1'>this is style1.</style><style id='style2'>And this is Style 2</style>";
	drawInsertionPoint(x,y,100);
	fonts.drawFormatted(styledText, x, y);
}


void ofApp::testDrawFormattedColumn(float x, float y){
	string styledText = "<style id='style1'>This is style1 style.</style> <style id='style2'>And this is Style2, adjusted to the column width.</style>";
	styledText += " <style id='style3'>And this is stlye3, which has a bigger font size.</style>";
	drawInsertionPoint(x,y,100);
	float colW = 500;// + (0.5 + 0.5 * sin(ofGetElapsedTimef() * 0.33)) * 200;
	ofSetColor(255,33);
	float boxH = 300;
	ofDrawLine(x , y - 15, x, y + boxH);
	ofDrawLine(x + colW, y - 15, x + colW, y + boxH);
	auto align = getCyclingAlignment();
	fonts.drawFormattedColumn(styledText, x, y, colW, align, debug);
}


void ofApp::testDrawTabs(float x, float y){

	string code =
	"class ofApp : public ofBaseApp{\n"
	"	public:\n"
	"		void setup();\n"
	"		void update();\n"
	"}";

	ofxFontStashStyle style;
	style.fontID = "veraMono";
	style.fontSize = 16;
	style.color = ofColor::white;

	fonts.drawColumn(code, style, x, y, ofGetWidth(), OF_ALIGN_HORZ_LEFT, debug);
}

void ofApp::keyPressed(int key){

	if(key == 'd'){
		debug ^= true;
		RUI_PUSH_TO_CLIENT();
	}
}



void ofApp::drawInsertionPoint(float x, float y, float w){
	ofSetColor((ofGetFrameNum() * 20)%255,200);
	ofDrawCircle(x,y, 1.5);
	ofSetColor(255,64);
	ofDrawLine(x - 10, y, x + w, y);
	ofSetColor(255);
}


ofAlignHorz ofApp::getCyclingAlignment(){

	vector<ofAlignHorz> hor = {OF_ALIGN_HORZ_LEFT, OF_ALIGN_HORZ_CENTER, OF_ALIGN_HORZ_RIGHT};
	float pct = (ofGetFrameNum()%300 / 300.0f) ;
	int hIndex = (ofMap(pct, 0, 1, 0, hor.size()));

	hIndex = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 3, true);
	return hor[MIN(hIndex, 2)];
}
