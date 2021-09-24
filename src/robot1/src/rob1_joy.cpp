/**
射出機構：モーター1＆エンコーダー1
回収機構：ピストン
花火点火機構：エアシリンダー＆LED
足回り：モータ－2＆エンコーダー2,モーター3＆エンコーダー3

dr_joy.cpp（Gitのfwsリポジトリにある）をコピペして使えそうなところを残した。
まだいろいろ途中
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

enum MacroesJoyButtons
{ //デジタル値0と1が瞬間的に変わる
    BUTTONS_CROSS = 0,
    BUTTONS_CIRCLE = 1,
    BUTTONS_TRIANGLE = 2,
    BUTTONS_SQUARE = 3,
    BUTTONS_L1 = 4,
    BUTTONS_R1 = 5,
    BUTTONS_L2 = 6,
    BUTTONS_R2 = 7,
    BUTTONS_SELECT = 8,
    BUTTONS_START = 9,
    BUTTONS_PAIRING = 10,
    BUTTONS_STICK_LEFT = 11,
    BUTTONS_STICK_RIGHT = 12,
    BUTTONS_CROSS_UP = 13,
    BUTTONS_CROSS_DOWN = 14,
    BUTTONS_CROSS_LEFT = 15,
    BUTTONS_CROSS_RIGHT = 16
};
enum MacroesJoyAxes
{ // アナログ値0と1が段階的に変わる
    AXES_STICK_LEFT_X = 1,
    AXES_STICK_LEFT_Y = 0,
    AXES_STICK_RIGHT_X = 4,
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
std_msgs::Float32MultiArray order_msg;


const float max_robot_velocity = 1.0f;
const float max_robot_rad_velocity = 1.0f;
float enc[3] = {0,0,0};

//平滑化フィルタ
float smoothly_filter_data_x[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float smoothly_filter_data_y[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/**********************************************************************
   Proto_type_Declare functions
**********************************************************************/
void joy_ps3_Callback(const sensor_msgs::Joy::ConstPtr &joy_msg);
void enc_Callback(const std_msgs::Float32MultiArray::ConstPtr &msg);
//平滑化フィルタ
inline float smoothly_filter_x(float raw_data);
inline float smoothly_filter_y(float raw_data);

/**********************************************************************
   Main
**********************************************************************/

int main(int argc, char **argv)
{
    ros::init(argc, argv, "tr_joy");
    ros::NodeHandle nh;
    ros::Publisher pub_order = nh.advertise<std_msgs::Float32MultiArray>("order", 100);

    ros::Subscriber sub_first = nh.subscribe("joy", 1, joy_ps3_Callback); //joy
    ros::Subscriber enc_sub = nh.subscribe("enc", 1, enc_Callback);
    ros::Rate loop_rate(1.0f / CTRL_PERIOD);

    order_msg.data.resize(6);//{Motor1,Motor2,Motor3,piston,cylinder,LED}

    while (ros::ok()) 
    {
        ros::spinOnce();
        
        pub_order.publish(order_msg);  
        loop_rate.sleep();
    }
    return 0;
}

/**********************************************************************
   Functions
**********************************************************************/

void joy_ps3_Callback(const sensor_msgs::Joy::ConstPtr &joy_msg)
{
    JoyButtonsArray = joy_msg->buttons;
    JoyAxesArray = joy_msg->axes;
}

void enc_Callback(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
    for (int i = 0; i < 3; i++)
    {
        enc[i] = msg->data[i];
    }
}

inline float smoothly_filter_x(float raw_data)
{
    for (int i = 4; i > 0; i--) //4<-3<-2<-1<-0
    {
        smoothly_filter_data_x[i] = smoothly_filter_data_x[i - 1]; //データの更新
    }
    smoothly_filter_data_x[0] = raw_data; //新しく受け取ったデータの格納
    float sum_data = 0;
    for (int i = 0; i < 5; i++)
    {
        sum_data += smoothly_filter_data_x[i]; //５つ前のデータをすべて足す．
    }
    sum_data = sum_data / 5.0f; //これまでの平均を出す
    // ROS_INFO("xs= %4.4f, \r\n", sum_data);
    return sum_data;
}

inline float smoothly_filter_y(float raw_data) //xと同様
{
    for (int i = 4; i > 0; i--)
    {
        smoothly_filter_data_y[i] = smoothly_filter_data_y[i - 1];
    }
    smoothly_filter_data_y[0] = raw_data;
    float sum_data = 0;
    for (int i = 0; i < 5; i++)
    {
        sum_data += smoothly_filter_data_y[i];
    }
    sum_data = sum_data / 5.0f;
    return sum_data;
}