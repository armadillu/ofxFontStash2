//
//  ofxFontStash2.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer MesiÃ  on 12/7/15.
//
//

#pragma once
#include "ofMain.h"

#include "ofx_fontstash.h"

#ifdef GL_VERSION_3
	#include "gl3fontstash.h"
	#define FONT_STASH_PRE_DRAW 	bool wasDrawing = drawingFS; \
									if(!drawingFS){ \
										preFSDraw(); \
										drawingFS = true; \
									}

	#define FONT_STASH_POST_DRAW 	if(!wasDrawing){	\
										postFSDraw();	\
										drawingFS = false;	\
									}
#else
	#include "glfontstash.h"
	#define FONT_STASH_PRE_DRAW 	;
	#define FONT_STASH_POST_DRAW 	;
#endif

#include "ofxFontStashParser.h"
#include "ofxFontStashStyle.h"

class ofxFontStash2{

public:


	ofxFontStash2();
	void setup(int atlasSizePow2);

	/// load fonts
	bool addFont(const string& fontID, const string& fontFile); //returns fontID
	bool isFontLoaded(const string& fontID); //checks if a font was already loaded

	/// work with font styles
	void addStyle(const string& styleID, ofxFontStashStyle style);
	map<string, ofxFontStashStyle> getStyleList(){ return styleIDs; }

	/// draw single line string
	/// returns text width
	/// multiline ("\n") not supported - to use for one-liners
	float draw(const string& text, const ofxFontStashStyle& style, float x, float y);

	/// draw xml formatted string
	/// return text width
	/// multiline ("\n") not supported - to use for one-liners with style
	float drawFormatted(const string& styledText, float x, float y);

	/// draw string with fixed maximum width
	/// returns text bbox
	/// multiline ("\n") supported - and it will break lines on its own given a column width
	ofRectangle drawColumn(const string& text, const ofxFontStashStyle& style, float x, float y, float width, bool debug=false);

	/// draw xml formatted string with fixed maximum width
	/// returns height of the text block
	/// multiline ("\n") supported - and it will break lines on its own given a column width
	ofRectangle drawFormattedColumn(const string& styledText, float x, float y, float width, bool debug=false);

	///diy layout + draw - so you can inspect intermediate states and do as you wish
	/// 1 - Parse your text to get the vector<StyledText> blocks.
	/// 2 - Layout the lines to a certain column width; you get a vector of styled words (vectos<StyledText>).
	///	3 - Draw the layout; its a vector of Lines so you can dig and edit before you draw.
	/*
		vector<StyledText> parsed = fs.parseStyledText(myStyledText);
		vector<StyledLine> laidOutLines = fs.layoutLines(parsed, 300);
		ofRectangle bbox = drawLines(laidOutLines, x, y);

	 */


	// Utils below this line ///////////////////////////////////////////////////////////////////////////////

	/// split up styledText into a vector of blocks with the same style
	vector<StyledText> parseStyledText(const string & styledText);

	/// layout StyledText blocks
	const vector<StyledLine> layoutLines(const vector<StyledText> &blocks, float targetWidth, bool debug=false);

	/// draw already prepared StyledLine«s
	ofRectangle drawLines(const vector<StyledLine> &lines, float x, float y, bool debug=false);
	
	/// draw and layout blocks
	ofRectangle drawAndLayout(vector<StyledText> &blocks, float x, float y, float width, bool debug=false);

	/// only applies to draw(); return the bbox of the text
	ofRectangle getTextBounds( const string &text, const ofxFontStashStyle &style, const float x, const float y );

	ofRectangle getTextBounds(const vector<StyledLine> &lines, float x, float y);


	void getVerticalMetrics( const ofxFontStashStyle& style, float* ascender, float* descender, float* lineH);

	/// a global lineH multiplier that affects all loaded fonts.
	void setLineHeightMult(float l){lineHeightMultiplier = l;}

	// sets a single font as fallback for all fonts
	// this can only be set once
	void setGlobalFallbackFont(const string& fallbackFontID);
	string getGlobalFallbackFont();
	
	// add fallback variant to a font
	// removing/listing the fallbacks is currently not possible.
	void addFallbackFont(const string& fontID, const string &fallbackFontID);
	
	FONScontext * getFSContext(){return fs;}

	/// allows for higher pixel densities.
	/// this will increase texture resolution during drawing,
	/// but will leave all sizes exactly the same
	float pixelDensity;
	
	// width of tabs (measured in spaces)
	int tabWidth = 4;
	
	/// with this option one can scale all fonts up/down by a factor
	float fontScale;
	
protected:

	void preFSDraw();
	void postFSDraw();

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
	
	string globalFallbackFontID; // id of the fallback font for all loaded fonts

	vector<SplitTextBlock> splitWords( const vector<StyledText> & blocks);

	ofShader nullShader;
	bool drawingFS;
};
