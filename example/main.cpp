#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	#ifdef NANOVG_GL2_IMPLEMENTATION
	settings.setGLVersion(2, 1);  // Fixed pipeline
	#endif
	#ifdef NANOVG_GL3_IMPLEMENTATION
	settings.setGLVersion(3, 2);  // Programmable pipeline >> you need to define GL_VERSION_3 in you pre-processor macros!
	#endif
	settings.stencilBits = 8;
	settings.numSamples = 4;
	int w = 1300;
	int h = 1050;
#if OF_VERSION_MINOR < 10
	settings.width = w;
	settings.height = w;
#else
	settings.setSize(w, h);
#endif
	ofCreateWindow(settings);
	ofRunApp(new ofApp());

}
