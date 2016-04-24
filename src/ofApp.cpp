#include "ofApp.h"

using namespace ofxCv;
using namespace cv;


//--------------------------------------------------------------
void ofApp::setup(){
    
    // *** VIDEO ***
    
    ofSetVerticalSync(true);
    
    bool check;
    
    check = player.loadMovie("m_sol_05092105_day.mp4");
    if (!check){
        cerr << "Loading video failed ...\n";
        ::exit(0);
    }
    
    player.play();
    
    videoWidth = player.getWidth();
    videoHeight = player.getHeight();
    
    // imitate() will set up previous and diff
    // so they have the same size and type as cam
    imitate(previous, player);
    imitate(diff, player);
    
    float movement = 0;
    float movementBuffer = 0;
    
    movHistory.assign(videoWidth, 0.0);
    
    // Time counter
    int frameRate = 30;
    float updateInterval = 0.1;
    
    ofSetFrameRate(frameRate);
    frameLimit= roundf(updateInterval / frameRate);
    frameCounter = 0;
    
    // *** AUDIO ***
    
    // 0 output channels,
    // 2 input channels
    // 44100 samples per second
    // 256 samples per buffer
    // 4 num buffers (latency)
    
    soundStream.listDevices();
    
    //if you want to set a different device id
    //soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
    
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(videoWidth , 0.0);
    
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // *** VIDEO ***
    
    player.update();
    
    if (player.isFrameNew()){
        
        // take the absolute difference of prev and cam and save it inside diff
        absdiff(previous, player, diff);
        diff.update();
        
        // like ofSetPixels, but more concise and cross-toolkit
        copy(player, previous);
        
        // mean() returns a Scalar. it's a cv:: function so we have to pass a Mat
        diffMean = mean(toCv(diff));
        //diffMean /= Scalar(100);
        
        // use the [] operator to get elements from a Scalar
        movementBuffer += (diffMean[0] + diffMean[1] + diffMean[2]) / 3;
        
        ++frameCounter;
        
        if (frameCounter >= frameLimit){
            
            // update movement
            float movement;
            movement = movementBuffer / frameCounter;
            
            smoothedMov *= 0.9;
            smoothedMov += 1 * movement;
            
            //lets scale the mov up to a 0-1 range
            scaledMov = ofMap(smoothedMov, 0.0, 20, 0.0, 1.0, true);
            
            //lets record the volume into an array
            movHistory.push_back( scaledMov );
            
            //if we are bigger the the size we want to record - drop the oldest value
            if( movHistory.size() >=  (videoWidth) ){
                movHistory.erase(movHistory.begin(), movHistory.begin()+1);
            }
            
            // reset variables
            frameCounter = 0;
            movementBuffer = 0;
            
        }
        
    }
    
    
    // *** AUDIO ***
    
    //lets scale the vol up to a 0-1 range
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    //lets record the volume into an array
    volHistory.push_back( scaledVol );
    
    //if we are bigger the the size we want to record - drop the oldest value
    if( volHistory.size() >=  (videoWidth)){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(ofColor::black);
    
    // *** VIDEO ***
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(50, 50, 0);
    
        player.draw(0, 0);
        ofDrawBitmapString("RAW VIDEO", 0, 18 + videoHeight);
    
    ofPopMatrix();
    ofPopStyle();
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate( 150 + videoWidth, 50, 0);
    
        diff.draw(0, 0);
        ofDrawBitmapString("PROCESSED VIDEO", 0, 18 + videoHeight);
    
    ofPopMatrix();
    ofPopStyle();
    
    ofPopMatrix();
    ofPopStyle();
    
    // *** VIDEO ***
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(50, 50 + videoHeight, 0);
    
        ofDrawBitmapString("MOVEMENT LEVEL (0-100): " + ofToString(scaledMov * 100, 0), 0, (videoHeight - 100) + 18);
        ofNoFill();
        ofRect(0, (videoHeight - 100) - 300, videoWidth, 300);
    
        // draw instant movement as the area of a circle
        ofSetColor(0, 0, 204);
        // concentric circles
        float areaMov = scaledMov * 100.0f;
        ofFill();
        ofCircle((videoWidth)/2, (videoHeight - 100)/2, areaMov);
        
        // draw the movement history as a graph
        ofSetColor(255, 0, 127);
        ofFill();
        ofBeginShape();
        for (unsigned int i = 0; i < movHistory.size(); i++){
            if( i == 0 ) ofVertex(i, videoHeight - 100);
            
            ofVertex(i, (videoHeight - 100) - movHistory[i] * 300);
            
            if( i == movHistory.size() -1 ) ofVertex(i, (videoHeight - 100));
        }
        ofEndShape(false);
    
    ofPopMatrix();
    ofPopStyle();
    
    // *** AUDIO ***
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(150 + videoWidth, 50 + videoHeight, 0);
    
        ofDrawBitmapString("SOUND LEVEL (0-100): " + ofToString(scaledVol * 100.0, 0), 0, (videoHeight - 100) + 18);
        ofNoFill();
        ofRect(0, (videoHeight - 100) - 300, videoWidth, 300);
        
        // draw instant sound as the area of a circle
        ofSetColor(0, 0, 204);
        float areaVol = scaledVol * 100.0f;
        ofFill();
        ofCircle((videoWidth)/2, (videoHeight - 100)/2, areaVol);
        
        // draw the volume history as a graph
        ofSetColor(255, 0, 127);
        ofFill();
        ofBeginShape();
        for (unsigned int i = 0; i < volHistory.size(); i++){
            if( i == 0 ) ofVertex(i, videoHeight - 100);
            
            ofVertex(i, (videoHeight - 100) - volHistory[i] * 300);
            
            if( i == volHistory.size() -1 ) ofVertex(i, (videoHeight - 100));
        }
        ofEndShape(false);		
        
    ofPopMatrix();
    ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms
    curVol /= (float)numCounted;
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.9;
    smoothedVol += 1 * curVol;
    
    //smoothedVol = 0; // NO AUDIO

}
