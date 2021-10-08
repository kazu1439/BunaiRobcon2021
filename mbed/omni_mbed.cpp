/**********************************************************************
   File    joy_sample.cpp
   Author  Takahiro Yamazaki
   Environment    ROS_kinetic
   OS       Ubuntu 16.04 LTS
**********************************************************************/
/**********************************************************************
   Problems to be fixed
    -
**********************************************************************/
/**********************************************************************
   Include Libraries
**********************************************************************/

#include <ros.h>
#include <sensor_msgs/Joy.h>
#include <vector>
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Bool.h"
#include "mbed.h"
#include "dc_motor_1.h"


/**********************************************************************
   Declare variables
**********************************************************************/
#define CTRL_PERIOD 0.02f
dc_motor_1 M0(PB_0, PF_1, 1);
dc_motor_1 M1(PA_7, PB_5, 1 ); 
dc_motor_1 M2(PA_3, PA_1, 1 ); 

Timer      ControlTicker;

float box[3] = {0.0f, 0.0f, 0.0f}; //v0, v1 ,v2を格納

float v0;
float v1;
float v2;
/**********************************************************************
   Proto_type_Declare functions
**********************************************************************/
void recieve_cb( const std_msgs::Float32MultiArray &msg );
ros::Subscriber <std_msgs::Float32MultiArray> sub_speed("v0_v1_v2", &recieve_cb);


/**********************************************************************
   Main
**********************************************************************/

int main( int argc, char **argv ){
    ControlTicker.start();
  /*----- NodeHandle 設定 -----*/
  ros::NodeHandle nh;
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.subscribe(sub_speed);
  

    for(;;){
       if (ControlTicker.read() >= CTRL_PERIOD) {
        ControlTicker.reset();
        
        v0 = box[0];
        v1 = box[1];
        v2 = box[2];


        M0.drive(v0);
        M1.drive(v1);
        M2.drive(v2);
        nh.spinOnce();
       }
    }
    return 0;
}

/**********************************************************************
   Functions
**********************************************************************/

void recieve_cb(const std_msgs::Float32MultiArray &msg)
{
    for(int i = 0; i < 3; i++){
        box[i] = msg.data[i];
        }
}
