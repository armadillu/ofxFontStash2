//
//  ofxFontStash2.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#pragma once
#include "ofMain.h"

#include "ofxFontStashParser.h"
#include "ofxFontStashStyle.h"

#include "ofxfs2_fontstash.h"
#include "ofxfs2_nanovg.h"

namespace ofxFontStash2{

class Fonts{

public:

	Fonts();
	~Fonts();

	void setup(bool debug = false);

	/// load fonts
	bool addFont(const std::string & fontID, const std::string & fontFile); //returns fontID
	bool isFontLoaded(const std::string & fontID); //checks if a font was already loaded
	std::vector<std::string> getFontIDs(); //returns a list of all fonts that are ready to use

	/// work with font styles
	bool addStyle(const std::string & styleID, const Style & style);
	bool removeStyle(const std::string & styleID);
	bool styleExists(const std::string & styleID);
	Style getStyle(const std::string & styleID, bool * exists = nullptr);
	std::unordered_map<std::string, Style> & getStyles(){ return styleIDs; }
	void setDefaultStyle(const std::string & styleID){ defaultStyleID = styleID; } //how do we render formatted text whose style is undefined? We will use this style.

	/// draw single line string
	/// returns text width (dx)
	/// multiline ("\n") not supported - to use for one-liners
	float draw(const std::string & text, const Style & style, float x, float y);

	/// draw xml formatted string
	/// return text width
	/// multiline ("\n") not supported - to use for one-liners with style
	float drawFormatted(const std::string & styledText, float x, float y);

	/// draw string with fixed maximum width, breaking lines to fit the width
	/// returns text bbox
	/// multiline ("\n") supported - and it will break lines on its own given a column width
	ofRectangle drawColumn(const std::string & text, const Style & style, float x, float y, float width, ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT, bool debug = false);

	/// draw xml formatted string with fixed maximum width, breaking lines to fit the width
	/// returns text bbox
	/// multiline ("\n") supported - and it will break lines on its own given a column width
	ofRectangle drawFormattedColumn(const std::string & styledText, float x, float y, float width, ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT, bool debug = false);

	// DIY layout + draw - so you can inspect intermediate states and do as you wish //
	/// 1 - Parse your text to get the vector<StyledText> blocks.
	/// 2 - Layout the lines to a certain column width; you get a vector of styled words (vector<StyledText>).
	///	3 - Draw the layout; its a vector of Lines so you can dig and edit before you draw.

	/*
		// Styled Text //
	 	vector<StyledText> parsed;
	 	fs.parseStyledText(myStyledText, parsed);
		vector<StyledLine> laidOutLines;
	 	fs.layoutLines(parsed, columnWidth, laidOutLines);
		ofRectangle bbox = fs.drawLines(laidOutLines, x, y);
		
		// Plain Text //
		Style style;
		float columnWidth;
		std::string text = "my text";
		vector<StyledLine> laidOutLines;
	 	fonts.layoutLines({{text, style}}, columnWidth, laidOutLines);
		ofRectangle bbox = fonts.drawLines(laidOutLines, xx, yy);
	 */

	///draw a paragraph relying on NanoVG for layout & linebreaking
	void drawColumnNVG(const std::string& text, const Style& style,
					   float x, float y, float width,
					   ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT);

	///get the bbox containing the text laid out in a drawColumnNVG() call
	ofRectangle getTextBoundsNVG(const std::string& text,
								 const Style& style,
								 float x, float y, float width,
								 ofAlignHorz horAlign);


	// Utils below this line ///////////////////////////////////////////////////////////////////////////////

	/// split up styledText into a vector of blocks with the same style
	void parseStyledText(const std::string & styledText, std::vector<StyledText> & output);

	/// layout StyledText blocks
	void layoutLines(const std::vector<StyledText> &blocks, //input
					 float targetWidth, //input
					 std::vector<StyledLine> & output, //output
					 ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT, //input
					 int limitToNLines = 0, //stop layout if lines are > limitToNLines
					 bool debug = false);

	/// draw already prepared StyledLine´s
	ofRectangle drawLines(const std::vector<StyledLine> & lines,
						  float x, float y,
						  ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT,
						  bool debug = false);
	
	/// draw and layout blocks
	ofRectangle drawAndLayout(const std::vector<StyledText> & blocks,
							  float x, float y,
							  float width,
							  ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT,
							  bool debug=false);

	/// only applies to draw(); return the bbox of the text
	ofRectangle getTextBounds(const std::string &text, const Style &style, const float x, const float y );
	ofRectangle getTextBounds(const std::vector<StyledLine> & lines, float x, float y);

	///get font metrics for a particular style
	void getVerticalMetrics( const Style & style, float* ascender, float* descender, float* lineH);

	/// a global lineH multiplier that affects all loaded fonts.
	void setLineHeightMult(float l){lineHeightMultiplier = l;}

	///in case you want to issue nanoVG calls directly
	NVGcontext * getNanoVGContext(){return ctx;}

	/// sets a single font as fallback for all fonts
	/// this can only be set once - useful for emoji
	void setGlobalFallbackFont(const std::string& fallbackFontID);
	std::string getGlobalFallbackFont();
	
	/// add fallback variant to a font
	/// removing/listing the fallbacks is currently not possible.
	void addFallbackFont(const std::string& fontID, const std::string &fallbackFontID);
	
	/// allows for higher pixel densities.
	/// this will increase texture resolution during drawing,
	/// but will leave all sizes exactly the same
	/// this can be changed between calls, meaning it's read (and applied) before each draw call
	float pixelDensity;
	
	/// width of tabs (measured in # of spaces)
	int tabWidth = 4;

	/// with this option one can scale all font drawing up/down by a factor
	float fontScale;

	/// Batch mode
	void beginBatch(); 	//if you can pack a few draw calls togehter in a batch, you will get better performance
	void endBatch(); 	//by wrapping them around beginBath() & endBatch()
	bool isInBatch(){ return inBatchMode;}

protected:

	float lineHeightMultiplier;

	std::string toString(TextBlockType t){
		switch(t){
			case BLOCK_WORD: return "BLOCK_WORD";
			case SEPARATOR: return "SEPARATOR";
			case SEPARATOR_INVISIBLE: return "SEPARATOR_INVISIBLE";
			default: return "UNKNOWN";
		}
	}

	int getFsID(const std::string& userFontID);

	float calcWidth(const StyledLine & line);
	float calcLineHeight(const StyledLine & line);
	
	bool applyStyle(const Style & style);

	NVGcolor toFScolor(const ofColor & c);
	NVGcontext* ctx = nullptr;

	std::unordered_map<std::string, int> fontIDs; //userFontID to fontStashFontID
	std::unordered_map<std::string, Style> styleIDs;

	std::string defaultStyleID; //how do we render formatted text whose style is undefined?
	
	std::string globalFallbackFontID; // id of the fallback font for all loaded fonts

	void splitWords( const std::vector<StyledText> & blocks, std::vector<TextBlock> & words);

	ofShader nullShader;
	bool inBatchMode = false;

	std::vector<std::string> reservedStyleNames = {"style"};

	void applyOFMatrix();
	void begin();
	void end();
};

}
