//
//  Style.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxfs2_nanovg.h"

namespace ofxFontStash2{

struct Style{

	std::string fontID;
	float fontSize = 12.0f;
	ofColor color;
	unsigned char blur = 0;
	NVGalign alignmentV = (NVGalign)(NVG_ALIGN_BASELINE); //NOTE H alignment is ignored!
	float lineHeightMult = 1.0;
	float spacing = 0;

	Style(const std::string & fontID, float fontSize, const ofColor & color){
		this->fontSize = fontSize;
		this->color = color;
		this->fontID = fontID;
	}

	Style(const std::string & fontID, float fontSize){
		this->fontSize = fontSize;
		this->fontID = fontID;
	}

	Style(){};

	bool operator== (const Style &b){
		return (fontID == b.fontID &&
				fabs(fontSize - b.fontSize) < 0.001f &&
				blur == b.blur &&
				alignmentV == b.alignmentV &&
				color == b.color &&
				fabs(lineHeightMult - b.lineHeightMult) < 0.001f &&
				fabs(spacing - b.spacing) < 0.001f
				);
	}

	std::string toString() const{
		stringstream ss;
		std::string colorT = ofToString((int)color.r) + "," + ofToString((int)color.g) + "," + 
			ofToString((int)color.b) + "," + ofToString((int)color.a);

		ss << "fontID:" << fontID << " fontSize:" << fontSize << " blur:" << (unsigned int)(blur) <<
			" alignmentV:" << toString((NVGalign)alignmentV) << " color:[" << colorT << "] lineHeightMult:" << 
			lineHeightMult << " spacing:" << spacing;
		return ss.str();
	}

	std::string toString(NVGalign va) const{
		switch(va){
			case NVG_ALIGN_BASELINE: return "BASELINE";
			case NVG_ALIGN_TOP: return "TOP";
			case NVG_ALIGN_MIDDLE: return "MIDDLE";
			case NVG_ALIGN_BOTTOM: return "BOTTOM";
			default: return "UNKNOWN";
		}
	}

	bool operator!= (const Style & b){
		return !(*this == b);
	}
};


struct StyledText{
	std::string text;
	Style style;
};


// There are three types of blocks:
// - WORDS: 'hey' and 'wow'
// - SEPARATOR: word separators that allow breaking into a new line after, like '.' or '-'
// - SEPARATOR_INVISIBLE: just as separator, but there's no need to ever draw them (space, new line)
// The enum values are chosen so that you can check if it's_any kind of separator with (type & SEPARATOR) != 0
enum TextBlockType : unsigned char{
	BLOCK_WORD = 1,
	SEPARATOR = 2, //this will never be used bc we ignore punctuation types now (they are part of words)
	SEPARATOR_INVISIBLE = 3
};


struct TextBlock{
	TextBlockType type;
	StyledText styledText;

	TextBlock(TextBlockType type, const std::string & text, const Style & style){
		this->type = type;
		this->styledText.text = text;
		this->styledText.style = style;
	}

	TextBlock(){}
};


struct LineElement{
	TextBlock content;
	ofRectangle area;
	float lineHeight = 1.0; //not of this block, but of this style
	float baseLineY = 0.0f;
	float x = 0.0f; //x
	LineElement(){}
	LineElement(const TextBlock & b, const ofRectangle & r){
		this->content = b;
		this->area = r;
	}
};


struct StyledLine{
	float lineH = 0;
	float lineW = 0;
	float boxW = 0;
	std::vector<LineElement> elements;
	StyledLine(){
		//elements.reserve(50);
	}
};

}
