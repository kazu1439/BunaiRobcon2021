/**********************************************************************
部内ロボコン　機体２　ジョイコン
**********************************************************************/
/**********************************************************************
   Include Libraries
**********************************************************************/
#include "ros/ros.h"
#include <sensor_msgs/Joy.h>
#include <vector>
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Bool.h"
#include <Eigen/Dense>

/**********************************************************************
   Declare variables(変数宣言)
**********************************************************************/
#define CYCLE_PERIOD 0.02f

enum MacroesJoyButtons
{ //デジタル値
    BUTTONS_SELECT = 0,
    BUTTONS_STICK_LEFT = 1,
    BUTTONS_STICK_RIGHT = 2,
    BUTTONS_START = 3,
    BUTTONS_CROSS_UP = 4,
    BUTTONS_CROSS_RIGHT = 5,
    BUTTONS_CROSS_DOWN = 6,
    BUTTONS_CROSS_LEFT = 7,
    BUTTONS_L2 = 8,
    BUTTONS_R2 = 9,
    BUTTONS_L1 = 10,
    BUTTONS_R1 = 11,
    BUTTONS_TRIANGLE = 12,
    BUTTONS_CIRCLE = 13,
    BUTTONS_CROSS = 14,
    BUTTONS_SQUARE = 15,
    BUTTONS_PAIRING = 16
};
enum MacroesJoyAxes
{ // アナログ値
    AXES_STICK_LEFT_X = 0,
    AXES_STICK_LEFT_Y = 1,
    AXES_STICK_RIGHT_X = 2,
    AXES_STICK_RIGHT_Y = 3,
    AXES_BUTTON_CROSS_UP = 8,
    AXES_BUTTON_CROSS_RIGHT = 9,
    AXES_BUTTON_CROSS_DOWN = 10,
    AXES_BUTTON_CROSS_LEFT = 11,
    AXES_BUTTON_L2 = 12,
    AXES_BUTTON_R2 = 13,
    AXES_BUTTON_L1 = 14,
    AXES_BUTTON_R1 = 15,
    AXES_BUTTON_TRIANGLE = 16,
    AXES_BUTTON_CIRCLE = 17,
    AXES_BUTTON_CROSS = 18,
    AXES_BUTTON_SQUARE = 19
};

std::vector<int> JoyButtonsArray(17, 0);
std::vector<float> JoyAxesArray(20, 0.0);

float a = 50;

std_msgs::Float32MultiArray msg_float;

/**********************************************************************
  Proto_type_Declare functions
**********************************************************************/
void joy_ps3_Callback(const sensor_msgs::Joy::ConstPtr &joy_msg);
/**********************************************************************
   Main
**********************************************************************/

int main(int argc, char **argv)
{
    ros::init(argc, argv, "robo2_joy");
    ros::NodeHandle nh;
    ros::Subscriber sub_first = nh.subscribe("joy_sub", 1, joy_ps3_Callback);         //joyコンの値を受け取る
    
    /*足回り用*/
    ros::Publisher pub_counter = nh.advertise<std_msgs::Float32>("joy_content", 100); //速度,旋回速度を送る

    /*射出、回収用*/
    ros::Publisher pub_counter = nh.advertise<std_msgs::Float32>("joy_retrieve_launch", 100); //シリンダー１、シリンダー２、サーボモーター１用送る

    /*花火打ち上げ用*/
    ros::Publisher pub_counter = nh.advertise<std_msgs::Float32>("firework", 100);//サーボモーター２,LED


    ros::Rate loop_rate(1.0f / CYCLE_PERIOD);

    while (ros::ok())
    {
        ros::spinOnce(); //callback関数を読み込む
        
        /*足回り用*/
        msg_float.data.resize(3);
        msg_float.data[0] = a * JoyAxesArray[AXES_STICK_LEFT_X]; //x軸に対する速度
        msg_float.data[1] = a * JoyAxesArray[AXES_STICK_LEFT_Y]; //ｙ軸に対する速度
        msg_float.data[2] = JoyAxesArray[AXES_STICK_RIGHT_X];    //旋回速度

        /*射出、回収用*/
        emission.data.resize(3);
        emission.data[3] = JoyAxesArray[AXES_BUTTON_CROSS_UP]; //シリンダー１
        emission.data[4] = JoyAxesArray[AXES_BUTTON_CROSS_RIGHT]; //シリンダー２
        emission.data[5] = JoyAxesArray[AXES_BUTTON_CROSS_LEFT];//サーボモーター1        

        /*花火打ち上げ用*/
        shot.data.resize(2);
        shot.data[6] = JoyAxesArray[AXES_BUTTON_TRIANGLE]; //サーボモーター２
        shot.data[7] = JoyAxesArray[AXES_BUTTON_CIRCLE]; //LED

        pub_counter.publish(msg_float);
        pub_counter.publish(emission);
        pub_counter.publish(shot);

        
        loop_rate.sleep();
    }
    return 0;
}

/**********************************************************************
   Functions <Call Back>
**********************************************************************/

void joy_ps3_Callback(const sensor_msgs::Joy::ConstPtr &joy_msg)
{
    JoyButtonsArray = joy_msg->buttons;
    JoyAxesArray = joy_msg->axes;
}