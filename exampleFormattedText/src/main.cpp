#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;

	#ifdef TARGET_OPENGLES
		settings.setGLESVersion(2); 
	#else
		#ifdef NANOVG_GL2_IMPLEMENTATION
		settings.setGLVersion(2, 1);  // Fixed pipeline
		#endif

		#ifdef NANOVG_GL3_IMPLEMENTATION
		settings.setGLVersion(3, 2);  // Programmable pipeline >> you need to define GL_VERSION_3 in you pre-processor macros!
		#endif
	#endif

	settings.stencilBits = 8;
	int w = 1024;
	int h = 768;
	#if OF_VERSION_MINOR < 10
	settings.width = w;
	settings.height = w;
	#else
	settings.setSize(w, h);
	#endif
	ofCreateWindow(settings);
	ofRunApp(new ofApp());

}
