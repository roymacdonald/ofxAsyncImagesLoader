#pragma once

#include "ofThread.h"
#include "ofImage.h"
#include "ofPixels.h"
#include "ofURLFileLoader.h"
#include "ofTypes.h"
#include "ofThreadChannel.h"

class ofxAsyncImagesLoader{
public:
    ofxAsyncImagesLoader();
    ~ofxAsyncImagesLoader();
    
    void add(ofImage& image, std::string file);
    void add(ofImage* image, std::string file);
    void add(std::vector<ofImage>& images, const std::vector<std::string>& files);
    void add(std::map<std::string, ofImage> images);
    
    ///\brief start the threadpool and loading.
    ///\param bBlocking if true it will block and only return when all images have been loaded. if false it will return immediately. In such case you want to listen to the allLoadedEvent to get notified when it has loaded all the images.
    void start(bool bBlocking);
    
    ofEvent<void> allLoadedEvent;
    
private:
    void close();
    // Worker function to pull and execute tasks from the queue
    void worker();
    void update(ofEventArgs & a);
//    virtual void threadedFunction();
//    void urlResponse(ofHttpResponse & response);
    
    // Entry to load.
    struct ofImageLoaderEntry {
    public:
        ofImageLoaderEntry() {
            image = NULL;
        }
        
        ofImageLoaderEntry(ofImage & pImage) {
            image = &pImage;
        }
        ofImage* image;
        std::string filename;
        std::string url;
        std::string name;
    };


    typedef std::map<std::string, ofImageLoaderEntry>::iterator entry_iterator;

    size_t totalToLoad = 0;
    std::atomic<size_t> numLoaded;

//    std::map<std::string,ofImageLoaderEntry> images_async_loading; // keeps track of images which are loading async
    ofThreadChannel<ofImageLoaderEntry> images_to_load_from_disk;
    ofThreadChannel<ofImageLoaderEntry> images_to_update;
    
    ofEventListener updateListener;

    std::vector<std::thread> workers;
    
    bool isDone();
    
    void updateTexture(ofImageLoaderEntry& entry);
    void waitAndUpdate();
    
};


