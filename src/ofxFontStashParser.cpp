//
//  ofxFontStashParser.cpp
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#include "ofxFontStashParser.h"

vector<ofxFontStashParser::StyledText>
ofxFontStashParser::parseText(const string& text){

	vector<StyledText> parsedText;
	GumboOutput* output = gumbo_parse(text.c_str());
	recursiveParse(output->root, parsedText);
	gumbo_destroy_output(&kGumboDefaultOptions, output);
	return parsedText;
}


void ofxFontStashParser::recursiveParse(GumboNode* node, vector<StyledText> & parsedText) {

	if (node->type == GUMBO_NODE_TEXT) {
		if(node->parent && node->parent->type == GUMBO_NODE_ELEMENT){
			StyledText styledTextBlock;
			styledTextBlock.text = node->v.text.text;

			if(node->parent->v.element.tag == GUMBO_TAG_STYLE){

				GumboAttribute* font = gumbo_get_attribute(&node->parent->v.element.attributes, "font");
				if(font){
					styledTextBlock.style.fontID = font->value;
				}
				GumboAttribute* size = gumbo_get_attribute(&node->parent->v.element.attributes, "size");
				if(size){
					styledTextBlock.style.fontSize = ofToFloat(size->value);
				}

				GumboAttribute* blur = gumbo_get_attribute(&node->parent->v.element.attributes, "blur");
				if(blur){
					styledTextBlock.style.blur = ofToFloat(blur->value);
				}

				GumboAttribute* color = gumbo_get_attribute(&node->parent->v.element.attributes, "color");
				if(color){
					string hex = color->value;
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
						styledTextBlock.style.color = ofColor::fromHex(hexInt, alpha);
						//cout << styledTextBlock.style.color << endl;
					}
				}
				styledTextBlock.style.valid = true;
			}else{
				//ofLogError() << "text node with no style?! '" << node->v.text.text << "'";
				styledTextBlock.style.valid = false;
			}
			parsedText.push_back(styledTextBlock);
		}else{
			ofLogError() << "text node with no parent?!";
		}
		return;
	} else if (node->type == GUMBO_NODE_ELEMENT) {
		GumboVector* children = &node->v.element.children;
		for (unsigned int i = 0; i < children->length; ++i) {
			recursiveParse((GumboNode*) children->data[i], parsedText);
		}
		return;
	} else {
		return ;
	}
}
