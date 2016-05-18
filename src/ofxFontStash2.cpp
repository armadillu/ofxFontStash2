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
	drawingFS = false;
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
	mat[0] = 2.0 / (ofGetWidth() * pixelDensity);
	mat[5] = -2.0 / (ofGetHeight() * pixelDensity);
	mat[10] = 2.0;
	mat[12] = -1.0;
	mat[13] = 1.0;
	mat[14] = -1.0;
	mat[15] = 1.0;
	gl3fonsProjection(fs, &mat[0]);
#endif
}

float ofxFontStash2::draw(const string& text, const ofxFontStashStyle& style, float x, float y){

	FONT_STASH_PRE_DRAW;
	ofPushMatrix();
	ofScale(1/pixelDensity, 1/pixelDensity);
	applyStyle(style);
	float dx = fonsDrawText(fs, x*pixelDensity, y*pixelDensity, text.c_str(), NULL)/pixelDensity;
	ofPopMatrix();
	FONT_STASH_POST_DRAW;
	return dx;
}

float ofxFontStash2::drawColumn(const string& text, const ofxFontStashStyle &style, float x, float y, float targetWidth, bool debug){
	vector<StyledText> blocks;
	StyledText block{text, style}; 
	blocks.push_back(block);
	return drawAndLayout(blocks, x, y, targetWidth, debug);
}

float ofxFontStash2::drawFormatted(const string& text, float x, float y){

	FONT_STASH_PRE_DRAW;
	ofxFontStashParser parser;
	vector<StyledText> blocks = parser.parseText(text, styleIDs);
	float xx = x;
	float yy = y;
	for(int i = 0; i < blocks.size(); i++){
		xx += draw(blocks[i].text, blocks[i].style, xx, yy);
	}

	FONT_STASH_POST_DRAW;
	return xx-x;
}

float ofxFontStash2::drawFormattedColumn(const string& text, float x, float y, float targetWidth, bool debug){
	if (targetWidth < 0) return 0;
	
	TS_START_NIF("parse text");
	vector<StyledText> blocks = ofxFontStashParser::parseText(text, styleIDs);
	TS_STOP_NIF("parse text");
	
	return drawAndLayout(blocks, x, y, targetWidth, debug);
}

const vector<StyledLine> ofxFontStash2::layoutLines(const vector<StyledText> &blocks, float targetWidth, bool debug ){
	float x = 0;
	float y = 0;
	if (targetWidth < 0) return vector<StyledLine>();
	float xx = x;
	float yy = y;
	
	TS_START_NIF("split words");
	vector<SplitTextBlock> words = splitWords(blocks);
	TS_STOP_NIF("split words");
	
	if (words.size() == 0) return vector<StyledLine>();
	
	vector<StyledLine> lines;
	
	// here we create the first line. a few things to note:
	// - in general, like in a texteditor, the line exists first, then content is added to it.
	// - 'line' here refers to the visual representation. even a line with no newline (\n) can span multiple lines
	lines.push_back(StyledLine());
	
	ofxFontStashStyle currentStyle;
	currentStyle.fontSize = -1; // this makes sure the first style is actually applied, even if it's the default style
		
	float lineWidth = 0;
	int wordsThisLine = 0;
	
	vector<float> lineHeigts;
	float currentLineH = 0;
	
	float bounds[4];
	float dx;
	LineElement le;
	
	TS_START("walk words");
	for(int i = 0; i < words.size(); i++){
		StyledLine &currentLine = lines.back();
		
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
		
		if( words[i].type == SEPARATOR_INVISIBLE && words[i].styledText.text == "\n" ){
			stayOnCurrentLine = false;
			dx = 0;
			
			// add a zero-width enter mark. this is used to keep track
			// of the vertical spacing of empty lines.
			le = LineElement(words[i], ofRectangle(xx,yy,0,currentLineH));
			le.baseLineY = yy;
			le.x = xx;
			le.lineHeight = currentLineH;
			currentLine.elements.push_back(le);
			
			float lineH = calcLineHeight(currentLine);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x + dx;
			
			// no!
			//i--; //re-calc dimensions of this word on a new line!
			yy += lineH;
			
			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;
			lines.push_back(StyledLine());
			continue;
		}
		else{
			//TS_START_ACC("fonsTextBounds");
			// applyStyle() already upscaled the font size for the display resolution.
			// Here we do the same for x/y.
			// The result gets the inverse treatment.
			dx = fonsTextBounds(	fs,
								xx*pixelDensity,
								yy*pixelDensity,
								words[i].styledText.text.c_str(),
								NULL,
								&bounds[0]
								)/pixelDensity;
			bounds[0]/=pixelDensity;
			bounds[1]/=pixelDensity;
			bounds[2]/=pixelDensity;
			bounds[3]/=pixelDensity;
			//TS_STOP_ACC("fonsTextBounds");
			
			//hansi: using dx instead of bounds[2]-bounds[0]
			//dx is the right size for spacing out text. bounds give exact area of the char, which isn't so useful here.
			ofRectangle where = ofRectangle(bounds[0], bounds[1] , dx, bounds[3] - bounds[1]);
			
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
			
		} else if( words[i].type == SEPARATOR_INVISIBLE && words[i].styledText.text == " " ){
			// ignore spaces when moving into the next line!
			continue;
		} else{ //too long, start a new line
			
			//TS_START_ACC("new line");
			//calc height for this line - taking in account all words in the line
			float lineH = lineHeightMultiplier * calcLineHeight(currentLine);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x;
			
			i--; //re-calc dimensions of this word on a new line!
			yy += lineH;
			
			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;
			lines.push_back(StyledLine());
			//TS_STOP_ACC("new line");
		}
	}

	//TS_START_ACC("last line");
	// update dimensions of the last line
	StyledLine &currentLine = lines.back();
	if( currentLine.elements.size() == 0 ){
		// but at least one spacing character, so we have a line height.
		le = LineElement(SplitTextBlock(SEPARATOR_INVISIBLE,"",currentStyle), ofRectangle(xx,yy,0,currentLineH));
		le.baseLineY = yy;
		le.x = xx;
		le.lineHeight = currentLineH;
		currentLine.elements.push_back(le);
	}
	
	float lineH = calcLineHeight(currentLine);
	currentLine.lineH = lineH;
	currentLine.lineW = xx - x + dx;
	
	//TS_STOP_ACC("last line");

	TS_STOP("walk words");
	
	return lines; 
}

float ofxFontStash2::drawLines(const vector<StyledLine> &lines, float x, float y, bool debug){

	// if possible get rid of this translate:
	#ifndef GL_VERSION_3
	ofPushMatrix();
	ofTranslate(x, y);
	#endif

	ofVec2f offset;
	#ifdef GL_VERSION_3
	offset.x = x * pixelDensity;
	offset.y = y * pixelDensity;
	#endif

//	TS_START("count words");
//	int nDrawnWords;
//	for(int i = 0; i < lines.size(); i++){
//		for(int j = 0; j < lines[i].elements.size(); j++){
//			nDrawnWords ++;
//		}
//	}
//	TS_STOP("count words");

	//debug line heights!
	if(debug){
		TS_START("draw line Heights");
		ofSetColor(0,255,0,32);
		float yy = 0;
		for( const StyledLine &line : lines ){
			ofDrawLine(offset.x + 0.5f, offset.y + yy + 0.5f, offset.x + line.lineW + 0.5f, offset.y + yy + 0.5f);
			yy += line.lineH;
		}
		TS_STOP("draw line Heights");
	}

	ofxFontStashStyle drawStyle;
	drawStyle.fontSize = -1;

	float offY = 0.0f; // only track for return value
	TS_START("draw all lines");

	FONT_STASH_PRE_DRAW;

	#ifndef GL_VERSION_3
	if (pixelDensity != 1.0f){ //hmmmm
		ofScale(1.0f/pixelDensity, 1.0f/pixelDensity);
	}
	#endif


	for(int i = 0; i < lines.size(); i++){
		y += lines[i].lineH;
		
		for(int j = 0; j < lines[i].elements.size(); j++){

			if(lines[i].elements[j].content.type != SEPARATOR_INVISIBLE ){ //no need to draw the invisible chars

				const StyledLine &l = lines[i];
				const LineElement &el = l.elements[j];
				const string & texttt = el.content.styledText.text;

				if (el.content.styledText.style.valid &&
					drawStyle != el.content.styledText.style ){

					drawStyle = el.content.styledText.style;
					TS_START_ACC("applyStyle");
					applyStyle(drawStyle);
					TS_STOP_ACC("applyStyle");
				}
				//lines[i].elements[j].area.y += lines[i].lineH -lines[0].lineH;

				//TS_START_ACC("fonsDrawText");
				fonsDrawText(fs,
							 el.x * pixelDensity + offset.x,
							 (el.baseLineY + l.lineH - lines[0].lineH) * pixelDensity + offset.y,
							 texttt.c_str(),
							 NULL);
				//TS_STOP_ACC("fonsDrawText");

				//debug rects
				if(debug){
					//TS_START_ACC("debug rects");
					if(el.content.type == BLOCK_WORD) ofSetColor(70 * i, 255 - 70 * i, 0, 200);
					else ofSetColor(50 * i,255 - 50 * i, 0, 100);
					const ofRectangle &r = el.area;
					ofDrawRectangle(pixelDensity * r.x, pixelDensity * r.y, pixelDensity * r.width, pixelDensity * r.height);
					ofFill();
				}
			}
		}
	}
	TS_STOP("draw all lines");

	#ifndef GL_VERSION_3
	ofPopMatrix();
	#endif
	FONT_STASH_POST_DRAW;

	if(debug){
		ofSetColor(255);
	}
	return offY;
}



float ofxFontStash2::calcWidth(const StyledLine & line){
	float w = 0;
	for(int i = 0; i < line.elements.size(); i++){
		w += line.elements[i].area.width;
	}
	return w;
}

float ofxFontStash2::calcLineHeight(const StyledLine & line){
	float h = 0;
	for(int i = 0; i < line.elements.size(); i++){
		if (line.elements[i].lineHeight > h){
			h = line.elements[i].lineHeight;
		}
	}
	return h;
}


vector<SplitTextBlock>
ofxFontStash2::splitWords( const vector<StyledText> & blocks){

	std::locale loc = std::locale("");

	vector<SplitTextBlock> wordBlocks;
	std::string currentWord;

	for(int i = 0; i < blocks.size(); i++){

		const StyledText & block = blocks[i];
		string blockText = block.text;

		for(auto c: ofUTF8Iterator(blockText)){

			bool isSpace = std::isspace<wchar_t>(c,loc);
			bool isPunct = std::ispunct<wchar_t>(c,loc);
			
			if(isSpace || isPunct){

				if(currentWord.size()){
					SplitTextBlock word = SplitTextBlock(BLOCK_WORD, currentWord, block.style);
					currentWord.clear();
					wordBlocks.push_back(word);
				}
				string separatorText;
				utf8::append(c, back_inserter(separatorText));
				SplitTextBlock separator = SplitTextBlock(isPunct?SEPARATOR:SEPARATOR_INVISIBLE, separatorText, block.style);
				wordBlocks.push_back(separator);

			}else{
				utf8::append(c, back_inserter(currentWord));
			}
		}
		if(currentWord.size()){ //add last word
			SplitTextBlock word = SplitTextBlock(BLOCK_WORD, currentWord, block.style);
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
	if( pixelDensity != 1 ){
		*ascender /= pixelDensity;
		*descender /= pixelDensity;
		*lineH /= pixelDensity;
	}
}


bool ofxFontStash2::applyStyle(const ofxFontStashStyle & style){
	//if(style.fontID.size()){
		fonsClearState(fs);
		int id = getFsID(style.fontID);
		fonsSetFont(fs, id);
		fonsSetSize(fs, style.fontSize * pixelDensity * fontScale);
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

void ofxFontStash2::preFSDraw(){
	//cout << "preFSDraw()" << endl;
	updateFsPrjMatrix();
	//fonstash sets its own shader to draw text- once done, we need OF to enable back its own
	//there's no way to do that in the current API, the hacky workaround is to unbind() an empty shader
	//that we keep around - which end up doing what we ultimatelly want
	nullShader.begin();
}


void ofxFontStash2::postFSDraw(){
	//cout << "postFSDraw()" << endl;
	nullShader.end(); //shader wrap
}
