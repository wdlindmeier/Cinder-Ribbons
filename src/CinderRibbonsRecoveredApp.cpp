#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
//#import "RibbonParticle.h"
#import "Ribbon.h"
#include <list>
//#include <vector>
#include <math.h>

#define MAX_RIBBONS	20

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
	list<Ribbon *>			mRibbons;
	
	// TODO: We will need to manage multiple simultanious ribbons
	Ribbon					*mCurrentRibbon;
	
	bool mMouseIsDown;
	
};

void RibbonTestApp::setup()
{
	mMouseIsDown = false;
	mMousePos = Vec2i::zero();
	mCurrentRibbon = NULL;
}

void RibbonTestApp::mouseDown( MouseEvent event )
{
	mMouseIsDown = true;
	
	// Start a ribbon
	Ribbon *r = new Ribbon();
	mRibbons.push_back(r);
	mCurrentRibbon = r;
	
}

void RibbonTestApp::mouseUp( MouseEvent event )
{
	mMouseIsDown = false;
	// End the ribbon
	if(mCurrentRibbon){
		mCurrentRibbon->addFinalParticle(getMousePos());
		mCurrentRibbon = NULL;
	}
	
}

void RibbonTestApp::update()
{
	if(mMouseIsDown){

		// Add to the current ribbon		
		if(mCurrentRibbon){
			mCurrentRibbon->addParticle(getMousePos());
		}
		
	}
	
	for(list<Ribbon *>::iterator r = mRibbons.begin(); r != mRibbons.end();++r){
		(*r)->update();
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
	
	for(list<Ribbon *>::iterator r = mRibbons.begin(); r != mRibbons.end(); r++){
		(*r)->draw();
	}	
}


CINDER_APP_BASIC( RibbonTestApp, RendererGl )
