#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    /*--
    
     JSON Structure:
     
     {
        "frames": [                             Frames - same as SVO, will be "null" if no people detected
            [                                   Array of People
                 {                              Person Object
                     "0": [                     Joint Index - ie. elbow, shoulder etc, see "keypoints_pose_25.png"
                        0.3323,                 X
                        0.1746,                 Y
                        0.8711,                 Z
                        0.3236,                 Certainty - from 0 to 1
                        2                       Connecting Joint Index - where to draw a line to
                     ]
                 }
            ]
        ],
        "conf": {
            "info": "1234 frames out of 1234",
            "plane": {
                "normal": [0,0,0],
                "center": [0,0,0],
                "extents": [0,0,0]
            }
        }
     
     }
    
    
    --*/
    
    
    // TODOS
    // 1. grey out by certainty
    // 2. overlay two views
    // 3. trails
    
    
    
    ofEnableDepthTest();
    ofSetCircleResolution(64);

    
    json = ofLoadJson("24123_2019-08-12_12-39-18.json");
    ofLog() << "Beginning..." << json["frames"].size();

    ofLog::setAutoSpace(true);
    ofBackground(0);
    ofSetFrameRate(60);
    
    frame = 0;
    frameIter = 0.1;
    inited = false;
    maxParticles = 100;
    timeLimit = 4;

    ofJson j;

    int i = 0;
    for (auto & frame : json["frames"]) {
        if (!frame.is_null()) {
            j[ofToString(i)] = frame;
        } else {
            j[ofToString(i)]  = ofJson::array();
        }
        i += 1;
    }

    ofSavePrettyJson("test-format.json", j);

}

//--------------------------------------------------------------
void ofApp::update(){

}

ofVec3f ofApp::getPointFromJson(ofJson & j) {

    float x = j[0].get<float>();
    float y = j[1].get<float>();
    float z = j[2].get<float>();

    return ofVec3f(x,y,z);
}
//--------------------------------------------------------------
void ofApp::draw(){
    
    
    cam.begin();

    if (json["frames"].size() > 0 ) {

        frame += frameIter;
        if (json["frames"][(int)frame].is_null()) frame += 1;
        if (frame >= json["frames"].size()) frame = 0;
        
        int personIdx = 0;
        
        for (auto & person : json["frames"][(int)frame]) {
            
            if (personIdx >= particles.size()) {
                std::map<string, vector<Joint>> m;
                particles.push_back( m );
            }
            
            
            for (auto & point : person.items()) {

                if (!point.value().is_null()) {

                    
                    int keyConnector = point.value()[4].get<int>();
                    int colorMap = ofMap(keyConnector, 0, 20, 100, 255);
                    string keyConnectorStr = ofToString( keyConnector );
                    float x = point.value()[0].get<float>();
                    float y = point.value()[1].get<float>();
                    float z = point.value()[2].get<float>();

                    ofVec3f v1 = getPointFromJson(point.value());
                    ofSetColor( colorMap, 255 - colorMap, 255);
//                    ofDrawCircle(v1, 0.02);
                    
                    if ( particles[personIdx].find(keyConnectorStr) == particles[personIdx].end() ) {
                        particles[personIdx][keyConnectorStr] = {};
                    }
                    
                    if (!inited) {
                        cam.setTarget( v1 );
                        cam.rollDeg(180);
                        inited = true;
                    }
                    
                    

                    if (!person[keyConnectorStr].empty()) {
                        ofVec3f v2 = getPointFromJson( person[keyConnectorStr] );
                        
                        if (isValid(v1,v2)) {
                            
                            ofSetLineWidth(0.05);
//                            ofDrawLine(v1,v2);
                        }
                        
                        Joint j(v1, v2);
                        particles[personIdx][keyConnectorStr].push_back(j);
                        
                    } else {
                        
                        Joint j(v1);
                        particles[personIdx][keyConnectorStr].push_back(j);
                        
                    }

                }

            }
            
            personIdx += 1;
        }
        
        bool drawLines = false;
        bool drawDots = false;
        bool drawPaths = true;
        
        int pIdx = 0;
        for (auto & p : particles) {
            for (auto & j : p) {
                for (vector<Joint>::iterator it = j.second.begin(); it != j.second.end();) {
                    float t = ofGetElapsedTimef();
                    if( t > it->timestamp + timeLimit  ) {
                        it = j.second.erase(it);
                    } else {
                        
                        int colorMap = ofMap(pIdx, 0, particles.size(), 0, 255);
                        ofSetColor(colorMap,255-colorMap,255, ofMap(t, it->timestamp, it->timestamp + timeLimit, 255, 0));
                        
                        if (drawDots) ofDrawCircle( it->v1,  0.025);
                        if (drawLines) {
                            if ( it->isLine && isValid(it->v1, it->v2) ) {
                                ofDrawLine(it->v1, it->v2);
                            }
                        }
                        
                        ++it;
                    }
                }
                
                if (drawPaths) {
                    for (int i = 0; i < j.second.size(); i++) {
                        if (i > 0) {
                            
                            Joint j1 = j.second[i];
                            Joint j2 = j.second[i-1];
                            if (isValid(j1.v1, j1.v2) && isValid(j2.v1, j2.v2)) {
                            
                                int alpha = ofMap(i, 0, j.second.size(), 255, 0);
                                ofSetColor(255,0,0,alpha);
                                ofDrawLine(j1.v1, j2.v1);
                            }
                        }
                    }
                }
            }
            pIdx += 1;
        }

    }


    
    cam.end();
    ofSetColor(255);
    ofDrawBitmapString(ofToString(frame), 20, 20);
}

bool ofApp::isValid(ofVec3f v1, ofVec3f v2) {
    
    float distance = sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z));
    return distance < 1;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
//    frame = ofMap(x, 100, ofGetWidth() - 200, 0, json["frames"].size(), true);
    if (ofGetKeyPressed(OF_KEY_SHIFT)) {
        frameIter = ofMap( x, 0, ofGetHeight(), 0, 2 );
    }
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
