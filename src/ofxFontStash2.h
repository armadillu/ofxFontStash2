//
//  ofxFontStash2.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#ifndef __ofxFontStash2__ofxFontStash2__
#define __ofxFontStash2__ofxFontStash2__

#include "ofMain.h"

#include "fontstash.h"
#include "glfontstash.h"

#include "ofxFontStashParser.h"
#include "ofxFontStashStyle.h"

class ofxFontStash2{

public:


	ofxFontStash2();
	void setup(int atlasSizePow2);

	bool addFont(const string& fontID, const string& fontFile); //returns fontID


	//simple draw - no multiline awareness
	float draw(const string& text, const ofxFontStashStyle& style, float x, float y);
	void drawFormatted(const string& text, float x, float y);
	void drawFormattedColumn(const string& text, float x, float y, float width);

	void getVerticalMetrics( const ofxFontStashStyle& style, float* ascender, float* descender, float* lineH);

	FONScontext * getFSContext(){return fs;}

protected:

	int getFsID(const string& userFontID);
	void applyStyle(const ofxFontStashStyle& style);

	unsigned int toFScolor(const ofColor& c);
	FONScontext * fs;
	map<string, int> fontIDs; //userFontID to fontStashFontID


};

#endif /* defined(__ofxFontStash2__ofxFontStash2__) */
