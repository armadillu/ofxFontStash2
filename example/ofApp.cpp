#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	fonts.setup(512);

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
	//style.fontSize = 44;
	fonts.addStyle("style3", style);

	ofBackground(22);
	TIME_SAMPLE_ENABLE();
	TIME_SAMPLE_SET_AVERAGE_RATE(0.01);
	TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_BOTTOM_RIGHT);
	TIME_SAMPLE_SET_PRECISION(4);

	RUI_SETUP();
	RUI_SHARE_PARAM(debug);
	RUI_LOAD_FROM_XML();

	ofDisableAntiAliasing(); //to get precise lines
	fonts.pixelDensity = 1.0;
}


//--------------------------------------------------------------
void ofApp::update(){

}


//--------------------------------------------------------------
void ofApp::draw(){

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

}


void ofApp::drawInsertionPoint(float x, float y, float w){
	ofSetColor((ofGetFrameNum() * 20)%255,200);
	ofDrawCircle(x,y, 1.5);
	ofSetColor(255,64);
	ofDrawLine(x - 10, y, x + w, y);
	ofSetColor(255);
}


void ofApp::testDraw(float x, float y){

	ofxFontStashStyle style;
	style.fontID = "Arial";
	style.fontSize = 22;
	//style.alignment = getCyclingAlignment();
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
	//style.alignment = getCyclingAlignment();

	string text = "Testing drawColumn() methods with a long string and no line breaks whatsoever. Also, adding some fancy unicode バナナのようなサル. And back to normal...";
	drawInsertionPoint(x,y,100);
	float colW = 300 + (0.5 + 0.5 * sin(ofGetElapsedTimef() * 0.33)) * 200;
	ofSetColor(255,33);
	float boxH = 300;
	ofDrawLine(x , y - 15, x, y + boxH);
	ofDrawLine(x + colW, y - 15, x + colW, y + boxH);
	ofRectangle bbox = fonts.drawColumn(text, style, x, y, colW, debug);
	ofSetColor(255,13);
	//ofDrawRectangle(x, y, colW, colH);
	ofDrawRectangle(bbox);
}


void ofApp::testDrawFormatted(float x, float y){

	string styledText = "<style id='style1'>this is style1.</style><style id='style2'>And this is Style 2</style>";
	drawInsertionPoint(x,y,100);
	fonts.drawFormatted(styledText, x, y);
}


void ofApp::testDrawFormattedColumn(float x, float y){
	string styledText = "<style id='style1'>This is style1 style.</style>\n\n<style id='style2'>And this is Style2, adjusted to the column width.</style>";
	styledText += " <style id='style3'>And this is stlye3, which has a bigger font size.</style>";
	drawInsertionPoint(x,y,100);
	float colW = 300 + (0.5 + 0.5 * sin(ofGetElapsedTimef() * 0.33)) * 200;
	ofSetColor(255,33);
	float boxH = 300;
	ofDrawLine(x , y - 15, x, y + boxH);
	ofDrawLine(x + colW, y - 15, x + colW, y + boxH);
	fonts.drawFormattedColumn(styledText, x, y, colW, debug);
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

	fonts.drawColumn(code, style, x, y, ofGetWidth(), debug);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if(key == 'd'){
		debug ^= true;
		RUI_PUSH_TO_CLIENT();
	}
}


FONSalign ofApp::getCyclingAlignment(){
	FONSalign horizontal;
	int hor = (ofGetFrameNum() / 30)%3;
	switch(hor){
		case 0 : horizontal = FONS_ALIGN_LEFT; break;
		case 1 : horizontal = FONS_ALIGN_CENTER; break;
		case 2 : horizontal = FONS_ALIGN_RIGHT; break;
	}
	FONSalign vertical;
	int ver = (ofGetFrameNum() / 30)%4;
	switch(ver){
		case 0 : vertical = FONS_ALIGN_TOP; break;
		case 1 : vertical = FONS_ALIGN_MIDDLE; break;
		case 2 : vertical = FONS_ALIGN_BOTTOM; break;
		case 3 : vertical = FONS_ALIGN_BASELINE; break;
	}
	return (FONSalign)(horizontal | vertical);
}
