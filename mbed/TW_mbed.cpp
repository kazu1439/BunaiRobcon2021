/**********************************************************************
   対向二輪のmbedでのモーター制御
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

Timer      ControlTicker;

float box[2] = {0.0f, 0.0f}; //vr, vlを格納

float vr;
float vl;
/**********************************************************************
   Proto_type_Declare functions
**********************************************************************/
void recieve_cb( const std_msgs::Float32MultiArray &msg );
ros::Subscriber <std_msgs::Float32MultiArray> sub_speed("vr_vl", &recieve_cb);


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
        
        vr = box[0];
        vl = box[1];
        
        M0.drive(vr);
        M1.drive(vl);
        
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
    for(int i = 0; i < 2; i++){
        box[i] = msg.data[i];
        }
}
