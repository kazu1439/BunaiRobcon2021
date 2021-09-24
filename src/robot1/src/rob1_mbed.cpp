/**
mbed側のプログラム。buildはエラー無く通ったが、動作確認はできていない。ポート番号は要変更。
**/
/**********************************************************************
Include Libraries
**********************************************************************/
#include "mbed.h"
#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include "rotary_encoder.h"
#include "dc_motor_1.h"
/**********************************************************************
Declare MACRO
**********************************************************************/
#define CTRL_PERIOD 0.02f
/**********************************************************************
Proto_type_Declare functions
**********************************************************************/
inline void order_Callback(const std_msgs::Float32MultiArray &msg);
inline void SetOrder();
inline void SetEnc();
/**********************************************************************
Declare variables
**********************************************************************/
Timer ControlTicker;
std_msgs::Float32MultiArray enc_msg;//エンコーダーの値を格納
float order_catch[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};//モーター1、モーター2、モーター3、ピストン、エアシリンダー、LEDの値を格納
/**********************************************************************
Declare Instances
**********************************************************************/
Serial PC(USBTX, USBRX, 115200);
rotary_encoder ENC1(D5,D4,NC,100,rotary_encoder::X4_ENCODING);
rotary_encoder ENC2(D5,D4,NC,100,rotary_encoder::X4_ENCODING);
rotary_encoder ENC3(D5,D4,NC,100,rotary_encoder::X4_ENCODING);
dc_motor_1 Motor1( D3, D8, 0 );
dc_motor_1 Motor2( D3, D8, 0 );
dc_motor_1 Motor3( D3, D8, 0 );
DigitalOut piston(A1);
DigitalOut cylinder(A1);
DigitalOut LED(A1);

ros::Publisher enc_pub("enc", &enc_msg);
ros::Subscriber <std_msgs::Float32MultiArray>order_sub("order", &order_Callback);

int main(int argc, char **argv)
{
  ControlTicker.start();
  ros::NodeHandle n;
  n.getHardware()->setBaud(115200);
  n.initNode();
  n.advertise(enc_pub);
  n.subscribe(order_sub);
  
  enc_msg.data_length = 3;
  enc_msg.data = (float *)malloc(sizeof(float)*3);
  for(int i = 0; i < enc_msg.data_length; i++){
      enc_msg.data[i] = 0.0;
  }
  for(;; ){
    if ( ControlTicker.read() >= CTRL_PERIOD ){
      ControlTicker.reset();
      SetOrder();
      SetEnc();
      enc_pub.publish(&enc_msg);
      n.spinOnce();
    }
  }
}

inline void order_Callback(const std_msgs::Float32MultiArray &msg)//
{
  for (int i = 0; i < 6; i++)
    {
      order_catch[i] = msg.data[i];
    }
}

inline void SetOrder()
{
  Motor1.drive(order_catch[0]);
  Motor2.drive(order_catch[1]);
  Motor3.drive(order_catch[2]);
  piston = order_catch[3];
  cylinder = order_catch[4];
  LED = order_catch[5];
}

inline void SetEnc(){
  enc_msg.data[0] = ENC1.getRad();
  enc_msg.data[1] = ENC2.getRad();
  enc_msg.data[2] = ENC3.getRad();
}