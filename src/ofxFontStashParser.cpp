//
//  ofxFontStashParser.cpp
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#include "ofxFontStashParser.h"
#include "pugixml.hpp"

using namespace pugi;

vector<ofxFontStashParser::StyledText>
ofxFontStashParser::parseText(const string& text, const map<string, ofxFontStashStyle> & styleIDs){

	vector<StyledText> parsedText;
	// pugi has a some interesting whitespace options available
	// e.g. parse_fragment, parse_trim_pcdata, parse_ws_pcdata_single
	xml_document doc;
	xml_parse_result result = doc.load_string(text.c_str());
	if(result.status == status_ok){
		ofxFontStashStyle baseStyle;
		recursiveParse(doc, baseStyle, styleIDs, parsedText);
	}
	else{
		ofLogError()
			<< "ofxFontStash: xml parsing error in " << text << ": " << endl
			<< result.description() << endl;
	}
	
	return parsedText;
}
 

void ofxFontStashParser::recursiveParse(xml_node & parentNode,
										ofxFontStashStyle style,
										const map<string, ofxFontStashStyle> & styleIDs,
										vector<StyledText> & parsedText) {

	for( xml_node node : parentNode ){
		// handle nodes
		if( node.type() == node_element ){

			// <style></style>
			if( strcmp( node.name(), "style") == 0){
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
					int slen = hex.length();
					if(slen > 1){
						hex = hex.substr(1, slen);
						int alpha = 255;
						if(hex.length() == 8){
							string a = hex.substr(6,2);
							hex = hex.substr(0,6);
							alpha =  ofHexToInt("0000" + a); //add 255 alpha if not specified
						}
						int hexInt = ofHexToInt(hex);
						style.color = ofColor::fromHex(hexInt, alpha);
					}
				}
				
			}
			
			recursiveParse(node, style, styleIDs,parsedText);
		}
		
		// handle whitespace
		else if(node.type() == node_pcdata || node.type() == node_cdata){
			parsedText.push_back({node.text().get(),style});
		}

	}
}
