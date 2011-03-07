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

//bool pnpoly(int nVert, float *vertx, float *verty, float testx, float testy)
bool pointFallsWithinShape(const Vec2i &testPoint, list<RibbonParticle *> *particles)
{
	int nVert = particles->size();
	float vertx[nVert];
	float verty[nVert];
	int n=0;
	for(list<RibbonParticle *>::iterator p = particles->begin(); p != particles->end(); ++p){
		vertx[n] = (*p)->mPos.x;
		verty[n] = (*p)->mPos.y;
		n++;
	}

	int testx = testPoint.x;
	int testy = testPoint.y;
	int i, j = 0;
	bool c = false;
	for (i = 0, j = nVert-1; i < nVert; j = i++) {
		if (((verty[i]>testy) != (verty[j]>testy)) &&
			(testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]))
			c = !c;
	}
	return c;
}
/*
bool get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
						   float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;
	
    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);
	
    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return true;
    }
	
    return false; // No collision
}
*/
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