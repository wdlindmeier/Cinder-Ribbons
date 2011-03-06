#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/Path2d.h"
#include "cinder/Shape2d.h"
#import "RibbonParticle.h"
#include <list>
//#include <vector>
#include <math.h>

#define MAX_PARTICLES	200

using namespace ci;
using namespace ci::app;
using namespace std;

class RibbonTestApp : public AppBasic {
public:
	void setup();
	void mouseDown( MouseEvent event );	
	void mouseUp( MouseEvent event );	
	void update();
	void draw();
	
	Vec2i					mMousePos;
	list<RibbonParticle *>	mParicles;
	RibbonParticle			*mParticleHead;
	bool mMouseIsDown;
	
};

void RibbonTestApp::setup()
{
	mMouseIsDown = false;
	mMousePos = Vec2i::zero();
	mParticleHead = NULL;
}

void RibbonTestApp::mouseDown( MouseEvent event )
{
	mMouseIsDown = true;
	
	// Adding a "zero" particle so the ribbons dont connect
	RibbonParticle *p = new RibbonParticle;
	p->mPos = getMousePos();
	mParicles.push_back(p);
	
	RibbonParticle *backP = mParicles.back();	
	if(mParticleHead){ 
		mParticleHead->mNextParticle = backP;
		backP->mPrevParticle = mParticleHead;
	}
	mParticleHead = backP;
	
}

void RibbonTestApp::mouseUp( MouseEvent event )
{
	mMouseIsDown = false;
	mMousePos = Vec2i::zero();
	
	// Adding a "zero" particle so the ribbons dont connect
	RibbonParticle *p = new RibbonParticle;
	Vec2f lastVel = Vec2f(mParticleHead->mVel);
	p->mPos = Vec2f(getMousePos().x + (lastVel.x * 0.5), 
					getMousePos().y + (lastVel.y * 0.5));
	mParicles.push_back(p);
	
	RibbonParticle *backP = mParicles.back();
	if(mParticleHead){
		mParticleHead->mNextParticle = backP;
		backP->mPrevParticle = mParticleHead;
	}
	mParticleHead = backP;
}

void RibbonTestApp::update()
{
	if(mMouseIsDown){
		Vec2i newMousePos = getMousePos();
		Vec2f mouseVel = Vec2f::zero();
		if(mMousePos != Vec2i::zero()){
			mouseVel = newMousePos - mMousePos;
		}
		mMousePos = newMousePos;
		RibbonParticle *p = new RibbonParticle;
		p->mPos = mMousePos;
		p->mVel = mouseVel;
		
		Vec2f velNormal(mouseVel);
		velNormal.rotate(90 * M_PI / 180.0);
		p->mVelNormal = velNormal;
		
		mParicles.push_back(p);
		
		RibbonParticle *backP = mParicles.back();
		if(mParticleHead){
			mParticleHead->mNextParticle = backP;
			backP->mPrevParticle = mParticleHead;
		}
		mParticleHead = backP;
	}
	
	for(list<RibbonParticle *>::iterator p = mParicles.begin(); p != mParicles.end();++p){
		(*p)->update();
	}
	
	/* 
	 // NOTE: This causes memory issues
	 for(list<RibbonParticle *>::iterator p = mParicles.begin(); p != mParicles.end();){
	 if((*p)->mAge > 200){
	 mParicles.erase(p);
	 delete *p;
	 }else{
	 ++p;
	 }
	 }
	 */
	
	int particleSize = mParicles.size();
	if(particleSize > MAX_PARTICLES){
		int delta = particleSize - MAX_PARTICLES;
		for(int i=0;i<delta;i++){
			RibbonParticle *p = mParicles.front();
			mParicles.pop_front();
			delete p;
		}
	}
	
}

void RibbonTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
	
	Vec2i	normPos1, normPos2;
	normPos1 = Vec2f::zero();
	normPos2 = Vec2f::zero();
	
	for(list<RibbonParticle *>::iterator p = mParicles.begin(); p != mParicles.end(); ++p){
		
		int x1 = (*p)->mPos.x - (*p)->mVel.x;
		int x2 = (*p)->mPos.x;
		int y1 = (*p)->mPos.y - (*p)->mVel.y;
		int y2 = (*p)->mPos.y;
		
		// Capture the previous normal positions
		Vec2i prevNormPos1(normPos1);
		Vec2i prevNormPos2(normPos2);
		
		// If this is the first segment, make the normals the 
		// same as the position (we want it to taper in)		
		if(prevNormPos1 == Vec2i::zero() &&
		   prevNormPos2 == Vec2i::zero()){
			
			normPos1 = Vec2i(x1, y1);
			normPos2 = Vec2i(x2, y2);
			
			// TMP / TEST
			prevNormPos1 = normPos1;
			prevNormPos2 = normPos2;
			
		}else{
			
			int normX1 = (*p)->mPos.x - ((*p)->mVelNormal.x * 0.5);
			int normX2 = (*p)->mPos.x + ((*p)->mVelNormal.x * 0.5);
			int normY1 = (*p)->mPos.y - ((*p)->mVelNormal.y * 0.5);
			int normY2 = (*p)->mPos.y + ((*p)->mVelNormal.y * 0.5);
			normPos1 = Vec2i(normX1,normY1);
			normPos2 = Vec2i(normX2,normY2);			
			
		}
		
		// Draw the shape between the normals
		Path2d path;		
		path.moveTo(prevNormPos1.x, prevNormPos1.y);
		path.lineTo(normPos1.x, normPos1.y);
		path.lineTo(normPos2.x, normPos2.y);
		path.lineTo(prevNormPos2.x, prevNormPos2.y);
		path.close();
		
		//		float distance = Vec2f(x2-x1, y2-y1).length();
		
		// Draw the filled ribbon
		gl::color(Color(1.0,1.0,0));
		gl::drawSolid(path);
		
		// Draw the surface normal
		gl::color(Color(1.0,0,0));
		gl::drawLine(normPos1, normPos2);
		
		// Draw a line indicating it's position w/ velocity
		gl::color(Color::black());
		gl::drawLine(Vec2i(x1,y1), Vec2i(x2, y2));		
		
	}
	
}


CINDER_APP_BASIC( RibbonTestApp, RendererGl )
