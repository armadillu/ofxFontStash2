#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	

	ofBackground(22);
	//ofSetOrientation(OF_ORIENTATION_90_LEFT);
	
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
	//style.fontSize = 44;
	fonts.addStyle("style3", style);

	ofDisableAntiAliasing(); //to get precise lines
	fonts.pixelDensity = 1.0;

	isSetup= true;;
}

//--------------------------------------------------------------
void ofApp::update(){

	if(!isSetup) return;
}

//--------------------------------------------------------------
void ofApp::draw(){

	if(!isSetup) return;

	float xx = 40;
	float yy = 40;

	testDraw(xx,yy);

	yy += 100;
	testDrawColumn(xx,yy);

	yy = 40;
	xx = 600;
	testDrawFormatted(xx, yy);

	yy += 100;
	testDrawFormattedColumn(xx, yy);

	xx = 40;
	yy += 450;
	testDrawTabs(xx, yy);

}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

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
	float colW = 300 + (0.5 + 0.5 * sin(ofGetElapsedTimef() * 0.33)) * 200;
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
