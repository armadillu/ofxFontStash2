//
//  ofxFontStash2.cpp
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#include "ofxFontStash2.h"

#define FONTSTASH_IMPLEMENTATION //only include implementation from cpp file
#include "fontstash.h"
#undef FONTSTASH_IMPLEMENTATION

#define GLFONTSTASH_IMPLEMENTATION //only include implementation from cpp file
#include "glfontstash.h"
#undef GLFONTSTASH_IMPLEMENTATION


ofxFontStash2::ofxFontStash2(){
	fs = NULL;
}

void ofxFontStash2::setup(int atlasSizePow2){

	fs = glfonsCreate(atlasSizePow2, atlasSizePow2, FONS_ZERO_TOPLEFT);
	if (fs == NULL) {
		ofLogError("ofxFontStash2") << "Could not create stash.";
		return;
	}
}


bool ofxFontStash2::addFont(const string& fontID, const string& fontFile){

	int id = fonsAddFont(fs, fontID.c_str(), ofToDataPath(fontFile).c_str());
	if(id != FONS_INVALID){
		fontIDs[fontID] = id;
		return true;
	}else{
		ofLogError("ofxFontStash2") << "Could not load font '" << fontFile << "'";
		return false;
	}
}


float ofxFontStash2::draw(const string& text, const ofxFontStashStyle& style, float x, float y){
	applyStyle(style);
	float dx = fonsDrawText(fs, x, y, text.c_str(), NULL);
	return dx;
}


void ofxFontStash2::drawFormatted(const string& text, float x, float y){

	ofxFontStashParser parser;
	vector<ofxFontStashParser::StyledText> blocks = parser.parseText(text);
	float xx = x;
	float yy = y;
	for(int i = 0; i < blocks.size(); i++){
		xx = draw(blocks[i].text, blocks[i].style, xx, yy);
	}
}



void ofxFontStash2::drawFormattedColumn(const string& text, float x, float y, float targetWidth){

	if (targetWidth < 0) return;
	float xx = x;
	float yy = y;

	vector<ofxFontStashParser::StyledText> blocks = ofxFontStashParser::parseText(text);
	vector<SplitTextBlock> words = splitWords(blocks);

	if (words.size() == 0) return;

	vector<StyledLine> lines;
	ofxFontStashStyle currentStyle;
	StyledLine currentLine;

	float lineWidth = 0;
	int wordsThisLine = 0;

	vector<float> lineHeigts;
	lineHeigts.push_back(yy);

	for(int i = 0; i < words.size(); i++){

		if(currentStyle != words[i].styledText.style){
			//cout << "   new style!" << endl;
			currentStyle = words[i].styledText.style;
			applyStyle(currentStyle);
		}

		float bounds[4];
		float dx;
		//TS_START_ACC("fonsTextBounds");
		dx = fonsTextBounds(	fs,
								xx,
								0,
								words[i].styledText.text.c_str(),
								NULL,
								&bounds[0]
								);
		//TS_STOP_ACC("fonsTextBounds");

		//ofRectangle where = ofRectangle(bounds[0], -bounds[3] , dx, -(bounds[3] - bounds[1]));
		ofRectangle where = ofRectangle(bounds[0], bounds[3] , dx, -(bounds[3] - bounds[1]));
		LineElement le = LineElement(words[i], where);

		float nextWidth = lineWidth + dx;

		if (nextWidth < targetWidth //if wider than targetW
			||
			(wordsThisLine == 0 && (nextWidth >= targetWidth))){ //this is the 1st word in this line but even that doesnt fit

			currentLine.elements.push_back(le);
			lineWidth += dx;
			xx += dx;
			wordsThisLine++;

		}else{ //too long, start a new line

			//calc height for this line - taking in account all words in the line
			float lineH = calcLineHeight(currentLine);
			lineHeigts.push_back(yy + lineH);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x + dx;

			i--; //re-calc dimensions of this word on a new line!
			
			yy += lineH;

			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;

			lines.push_back(currentLine);

			currentLine.elements.clear();
		}
		//float w = calcWidth(currentLine);
	}


	ofxFontStashStyle drawStyle;

	for(int i = 0; i < lines.size(); i++){
		for(int j = 0; j < lines[i].elements.size(); j++){

			lines[i].elements[j].area.y += lineHeigts[i + 1];

			if (drawStyle != lines[i].elements[j].content.styledText.style ){
				drawStyle = lines[i].elements[j].content.styledText.style;
				applyStyle(drawStyle);
			}
			//TS_START_ACC("fonsDrawText");
			fonsDrawText(fs,
						 lines[i].elements[j].area.x,
						 lines[i].elements[j].area.y,
						 lines[i].elements[j].content.styledText.text.c_str(),
						 NULL);
			//TS_STOP_ACC("fonsDrawText");

			//debug rects
			if(lines[i].elements[j].content.type == WORD) ofSetColor(255,12);
			else ofSetColor(0,255,0,12);
			ofDrawRectangle(lines[i].elements[j].area);
		}
	}
}

float ofxFontStash2::calcWidth(StyledLine & line){
	float w = 0;
	for(int i = 0; i < line.elements.size(); i++){
		w += line.elements[i].area.width;
	}
	return w;
}

float ofxFontStash2::calcLineHeight(StyledLine & line){
	float h = 0;
	for(int i = 0; i < line.elements.size(); i++){
		if (-line.elements[i].area.height > h){
			h = -line.elements[i].area.height;
		}
	}
	return h;
}


vector<ofxFontStash2::SplitTextBlock>
ofxFontStash2::splitWords( vector<ofxFontStashParser::StyledText> & blocks){

	std::locale loc = std::locale("");

	vector<SplitTextBlock> wordBlocks;
	std::string currentWord;

	for(int i = 0; i < blocks.size(); i++){

		ofxFontStashParser::StyledText & block = blocks[i];
		string blockText = block.text;

		for(auto c: ofUTF8Iterator(blockText)){

			bool isSpace = std::isspace<wchar_t>(c,loc);
			bool isPunct = std::ispunct<wchar_t>(c,loc);
			
			if(isSpace || isPunct){

				if(currentWord.size()){
					SplitTextBlock word = SplitTextBlock(WORD, currentWord, block.style);
					currentWord.clear();
					wordBlocks.push_back(word);
				}
				string separatorText;
				utf8::append(c, back_inserter(separatorText));
				SplitTextBlock separator = SplitTextBlock(SEPARATOR, separatorText, block.style);
				wordBlocks.push_back(separator);

			}else{
				utf8::append(c, back_inserter(currentWord));
			}
		}
		if(currentWord.size()){ //add last word
			SplitTextBlock word = SplitTextBlock(WORD, currentWord, block.style);
			currentWord.clear();
			wordBlocks.push_back(word);
		}
	}
	return wordBlocks;
}


void ofxFontStash2::getVerticalMetrics(const ofxFontStashStyle & style, float* ascender, float* descender, float* lineH){
	applyStyle(style);
	fonsVertMetrics(fs, ascender, descender, lineH);
}


void ofxFontStash2::applyStyle(const ofxFontStashStyle & style){
	fonsClearState(fs);
	int id = getFsID(style.fontID);
	fonsSetFont(fs, id);
	fonsSetSize(fs, style.fontSize);
	fonsSetColor(fs, toFScolor(style.color));
	fonsSetAlign(fs, style.alignment);
	fonsSetBlur(fs, style.blur);
}

int ofxFontStash2::getFsID(const string& userFontID){

	map<string, int>::iterator it = fontIDs.find(userFontID);
	if(it != fontIDs.end()){
		return it->second;
	}
	ofLogError("ofxFontStash2") << "Invalid Font ID!";
	return FONS_INVALID;
}

unsigned int ofxFontStash2::toFScolor(const ofColor & c){
	return glfonsRGBA(c.r, c.g, c.b, c.a);
}