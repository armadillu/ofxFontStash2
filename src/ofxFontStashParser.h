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

	static vector<StyledText> parseText(const string& text,
										const unordered_map<string, ofxFontStashStyle> & styleIDs,
										const string & defaultStyleID
										);

	static ofColor colorFromHex(const string & hex);

protected:

	static void recursiveParse(	pugi::xml_node & node,
								int & level,
							   	vector<ofxFontStashStyle> & styleStack,
							   	const unordered_map<string, ofxFontStashStyle> & styleIDs,
							   	vector<StyledText> & parsedText);

	static void handleAttributes(pugi::xml_node & node, ofxFontStashStyle & currStyle);
	static bool isSeparator(char s);
};
