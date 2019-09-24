#pragma once

#include "ofMain.h"


struct Joint {
public:
    ofVec3f v1;
    ofVec3f v2;
    bool isLine;
    float timestamp;
    Joint(ofVec3f vv1) {
        v1 = vv1;
        isLine = false;
        timestamp = ofGetElapsedTimef();
    }
    Joint(ofVec3f vv1, ofVec3f vv2) {
        v1 = vv1;
        v2 = vv2;
        isLine = true;
        timestamp = ofGetElapsedTimef();
    }
};

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofVec3f getPointFromJson(ofJson & j);

        ofJson json;
        float frame;
        float frameIter;
        bool inited;
        float timeLimit;
        int maxParticles;
    
        ofEasyCam cam;
    
        vector<std::map<string, vector<Joint>>> particles; // person vector, joint map, history vector
    bool isValid(ofVec3f v1, ofVec3f v2);
};
