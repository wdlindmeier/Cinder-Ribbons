/*
 *  Goal.h
 *  CinderRibbonsRecovered
 *
 *  Created by Bill Lindmeier on 3/7/11.
 *  Copyright 2011 R/GA. All rights reserved.
 *
 */

#include "cinder/Vector.h"

class Goal {

public:  
    Goal(const ci::Vec2i &position);
    void draw();
    void update();
    bool    mIsCaptured;
    ci::Vec2i   mPos;
    int     mAge;
    
};
