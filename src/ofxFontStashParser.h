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

	friend class ofxFontStash2;
	
public:

	static vector<StyledText> parseText(const string& text,
										const unordered_map<string, ofxFontStashStyle> & styleIDs,
										unordered_map<size_t, TemporaryFontStashStyle> & tempStyles,
										const string & defaultStyleID
										);

	static ofColor colorFromHex(const string & hex);

protected:

	static void recursiveParse(	pugi::xml_node & node,
								int & level,
							   	vector<StyleID> & styleStack,
							   	const unordered_map<string, ofxFontStashStyle> & styleIDs,
							   	unordered_map<size_t, TemporaryFontStashStyle> & tempStyles,
							   	vector<StyledText> & parsedText);

	static void handleAttributes(pugi::xml_node & node, ofxFontStashStyle & currStyle, bool & hasAttributes);
	static bool isSeparator(char s);


	static StyleID createTempStyle(const ofxFontStashStyle & s,
								   unordered_map<size_t, TemporaryFontStashStyle> & tempStyles);

	static ofxFontStashStyle getCurrentStackStyle(vector<StyleID> & styleStack,
												  const unordered_map<string, ofxFontStashStyle> & styleIDs,
												  unordered_map<size_t, TemporaryFontStashStyle> & tempStyles
												  );


	static size_t tempStyleCount;

};
