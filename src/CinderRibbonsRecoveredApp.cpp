#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#import "RibbonParticle.h"
#import "Ribbon.h"
#include <list>
//#include <vector>
#include <math.h>
#include "cinder/Rand.h"
#import "Goal.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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

class RibbonTestApp : public AppBasic {
	
public:
	void setup();
	void mouseUp( MouseEvent event );	
	void update();
	void draw();
	void addNewRibbon();
	
	Vec2i					mMousePos;
    Ribbon                  *mCurrentRibbon;
    list<Goal *>            mGoals;
    int                     mMaxGoalAge;
	bool					mIsTracing;
	
};

void RibbonTestApp::setup()
{
    mMaxGoalAge = 100;
	mMousePos = Vec2i::zero();
	mIsTracing = false;
    mCurrentRibbon = NULL;
}

void RibbonTestApp::mouseUp( MouseEvent event )
{

	mIsTracing = !mIsTracing;
	
	// If we stopped drawing and there are any ribbons, close them out
	if(!mIsTracing){

        if(mCurrentRibbon){
            mCurrentRibbon->addFinalParticle(getMousePos());
        }
	}else{
		
		this->addNewRibbon();

	}
	
}

void RibbonTestApp::addNewRibbon()
{
    if(mCurrentRibbon){
        delete mCurrentRibbon;
    }
    mCurrentRibbon = new Ribbon();
}

void RibbonTestApp::update()
{
    if(Rand::randInt(1000) < 5){
        // Randomly add goals
        Goal *g = new Goal(Vec2i(Rand::randInt(640), Rand::randInt(480)));
        mGoals.push_back(g);
    }

    // IMPORTANT: Only add particles if the mouse has moved
	if(mIsTracing && mMousePos != getMousePos()){
        mMousePos = getMousePos();
        mCurrentRibbon->addParticle(mMousePos);
	}
    
    // NOTE: This must happen after addParticle and before ribbon.update()
    if(mCurrentRibbon && mCurrentRibbon->mIntersectionParticles.size() > 0){
        // There's an intersection. Do a point check on all of the goals
        for(list<Goal *>::iterator g = mGoals.begin(); g != mGoals.end(); ++g){
            bool hitTest = pointFallsWithinShape((*g)->mPos, &(mCurrentRibbon->mIntersectionParticles));
            if(hitTest){
                (*g)->mIsCaptured = true;
                console() << "HIT AT x:" << (*g)->mPos.x << " y:" << (*g)->mPos.y << "\n";
            }            
        }        
    }
	
    if(mCurrentRibbon){
        mCurrentRibbon->update(); 
    }

	for(list<Goal *>::iterator g = mGoals.begin(); g != mGoals.end(); ++g){

		(*g)->update();
        
        // If it's too old or is captured, remove it
        if( (*g)->mAge > mMaxGoalAge || (*g)->mIsCaptured ){
            delete (*g);
            mGoals.remove((*g));
        }else{
//            ++g;
        }
	}    
}


void RibbonTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	
    if(mCurrentRibbon){
        mCurrentRibbon->draw();
    }

    for(list<Goal *>::iterator g = mGoals.begin(); g != mGoals.end(); ++g){
		(*g)->draw();
	}	
    
    gl::color(Color::white());
    gl::drawSolidCircle(Vec2i(100,100), 10);
}

CINDER_APP_BASIC( RibbonTestApp, RendererGl );