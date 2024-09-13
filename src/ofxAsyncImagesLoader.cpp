//
//  ofxAsyncImagesLoader.cpp
//  AsynImgLoad
//
//  Created by Roy Macdonald on 13-09-24.
//

#include "ofxAsyncImagesLoader.h"
#include "ofLog.h"
#include <sstream>

ofxAsyncImagesLoader::ofxAsyncImagesLoader():numLoaded(0){
    
}
//--------------------------------------------------------------
void ofxAsyncImagesLoader::start(bool bBlocking){
    if(totalToLoad == 0){
        ofLogError("ofxAsyncImagesLoader::start") << "you first need to add images to load before calling start";
        return;
    }
    
    
    if(!bBlocking){
        updateListener = ofEvents().update.newListener( this, &ofxAsyncImagesLoader::update);
    }
    // Launch threads based on hardware concurrency
    size_t numThreads = std::thread::hardware_concurrency();
    
    //only start the needed amount of threads.
    numThreads = std::min(totalToLoad, numThreads);
    
    ofLogNotice("ofxAsyncImagesLoader::start") << "Launching " << numThreads << " worker threads";
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ofxAsyncImagesLoader::worker, this);
    }
    
    if(bBlocking){
        waitAndUpdate();
    }
}

//--------------------------------------------------------------
ofxAsyncImagesLoader::~ofxAsyncImagesLoader(){
    close();
}

//--------------------------------------------------------------
void ofxAsyncImagesLoader::close(){
    images_to_load_from_disk.close();
    images_to_update.close();
    for (std::thread& worker : workers) {
        worker.join();
    }
    updateListener.unsubscribe();
    
}

//--------------------------------------------------------------
void ofxAsyncImagesLoader::add(ofImage* image, std::string file){
    if(image){
        add(*image, file);
    }
}

//--------------------------------------------------------------
void ofxAsyncImagesLoader::add(std::vector<ofImage>& images, const std::vector<std::string>& files){
    if(images.size() != files.size()){
        ofLogWarning("ofxAsyncImagesLoader::add") << "files and images have different sizes. loading as much as possible.";
    }
    size_t n = std::min(images.size(), files.size());
    
    for(size_t i = 0; i < n; i++){
        add(images[i],files[i]);
    }
    
}

//--------------------------------------------------------------
void ofxAsyncImagesLoader::add(std::map<std::string, ofImage> images){
    for(auto& i: images){
        add(i.second, i.first);
    }
}

// Load an image from disk.
//--------------------------------------------------------------
void ofxAsyncImagesLoader::add(ofImage& image, std::string filename) {
    totalToLoad++;
    ofImageLoaderEntry entry(image);
    entry.filename = filename;
    entry.image->setUseTexture(false);
    entry.name = filename;
    
    ofLogVerbose("ofxAsyncImagesLoader::load") << entry.filename;
    
    images_to_load_from_disk.send(entry);
}
//--------------------------------------------------------------
void ofxAsyncImagesLoader::worker(){
   while (true) {
       ofImageLoaderEntry entry;
       if( images_to_load_from_disk.receive(entry) ) {
           
           if(entry.image->load(entry.filename) )  {
//               ofLogNotice("ofxAsyncImagesLoader::worker")  << "load attempt";
               images_to_update.send(entry);
           }else{
               numLoaded++;
               ofLogError("ofxAsyncImagesLoader") << "couldn't load file: \"" << entry.filename << "\"";
           }
       }else{
           return;
       }
   }
}


// Check the update queue ands update the texture
//--------------------------------------------------------------
void ofxAsyncImagesLoader::update(ofEventArgs & a){
    // Load 1 image per update so we don't block the gl thread for too long
    ofImageLoaderEntry entry;
    if (images_to_update.tryReceive(entry)) {
        updateTexture(entry);
    }
    if(isDone()){
        ofNotifyEvent(allLoadedEvent,this);
    }
}
//--------------------------------------------------------------
void ofxAsyncImagesLoader::waitAndUpdate(){
    while(true){
        ofImageLoaderEntry entry;
        if (images_to_update.receive(entry)) {
            updateTexture(entry);
        }
        if(isDone()){
            return;
        }
    }
}
//--------------------------------------------------------------
void ofxAsyncImagesLoader::updateTexture(ofImageLoaderEntry& entry){
    entry.image->setUseTexture(true);
    entry.image->update();
    numLoaded++;
    ofLogVerbose("ofxAsyncImagesLoader::update") << "tex updated: " << entry.filename;
}

//--------------------------------------------------------------
bool ofxAsyncImagesLoader::isDone(){
    return(totalToLoad == numLoaded)  && (images_to_load_from_disk.empty() && images_to_update.empty());
}
