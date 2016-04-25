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
#ifdef GL_VERSION_3
	#include "gl3fontstash.h"
#else
	#include "glfontstash.h"
#endif
#undef GLFONTSTASH_IMPLEMENTATION


ofxFontStash2::ofxFontStash2(){
	fs = NULL;
	lineHeightMultiplier = 1.0;
	pixelDensity = 1.0;
	fontScale = 1.0;
}

void ofxFontStash2::setup(int atlasSizePow2){

	#ifdef GL_VERSION_3
	ofLogNotice("ofxFontStash2") << "creating fontStash GL3";
	fs = gl3fonsCreate(atlasSizePow2, atlasSizePow2, FONS_ZERO_TOPLEFT);
	#else
	ofLogNotice("ofxFontStash2") << "creating fontStash GL2";
	fs = glfonsCreate(atlasSizePow2, atlasSizePow2, FONS_ZERO_TOPLEFT);
	#endif
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


bool ofxFontStash2::isFontLoaded(const string& fontID){
	std::map<string, int>::iterator iter = fontIDs.find( fontID );
	return iter == fontIDs.end()?false:true;
}


void ofxFontStash2::addStyle(const string& styleID, ofxFontStashStyle style){
	styleIDs[styleID] = style;
}


void ofxFontStash2::updateFsPrjMatrix(){

#ifdef GL_VERSION_3
	GLfloat mat[16];
	memset(mat, 0, 16 * sizeof(GLfloat));
	mat[0] = 4.0 / ofGetScreenWidth();
	mat[5] = -4.0 / ofGetScreenHeight();
	mat[10] = 2.0;
	mat[12] = -1.0;
	mat[13] = 1.0;
	mat[14] = -1.0;
	mat[15] = 1.0;
	gl3fonsProjection(fs, &mat[0]);
#endif

}

float ofxFontStash2::draw(const string& text, const ofxFontStashStyle& style, float x, float y){

	updateFsPrjMatrix();

	ofPushMatrix();
	ofScale(1/pixelDensity, 1/pixelDensity);
	applyStyle(style);
	float dx = fonsDrawText(fs, x*pixelDensity, y*pixelDensity, text.c_str(), NULL)/pixelDensity;
	ofPopMatrix();
	return dx;
}

float ofxFontStash2::drawColumn(const string& text, const ofxFontStashStyle &style, float x, float y, float targetWidth, bool debug){
	//TODO this ignores \n ! need to parse!
	//TODO double check the todo, this might work already.
	vector<ofxFontStashParser::StyledText> blocks;
	ofxFontStashParser::StyledText block{text, style}; 
	blocks.push_back(block);
	return drawBlocks(blocks, x, y, targetWidth, debug);
}

float ofxFontStash2::drawFormatted(const string& text, float x, float y){

	ofxFontStashParser parser;
	vector<ofxFontStashParser::StyledText> blocks = parser.parseText(text, styleIDs);
	float xx = x;
	float yy = y;
	for(int i = 0; i < blocks.size(); i++){
		xx += draw(blocks[i].text, blocks[i].style, xx, yy);
	}
	
	return xx-x; 
}

float ofxFontStash2::drawFormattedColumn(const string& text, float x, float y, float targetWidth, bool debug){
	if (targetWidth < 0) return 0;
	
	TS_START_NIF("parse text");
	vector<ofxFontStashParser::StyledText> blocks = ofxFontStashParser::parseText(text, styleIDs);
	TS_STOP_NIF("parse text");
	
	return drawBlocks(blocks, x, y, targetWidth, debug);
}

float ofxFontStash2::drawBlocks(vector<ofxFontStashParser::StyledText> &blocks, float x, float y, float targetWidth, bool debug){

	if (targetWidth < 0) return 0;
	float xx = x;
	float yy = y;

	TS_START_NIF("split words");
	vector<SplitTextBlock> words = splitWords(blocks);
	TS_STOP_NIF("split words");

	if (words.size() == 0) return 0;

	vector<StyledLine> lines;
	ofxFontStashStyle currentStyle;
	currentStyle.fontSize = -1; // this makes sure the first style is actually applied, even if it's the default style
	
	StyledLine currentLine;

	float lineWidth = 0;
	int wordsThisLine = 0;

	vector<float> lineHeigts;
	lineHeigts.push_back(0);
	float currentLineH = 0;

	float bounds[4];
	float dx;
	LineElement le;
	
	TS_START("walk words");
	for(int i = 0; i < words.size(); i++){

		//TS_START_ACC("word style");
		if(words[i].styledText.style.valid && currentStyle != words[i].styledText.style ){
			//cout << "   new style!" << endl;
			currentStyle = words[i].styledText.style;
			if(applyStyle(currentStyle)){
				fonsVertMetrics(fs, NULL, NULL, &currentLineH);
				currentLineH/=pixelDensity;
			}else{
				ofLogError() << "no style font defined!";
			}
		}
		//TS_STOP_ACC("word style");
		
		bool stayOnCurrentLine = true;
		
		if( words[i].type == SEPARATOR && words[i].styledText.text == "\n" ){
			stayOnCurrentLine = false;
			dx = 0;
			float lineH = lineHeightMultiplier * calcLineHeight(currentLine);
			if(debug) lineHeigts.push_back(yy + lineH);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x + dx;
			
			// no!
			//i--; //re-calc dimensions of this word on a new line!
			yy += lineH;
			
			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;
			lines.push_back(currentLine);
			currentLine.elements.clear();
			continue;
		}
		else{
			//TS_START_ACC("fonsTextBounds");
			dx = fonsTextBounds(	fs,
									xx,
									yy,
									words[i].styledText.text.c_str(),
									NULL,
									&bounds[0]
									)/pixelDensity;
			bounds[0]/=pixelDensity;
			bounds[1]/=pixelDensity;
			bounds[2]/=pixelDensity;
			bounds[3]/=pixelDensity;
			//TS_STOP_ACC("fonsTextBounds");

			ofRectangle where = ofRectangle(bounds[0], bounds[1] , bounds[2] - bounds[0], bounds[3] - bounds[1]);

			le = LineElement(words[i], where);
			le.baseLineY = yy;
			le.x = xx;
			le.lineHeight = currentLineH;

			float nextWidth = lineWidth + dx;
			
			//if not wider than targetW
			// ||
			//this is the 1st word in this line but even that doesnt fit
			stayOnCurrentLine = nextWidth < targetWidth || (wordsThisLine == 0 && (nextWidth >= targetWidth));

		}
		
		if (stayOnCurrentLine){

			//TS_START_ACC("stay on line");
			currentLine.elements.push_back(le);
			lineWidth += dx;
			xx += dx;
			wordsThisLine++;
			//TS_STOP_ACC("stay on line");

		} else if( words[i].type == SEPARATOR && words[i].styledText.text == " " ){
			// ignore separators when moving into the next line!
			continue;
		} else{ //too long, start a new line

			//TS_START_ACC("new line");
			//calc height for this line - taking in account all words in the line
			float lineH = lineHeightMultiplier * calcLineHeight(currentLine);
			if(debug) lineHeigts.push_back(yy + lineH);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x + dx;

			i--; //re-calc dimensions of this word on a new line!
			yy += lineH;

			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;
			lines.push_back(currentLine);
			currentLine.elements.clear();
			//TS_STOP_ACC("new line");
		}

		//TS_START_ACC("last line");
		if(stayOnCurrentLine && i == words.size() -1){ //addd last line
			float lineH = lineHeightMultiplier * calcLineHeight(currentLine);
			lineHeigts.push_back(yy + lineH);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x + dx;
			lines.push_back(currentLine);
		}
		//TS_STOP_ACC("last line");
		//float w = calcWidth(currentLine);
	}
	TS_STOP("walk words");

	TS_START("count words");
	int nDrawnWords;
	for(int i = 0; i < lines.size(); i++){
		for(int j = 0; j < lines[i].elements.size(); j++){
			nDrawnWords ++;
		}
	}
	TS_STOP("count words");

	//debug line heights!
	if(debug){
		TS_START("draw line Heights");
		ofSetColor(0,255,0,32);
		ofPushMatrix();
		ofTranslate(0.5, 0.5);
		for(int i = 0; i < lineHeigts.size(); i++){
			float offset = -lines[0].lineH;
			ofDrawLine(0, lineHeigts[i] + offset, ofGetWidth(), lineHeigts[i] + offset);
		}
		ofPopMatrix();
		TS_STOP("draw line Heights");
	}

	ofxFontStashStyle drawStyle;

	TS_START("draw all lines");

	updateFsPrjMatrix();

	if (pixelDensity != 1.0f){ //hmmmm
		ofPushMatrix();
		ofScale(1/pixelDensity, 1/pixelDensity);
	}
	for(int i = 0; i < lines.size(); i++){
		for(int j = 0; j < lines[i].elements.size(); j++){

			//if(lines[i].elements[j].content.type == WORD){ //only draw words!
			if(lines[i].elements[j].content.styledText.text != " "){ //no need to draw whitespace

				if (lines[i].elements[j].content.styledText.style.valid &&
					drawStyle != lines[i].elements[j].content.styledText.style ){

					drawStyle = lines[i].elements[j].content.styledText.style;
					TS_START_ACC("applyStyle");
					applyStyle(drawStyle);
					TS_STOP_ACC("applyStyle");
				}
				lines[i].elements[j].area.y += lines[i].lineH -lines[0].lineH;

				//TS_START_ACC("fonsDrawText");
				string & texttt = lines[i].elements[j].content.styledText.text;
				fonsDrawText(fs,
							 lines[i].elements[j].x*pixelDensity,
							 (lines[i].elements[j].baseLineY + lines[i].lineH -lines[0].lineH)*pixelDensity,
							 texttt.c_str(),
							 NULL);
				//TS_STOP_ACC("fonsDrawText");

				//debug rects
				if(debug){
					//TS_START_ACC("debug rects");
					if(lines[i].elements[j].content.type == WORD) ofSetColor(255,25);
					else ofSetColor(0,255,0,25);
					ofDrawRectangle(lines[i].elements[j].area);
					//TS_STOP_ACC("debug rects");
				}
			}
		}
	}
	if (pixelDensity != 1.0f){ //hmmmm
		ofPopMatrix();
	}
	TS_STOP("draw all lines");
	
	return lines.size() == 0? 0 : (lines.back().elements.back().baseLineY - y);
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
		if (line.elements[i].lineHeight > h){
			h = line.elements[i].lineHeight;
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


ofRectangle ofxFontStash2::getTextBounds( const string &text, const ofxFontStashStyle &style, const float x, const float y ){
	applyStyle(style);
	float bounds[4]={0,0,0,0};
	int advance = fonsTextBounds( fs, x*pixelDensity, y*pixelDensity, text.c_str(), NULL, bounds );
	advance/=pixelDensity;
	bounds[0]/=pixelDensity;
	bounds[1]/=pixelDensity;
	bounds[2]/=pixelDensity;
	bounds[3]/=pixelDensity;
	// here we use the "text advance" instead of the width of the rectangle,
	// because this includes spaces at the end correctly (the text bounds "x" and "x " are the same,
	// the text advance isn't). 
	return ofRectangle(bounds[0],bounds[1],advance,bounds[3]-bounds[1]);
}

void ofxFontStash2::getVerticalMetrics(const ofxFontStashStyle & style, float* ascender, float* descender, float* lineH){
	applyStyle(style);
	fonsVertMetrics(fs, ascender, descender, lineH);
}


bool ofxFontStash2::applyStyle(const ofxFontStashStyle & style){
	//if(style.fontID.size()){
		fonsClearState(fs);
		int id = getFsID(style.fontID);
		fonsSetFont(fs, id);
		fonsSetSize(fs, style.fontSize*pixelDensity*fontScale);
		fonsSetColor(fs, toFScolor(style.color));
		fonsSetAlign(fs, style.alignment);
		fonsSetBlur(fs, style.blur);
	
	return id != FONS_INVALID;
	//}
}

int ofxFontStash2::getFsID(const string& userFontID){

	map<string, int>::iterator it = fontIDs.find(userFontID);
	if(it != fontIDs.end()){
		return it->second;
	}
	ofLogError("ofxFontStash2") << "Invalid Font ID: " << userFontID;
	return FONS_INVALID;
}

unsigned int ofxFontStash2::toFScolor(const ofColor & c){
	#ifdef GL_VERSION_3
	return gl3fonsRGBA(c.r, c.g, c.b, c.a);
	#else
	return glfonsRGBA(c.r, c.g, c.b, c.a);
	#endif
}