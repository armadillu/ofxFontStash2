//
//  ofxFontStashParser.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxFontStashStyle.h"

namespace pugi{
	class xml_node;
}

class ofxFontStashParser{

public:

	static vector<StyledText> parseText(const string& text, const map<string, ofxFontStashStyle> & styleIDs);

	static ofColor colorFromHex(const string & hex);

protected:

	static void recursiveParse(pugi::xml_node & node,
							   ofxFontStashStyle style,
							   const map<string, ofxFontStashStyle> & styleIDs,
							   vector<StyledText> & parsedText);

};
