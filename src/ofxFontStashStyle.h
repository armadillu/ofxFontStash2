//
//  ofxFontStashStyle.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#pragma once

#include "ofMain.h"
#include "fontstash.h"


struct ofxFontStashStyle{

	bool valid; //will be true if style definition found.
	string fontID;
	float fontSize = 12;
	ofColor color = ofColor::white;
	int blur = 0;
	FONSalign alignment = (FONSalign)(FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
	float lineHeightMult = 1.0;

	ofxFontStashStyle(string fontID, float fontSize, const ofColor & color){
		valid = true;
		this->fontSize = fontSize;
		this->color = color;
		this->fontID = fontID;
	}

	ofxFontStashStyle(string fontID, float fontSize){
		valid = true;
		this->fontSize = fontSize;
		this->fontID = fontID;
	}

	ofxFontStashStyle(){
		valid = true;
	};

	bool operator== (const ofxFontStashStyle &b){
		return (fontID == b.fontID &&
				fontSize == b.fontSize &&
				blur == b.blur &&
				alignment == b.alignment &&
				color == b.color &&
				fabs(lineHeightMult - b.lineHeightMult) < 0.001f
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


// There are three types of blocks:
// - WORDS: 'hey' and 'wow'
// - SEPARATOR: word separators that allow breaking into a new line after, like '.' or '-'
// - SEPARATOR_INVISIBLE: just as separator, but there's no need to ever draw them (space, new line)
// The enum values are chosen so that you can check if it's_any kind of separator with (type & SEPARATOR) != 0
enum SplitBlockType{
	WORD = 1,
	SEPARATOR = 2,
	SEPARATOR_INVISIBLE = 3
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
	float lineHeight = 1.0; //not of this block, but of this style
	float baseLineY = 0.0f;
	float x = 0.0f; //x
	LineElement(){}
	LineElement(SplitTextBlock b, ofRectangle r){
		this->content = b;
		this->area = r;
	}
};


struct StyledLine{
	float lineH = 0;
	float lineW = 0;
	vector<LineElement> elements;
	StyledLine(){
		//elements.reserve(50);
	}
};

