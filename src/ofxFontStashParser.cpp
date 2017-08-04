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

vector<StyledText>
ofxFontStashParser::parseText(const string& text, const unordered_map<string, ofxFontStashStyle> & styleIDs){

	vector<StyledText> parsedText;
	// pugi has a some interesting whitespace options available
	// e.g. parse_fragment, parse_trim_pcdata, parse_ws_pcdata_single
	xml_document doc;
	int parseOptions = parse_cdata | parse_escapes | parse_wconv_attribute | parse_eol | parse_fragment;
	TS_START_ACC_NIF("pugi parse");
	xml_parse_result result = doc.load_string(text.c_str(), parseOptions);
	TS_STOP_ACC_NIF("pugi parse");
	if(result.status == status_ok){
		ofxFontStashStyle baseStyle;		
		TS_START_ACC_NIF("recursive parse");
		recursiveParse(doc, baseStyle, styleIDs, parsedText);
		TS_STOP_ACC_NIF("recursive parse");
	}
	else{
		ofLogError("ofxFontStashParser")
			<< "ofxFontStash: xml parsing error in " << text << ": " << endl
			<< result.description() << endl;
	}

	//this is all to add a space between two styled paragraphs. Otherwise they just have no space
	//betwen them, so things look like "there was a dog.Bananas are cool".
	//The parsing seems unable to get any xml contents outisde tags; but that would be a better solution.
 	//so in:
	//
	//	<style>hello</style> monkey <style>world</style>
	//
	//the "monkey" is totally lost, but if we found it, then we wouldn't need to be adding extra spaces

	if(true){ //TODO!
		TS_START_ACC_NIF("handle spaces between tags");
		vector<int> spacesToAdd;
		if (parsedText.size() > 1){
			for(int i = 0; i < parsedText.size() -1; i++){
				if (parsedText[i].style != parsedText[i+1].style){
					spacesToAdd.push_back(i);
				}
			}
		}

		int indexOffset = 1;
		for(int i = 0; i < spacesToAdd.size(); i++){
			int index = spacesToAdd[i];
			StyledText st;
			st.text = " ";
			st.style = parsedText[i].style;
			parsedText.insert(parsedText.begin() + index + indexOffset, st);
			indexOffset++;
		}
		TS_STOP_ACC_NIF("handle spaces between tags");
	}

	return parsedText;
}
 

void ofxFontStashParser::recursiveParse(xml_node & parentNode,
										ofxFontStashStyle style,
										const unordered_map<string, ofxFontStashStyle> & styleIDs,
										vector<StyledText> & parsedText) {

	for( xml_node node : parentNode ){
		// handle nodes
		if( node.type() == node_element ){


			// <myStyle>bla bla</myStyle> ///////////////////////////////////
			auto it = styleIDs.find(node.name());
			if( it != styleIDs.end() ){
				style = it->second;
			}

			// <style></style> /////////////////////////////////////////////
			else if( strcmp( node.name(), "style") == 0){
				xml_attribute attr;
				if((attr = node.attribute("id"))){
					auto it = styleIDs.find(attr.value());
					if( it != styleIDs.end() ){
						style = it->second;
					}
				}
				
				if((attr = node.attribute("font"))){
					style.fontID = attr.value();
				}
				
				if((attr = node.attribute("size"))){
					style.fontSize = ofToFloat(attr.value());
				}
				
				if((attr = node.attribute("blur"))){
					style.blur = ofToFloat(attr.value());
				}
				
				if((attr = node.attribute("color"))){
					string hex = attr.value();
					style.color = colorFromHex(hex);
				}
			}
			// <br/> //////////////////////////////////////////////////////
			// not to go full html, but <br/> is quite handy (and \n is ignored by
			// the current parser settings)
			else if( strcmp( node.name(), "br") == 0){
				parsedText.push_back({"\n", style});
			}

			recursiveParse(node, style, styleIDs,parsedText);
		}
		
		// handle whitespace
		else if(node.type() == node_pcdata || node.type() == node_cdata){
			string text = node.text().get();
			StyledText st;
			st.text = node.text().get();
			st.style = style;
			parsedText.push_back(st);
		}
	}
}

ofColor ofxFontStashParser::colorFromHex(const string & hexS){

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
		}
		int hexInt = ofHexToInt(hex);
		return ofColor::fromHex(hexInt, alpha);
	}

	return retColor;
}
