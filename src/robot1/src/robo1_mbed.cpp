/*

*/
/**********************************************************************
Define MACROES
**********************************************************************/
#define CTRL_PERIOD 0.02f
/**********************************************************************
Include Libraries
**********************************************************************/
// #include <mbed.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <math.h>
#include <rotary_encoder.h>
/**********************************************************************
Declare variables
**********************************************************************/
bool ignition_flag = false;
bool injection_flag = false;
bool collect_flag = false;

bool a = true;
float enc;
float first_angle = 1.57;

float front_back;
float right_left;





// DigitalIn Sw(A1);
// DigitalOut Led0(A3);
// DigitalOut Led1(A0);
// Timer ControlTicker;
dc_motor_1 M_r( A6, A11, 1);//右車輪
dc_motor_1 M_l( D3, D8, 1 ); //左車輪
AnalogOut collect();//回収機構の電磁弁
AnalogOut ignition(A3);//花火の電磁弁
DigitalOut led0(A4);//花火
// LPF lpf_0( CTRL_PERIOD, 0.3 );
// PID pid( CTRL_PERIOD );
rotary_encoder ENC_inject(D4,D5,NC,50,rotary_encoder::X4_ENCODING);
rotary_encoder ENC_M_l();//左舎利のエンコーダ
rotary_encoder ENC_M_r();//右車輪のエンコーダ

/**********************************************************************
Proto_type_Declare functions
**********************************************************************/
void cb_function(const std_msgs::Int32MultiArray &function_array);
void cb_directions(const std_msgs::Float32MultiArray::ConstPtr &direction_array);

/**********************************************************************
Main
**********************************************************************/
int main(int argc, char **argv)
{
  ros::init(argc, argv, "robo_1");
  ros::NodeHandle nh;
  ros::Rate       loop_rate(1.0 / CTRL_PERIOD);
  ros::Subscriber sub_function_flag = nh.subscribe("inject_flag",1,cb_function);
  ros::Subscriber sub_directions = nh.subscribe("direction_array",1,cb_directions);

  while (ros::ok()) {
    ros::spinOnce();
    //常に射出の待機状態にする
    //最初だけ回る、射出の際は一周するので初期位置に戻ってくる
    enc = 0;
    while(a){
      if(enc>1.57){
        Minject.drive(0);
        a =false;
        break;
      }
      Minject.drive(10);
      enc = ENC_inject.getRad();
    }

    Move(front_back, right_left);
    
    //回収
    Collect()
    //射出
    else if(injection_flag){
      Injection();
    }
    //花火
    else if(ignition_flag){
      Ignition();
    }

    }
  return 0;
}

/**********************************************************************
Functions
**********************************************************************/
void cb_directions(const std_msgs::Float32MultiArray::ConstPtr &array){
  front_back = array->data[0];
  right_left = array->data[1];

}

void cb_function(const std_msgs::Int32MultiArray::ConstPtr& array){
  //The JoyButtonsArray should output 0 or 1.
  // change the state of each sign when push buttons 
  // ignition_flag = array->data[0];
  if (array->data[0]){
    ignition_flag= true;
  }
  // injection_flag = array->data[1];
  else if (array->datra[1]){
    injection_flag = true;
  }
  // collect_flag = array->data[2];
  else if (array->data[2]){
    collect_flag = true;
  } 

}

void Move(float front_back, float right_left){
  // stop
  if (front_back != 0 && right_left!=0) || (front_back == 0 && right_left==0){
    M_r.drive(0);
    M_l.drive(0);    
  }
  //forward
  else if(front_back>0){
    M_r.drive(front_back);
    M_l.drive(front_back);
  }
  //backward
  else if(front_back < 0){
    M_r.drive(front_back);
    M_l.drive(front_back);
  }
  //turn right
  else if(right_left>0){
    M_r.drive(-right_left);
    M_l.drive(right_left);
  }
  //turn left
  else if (right_left < 0){
    M_r.drive(-right_left);
    M_l.drive(right_left);
  }
}

void Ignition(){
  //ピストンを伸ばす→待期(とりあえず5秒)→ピストンを戻す→LEDを付ける
  ignition = 1;
  ros::Duration(5.0).sleep();
  ignition = 0;
  led0 = 1;
  ignition_flag = false;
}


void Injection(){
    enc=0;
    while(enc < 6.28){
        //motor回転
        Minject.drive(10);
        enc = ENC_inject.getRad();
    }
    inject_flag = false;
    enc = 0;
    // a = true;
}

void Collect(){
  if(collect_flag && state == false){
    //ピストンを伸ばす
    collect = 1;
    state = !state;
  }
  if(collect_flag && state == true){
    //ピストンを縮める
    collect = 0;
    state = !state;
  }

}