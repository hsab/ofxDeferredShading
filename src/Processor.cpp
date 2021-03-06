#include "Processor.hpp"

using namespace DeferredEffect;

void Processor::init(unsigned w, unsigned h) {
    width = w;
    height = h;
    
    for (int i = 0; i < 2; i ++) {
        pingPong[i].allocate(width, height, GL_RGBA);
    }
    
    numProcessedPasses = 0;
    currentReadFbo = 0;
    
    gbuffer.setup(width, height);
    
}

void Processor::begin(ofCamera& cam, bool bUseOwnShader) {
 
    for (auto pass : passes) {
        if (pass->getEnabled()) {
            pass->update(cam);
        }
    }
    
    gbuffer.begin(cam, bUseOwnShader);
    
}

void Processor::end(bool autoDraw) {
    
    gbuffer.end();
    
    ofPushStyle();
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    ofSetColor(255, 255, 255);
    process();
    if (autoDraw) draw();
    glPopAttrib();
    ofPopStyle();
}

void Processor::draw(float x, float y) const {
    draw(x, y, width, height);
}

void Processor::draw(float x, float y, float w, float h) const {
    pingPong[currentReadFbo].draw(0, 0, w, h);
}


void Processor::debugDraw() {
    gbuffer.debugDraw();
}

void Processor::process(){
    
    numProcessedPasses = 0;
    for (auto pass : passes) {
        if (pass->getEnabled()) {
            if (numProcessedPasses == 0) pass->render(gbuffer.getFbo(), pingPong[1 - currentReadFbo], gbuffer);
            else pass->render(pingPong[currentReadFbo], pingPong[1 - currentReadFbo], gbuffer);
            currentReadFbo = 1 - currentReadFbo;
            numProcessedPasses++;
        }
        
    }
    
    
}
