/*
 *  Goal.cpp
 *  CinderRibbonsRecovered
 *
 *  Created by Bill Lindmeier on 3/7/11.
 *  Copyright 2011 R/GA. All rights reserved.
 *
 */

#include "Goal.h"
#include "cinder/gl/gl.h"

using namespace ci;

Goal::Goal(const Vec2i &position)
{
    mPos = position;
    mIsCaptured = false;
    mAge = 0;    
}

void Goal::update(){
    mAge++;
}

void Goal::draw(){
    if(mIsCaptured){
        gl::color(Color(0.0, 1.0, 0.0));
    }else{
        gl::color(Color(1.0, 0.0, 0.0));
    }
    
    gl::drawSolidCircle(mPos, 10.0);
}