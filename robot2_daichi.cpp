/**
射出機構：エアシリンダー1
回収機構：エアシリンダー2＆サーボモーター1
花火点火機構：サーボモーター2＆LED
足回り：モータ－1＆モーター2＆モーター3

mbed側のプログラム。buildはエラー無く通ったけど、動くかどうかは未知数。ポート番号は要変更。
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
inline void if_retrieval_launch();
/**********************************************************************
Declare variables
**********************************************************************/
Timer ControlTicker;
float order_catch[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; //モーター1、モーター2、モーター3、エアシリンダー1、エアシリンダー2、サーボモーター1、サーボモーター2、LEDの値を格納

int cylinder1_move = 0; //エアシリンダー1の伸び縮みを格納(伸びているとき1,縮んでいるとき0)
int count = 0;          //エアシリンダー1の2秒待つときのカウント
int servo1_angle = 0;   //サーボモーター1の角度を格納(0°の時0,180°の時1)
int cylinder2_move = 0; //エアシリンダー2の伸び縮みを格納(伸びているとき1,縮んでいるとき0)
/**********************************************************************
Declare Instances
**********************************************************************/
Serial PC(USBTX, USBRX, 115200);
dc_motor_1 Motor1(D3, D8, 0);
dc_motor_1 Motor2(D3, D8, 0);
dc_motor_1 Motor3(D3, D8, 0);
DigitalOut cylinder1(A1);
DigitalOut cylinder2(A1);
ServoMotor servo1(D4, 0.0);
ServoMotor servo2(D4, 0.0);
DigitalOut LED(A1);

ros::Subscriber<std_msgs::Float32MultiArray> order_sub("firework", &firework_Callback);

int main(int argc, char **argv)
{
    ControlTicker.start();
    ros::NodeHandle n;
    n.getHardware()->setBaud(115200);
    n.initNode();
    n.subscribe(order_sub);

    for (;;)
    {
        if (ControlTicker.read() >= CTRL_PERIOD)
        {
            ControlTicker.reset();
            SetOrder();
            if_retrieval_launch();
            n.spinOnce();
        }
    }
}

inline void SetOrder()
{
    Motor1.drive(order_catch[0]);
    Motor2.drive(order_catch[1]);
    Motor3.drive(order_catch[2]);
    cylinder1 = cylinder1_move;     //発射機構
    cylinder2 = cylinder2_move;     //回収機構
    servo1.rot(180 * servo1_angle); //(回収機構)0°か180°
    servo2.rot(order_catch[6]);
    LED = order_catch[7];
}

inline void firework_Callback(const std_msgs::Float32MultiArray &msg) //
{
    order_catch[6] = msg.data[0];
    order_catch[7] = msg.data[1];
    order_catch[3] = msg.data[2]; //エアシリンダー1(発射機構)
    order_catch[5] = msg.data[3]; //サーボモーター1(回収機構)
    order_catch[4] = msg.data[4]; //エアシリンダー2(回収機構)
}

inline void if_retrieval_launch()
{
    //発射機構のエアシリンダーの制御
    if (order_catch[3])
    {
        cylinder1_move = 1;
    }
    if (cylinder1_move)
    {
        count++;
    }
    //プログラムは50Hzで動いている、2秒待つことはcount = 100になる時
    if (count == 100)
    {
        cylinder1_move = 0;
        count = 0;
    }
    //回収機構のサーボモーターの制御
    if (order_catch[5] && servo1_angle == 0)
    {
        servo1_angle = 1;
    }
    else if (order_catch[5] && servo1_angle == 1)
    {
        servo1_angle = 0;
    }
    //回収機構のエアシリンダーの制御
    if (order_catch[4] && cylinder2_move == 0)
    {
        cylinder2_move = 1;
    }
    else if (order_catch[4] && cylinder2_move == 1)
    {
        cylinder2_move = 0;
    }
}