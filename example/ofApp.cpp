#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	fonts.setup(512);
	fonts.addFont("veraMono", "fonts/VeraMono.ttf");
	fonts.addFont("veraMonoBold", "fonts/VeraMono-Bold.ttf");
	fonts.addFont("Helvetica", "fonts/HelveticaNeue.ttf");
	fonts.setLineHeightMult(0.9);
	ofBackground(22);
	TIME_SAMPLE_ENABLE();
	TIME_SAMPLE_SET_AVERAGE_RATE(0.05);
	TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_TOP_RIGHT);

	RUI_SETUP();
	RUI_SHARE_PARAM(debug);
	RUI_LOAD_FROM_XML();

}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

//	ofSetColor(255,0,0);
//	ofDrawRectangle(0, 0, 100, 100);

	ofxFontStashStyle style;
	style.fontID = "veraMono";
	style.fontSize = 22;
	style.color = ofColor::white;
	if(ofGetFrameNum()%60 < 30) style.blur = 4;
//
//	ofSetColor(255);
//	fonts.draw("banana! monkey!", style, mouseX, mouseY);
//
	string formattedText = "<style font=veraMono size=17 color=#ff0000>This is: red! veramono 12.</style> <style font=veraMonoBold size=15 color=#00ff00>Lorem ipsum</style> <style font=veraMono size=15 color=#00ff00> dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.</style>";
//	//string formattedText = "<style font=veraMono size=20 color=#ff0000>this is: red! veramono 12 this is: red! veramono 12</style> ";
	formattedText += "<style font=Helvetica size=17 color=#bbbbbb>Lorem ipsum ad his scripta blandit partiendo, eum fastidii accumsan euripidis in, eum liber hendrerit an. Qui ut wisi </style><style font=Helvetica size=33 color=#bbbbbb>vocibus suscipiantur, </style><style font=Helvetica size=15 color=#bbbbbb>quo dicit ridens inciderint id. Quo mundi lobortis reformidans eu, legimus senserit definiebas an eos. Eu sit tincidunt incorrupte definitionem, vis mutat affert percipit cu, eirmod consectetuer signiferumque eu per. In usu latine equidem dolores. Quo no falli viris intellegam, ut fugit veritus placerat per. \n Ius id vidit volumus mandamus, vide veritus democritum te nec, ei eos debet libris consulatu. No mei ferri graeco dicunt, ad cum veri accommodare. Sed at malis omnesque delicata, usu et iusto zzril meliore. Dicunt maiorum eloquentiam cum cu, sit summo dolor essent te. Ne quodsi nusquam legendos has, ea dicit voluptua eloquentiam pro, ad sit quas qualisque. Eos vocibus deserunt quaestio ei. \n Blandit incorrupte quaerendum in quo, nibh impedit id vis, vel no nullam semper audiam. Ei populo graeci consulatu mei, has ea stet modus phaedrum. Inani oblique ne has, duo et veritus detraxit. Tota ludus oratio ea mel, offendit persequeris ei vim. Eos dicat oratio partem ut, id cum ignota senserit intellegat. Sit inani ubique graecis ad, quando graecis liberavisse et cum, dicit option eruditi at duo. Homero salutatus suscipiantur eum id, tamquam voluptaria expetendis ad sed, nobis feugiat similique usu ex.</style> ";
	formattedText += "<style font=Helvetica size=18 color=#00ffff>Lorem ipsum ad his scripta blandit partiendo, eum fastidii accumsan euripidis in, eum liber hendrerit an. Qui ut wisi vocibus suscipiantur, quo dicit ridens inciderint id. Quo mundi lobortis reformidans eu, legimus senserit definiebas an eos. Eu sit tincidunt incorrupte definitionem, vis mutat affert percipit cu, eirmod consectetuer signiferumque eu per. In usu latine equidem dolores. Quo no falli viris intellegam, ut fugit veritus placerat per. \n Ius id vidit volumus mandamus, vide veritus democritum te nec, ei eos debet libris consulatu. No mei ferri graeco dicunt, ad cum veri accommodare. Sed at malis omnesque delicata, usu et iusto zzril meliore. Dicunt maiorum eloquentiam cum cu, sit summo dolor essent te. Ne quodsi nusquam legendos has, ea dicit voluptua eloquentiam pro, ad sit quas qualisque. Eos vocibus deserunt quaestio ei. \n Blandit incorrupte quaerendum in quo, nibh impedit id vis, vel no nullam semper audiam. Ei populo graeci consulatu mei, has ea stet modus phaedrum. Inani oblique ne has, duo et veritus detraxit. Tota ludus oratio ea mel, offendit persequeris ei vim. Eos dicat oratio partem ut, id cum ignota senserit intellegat. Sit inani ubique graecis ad, quando graecis liberavisse et cum, dicit option eruditi at duo. Homero salutatus suscipiantur eum id, tamquam voluptaria expetendis ad sed, nobis feugiat similique usu ex.</style> ";
	//formattedText += "<style font=veraMono size=99 color=#00ff00>giant 99 text on vera mono</style> ";
	formattedText += "<style font=Helvetica size=44 color=#0000ff>this is blue Helvetica 44</style> ";
//	fonts.drawFormatted(formattedText, 200, 300);
//
//	ofSetColor(0,255,0,64);
//	ofDrawRectangle(50, 0, 200, 100);

	int x = 100;
	int y = 100;

	fonts.drawFormattedColumn(formattedText, x, y, mouseX - x, debug);
	ofSetColor(255,22);
	ofTranslate(0.5, 0.5);
	ofDrawLine(x, y, ofGetWidth(), y);
	ofDrawLine(x, 0, x, ofGetHeight());
	ofDrawLine(mouseX, 0, mouseX, ofGetHeight());


	ofSetColor(255);
	float t = ofGetElapsedTimef();;
	for(int i = 0; i < 1000; i++){
		TS_START_ACC("test");
		TS_STOP_ACC("test");
	}
	ofDrawBitmapString(ofToString((ofGetElapsedTimef()-t) * 1000.0f), 30, 30);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if(key == 'd'){
		debug ^= true;
		RUI_PUSH_TO_CLIENT();
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
