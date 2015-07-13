#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	fonts.setup(512);
	fonts.addFont("veraMono", "fonts/VeraMono.ttf");
	fonts.addFont("Helvetica", "fonts/HelveticaNeue.ttf");
	ofBackground(22);
}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255,0,0);
	ofDrawRectangle(0, 0, 100, 100);

	ofxFontStashStyle style;
	style.fontID = "veraMono";
	style.fontSize = 22;
	style.color = ofColor::white;
	if(ofGetFrameNum()%60 < 30) style.blur = 4;

	ofSetColor(255);
	fonts.draw("banana! monkey!", style, mouseX, mouseY);

	string formattedText = "<style font=veraMono size=12 color=#ff0000>this is red veramono 12</style>";
	formattedText += "<style font=Helvetica size=44 color=#000ff>this is blue Helvetica 44</style>";
	fonts.drawFormatted(formattedText, 200, 300);

	ofSetColor(0,255,0,64);
	ofDrawRectangle(50, 0, 200, 100);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
