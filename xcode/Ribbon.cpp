/*
 *  Ribbon.cpp
 *  CinderRibbonsRecovered
 *
 *  Created by William Lindmeier on 3/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Ribbon.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/Path2d.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace std;


bool getLineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;
	
    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	// NOTE: We dont want "or equal to"
//	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    if (s > 0 && s < 1 && t > 0 && t < 1)
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


Ribbon::Ribbon()
{
	mParticleHead = NULL;
}

Ribbon::~Ribbon()
{
	for(list<RibbonParticle *>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
		RibbonParticle *par = (*p);
		delete par;
	}
	mParticles.clear();
}

void Ribbon::update()
{	
	for(list<RibbonParticle *>::iterator p = mParticles.begin(); p != mParticles.end();++p){
		(*p)->update();
	}	
}

void Ribbon::addFinalParticle(const Vec2i &position)
{
	// Adding a "zero" particle so the ribbons dont connect
	if(mParticleHead){
		RibbonParticle *p = new RibbonParticle;
		Vec2f lastVel = Vec2f(mParticleHead->mVel);
		p->mPos = Vec2f(position.x + (lastVel.x * 0.5), 
						position.y + (lastVel.y * 0.5));
		mParticles.push_back(p);	
	}
}

void Ribbon::addParticle(const Vec2i &position)
{
	Vec2f partVel = Vec2f::zero();
	if(mParticleHead != NULL){
		// If there are already particles, infer the particle velocity from the last particle's position
		//partVel = position - mParticleHead->mPos;
		partVel = position - mParticleHead->mPos;
	}
	
	RibbonParticle *p = new RibbonParticle;
	p->mPos = position;	
	p->mVel = partVel;
	
	Vec2f velNormal(partVel);
	velNormal.rotate(90 * M_PI / 180.0);
	p->mVelNormal = velNormal;	
	
	mParticles.push_back(p);
	
	// Update the HEAD
	if(mParticleHead){
		mParticleHead->mNextParticle = p;
		p->mPrevParticle = mParticleHead;
		
		// Check if the particle intersects.
		// NOTE: This isn't the most sophisticated way of doing it since in many cases we'll dupe the array w/out ever using it
		std::list<RibbonParticle *>	shapeParticles;
		
		bool intersected = false;
		
		for(list<RibbonParticle *>::reverse_iterator op = mParticles.rbegin(); op != mParticles.rend(); ++op){
			
			if(!intersected){
				
				shapeParticles.push_front(*op);
				
				// If the other particle is not me, and the other particle has a previous particle
				if((*op) != p && (*op)->mPrevParticle && p->mPrevParticle && p->mPrevParticle != (*op)){
					
					if(getLineIntersection(p->mPos.x, p->mPos.y, p->mPrevParticle->mPos.x, p->mPrevParticle->mPos.y,
										   (*op)->mPos.x, (*op)->mPos.y, (*op)->mPrevParticle->mPos.x, (*op)->mPrevParticle->mPos.y,
										   NULL, NULL)){						
						intersected = true;			
					}
				}
				
			}else{
				
				// Deleting the remaining particles, since we're just going to use the new list
				delete *op;
			}
		}		
		
		if(intersected){
			mParticles.clear();
			mParticles = shapeParticles;
		}
	}
	mParticleHead = p;	
			
}

void Ribbon::draw()
{
	Vec2i	normPos1, normPos2;
	normPos1 = Vec2f::zero();
	normPos2 = Vec2f::zero();
	
	for(list<RibbonParticle *>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
		
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
		
		float value = ((*p)->mAge / 200.0);

		// Draw the filled ribbon
		gl::color(Color(1.0-value,1.0-value,0));
		gl::drawSolid(path);
		
		// Draw the surface normal
		gl::color(Color(1.0-value,0,0));
		gl::drawLine(normPos1, normPos2);
		
		// Draw a line indicating it's position w/ velocity
		gl::color(Color::black());
		gl::drawLine(Vec2i(x1,y1), Vec2i(x2, y2));		
		
	}
}
