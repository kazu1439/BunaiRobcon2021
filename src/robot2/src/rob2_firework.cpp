/**
射出機構：エアシリンダー1
回収機構：エアシリンダー2＆サーボモーター1
花火点火機構：サーボモーター2＆LED
足回り：モータ－1＆モーター2＆モーター3

花火点火機構のプログラム
mbed側にpublishするところだけ書いた
**/

/**********************************************************************
   Include Libraries
**********************************************************************/

#include "ros/ros.h"
#include "sensor_msgs/Joy.h"
#include <vector>
#include "std_msgs/Float32MultiArray.h"
#include <string>
#include <cstdio>
#include <cmath>

/**********************************************************************
   Declare variables
**********************************************************************/
#define CTRL_PERIOD 0.02f

std_msgs::Float32MultiArray firework_msg;

/**********************************************************************
   Proto_type_Declare functions
**********************************************************************/


/**********************************************************************
   Main
**********************************************************************/

int main(int argc, char **argv)
{
    ros::init(argc, argv, "rob2_firework");
    ros::NodeHandle nh;
    ros::Publisher pub_firework = nh.advertise<std_msgs::Float32MultiArray>("firework", 100);

    ros::Rate loop_rate(1.0f / CTRL_PERIOD);

    firework_msg.data.resize(2);//{servo2,LED}

    while (ros::ok()) 
    {
        ros::spinOnce();
        
        pub_firework.publish(firework_msg);  
        loop_rate.sleep();
    }
    return 0;
}

/**********************************************************************
   Functions
**********************************************************************/

