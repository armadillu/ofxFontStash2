//
//  ofxFontStashStyle.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxfs2_nanovg.h"

struct ofxFontStashStyle{

	string fontID;
	float fontSize = 12;
	ofColor color = ofColor::white;
	int blur = 0;
	NVGalign alignmentV = (NVGalign)(NVG_ALIGN_BASELINE); //NOTE H alignment is ignored!
	float lineHeightMult = 1.0;
	float spacing = 0;

	ofxFontStashStyle(string fontID, float fontSize, const ofColor & color){
		this->fontSize = fontSize;
		this->color = color;
		this->fontID = fontID;
	}

	ofxFontStashStyle(string fontID, float fontSize){
		this->fontSize = fontSize;
		this->fontID = fontID;
	}

	ofxFontStashStyle(){
	};

	bool operator== (const ofxFontStashStyle &b){
		return (fontID == b.fontID &&
				fabs(fontSize - b.fontSize) < 0.001f &&
				blur == b.blur &&
				alignmentV == b.alignmentV &&
				color == b.color &&
				fabs(lineHeightMult - b.lineHeightMult) < 0.001f &&
				fabs(spacing - b.spacing) < 0.001f
				);
	}

	string toString(){
		stringstream ss;
		string colorT = ofToString((int)color.r) + "," + ofToString((int)color.g) + "," + ofToString((int)color.b) + "," + ofToString((int)color.a);
		ss << "fontID:" << fontID << " fontSize:" << fontSize << " blur:" << blur <<
		" alignmentV:" << toString(alignmentV) << " color:[" << colorT << "] lineHeightMult:"
		<< lineHeightMult << " spacing:" << spacing;
		return ss.str();
	}
	
	bool operator!= (const ofxFontStashStyle &b){
		return !(*this == b);
	}

	string toString(NVGalign va){
		switch(va){
			case NVG_ALIGN_BASELINE: return "BASELINE";
			case NVG_ALIGN_TOP: return "TOP";
			case NVG_ALIGN_MIDDLE: return "MIDDLE";
			case NVG_ALIGN_BOTTOM: return "BOTTOM";
			default: return "UNKNOWN";
		}
	}
};


struct TemporaryFontStashStyle{ //a style with a short lifespan (will onyl stay around for a few frames)
	ofxFontStashStyle style;
	size_t numFramesAliveLeft;
};

struct StyleID{ //this is a weird hybrid structure that is key to a ofxFontStashStyle - can hold either a static styleID or a temporary stytleID

	bool isTempStyle; //if true, the style used in this text is defined inline (its temprary)
	string styleID; //this will be >0 if it refers to a "proper" style the user defined beforehand (ie "static" style)
	size_t tempStyleID; //this will be >= 0 if it refers to a style defined an inline style, ie not a "static" style but a temp one

	StyleID(const string & styleID_){
		isTempStyle = false;
		styleID = styleID_;
	}
	StyleID(size_t tempStyleID_){
		isTempStyle = true;
		tempStyleID = tempStyleID_;
	}

	StyleID(){};

	bool operator== (const StyleID &b){ //compare StyleID
		if(isTempStyle){
			return tempStyleID == b.tempStyleID;
		}else{
			return styleID == b.styleID;
		}
	}

	bool operator!= (const StyleID &b){
		return !(*this == b);
	}
};


struct StyledText{
	
	string text;
	//ofxFontStashStyle style_;
	StyleID styleID;

//	StyledText(const string & text, const ofxFontStashStyle & style){
//		this->text = text;
//		style_ = style;
//	}

	StyledText(const string & text, const StyleID & styleID_){
		this->text = text;
		styleID = styleID_;
	}

	StyledText(){};
};




// There are three types of blocks:
// - WORDS: 'hey' and 'wow'
// - SEPARATOR: word separators that allow breaking into a new line after, like '.' or '-'
// - SEPARATOR_INVISIBLE: just as separator, but there's no need to ever draw them (space, new line)
// The enum values are chosen so that you can check if it's_any kind of separator with (type & SEPARATOR) != 0
enum TextBlockType{
	BLOCK_WORD = 1,
	SEPARATOR = 2,
	SEPARATOR_INVISIBLE = 3
};


struct TextBlock{
	TextBlockType type;
	StyledText styledText;
	TextBlock(TextBlockType type, string text, StyleID styleID){
		this->type = type;
		this->styledText.text = text;
		this->styledText.styleID = styleID;
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
	LineElement(TextBlock b, ofRectangle r){
		this->content = b;
		this->area = r;
	}
};


struct StyledLine{
	float lineH = 0;
	float lineW = 0;
	float boxW = 0;
	vector<LineElement> elements;
	StyledLine(){
		//elements.reserve(50);
	}
};

