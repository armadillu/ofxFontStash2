//
//  ofxFontStashParser.cpp
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#include "ofxFontStashParser.h"
#include "pugixml.hpp"

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


using namespace pugi;
using namespace ofxFontStash2;
using namespace std;

void Parser::parseText(const string& text,
								  const unordered_map<string, Style> & styleIDs,
								  const string & defaultStyleID,
								  vector<StyledText> & parsedText //output
							  ){

	// pugi has a some interesting whitespace options available
	// e.g. parse_fragment, parse_trim_pcdata, parse_ws_pcdata_single

	xml_document doc;
	int parseOptions = parse_cdata | parse_escapes | parse_wconv_attribute | parse_eol | parse_fragment;

	TS_START_ACC_NIF("pugi parse");
	xml_parse_result result = doc.load_string(text.c_str(), parseOptions);
	TS_STOP_ACC_NIF("pugi parse");

	if(result.status == status_ok){

		vector<Style> stylesStack;
		if (defaultStyleID.size()){ //user specified a default style, init our stack with it.
			auto it = styleIDs.find(defaultStyleID);
			if(it != styleIDs.end()){
				stylesStack.emplace_back(it->second);
			}
		}
		if(stylesStack.size() == 0){ //user didnt specify a custom default style - use our nasty default
			if(styleIDs.size()){
				stylesStack.emplace_back(std::begin(styleIDs)->second); //get 1st style
			}else{
				stylesStack.emplace_back(Style());
			}
		}

		TS_START_ACC_NIF("recursive parse");
		recursiveParse(doc, stylesStack, styleIDs, parsedText);
		TS_STOP_ACC_NIF("recursive parse");

	}else{
		ofLogError("ofxFontStash2") << "xml parsing error in \"" << text << "\": \"" << result.description() << "\"";
	}

	#if 0
	//this is all to add a space between two styled paragraphs. Otherwise they just have no space
	//betwen them, so things look like "there was a dog.Bananas are cool".
	//The parsing seems unable to get any xml contents outisde tags; but that would be a better solution.
 	//so in:
	//	<style>hello</style> monkey <style>world</style>
	//the "monkey" is totally lost, but if we found it, then we wouldn't need to be adding extra spaces
	//**** this is not place to do this as we haven't split words into spaces yet - this is only split by style tags.
	//So this stuff is OFF for now.
	TS_START_ACC("handle spaces between tags");
	vector<int> spacesToAdd;
	std::locale loc = std::locale("");
	if (parsedText.size() > 1){
		for(int i = 0; i < parsedText.size() -1; i++){
			if (parsedText[i].style != parsedText[i+1].style){
				bool leftHasSeparator = false;
				bool rightHasSeparator = false;
				if (parsedText[i].text.size() && isSeparator(parsedText[i].text.back())){
					leftHasSeparator = true;
				}
				if (parsedText[i+1].text.size() && isSeparator(parsedText[i+1].text.front())){
					rightHasSeparator = true;
				}
				//handle case where "<style1>bye</style1>.<style2>" should render as "bye." not "bye ."
				bool isPunctuation;
				if (parsedText[i+1].text.size() == 1 && parsedText[i+1].text != "\n" && parsedText[i+1].text != " "){
					for(auto & c: ofUTF8Iterator(parsedText[i+1].text)){
						isPunctuation = std::ispunct<wchar_t>(c,loc);
					}
				}
				if(!rightHasSeparator && !leftHasSeparator && !isPunctuation){
					spacesToAdd.emplace_back(i);
				}
			}
		}
	}

	int indexOffset = 1;
	for(int i = 0; i < spacesToAdd.size(); i++){
		int index = spacesToAdd[i];
		StyledText st;
		st.text = " ";
		st.style = parsedText[index].style;
		parsedText.insert(parsedText.begin() + index + indexOffset, st);
		indexOffset++;
	}
	TS_STOP_ACC("handle spaces between tags");
	#endif
}
 

void Parser::recursiveParse(xml_node & parentNode,
										vector<Style> & styleStack,
										const unordered_map<string, Style> & styleIDs,
										vector<StyledText> & parsedText) {

	//bool debug = false;
	Style style;
	for(xml_node & node : parentNode){
		// handle nodes
		if( node.type() == node_element ){

			// <style id = "" ></style> //////////////////////////////////////////////////////
			if( strcmp( node.name(), "style") == 0){
				//if(debug) ofLogNotice("ofxFontStash2::Parser") << "NODE<style> stack:" << styleStack.size();
				xml_attribute attr;
				if((attr = node.attribute("id"))){
					auto it = styleIDs.find(attr.value());
					if( it != styleIDs.end() ){
						style = it->second;
					}
				}
				handleAttributes(node, style);
			}

			// <br/> /////////////////////////////////////////////////////////////////////////
			else if( strcmp( node.name(), "br") == 0){
				//if(debug) ofLogNotice("ofxFontStash2::Parser") << "NODE<br> stack:" << styleStack.size();
				Style st = styleStack.back();
				xml_attribute attr;
				if((attr = node.attribute("heightMult"))){
					st.lineHeightMult *= ofToFloat(attr.value());
				}
				parsedText.emplace_back(StyledText({"\n", st}));
			}else{

			// <myStyle></myStyle> /////////////////////////////////////////////////////////// keep this as the last option as its the most expensive to evaluate
				auto it = styleIDs.find(node.name());
				if( it != styleIDs.end() ){
					//if(debug) ofLogNotice("ofxFontStash2::Parser") << "NODE<" << node.name() << " stack:" << styleStack.size();
					style = it->second;
					handleAttributes(node, style);
				}
			}

			styleStack.emplace_back(style);
			recursiveParse(node, styleStack, styleIDs, parsedText);
		}
		
		// handle text inside tags
		else if(node.type() == node_pcdata || node.type() == node_cdata){
			string text = node.text().get();
			//if(debug) ofLogNotice("ofxFontStash2::Parser") << "Content [\"" << text << "\"] stack:" << styleStack.size();
			StyledText st;
			st.text = node.text().get();
			st.style = styleStack.back();
			parsedText.emplace_back(st);
		}
	}

	if(styleStack.size() > 1){
		styleStack.pop_back(); //always keep the default ([0]) style no matter what.
	}
}

void Parser::handleAttributes(xml_node & node, Style & currStyle){

	xml_attribute attr;
	if((attr = node.attribute("font"))){
		currStyle.fontID = attr.value();
	}

	if((attr = node.attribute("size"))){
		currStyle.fontSize = ofToFloat(attr.value());
	}

	if((attr = node.attribute("blur"))){
		currStyle.blur = ofToFloat(attr.value());
	}

	if((attr = node.attribute("color"))){
		string hex = attr.value();
		currStyle.color = colorFromHex(hex);
	}
}

ofColor Parser::colorFromHex(const string & hexS){

	ofColor retColor;
	string hex = hexS;

	int slen = hex.length();
	if(slen > 1){
		hex = hex.substr(1, slen);
		int alpha = 255;
		if(hex.length() == 8){ //hex string has alpha!
			string a = hex.substr(6,2);
			hex = hex.substr(0,6);
			alpha = ofHexToInt("0000" + a); //add 255 alpha if not specified
		}else if(hex.length() == 2){ //user specified 1 component gray (ie ofColor(244))
			int gray = ofHexToInt("0000" + hex);
			return ofColor(gray);
		}
		int hexInt = ofHexToInt(hex);
		return ofColor::fromHex(hexInt, alpha);
	}

	return retColor;
}


bool Parser::isSeparator(char s){
	return s == ' ' || s == '\n';
}
