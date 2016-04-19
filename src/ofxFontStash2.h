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

	//simple draw - no multiline awareness - '\n' will be drawn as squares!"
	float draw(const string& text, const ofxFontStashStyle& style, float x, float y);

	//will break lines according to column width - '\n' will be drawn as squares!
	void drawColumn(const string& text, const ofxFontStashStyle& style, float x, float y, float width, bool debug=false);
	void drawFormatted(const string& text, float x, float y);

	//"\n" aware - will break lines but also will break lines to fit column width
	void drawFormattedColumn(const string& text, float x, float y, float width, bool debug=false);

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

	enum SplitBlockType{
		WORD,
		SEPARATOR
	};


	string toString(SplitBlockType t){
		if (t == WORD) return "WORD";
		else return "SEPARATOR";
	}


	struct SplitTextBlock{
		SplitBlockType type;
		ofxFontStashParser::StyledText styledText;
		SplitTextBlock(SplitBlockType type, string text, ofxFontStashStyle style){
			this->type = type;
			this->styledText.text = text;
			this->styledText.style = style;
		}
		SplitTextBlock(){}
	};

	struct LineElement{
		SplitTextBlock content;
		ofRectangle area;
		float lineHeight; //not of this block, but of this style
		float baseLineY;
		float x; //x
		LineElement(SplitTextBlock & b, ofRectangle r){
			this->content = b;
			this->area = r;
		}
	};

	struct StyledLine{
		float lineH;
		float lineW;
		vector<LineElement> elements;
		StyledLine(){
			//elements.reserve(50);
			lineH = lineW = 0;
		}
	};

	int getFsID(const string& userFontID);

	float calcWidth(StyledLine & line);
	float calcLineHeight(StyledLine & line);
	
	void applyStyle(const ofxFontStashStyle& style);

	unsigned int toFScolor(const ofColor& c);
	FONScontext * fs;

	map<string, int> fontIDs; //userFontID to fontStashFontID

	map<string, ofxFontStashStyle> styleIDs;

	vector<SplitTextBlock> splitWords( vector<ofxFontStashParser::StyledText> & blocks);
	void drawBlocks(vector<ofxFontStashParser::StyledText> &blocks, float x, float y, float targetWidth, bool debug=false);


};

#endif /* defined(__ofxFontStash2__ofxFontStash2__) */
