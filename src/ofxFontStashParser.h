//
//  ofxFontStashParser.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#ifndef __ofxFontStash2__ofxFontStashParser__
#define __ofxFontStash2__ofxFontStashParser__

#include "ofMain.h"
#include "ofxFontStashStyle.h"

namespace pugi{
	class xml_node;
}

class ofxFontStashParser{

public:

	struct StyledText{
		string text;
		ofxFontStashStyle style;
	};

	static vector<StyledText> parseText(const string& text, const map<string, ofxFontStashStyle> & styleIDs);

protected:

	static void recursiveParse(pugi::xml_node & node,
							   ofxFontStashStyle style,
							   const map<string, ofxFontStashStyle> & styleIDs,
							   vector<StyledText> & parsedText);

};

#endif /* defined(__ofxFontStash2__ofxFontStashParser__) */
