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
#include "gumbo.h"
#include "ofxFontStashStyle.h"

class ofxFontStashParser{

public:

	struct StyledText{
		string text;
		ofxFontStashStyle style;
	};

	ofxFontStashParser();

	vector<StyledText> parseText(const string& text);

protected:

	void recursiveParse(GumboNode* node);
	vector<StyledText> parsedText;

};

#endif /* defined(__ofxFontStash2__ofxFontStashParser__) */
