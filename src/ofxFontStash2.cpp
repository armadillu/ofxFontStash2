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
#include "ofxfs2_nanovg.h"
#include "ofxfs2_nanovg_gl.h"
#undef FONTSTASH_IMPLEMENTATION

#if !defined(NANOVG_GL3_IMPLEMENTATION) && !defined(NANOVG_GL2_IMPLEMENTATION) && !defined(NANOVG_GLES2_IMPLEMENTATION)
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
	#pragma push_macro("TS_START_ACC_NIF")
	#define TS_START_ACC_NIF
	#pragma push_macro("TS_STOP_ACC_NIF")
	#define TS_STOP_ACC_NIF
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

using namespace ofxFontStash2;
using namespace std;

Fonts::Fonts(){

	lineHeightMultiplier = 1.0;
	pixelDensity = 1.0;
	fontScale = 1.0;
}

Fonts::~Fonts(){
	#ifdef NANOVG_GL3_IMPLEMENTATION
	ofxfs2_nvgDeleteGL3(ctx);
	#elif defined NANOVG_GL2_IMPLEMENTATION
	ofxfs2_nvgDeleteGL2(ctx);
	#elif defined NANOVG_GLES2_IMPLEMENTATION
	ofxfs2_nvgDeleteGLES2(ctx);
	#endif
}


void Fonts::setup(bool debug){

	bool stencilStrokes = false;

	#ifdef NANOVG_GL3_IMPLEMENTATION
	ctx = ofxfs2_nvgCreateGL23(/*NVG_ANTIALIAS | */(stencilStrokes?NVG_STENCIL_STROKES:0) | (debug?NVG_DEBUG:0));
	#elif NANOVG_GL2_IMPLEMENTATION
	ctx = ofxfs2_nvgCreateGL22(/*NVG_ANTIALIAS | */(stencilStrokes?NVG_STENCIL_STROKES:0) | (debug?NVG_DEBUG:0));
	#elif defined NANOVG_GLES2_IMPLEMENTATION
	ctx = ofxfs2_nvgCreateGL2ES2(/*NVG_ANTIALIAS | */(stencilStrokes?NVG_STENCIL_STROKES:0) | (debug?NVG_DEBUG:0));
	#endif

	if (!ctx) {
		ofLogError("ofxFontStash2") << "Error creating nanovg context";
	}
}


bool Fonts::addFont(const string& fontID, const string& fontFile){

	bool ret = false;
	OFX_FONSTASH2_CHECK_RET

	ofLogNotice("ofxFontStash2") << "Adding font with ID '" << fontID << "' from file '" << fontFile << "'";
	int id = ofxfs2_nvgCreateFont(ctx, fontID.c_str(), ofToDataPath(fontFile).c_str());
	if(id != FONS_INVALID){
		fontIDs[fontID] = id;
		if(globalFallbackFontID != ""){
			addFallbackFont(fontID, globalFallbackFontID); 
		}
		return true;
	}else{
		ofLogError("ofxFontStash2") << "Could not load font from path \"" << fontFile << "\"";
		return false;
	}
}


bool Fonts::isFontLoaded(const string& fontID){
	auto iter = fontIDs.find( fontID );
	return iter != fontIDs.end();
}

vector<string> Fonts::getFontIDs(){
	vector<string> ids;
	for(auto & id : fontIDs){
		ids.emplace_back(id.first);
	}
	return ids;
}


bool Fonts::addStyle(const string& styleID, const Style & style){
	if ( find(reservedStyleNames.begin(), reservedStyleNames.end(), styleID) == reservedStyleNames.end()){
		auto it = styleIDs.find(styleID);
		if(it != styleIDs.end()){
			ofLogWarning("ofxFontStash2") << "Style with ID \"" << styleID << "\" already existed! Updating it with these contents: \"" << style.toString() << "\"";
		}
		styleIDs[styleID] = style;
		ofLogNotice("ofxFontStash2") << "Adding Style with ID \"" << styleID << "\" : \"" << style.toString() << "\"";
		return true;
	}else{
		ofLogError("ofxFontStash2") << "Can't create a style with an ID \"" << styleID << "\"; This is a reserved tag name";
		return false;
	}
}

bool Fonts::removeStyle(const string& styleID){
	auto it = styleIDs.find(styleID);
	if(it != styleIDs.end()){
		styleIDs.erase(it);
		ofLogNotice("ofxFontStash2") << "Removing Style with ID \"" << styleID << "\"";
		return true;
	}else{
		ofLogError("ofxFontStash2") << "Can't remove Style with ID \"" << styleID << "\" bc it doesn't exist";
		return false;
	}
}


bool Fonts::styleExists(const string& styleID){
	return styleIDs.find(styleID) != styleIDs.end();
}


ofxFontStash2::Style Fonts::getStyle(const string& styleID, bool * exists){
	auto it = styleIDs.find(styleID);
	if(it != styleIDs.end()){
		if(exists) *exists = true;
		return it->second;
	}
	if(exists) *exists = false;
	return ofxFontStash2::Style();
}


void Fonts::beginBatch(){
	if(!inBatchMode){
		begin();
		inBatchMode = true;
	}else{
		ofLogError("ofxFontStash2") << "Can't beginBatch() again - dont nest batches!";
	}
}

void Fonts::endBatch(){
	if(inBatchMode){
		inBatchMode = false;
		end();
	}else{
		ofLogError("ofxFontStash2") << "Can't end batch bc you havent started one!!";
	}
}

void Fonts::begin(){
	if(!inBatchMode){
		#if defined(NANOVG_GL3_IMPLEMENTATION) || defined(NANOVG_GLES2_IMPLEMENTATION)
		//NanoVG sets its own shader to draw text- once done, we need OF to enable back its own
		//there's no way to do that in the current API, the hacky workaround is to unbind() an empty shader
		//that we keep around - which end up doing what we ultimatelly want
		nullShader.begin();
		#endif
		ofxfs2_nvgBeginFrame(ctx, ofGetViewportWidth(), ofGetViewportHeight(), pixelDensity);
		applyOFMatrix();
	}
}

void Fonts::end(){
	if(!inBatchMode){
		//TS_START("endFrame");
		ofxfs2_nvgEndFrame(ctx);
		//TS_STOP("endFrame");
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//ofEnableAlphaBlending();
		#if defined(NANOVG_GL3_IMPLEMENTATION) || defined(NANOVG_GLES2_IMPLEMENTATION)
		nullShader.end(); //shader wrap
		#endif
	}
}


float Fonts::draw(const string& text, const Style& style, float x, float y){
	float ret = 0;
	OFX_FONSTASH2_CHECK_RET
	if(!inBatchMode){begin();}
	ofRectangle bounds = getTextBounds(text, style, x, y);
	//applyStyle(style); //getTextBounds already applies style
	float newX = ofxfs2_nvgText(ctx, x, y, text.c_str(), NULL);
	if(!inBatchMode){end();}
	//return newX;
	return bounds.width;
}


float Fonts::drawFormatted(const string& styledText, float x, float y){
	float ret = 0;
	OFX_FONSTASH2_CHECK_RET
	vector<StyledText> blocks;
	Parser::parseText(styledText, styleIDs, defaultStyleID, blocks);
	float xx = x;
	float yy = y;
	if(!inBatchMode){begin();}
	for(auto & b : blocks){
		xx += draw(b.text, b.style, xx, yy);
	}
	if(!inBatchMode){end();}
	return xx - x;
}


ofRectangle Fonts::drawColumn(const string& text,
									  const Style &style,
									  float x, float y, float targetWidth,
									  ofAlignHorz horAlign,
									  bool debug){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	vector<StyledText> blocks;
	blocks.emplace_back(StyledText({text, style}));
	return drawAndLayout(blocks, x, y, targetWidth, horAlign, debug);
}

void Fonts::drawColumnNVG(const string& text,
								  const Style& style,
								  float x, float y, float width,
								  ofAlignHorz horAlign){
	begin();
	applyStyle(style);
	NVGalign hAlign = NVGalign(0);
	switch(horAlign){
		case OF_ALIGN_HORZ_LEFT: hAlign = NVG_ALIGN_LEFT; break;
		case OF_ALIGN_HORZ_CENTER: hAlign = NVG_ALIGN_CENTER; break;
		case OF_ALIGN_HORZ_RIGHT: hAlign = NVG_ALIGN_RIGHT; break;
		default: break;
	}
	ofxfs2_nvgTextAlign(ctx, style.alignmentV | hAlign);
	ofxfs2_nvgTextLineHeight(ctx, style.lineHeightMult * lineHeightMultiplier);
	ofxfs2_nvgTextBox(ctx, x, y, width, text.c_str(), NULL);
	ofxfs2_nvgTextLineHeight(ctx, 1.0);
	end();
}

ofRectangle Fonts::getTextBoundsNVG(const string& text,
											const Style& style,
											float x, float y, float width,
											ofAlignHorz horAlign){
	applyStyle(style);
	NVGalign hAlign = NVGalign(0);
	switch(horAlign){
		case OF_ALIGN_HORZ_LEFT: hAlign = NVG_ALIGN_LEFT; break;
		case OF_ALIGN_HORZ_CENTER: hAlign = NVG_ALIGN_CENTER; break;
		case OF_ALIGN_HORZ_RIGHT: hAlign = NVG_ALIGN_RIGHT; break;
		default: break;
	}
	ofxfs2_nvgTextAlign(ctx, style.alignmentV | hAlign);
	ofxfs2_nvgTextLineHeight(ctx, style.lineHeightMult * lineHeightMultiplier);
	float bounds[4];
	ofxfs2_nvgTextBoxBounds(ctx, x, y, width, text.c_str(), NULL, bounds);
	ofxfs2_nvgTextLineHeight(ctx, 1.0);
	return ofRectangle(bounds[0], bounds[1], bounds[2]-bounds[0], bounds[3]-bounds[1]);
}


ofRectangle Fonts::drawFormattedColumn(	const string& styledText,
									   	float x, float y, float targetWidth,
										ofAlignHorz horAlign,
										bool debug){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	if (targetWidth < 0) return ofRectangle();
	
	TS_START_ACC_NIF("parse text");
	vector<StyledText> blocks;
	Parser::parseText(styledText, styleIDs, defaultStyleID, blocks);
	TS_STOP_ACC_NIF("parse text");
	
	return drawAndLayout(blocks, x, y, targetWidth, horAlign, debug);
}


ofRectangle Fonts::drawAndLayout(const vector<StyledText> &blocks,
								float x, float y, float width,
								ofAlignHorz align, bool debug){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	vector<StyledLine> lines;
	layoutLines(blocks, width, lines, align, 0, debug);
	return drawLines(lines, x, y, align, debug);
};


void Fonts::layoutLines(const vector<StyledText> &blocks,
						float targetWidth,
						vector<StyledLine> & outputLines,
						ofAlignHorz horAlign,
						int limitToNLines,
						bool debug){
	vector<StyledLine> ret;
	OFX_FONSTASH2_CHECK;
	if (targetWidth < 0) return;
	float x = 0;
	float y = 0;
	float xx = x;
	float yy = y;

	TS_START_NIF("layoutLines");
	TS_START_NIF("split words");
	vector<TextBlock> words;
	splitWords(blocks, words);
	TS_STOP_NIF("split words");
	
	if (words.size() == 0) return;

	// here we create the first line. a few things to note:
	// - in general, like in a texteditor, the line exists first, then content is added to it.
	// - 'line' here refers to the visual representation. even a line with no newline (\n) can span multiple lines
	outputLines.emplace_back(StyledLine());
	
	Style currentStyle;
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

		if(limitToNLines != 0 && outputLines.size() > limitToNLines){ //stop early if we reached max N of lines
			break;
		}
		StyledLine & currentLine = outputLines.back();
		currentLine.boxW = targetWidth;

		TS_START_ACC("word style");
		if(currentStyle != words[i].styledText.style ){
			currentStyle = words[i].styledText.style;
			if(applyStyle(currentStyle)){
				ofxfs2_nvgTextMetrics(ctx, NULL, NULL, &currentWordLineH);
			}else{
				ofLogError("ofxFontStash2") << "No style font defined!";
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
			currentLine.elements.emplace_back(le);
			
			float lineH = lineHeightMultiplier * currentStyle.lineHeightMult * calcLineHeight(currentLine);
			currentLine.lineH = lineH;
			currentLine.lineW = xx - x + dx;

			if (outputLines.size() > 1){
				accumulatedLineDiff += lineH - outputLines[outputLines.size()-2].lineH;
			}
			accLineHeighDiff.emplace_back(accumulatedLineDiff);


			yy += lineH;
			
			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;
			outputLines.emplace_back(StyledLine());
			continue;

		}else{ //non-linebreaks

			TS_START_ACC("fonsTextBounds");
			dx = ofxfs2_nvgTextBounds(ctx,
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
			currentLine.elements.emplace_back(le);
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
			if (outputLines.size() > 1){
				accumulatedLineDiff += lineH - outputLines[outputLines.size()-2].lineH;
			}
			accLineHeighDiff.emplace_back(accumulatedLineDiff);

			currentLine.lineH = lineH;
			currentLine.lineW = xx - x;
			i--; //re-calc dimensions of this word on a new line!
			yy += lineH;
			lineWidth = 0;
			wordsThisLine = 0;
			xx = x;

			outputLines.emplace_back(StyledLine());
			TS_STOP_ACC("new line");
		}
	}

	TS_START_ACC("last line");
	// update dimensions of the last line
	StyledLine &currentLine = outputLines.back();
	currentLine.boxW = targetWidth;
	if( currentLine.elements.size() == 0 ){
		// but at least one spacing character, so we have a line height.
		le = LineElement(TextBlock(SEPARATOR_INVISIBLE,"",currentStyle), ofRectangle(xx,yy,0,currentWordLineH));
		le.baseLineY = yy;
		le.x = xx;
		le.lineHeight = currentWordLineH;
		currentLine.elements.emplace_back(le);
	}
	
	float lineH = lineHeightMultiplier * currentStyle.lineHeightMult * calcLineHeight(currentLine);
	currentLine.lineH = lineH;
	currentLine.lineW = xx - x;
	if (outputLines.size() > 1){
		accumulatedLineDiff += lineH - outputLines[outputLines.size()-2].lineH;
	}
	accLineHeighDiff.emplace_back(accumulatedLineDiff);

	//now that we laid out all the lines and we know all the lineHs, lets re.adjust their line base y.

	for(int i = 0; i < outputLines.size(); i++){
		for(int j = 0; j < outputLines[i].elements.size(); j++){
			auto & el = outputLines[i].elements[j];
			el.area.y += accLineHeighDiff[i+1];
		}
	}

	//on right aligned layouts, remove the last space or similar so that right alight really kisses the edge
	if(horAlign == OF_ALIGN_HORZ_RIGHT || horAlign == OF_ALIGN_HORZ_CENTER){

		for(auto & l : outputLines){
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
}


ofRectangle Fonts::drawLines(const vector<StyledLine> &lines, float x, float y, ofAlignHorz horAlign, bool debug){

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
			if(i + 1 < lines.size()){
				yy += lines[i+1].lineH;
			}
		}
		TS_STOP("draw line Heights");
	}
	
	float yy = y; //we will increment yy as we draw lines
	Style drawStyle;
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

				if (drawStyle != el.content.styledText.style ){
					drawStyle = el.content.styledText.style;
					applyStyle(drawStyle);
				}

				float dx = ofxfs2_nvgText(ctx,
								   el.x + offset.x,
								   (el.baseLineY + l.lineH - lines[0].lineH) + offset.y,
								   el.content.styledText.text.c_str(),
								   NULL);
			}

			if(debug){ //draw rects on top of each block type
				ColoredRect cr;
				switch(lines[i].elements[j].content.type){
					case BLOCK_WORD: cr.color = ofColor(255, 0, 0, 30); break;
					case SEPARATOR: cr.color = ofColor(0, 0, 255, 60); break;
					case SEPARATOR_INVISIBLE: cr.color = ofColor(0, 255, 255, 30); break;
				}
				cr.rect = lines[i].elements[j].area;
				debugRects.emplace_back(cr);
			}
		}
	}
	end();
	
	if(debug){ //draw debug rects on top of each word
		for(auto & cr : debugRects){
			ofSetColor(cr.color);
			ofDrawRectangle(cr.rect.x + offset.x, cr.rect.y + offset.y, cr.rect.width, cr.rect.height);
		}
	}

	TS_STOP("draw all lines");

	if(debug){
		ofSetColor(255);
	}
	//return yy - lines.back().elements.back().lineHeight - y; //todo!
	ofRectangle bounds = getTextBounds(lines, x, y);
	//end();
	return bounds;
}


ofRectangle Fonts::getTextBounds(const vector<StyledLine> &lines, float x, float y){

	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	for(const auto & l : lines){
		for(const auto & e : l.elements){
			if(e.content.type == BLOCK_WORD){
				ret.growToInclude(e.area);
			}
		}
	}
	ret.x += x;
	ret.y += y;
	return ret;
}


float Fonts::calcWidth(const StyledLine & line){
	float w = 0;
	for(const auto & el : line.elements){
		w += el.area.width;
	}
	return w;
}


float Fonts::calcLineHeight(const StyledLine & line){
	float h = 0;
	for(auto & e : line.elements){
		if (e.lineHeight > h){
			h = e.lineHeight;
		}
	}
	return h;
}


void Fonts::splitWords(const vector<StyledText> & blocks, vector<TextBlock> & wordBlocks){

	static std::locale loc = std::locale("");
	static std::string currentWord;

	for(const auto & block : blocks){

		for(auto c : ofUTF8Iterator(block.text)){

			bool isSpace = std::isspace<wchar_t>(c,loc);
			//bool isPunct = std::ispunct<wchar_t>(c,loc);
			
			if(isSpace /*|| isPunct*/){

				if(currentWord.size()){
					wordBlocks.emplace_back(TextBlock(BLOCK_WORD, currentWord, block.style));
					currentWord.clear();
				}
				string separatorText;
				utf8::append(c, back_inserter(separatorText));
				wordBlocks.emplace_back(TextBlock(/*isPunct?SEPARATOR:*/SEPARATOR_INVISIBLE, separatorText, block.style));

			}else{
				utf8::append(c, back_inserter(currentWord));
			}
		}
		if(currentWord.size()){ //add last & first word
			wordBlocks.emplace_back(TextBlock(BLOCK_WORD, currentWord, block.style));
			currentWord.clear();
		}
	}
}


ofRectangle Fonts::getTextBounds( const string & text, const Style & style, const float x, const float y ){
	ofRectangle ret;
	OFX_FONSTASH2_CHECK_RET
	applyStyle(style);
	float bounds[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	float advance = ofxfs2_nvgTextBounds( ctx, x, y, text.c_str(), NULL, bounds );
	// here we use the "text advance" instead of the width of the rectangle,
	// because this includes spaces at the end correctly (the text bounds "x" and "x " are the same,
	// the text advance isn't). 
	return ofRectangle(bounds[0], bounds[1], advance, bounds[3]-bounds[1]);
}


void Fonts::getVerticalMetrics(const Style & style, float* ascender, float* descender, float* lineH){
	OFX_FONSTASH2_CHECK
	applyStyle(style);
	ofxfs2_nvgTextMetrics(ctx, ascender, descender, lineH);
}


void Fonts::setGlobalFallbackFont(const string& fallbackFontID){
	// first remove the existing font
	if(globalFallbackFontID != ""){
		// We can't, because afaik fontstash has no way of doing that yet
		// and i don't want to go to deep into their datastructures (??)
		ofLogWarning("ofxFontStash2") << "Setting the global fallback font multiple times is currently not supported. Funny behavior might arise";
	}
	
	int fallbackFontID_fs = fontIDs[fallbackFontID];
	for(auto & font : fontIDs){
		if(font.second != fallbackFontID_fs){
			ofxfs2_nvgAddFallbackFontId(ctx, font.second, fallbackFontID_fs);
		}
	}
	globalFallbackFontID = fallbackFontID; 
}


string Fonts::getGlobalFallbackFont(){
	return globalFallbackFontID;
}


void Fonts::addFallbackFont(const string& fontID, const string &fallbackFontID){
	int fontID_fs = fontIDs[fontID];
	int fallbackFontID_fs = fontIDs[fallbackFontID];
	ofxfs2_nvgAddFallbackFontId(ctx, fontID_fs, fallbackFontID_fs);
}


bool Fonts::applyStyle(const Style & style){
	bool ret = false;
	OFX_FONSTASH2_CHECK_RET
	int id = getFsID(style.fontID);
	if(id != FONS_INVALID){
		ofxfs2_nvgFontFaceId(ctx, id);
		ofxfs2_nvgFontSize(ctx, style.fontSize * fontScale);
		ofxfs2_nvgFillColor(ctx, toFScolor(style.color));
		ofxfs2_nvgTextAlign(ctx, style.alignmentV);
		ofxfs2_nvgTextLetterSpacing(ctx, style.spacing);
		ofxfs2_nvgFontBlur(ctx, style.blur);
		ofxfs2_nvgTextLineHeight(ctx, style.lineHeightMult); //??? we mostly handle this at ofxFontStash2 level
		return true;
	}
	return false;
}


int Fonts::getFsID(const string& userFontID){

	auto it = fontIDs.find(userFontID);
	if(it != fontIDs.end()){
		return it->second;
	}
	ofLogError("ofxFontStash2") << "Invalid Font ID: \"" << userFontID << "\"";
	return FONS_INVALID;
}


NVGcolor Fonts::toFScolor(const ofColor & c){
	return NVGcolor{c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f };
}

void Fonts::parseStyledText(const string & styledText, vector<StyledText> & output){
	Parser::parseText(styledText, styleIDs, defaultStyleID, output);
}


void Fonts::applyOFMatrix(){ //from ofxNanoVG

	OFX_FONSTASH2_CHECK
	ofMatrix4x4 ofMatrix = ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
	ofVec2f viewSize = ofVec2f(ofGetViewportWidth(), ofGetViewportHeight());

	ofVec2f translate = ofVec2f(ofMatrix(3, 0), ofMatrix(3, 1)) + viewSize/2;
	ofVec2f scale(ofMatrix(0, 0), ofMatrix(1, 1));
	ofVec2f skew(ofMatrix(0, 1), ofMatrix(1, 0));

	// handle OF style vFlipped inside FBO
	#if OF_VERSION_MINOR <= 9
	if (ofGetCurrentRenderer()->getCurrentOrientationMatrix()._mat[1][1] == 1) {
	#else
	if (ofGetCurrentRenderer()->getCurrentOrientationMatrix()[1][1] == 1) {
	#endif
		translate.y = (ofGetViewportHeight() - translate.y);
		scale.y *= -1;
		skew.y *= -1;
	}
	ofxfs2_nvgResetTransform(ctx);
	ofxfs2_nvgTransform(ctx, scale.x, -skew.y, -skew.x, scale.y, translate.x, translate.y);
}


#ifndef PROFILE_OFX_FONTSTASH2
	#pragma pop_macro("TS_START_NIF")
	#pragma pop_macro("TS_STOP_NIF")
	#pragma pop_macro("TS_START_ACC")
	#pragma pop_macro("TS_STOP_ACC")
	#pragma pop_macro("TS_START")
	#pragma pop_macro("TS_STOP")
#endif
