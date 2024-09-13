#pragma once

#include "ofMain.h"
#include "ofxAsyncImagesLoader.h"

//Uncomment the following line to use the nonblocking loader
//#define USE_NON_BLOCKING_LOADER

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
    
    void loadAsyncImgs(string dirpath);
    
    vector<ofImage> imgs;
    vector<ofRectangle> rects;
    ofRectangle mainRect;
    
    void makeRects();
    
    bool bIsSetup = false;
#ifdef USE_NON_BLOCKING_LOADER
    std::unique_ptr<ofxAsyncImagesLoader> loader = nullptr;
    ofEventListener allLoadedListener;
#endif
    
    void imgsLoadedCallback();
    
};
