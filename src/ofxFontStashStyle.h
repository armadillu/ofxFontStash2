//
//  ofxFontStashStyle.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#ifndef __ofxFontStash2__ofxFontStashStyle__
#define __ofxFontStash2__ofxFontStashStyle__

#include "ofMain.h"
#include "fontstash.h"

struct ofxFontStashStyle{

	bool valid; //will be true if style definition found.
	string fontID;
	float fontSize;
	ofColor color;
	int blur;
	FONSalign alignment; // default: FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE

	ofxFontStashStyle(){
		valid = true;
		fontSize = 12;
		color = ofColor::white;
		blur = 0;
		alignment = (FONSalign)(FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
	};

	bool operator== (const ofxFontStashStyle &b){
		return (fontID == b.fontID &&
				fontSize == b.fontSize &&
				blur == b.blur &&
				alignment == b.alignment &&
				color == b.color
				);
	}

	bool operator!= (const ofxFontStashStyle &b){
		return !(*this == b);
	}
};

struct StyledText{
	string text;
	ofxFontStashStyle style;
};

enum SplitBlockType{
	WORD,
	SEPARATOR
};


struct SplitTextBlock{
	SplitBlockType type;
	StyledText styledText;
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
	LineElement(){
	}
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

#endif /* defined(__ofxFontStash2__ofxFontStashStyle__) */
