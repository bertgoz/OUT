#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
    ofVideoPlayer player;
    int videoWidth;
    int videoHeight;
    
    // VIDEO
    ofPixels previous;
    ofImage diff;
    
    cv::Scalar diffMean;
    
    int frameCounter;
    int frameLimit;
    
    float movementBuffer;
    
    float smoothedMov;
    float scaledMov;

    vector <float> movHistory;
    
    // AUDIO
    void audioIn(float * input, int bufferSize, int nChannels);
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    float smoothedVol;
    float scaledVol;
    
    ofSoundStream soundStream;
		
};
