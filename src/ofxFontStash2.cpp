//
//  ofxFontStash2.cpp
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#include "ofxFontStash2.h"

#define NVG_DISABLE_FACE_CULL_FOR_TRIANGLES

#define FONTSTASH_IMPLEMENTATION
#include "ofx_nanovg.h"
#include "ofx_nanovg_gl.h"
#include "ofx_nanovg_gl_utils.h"
#undef FONTSTASH_IMPLEMENTATION

#if !defined(NANOVG_GL3_IMPLEMENTATION) && !defined(NANOVG_GLES2_IMPLEMENTATION) && !defined(NANOVG_GL2_IMPLEMENTATION)
#error "ofxFontStash2: Please add one of the following definitions to your project NANOVG_GL3_IMPLEMENTATION, NANOVG_GL2_IMPLEMENTATION, NANOVG_GLES2_IMPLEMENTATION"
#endif


#ifdef PROFILE_OFX_FONTSTASH2
	#include "ofxTimeMeasurements.h"
#else
	#pragma push_macro("TS_START_NIF")
	#define TS_START_NIF
	#pragma push_macro("TS_STOP_NIF")
	#define TS_STOP_NIF
	#pragma push_macro("TS_START_ACC")
	#define TS_START_ACC
	#pragma push_macro("TS_STOP_ACC")
	#define TS_STOP_ACC
	#pragma push_macro("TS_START")
	#define TS_START
	#pragma push_macro("TS_STOP")
	#define TS_STOP
#endif

#define OFX_FONSTASH2_CHECK		if(!ctx){\
									ofLogError("ofxFontStash2") << "set me up first!";\
									return;\
								}

#define OFX_FONSTASH2_CHECK_RET		if(!ctx){\
									ofLogError("ofxFontStash2") << "set me up first!";\
									return ret;\
								}


ofxFontStash2::ofxFontStash2(){

	lineHeightMultiplier = 1.0;
	pixelDensity = 1.0;
	fontScale = 1.0;
	drawingFS = false;

}

ofxFontStash2::~ofxFontStash2(){
	#ifdef NANOVG_GL3_IMPLEMENTATION
	ofx_nvgDeleteGL3(ctx);
	#elif defined NANOVG_GL2_IMPLEMENTATION
	ofx_nvgDeleteGL2(ctx);
	#elif defined NANOVG_GLES2_IMPLEMENTATION
	ofx_nvgDeleteGLES2(ctx);
	#endif
}


void ofxFontStash2::setup(){

	bool stencilStrokes = false;
	bool debug = false;

	#ifdef NANOVG_GL3_IMPLEMENTATION
	ctx = ofx_nvgCreateGL23(NVG_ANTIALIAS | (stencilStrokes?NVG_STENCIL_STROKES:0) | (debug?NVG_DEBUG:0));
	#elif NANOVG_GL2_IMPLEMENTATION
	ctx = ofx_nvgCreateGL22(NVG_ANTIALIAS | (stencilStrokes?NVG_STENCIL_STROKES:0) | (debug?NVG_DEBUG:0));
	#elif defined NANOVG_GLES2_IMPLEMENTATION
	ctx = ofx_nvgCreateGL2ES2(NVG_ANTIALIAS | (stencilStrokes?NVG_STENCIL_STROKES:0) | (debug?NVG_DEBUG:0));
	#endif

	if (!ctx) {
		ofLogError("ofxFontStash2") << "error creating nanovg context";
		return;
	}
}


bool ofxFontStash2::addFont(const string& fontID, const string& fontFile){

	bool ret = false;
	OFX_FONSTASH2_CHECK_RET

	int id = ofx_nvgCreateFont(ctx, fontID.c_str(), ofToDataPath(fontFile).c_str());
	if(id != FONS_INVALID){
		fontIDs[fontID] = id;
		return true;
	}else{
		ofLogError("ofxFontStash2") << "Could not load font '" << fontFile << "'";
		return false;
	}
}


bool ofxFontStash2::isFontLoaded(const string& fontID){
	auto iter = fontIDs.find( fontID );
	return iter != fontIDs.end();
}


void ofxFontStash2::addStyle(const string& styleID, ofxFontStashStyle style){
	styleIDs[styleID] = style;
}


void ofxFontStash2::begin(){
	OFX_FONSTASH2_CHECK
	//NanoVG sets its own shader to draw text- once done, we need OF to enable back its own
	//there's no way to do that in the current API, the hacky workaround is to unbind() an empty shader
	//that we keep around - which end up doing what we ultimatelly want
	nullShader.begin();
	ofx_nvgBeginFrame(ctx, ofGetViewportWidth(), ofGetViewportHeight(), pixelDensity);
	applyOFMatrix();
	//ofLogNotice("ofxFontStash2") << "begin() " << ofGetFrameNum();
}

void ofxFontStash2::end(){
	OFX_FONSTASH2_CHECK
	ofx_nvgEndFrame(ctx);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	nullShader.end(); //shader wrap
}


float ofxFontStash2::draw(const string& text, const ofxFontStashStyle& style, float x, float y){
	float ret = 0;
	OFX_FONSTASH2_CHECK_RET
	begin();
		ofRectangle bounds = getTextBounds(text, style, x, y);
		//applyStyle(style); //getTextBounds already applies style
		float dx = ofx_nvgText(ctx, x, y, text.c_str(), NULL); //TODO dx is bugged? why?
	end();
	return bounds.width;
}


float ofxFontStash2::drawFormatted(const string& styledText, float x, float y){
	float ret = 0;
	OFX_FONSTASH2_CHECK_RET
	vector<StyledText> blocks = ofxFontStashParser::parseText(styledText, styleIDs);
	float xx = x;
	float yy = y;
	for(int i = 0; i < blocks.size(); i++){
		xx += draw(blocks[i].text, blocks[i].style, xx, yy);
	}

	return xx-x;
}


ofRectangle ofxFontStash2::drawColumn(const string& text,
									  const ofxFontStashStyle &style,
									  float x, float y, float targetWidth,
									  ofAlignHorz horAlign,
									  bool debug){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	vector<StyledText> blocks;
	blocks.push_back((StyledText){text, style});
	return drawAndLayout(blocks, x, y, targetWidth, horAlign, debug);
}


ofRectangle ofxFontStash2::drawFormattedColumn(const string& styledText,
											   float x, float y, float targetWidth,
											   ofAlignHorz horAlign,
											   bool debug){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	if (targetWidth < 0) return ofRectangle();
	
	TS_START_NIF("parse text");
	vector<StyledText> blocks = ofxFontStashParser::parseText(styledText, styleIDs);
	TS_STOP_NIF("parse text");
	
	return drawAndLayout(blocks, x, y, targetWidth, horAlign, debug);
}


ofRectangle ofxFontStash2::drawAndLayout(vector<StyledText> &blocks,
										 float x, float y, float width,
										 ofAlignHorz align, bool debug){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	return drawLines(layoutLines(blocks, width, align, debug), x, y, align, debug);
};


const vector<StyledLine> ofxFontStash2::layoutLines(const vector<StyledText> &blocks,
													float targetWidth,
													ofAlignHorz horAlign,
													bool debug){
	vector<StyledLine> ret;
	OFX_FONSTASH2_CHECK_RET
	if (targetWidth < 0) return vector<StyledLine>();
	float x = 0;
	float y = 0;
	float xx = x;
	float yy = y;

	TS_START_NIF("layoutLines");
	TS_START_NIF("split words");
	vector<TextBlock> words = splitWords(blocks);
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
	float currentWordLineH = 0;
	
	float bounds[4];
	float dx;
	LineElement le;
	float accumulatedLineDiff = 0;
	vector<float> accLineHeighDiff = {0};

	for(int i = 0; i < words.size(); i++){
		StyledLine &currentLine = lines.back();
		currentLine.boxW = targetWidth;
		
		TS_START_ACC("word style");
		if(words[i].styledText.style.valid && currentStyle != words[i].styledText.style ){
			currentStyle = words[i].styledText.style;
			if(applyStyle(currentStyle)){
				ofx_nvgTextMetrics(ctx, NULL, NULL, &currentWordLineH);
			}else{
				ofLogError("ofxFontStash2") << "no style font defined!";
			}
		}
		TS_STOP_ACC("word style");
		
		bool stayOnCurrentLine = true;
		
		if( words[i].type == SEPARATOR_INVISIBLE && words[i].styledText.text == "\n" ){ //line break
			stayOnCurrentLine = false;
			dx = 0;
			
			// add a zero-width enter mark. this is used to keep track
			// of the vertical spacing of empty lines.
			le = LineElement(words[i], ofRectangle(xx,yy,0,currentWordLineH));
			le.baseLineY = yy;
			le.x = xx;
			le.lineHeight = currentWordLineH;
			currentLine.elements.push_back(le);
			
			float lineH = lineHeightMultiplier * currentStyle.lineHeightMult * calcLineHeight(currentLine);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x + dx;

			if (lines.size() > 1){
				accumulatedLineDiff += lineH - lines[lines.size()-2].lineH;
			}
			accLineHeighDiff.push_back(accumulatedLineDiff);


			yy += lineH;
			
			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;
			lines.push_back(StyledLine());
			continue;

		}else{ //non-linebreaks

			TS_START_ACC("fonsTextBounds");
			dx = ofx_nvgTextBounds(ctx,
								xx,
								yy,
								words[i].styledText.text.c_str(),
								NULL,
								&bounds[0]
								);

			TS_STOP_ACC("fonsTextBounds");

			// make tabs the specified number of spaces wide
			if(words[i].type == SEPARATOR_INVISIBLE && words[i].styledText.text == "\t" && tabWidth != 1){
				float space4 = tabWidth * dx; // usual case is 4 spaces
				float space3 = (tabWidth-1) * dx;
				float toNextTab = space4 - fmodf(bounds[2] + space3, space4);
				float inc = dx / 2 < toNextTab ? (toNextTab - dx) : (toNextTab + space4 - dx);
				bounds[2] += inc;
				dx += inc;
			}

			le = LineElement(words[i], ofRectangle(bounds[0], bounds[1], dx, bounds[3] - bounds[1]));
			le.baseLineY = yy;
			le.x = xx;
			le.lineHeight = currentWordLineH;
			
			float nextWidth = lineWidth + dx;
			
			//if not wider than targetW
			// ||
			//this is the 1st word in this line but even that doesnt fit
			stayOnCurrentLine = nextWidth < targetWidth || (wordsThisLine == 0 && (nextWidth >= targetWidth));
		}
		
		if (stayOnCurrentLine){

			TS_START_ACC("stay on line");
			currentLine.elements.push_back(le);
			lineWidth += dx;
			xx += dx;
			wordsThisLine++;
			TS_STOP_ACC("stay on line");

		} else if( words[i].type == SEPARATOR_INVISIBLE && words[i].styledText.text == " " ){

			// ignore spaces when moving into the next line!
			continue;

		} else{ //too long, start a new line
			
			TS_START_ACC("new line");
			//calc height for this line - taking in account all words in the line
			float lineH = lineHeightMultiplier * currentStyle.lineHeightMult * calcLineHeight(currentLine);
			if (lines.size() > 1){
				accumulatedLineDiff += lineH - lines[lines.size()-2].lineH;
			}
			accLineHeighDiff.push_back(accumulatedLineDiff);

			currentLine.lineH = lineH;
			currentLine.lineW = xx - x;
			i--; //re-calc dimensions of this word on a new line!
			yy += lineH;
			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;
			lines.push_back(StyledLine());
			TS_STOP_ACC("new line");
		}
	}

	TS_START_ACC("last line");
	// update dimensions of the last line
	StyledLine &currentLine = lines.back();
	currentLine.boxW = targetWidth;
	if( currentLine.elements.size() == 0 ){
		// but at least one spacing character, so we have a line height.
		le = LineElement(TextBlock(SEPARATOR_INVISIBLE,"",currentStyle), ofRectangle(xx,yy,0,currentWordLineH));
		le.baseLineY = yy;
		le.x = xx;
		le.lineHeight = currentWordLineH;
		currentLine.elements.push_back(le);
	}
	
	float lineH = lineHeightMultiplier * currentStyle.lineHeightMult * calcLineHeight(currentLine);
	currentLine.lineH = lineH;
	currentLine.lineW = xx - x;
	if (lines.size() > 1){
		accumulatedLineDiff += lineH - lines[lines.size()-2].lineH;
	}
	accLineHeighDiff.push_back(accumulatedLineDiff);

	//now that we laid out all the lines and we know all the lineHs, lets re.adjust their line base y.

	for(int i = 0; i < lines.size(); i++){
		for(int j = 0; j < lines[i].elements.size(); j++){
			auto & el = lines[i].elements[j];
			el.area.y += accLineHeighDiff[i+1];
		}
	}



	//on right aligned layouts, remove the last space or similar so that right alight really kisses the edge
	if(horAlign == OF_ALIGN_HORZ_RIGHT || horAlign == OF_ALIGN_HORZ_CENTER){

		for(auto & l : lines){
			if(l.elements.size() > 0){
				if(l.elements.back().content.type == SEPARATOR_INVISIBLE){
					float lastInvW = l.elements.back().area.width;
					for(auto & el : l.elements){
						el.x += lastInvW;
						el.area.x += lastInvW;
					}
					l.elements.erase(l.elements.begin() + l.elements.size() - 1);
					l.boxW -= lastInvW;
					//l.lineW -= lastInvW;
				}
			}
			float lineOffset = 0.0f;
			if(horAlign == OF_ALIGN_HORZ_RIGHT) lineOffset = targetWidth - l.lineW;
			if(horAlign == OF_ALIGN_HORZ_CENTER) lineOffset = (targetWidth - l.lineW) / 2.0f;
			for(auto & el : l.elements){
				el.x += lineOffset;
				el.area.x += lineOffset;
			}
		}
	}

	TS_STOP_ACC("last line");

	TS_STOP_NIF("layoutLines");
	return lines; 
}


ofRectangle ofxFontStash2::drawLines(const vector<StyledLine> &lines, float x, float y, ofAlignHorz horAlign, bool debug){

	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	ofVec2f offset;
	offset.x = x;
	offset.y = y;

	//debug line heights!
	if(debug){
		TS_START("draw line Heights");
		float yy = 0;
		ofSetColor(255,0,255,128);
		ofDrawCircle(offset.x, offset.y, 2.5);
		for( int i = 0; i < lines.size(); i++){
			ofSetColor(255,45);
			ofDrawLine(offset.x - 10, offset.y + yy , offset.x + lines[i].lineW , offset.y + yy );
			if(i < lines.size()){
				yy += lines[i+1].lineH;
			}
		}
		TS_STOP("draw line Heights");
	}

	float yy = y; //we will increment yy as we draw lines
	ofxFontStashStyle drawStyle;
	drawStyle.fontSize = -1;

	// debug /////////////////////////////
	struct ColoredRect{
		ofRectangle rect;
		ofColor color;
	};
	vector<ColoredRect> debugRects;
	/////////////////////////////////////


	TS_START("draw all lines");

	begin();
	for(int i = 0; i < lines.size(); i++){
		yy += lines[i].lineH;
		for(int j = 0; j < lines[i].elements.size(); j++){

			if(lines[i].elements[j].content.type != SEPARATOR_INVISIBLE ){ //no need to draw the invisible chars

				const StyledLine &l = lines[i];
				const LineElement &el = l.elements[j];

				if (el.content.styledText.style.valid && drawStyle != el.content.styledText.style ){
					drawStyle = el.content.styledText.style;
					TS_START_ACC("applyStyle");
					applyStyle(drawStyle);
					TS_STOP_ACC("applyStyle");
				}

				TS_START_ACC("fonsDrawText");

				float dx = ofx_nvgText(ctx,
								   el.x + offset.x,
								   (el.baseLineY + l.lineH - lines[0].lineH) + offset.y,
								   el.content.styledText.text.c_str(),
								   NULL);
				TS_STOP_ACC("fonsDrawText");
			}
			if(debug){ //draw rects on top of each block type
				ColoredRect cr;
				switch(lines[i].elements[j].content.type){
					case WORD: cr.color = ofColor(255, 0, 0, 30); break;
					case SEPARATOR: cr.color = ofColor(0, 0, 255, 60); break;
					case SEPARATOR_INVISIBLE: cr.color = ofColor(0, 255, 255, 30); break;
				}
				cr.rect = lines[i].elements[j].area;
				debugRects.push_back(cr);
			}
		}
	}
	end();
	
	if(debug){ //draw debug rects on top of each word
		for(auto & cr : debugRects){
			ofSetColor(cr.color);
			ofDrawRectangle(cr.rect.x + offset.x,
							cr.rect.y + offset.y,
							cr.rect.width,
							cr.rect.height
							);
		}
	}

	TS_STOP("draw all lines");

	if(debug){
		ofSetColor(255);
	}
	//return yy - lines.back().elements.back().lineHeight - y; //todo!
	ofRectangle bounds = getTextBounds(lines, x, y);
	bounds.x += x;
	bounds.y += y;
	end();
	return bounds;
}


ofRectangle ofxFontStash2::getTextBounds(const vector<StyledLine> &lines, float x, float y){

	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	for(auto & l : lines){
		for(auto & e : l.elements){
			if(e.content.type == WORD){
				ret = ret.getUnion(e.area);
			}
		}
	}
	return ret;
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


vector<TextBlock>
ofxFontStash2::splitWords( const vector<StyledText> & blocks){

	std::locale loc = std::locale("");

	vector<TextBlock> wordBlocks;
	std::string currentWord;

	for(int i = 0; i < blocks.size(); i++){

		const StyledText & block = blocks[i];
		string blockText = block.text;

		for(auto c: ofUTF8Iterator(blockText)){

			bool isSpace = std::isspace<wchar_t>(c,loc);
			bool isPunct = std::ispunct<wchar_t>(c,loc);
			
			if(isSpace || isPunct){

				if(currentWord.size()){
					TextBlock word = TextBlock(WORD, currentWord, block.style);
					currentWord.clear();
					wordBlocks.push_back(word);
				}
				string separatorText;
				utf8::append(c, back_inserter(separatorText));
				TextBlock separator = TextBlock(isPunct?SEPARATOR:SEPARATOR_INVISIBLE, separatorText, block.style);
				wordBlocks.push_back(separator);

			}else{
				utf8::append(c, back_inserter(currentWord));
			}
		}
		if(currentWord.size()){ //add last word
			TextBlock word = TextBlock(WORD, currentWord, block.style);
			currentWord.clear();
			wordBlocks.push_back(word);
		}
	}
	return wordBlocks;
}


ofRectangle ofxFontStash2::getTextBounds( const string &text, const ofxFontStashStyle &style, const float x, const float y ){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	applyStyle(style);
	float bounds[4]={0,0,0,0};

	float advance = ofx_nvgTextBounds( ctx, x, y, text.c_str(), NULL, bounds );
	// here we use the "text advance" instead of the width of the rectangle,
	// because this includes spaces at the end correctly (the text bounds "x" and "x " are the same,
	// the text advance isn't). 
	return ofRectangle(bounds[0],bounds[1],advance,bounds[3]-bounds[1]);
}


void ofxFontStash2::getVerticalMetrics(const ofxFontStashStyle & style, float* ascender, float* descender, float* lineH){
	OFX_FONSTASH2_CHECK
	applyStyle(style);
	ofx_nvgTextMetrics(ctx, ascender, descender, lineH);
}


bool ofxFontStash2::applyStyle(const ofxFontStashStyle & style){
	bool ret;
	OFX_FONSTASH2_CHECK_RET
	int id = getFsID(style.fontID);
	if(id != FONS_INVALID){
		ofx_nvgFontFaceId(ctx, id);
		ofx_nvgFontSize(ctx, style.fontSize * fontScale);
		ofx_nvgFillColor(ctx, toFScolor(style.color));
		ofx_nvgTextAlign(ctx, style.alignmentV);
		ofx_nvgFontBlur(ctx, style.blur);
		return true;
	}
	return false;
}


int ofxFontStash2::getFsID(const string& userFontID){

	map<string, int>::iterator it = fontIDs.find(userFontID);
	if(it != fontIDs.end()){
		return it->second;
	}
	ofLogError("ofxFontStash2") << "Invalid Font ID: " << userFontID;
	return FONS_INVALID;
}


NVGcolor ofxFontStash2::toFScolor(const ofColor & c){
	return NVGcolor{c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f };
}

vector<StyledText> ofxFontStash2::parseStyledText(const string & styledText){
	return  ofxFontStashParser::parseText(styledText, styleIDs);
}


void ofxFontStash2::applyOFMatrix(){ //from ofxNanoVG

	OFX_FONSTASH2_CHECK
	ofMatrix4x4 ofMatrix = ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
	ofVec2f viewSize = ofVec2f(ofGetViewportWidth(), ofGetViewportHeight());

	ofVec2f translate = ofVec2f(ofMatrix(3, 0), ofMatrix(3, 1)) + viewSize/2;
	ofVec2f scale(ofMatrix(0, 0), ofMatrix(1, 1));
	ofVec2f skew(ofMatrix(0, 1), ofMatrix(1, 0));

	// handle OF style vFlipped inside FBO
	if (ofGetCurrentRenderer()->getCurrentOrientationMatrix()[1][1] == 1) {
		translate.y = ofGetViewportHeight() - translate.y;
		scale.y *= -1;
		skew.y *= -1;
	}

	ofx_nvgResetTransform(ctx);
	ofx_nvgTransform(ctx, scale.x, -skew.y, -skew.x, scale.y, translate.x, translate.y);
}


#ifndef PROFILE_OFX_FONTSTASH2
	#pragma pop_macro("TS_START_NIF")
	#pragma pop_macro("TS_STOP_NIF")
	#pragma pop_macro("TS_START_ACC")
	#pragma pop_macro("TS_STOP_ACC")
	#pragma pop_macro("TS_START")
	#pragma pop_macro("TS_STOP")
#endif
