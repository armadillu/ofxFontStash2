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
#ifdef GL_VERSION_3
	#include "gl3fontstash.h"
#else
	#include "glfontstash.h"
#endif

#include "ofxFontStashParser.h"
#include "ofxFontStashStyle.h"

#ifdef USE_OFX_TIMEMEASUREMENTS
#include "ofxTimeMeasurements.h"
#else
#define TS_START_NIF //
#define TS_STOP_NIF //
#define TS_START_ACC //
#define TS_STOP_ACC //
#define TS_START //
#define TS_STOP //
#endif

class ofxFontStash2{

public:


	ofxFontStash2();
	void setup(int atlasSizePow2);

	bool addFont(const string& fontID, const string& fontFile); //returns fontID
	bool isFontLoaded(const string& fontID); //checks if a font was already loaded

	// draw single line string
	// returns text width
	// multiline ("\n") not supported
	float draw(const string& text, const ofxFontStashStyle& style, float x, float y);
	
	// draw string with fixed maximum width
	// returns height of the text block
	// multiline ("\n") not supported
	float drawColumn(const string& text, const ofxFontStashStyle& style, float x, float y, float width, bool debug=false);
	
	// draw xml formatted string
	// return text width
	// multiline ("\n") not supported
	float drawFormatted(const string& text, float x, float y);
	
	// draw xml formatted string with fixed maximum width
	// returns height of the text block
	// multiline ("\n") supported
	float drawFormattedColumn(const string& text, float x, float y, float width, bool debug=false);

	// layout styled text
	const vector<StyledLine> layoutLines(const vector<StyledText> &blocks, float targetWidth, bool debug=false);

	// draw prepared text blocks
	float drawLines(const vector<StyledLine> &lines, float x, float y, bool debug=false);
	
	// draw and layout blocks
	float drawAndLayout(vector<StyledText> &blocks, float x, float y, float width, bool debug=false);
	
	ofRectangle getTextBounds( const string &text, const ofxFontStashStyle &style, const float x, const float y );
	void getVerticalMetrics( const ofxFontStashStyle& style, float* ascender, float* descender, float* lineH);
	void setLineHeightMult(float l){lineHeightMultiplier = l;}

	void addStyle(const string& styleID, ofxFontStashStyle style);
	map<string, ofxFontStashStyle> getStyleList(){ return styleIDs; }

	FONScontext * getFSContext(){return fs;}

	// allows for higher pixel densities.
	// this will increase texture resolution during drawing,
	// but will leave all sizes exactly the same
	float pixelDensity;
	
	// with this option one can scale all fonts up/down by a factor
	float fontScale;
	
protected:

	void updateFsPrjMatrix();

	float lineHeightMultiplier;

	string toString(SplitBlockType t){
		switch(t){
			case WORD: return "WORD";
			case SEPARATOR: return "SEPARATOR";
			case SEPARATOR_INVISIBLE: return "SEPARATOR_INVISIBLE";
			default: return "UNKNOWN";
		}
	}
	int getFsID(const string& userFontID);

	float calcWidth(const StyledLine & line);
	float calcLineHeight(const StyledLine & line);
	
	bool applyStyle(const ofxFontStashStyle& style);

	unsigned int toFScolor(const ofColor& c);
	FONScontext * fs;

	map<string, int> fontIDs; //userFontID to fontStashFontID

	map<string, ofxFontStashStyle> styleIDs;

	vector<SplitTextBlock> splitWords( const vector<StyledText> & blocks);


};

#endif /* defined(__ofxFontStash2__ofxFontStash2__) */
