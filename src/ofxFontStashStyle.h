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
};

#endif /* defined(__ofxFontStash2__ofxFontStashStyle__) */
