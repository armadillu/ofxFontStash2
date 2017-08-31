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

namespace ofxFontStash2{

class Parser{

public:

	static void parseText(	const std::string& text, //input
							const std::unordered_map<std::string, Style> & styleIDs, //input
							const std::string & defaultStyleID, //input
							std::vector<StyledText> & parsedText //output
							);

	static ofColor colorFromHex(const std::string & hex);

protected:

	static void recursiveParse(	pugi::xml_node & node,
							   	vector<Style> & styleStack,
							   	const std::unordered_map<std::string, Style> & styleIDs,
							   	std::vector<StyledText> & parsedText);

	static void handleAttributes(pugi::xml_node & node, Style & currStyle);
	static bool isSeparator(char s);
};

}
