//
//  ofxFontStash2.cpp
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#include "ofxFontStash2.h"


#define FONTSTASH_IMPLEMENTATION //only include implementation from cpp file
#include "fontstash.h"
#undef FONTSTASH_IMPLEMENTATION

#define GLFONTSTASH_IMPLEMENTATION //only include implementation from cpp file
#include "glfontstash.h"
#undef GLFONTSTASH_IMPLEMENTATION


ofxFontStash2::ofxFontStash2(){
	fs = NULL;
}

void ofxFontStash2::setup(int atlasSizePow2){

	fs = glfonsCreate(atlasSizePow2, atlasSizePow2, FONS_ZERO_TOPLEFT);
	if (fs == NULL) {
		ofLogError("ofxFontStash2") << "Could not create stash.";
		return;
	}
}


bool ofxFontStash2::addFont(const string& fontID, const string& fontFile){

	int id = fonsAddFont(fs, fontID.c_str(), ofToDataPath(fontFile).c_str());
	if(id != FONS_INVALID){
		fontIDs[fontID] = id;
		return true;
	}else{
		ofLogError("ofxFontStash2") << "Could not load font '" << fontFile << "'";
		return false;
	}
}


float ofxFontStash2::draw(const string& text, const ofxFontStashStyle& style, float x, float y){
	applyStyle(style);
	float dx = fonsDrawText(fs, x, y, text.c_str(), NULL);
	return dx;
}


void ofxFontStash2::drawFormatted(const string& text, float x, float y){

	ofxFontStashParser parser;
	vector<ofxFontStashParser::StyledText> blocks = parser.parseText(text);
	float xx = x;
	float yy = y;
	for(int i = 0; i < blocks.size(); i++){
		draw(blocks[i].text, blocks[i].style, xx, yy);
	}

}

void ofxFontStash2::getVerticalMetrics(const ofxFontStashStyle & style, float* ascender, float* descender, float* lineH){
	applyStyle(style);
	fonsVertMetrics(fs, ascender, descender, lineH);
}


void ofxFontStash2::applyStyle(const ofxFontStashStyle & style){
	fonsClearState(fs);
	int id = getFsID(style.fontID);
	fonsSetFont(fs, id);
	fonsSetSize(fs, style.fontSize);
	fonsSetColor(fs, toFScolor(style.color));
	fonsSetAlign(fs, style.alignment);
	fonsSetBlur(fs, style.blur);
}

int ofxFontStash2::getFsID(const string& userFontID){

	map<string, int>::iterator it = fontIDs.find(userFontID);
	if(it != fontIDs.end()){
		return it->second;
	}
	ofLogError("ofxFontStash2") << "Invalid Font ID!";
	return FONS_INVALID;
}

unsigned int ofxFontStash2::toFScolor(const ofColor & c){
	return glfonsRGBA(c.r, c.g, c.b, c.a);
}