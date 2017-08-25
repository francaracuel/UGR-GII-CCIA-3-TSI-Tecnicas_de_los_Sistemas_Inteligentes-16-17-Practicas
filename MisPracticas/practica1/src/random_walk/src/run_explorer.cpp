////////////////////////////////////////////////////////////////////////////////
//
// Francisco Javier Caracuel Beltrán
//
// 3º A - TSI(CCIA) - 2017
//
// run_explorer.cpp
//

#include "explorer.h"

int main(int argc, char **argv) {

    // Initiate new ROS node named "explorer"
    ros::init(argc, argv, "explorer");

    // Create new stopper object
    Explorer stopper;

    // Start the movement
    stopper.startMoving();

    return 0;

};



