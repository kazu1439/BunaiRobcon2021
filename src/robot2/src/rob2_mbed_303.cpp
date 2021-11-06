/**
nucleo-446re用のプログラム
未だテスト段階
**/
/**********************************************************************
Include Libraries
**********************************************************************/
#include "mbed.h"
#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include "servo_motor.h"
#include "dc_motor_1.h"
/**********************************************************************
Declare MACRO
**********************************************************************/
#define CTRL_PERIOD 0.02f
/**********************************************************************
Proto_type_Declare functions
**********************************************************************/
inline void firework_Callback(const std_msgs::Float32MultiArray &msg);
inline void SetOrder();
inline void retrieve_launch_cb(const std_msgs::Float32MultiArray &msg); //回収機構と発射機構のcb
//inline void wheel_cb(const std_msgs::Float32MultiArray &msg);
/**********************************************************************
Declare variables
**********************************************************************/
Timer ControlTicker;
float order_catch[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; //モーター1、モーター2、モーター3、エアシリンダー1、エアシリンダー2、サーボモーター1、サーボモーター2、LEDの値を格納
/**********************************************************************
Declare Instances
**********************************************************************/
Serial PC(USBTX, USBRX, 115200);
//モータは第3引数の調節必要
DigitalOut cylinder0(PA_11);
DigitalOut cylinder1(PB_4);
ServoMotor servo0(PA_7, 0.0);
ServoMotor servo1(PA_3, 0.0);


ros::Subscriber<std_msgs::Float32MultiArray> order_sub("firework", &firework_Callback);
ros::Subscriber<std_msgs::Float32MultiArray> sub_retrieve_launch("robo_retrieve_launch", &retrieve_launch_cb);
//ros::Subscriber<std_msgs::Float32MultiArray> sub_wheel("v0_v1_v2", &wheel_cb);
int main(int argc, char **argv)
{
  ControlTicker.start();
  ros::NodeHandle n;
  n.getHardware()->setBaud(115200);
  n.initNode();
  n.subscribe(order_sub);
  n.subscribe(sub_retrieve_launch);
//  n.subscribe(sub_wheel);

  for (;;)
  {
    if (ControlTicker.read() >= CTRL_PERIOD)
    {
      ControlTicker.reset();
      SetOrder();
      n.spinOnce();
    }
  }
}

inline void SetOrder()
{
 // Motor0.drive(order_catch[0]);
//  Motor1.drive(order_catch[1]);
//  Motor2.drive(order_catch[2]);
//  cylinder0 = order_catch[3];
//  cylinder1 = order_catch[4];
  servo0.rot(order_catch[5]);
  servo1.rot(order_catch[6]);
//  LED = order_catch[7];
}

inline void firework_Callback(const std_msgs::Float32MultiArray &msg) //
{
  order_catch[6] = msg.data[0];//サーボ1
  order_catch[7] = msg.data[1];//LED
}

inline void retrieve_launch_cb(const std_msgs::Float32MultiArray &msg)
{
  order_catch[3] = msg.data[0]; //シリンダー0（発射機構）
  order_catch[4] = msg.data[1]; //シリンダー1（回収機構）
  order_catch[5] = msg.data[2]; //サーボモーター0（回収機構）
}

//inline void wheel_cb(const std_msgs::Float32MultiArray &msg){
//  order_catch[0] = msg.data[0];
//  order_catch[1] = msg.data[1];
//  order_catch[2] = msg.data[2];
//}
