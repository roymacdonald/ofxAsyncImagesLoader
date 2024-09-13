#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::loadAsyncImgs(string dirpath){
    
    ofSetLogLevel(OF_LOG_VERBOSE);
#ifndef USE_NON_BLOCKING_LOADER
    std::unique_ptr<ofxAsyncImagesLoader> loader;
#endif
    
    loader = std::make_unique<ofxAsyncImagesLoader>();
    
    ofDirectory dir;
    dir.allowExt("png");
    dir.listDir(dirpath);
    
    imgs.resize(dir.size());
    

    for(size_t i = 0; i < dir.size(); i++){
        loader->add(imgs[i], dir.getPath(i));
    }
    


    bool bBlockingLoaded = true;
#ifdef USE_NON_BLOCKING_LOADER
    bBlockingLoaded = false;
    allLoadedListener =  loader->allLoadedEvent.newListener(this, &ofApp::imgsLoadedCallback);
#endif
    loader->start(bBlockingLoaded);
    imgsLoadedCallback();
    
}

//--------------------------------------------------------------
void ofApp::imgsLoadedCallback(){
#ifdef USE_NON_BLOCKING_LOADER
    allLoadedListener.unsubscribe();
    loader = nullptr;
#endif
    makeRects();
    bIsSetup = true;
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    auto res = ofSystemLoadDialog("Select folder with images", true);
    if(res.bSuccess){
        loadAsyncImgs(res.getPath());
    }
}
//--------------------------------------------------------------
void ofApp::makeRects(){
    mainRect.set(0, 0, 0, 0 );
    
    for(size_t i = 0; i < imgs.size(); i++){
        rects.push_back(ofRectangle(0,0,imgs[i].getWidth(), imgs[i].getHeight()));
        if(i %2 == 0){
            rects.back().setPosition(mainRect.getBottomLeft());
        }else{
            if(i>0){
                rects.back().setPosition(rects[i-1].getTopRight());
            }
        }
        mainRect.growToInclude(rects.back());
    }
}
//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    if(!bIsSetup)return;
    
    ofRectangle viewport(0,0,ofGetWidth(), ofGetHeight());
    
    auto r = mainRect;
    r.scaleTo(viewport);
    
    ofPushMatrix();
    ofScale(r.width/mainRect.width);
    for(size_t i = 0; i < imgs.size(); i++){
        imgs[i].draw(rects[i]);
    }
    
    ofPopMatrix();
    
    

}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

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
