#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	ofGLWindowSettings settings;
	settings.setGLVersion(2, 1);  // Fixed pipeline
	#ifdef GL_VERSION_3
	settings.setGLVersion(3, 2);  // Programmable pipeline >> you need to define GL_VERSION_3 in you pre-processor macros!
	#endif
	settings.width = 1300;
	settings.height = 1050;
	ofCreateWindow(settings);
	ofRunApp(new ofApp());

}
