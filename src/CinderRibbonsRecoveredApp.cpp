#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
//#import "RibbonParticle.h"
#import "Ribbon.h"
#include <list>
//#include <vector>
#include <math.h>
#include "cinder/Rand.h"

#define MAX_RIBBONS	20

using namespace ci;
using namespace ci::app;
using namespace std;

class RibbonTestApp : public AppBasic {
	
public:
	void setup();
	void mouseUp( MouseEvent event );	
	void update();
	void draw();
	void addNewRibbon();
	
	Vec2i					mMousePos;
	list<Ribbon *>			mRibbons;	
	list<Ribbon *>			mCurrentRibbons;	
	bool					mIsTracing;
	
};

void RibbonTestApp::setup()
{
	mMousePos = Vec2i::zero();
	mIsTracing = false;
}

void RibbonTestApp::mouseUp( MouseEvent event )
{

	mIsTracing = !mIsTracing;
	
	// If we stopped drawing and there are any ribbons, close them out
	if(!mIsTracing){
		for(list<Ribbon *>::iterator r=mCurrentRibbons.begin(); r!=mCurrentRibbons.end();){
			(*r)->addFinalParticle(getMousePos());
			++r;
		}
		mCurrentRibbons.clear();
	}else{
		
		this->addNewRibbon();
		// Add a new ribbon to the current collection
		/*
		Ribbon *r = new Ribbon();
		mRibbons.push_back(r);
		mCurrentRibbons.push_back(r);		
		*/
	}
	
}

void RibbonTestApp::addNewRibbon()
{
	Ribbon *r = new Ribbon();
	mRibbons.push_back(r);
	mCurrentRibbons.push_back(r);			
}

void RibbonTestApp::update()
{

	if(mIsTracing){
		// Add to the current ribbons
		for(list<Ribbon *>::iterator r=mCurrentRibbons.begin(); r!=mCurrentRibbons.end();++r){
			(*r)->addParticle(getMousePos());
		}		
	}
	
	for(list<Ribbon *>::iterator r = mRibbons.begin(); r != mRibbons.end();++r){
		(*r)->update();
//		console() << "100x100 in ribbon? " << (pointFallsWithinShape(Vec2i(100,100), &((*r)->mParticles)) ? "YES" : "NO") << "\n";
	}
	
	int ribbonsSize = mRibbons.size();
	if(ribbonsSize > MAX_RIBBONS){
		int delta = ribbonsSize - MAX_RIBBONS;
		for(int i=0;i<delta;i++){
			Ribbon *r = mRibbons.front();
			mRibbons.pop_front();
			delete r;
		}
	}
	
}

void RibbonTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	
	for(list<Ribbon *>::iterator r = mRibbons.begin(); r != mRibbons.end(); ++r){
		(*r)->draw();
	}	
}

CINDER_APP_BASIC( RibbonTestApp, RendererGl );