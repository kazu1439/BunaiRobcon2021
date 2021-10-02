/**********************************************************************
joyコンを使って三輪オムニを動かすプログラム
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
    AXES_STICK_RIGHT_X = 3,
    AXES_STICK_RIGHT_Y = 4,
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

float a = 30;

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
    ros::init(argc, argv, "joy_omuni");
    ros::NodeHandle nh;
    ros::Subscriber sub_first = nh.subscribe("joy", 1, joy_ps3_Callback);                       //joyコンの値を受け取る
    ros::Publisher pub_counter = nh.advertise<std_msgs::Float32MultiArray>("joy_content", 100); //速度,旋回速度を送る
    ros::Rate loop_rate(1.0f / CYCLE_PERIOD);

    while (ros::ok())
    {
        ros::spinOnce(); //callback関数を読み込む

        // ROS_INFO("%f", JoyAxesArray[AXES_STICK_LEFT_Y]);

        msg_float.data.resize(3);

        msg_float.data[0] = a * JoyAxesArray[AXES_STICK_LEFT_X]; //x軸に対する速度
        msg_float.data[1] = a * JoyAxesArray[AXES_STICK_LEFT_Y]; //ｙ軸に対する速度
        msg_float.data[2] = JoyAxesArray[AXES_STICK_RIGHT_X];    //旋回速度

        pub_counter.publish(msg_float); //mbedに送る
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