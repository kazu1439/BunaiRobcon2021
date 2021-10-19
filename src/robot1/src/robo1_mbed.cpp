/*

*/
/**********************************************************************
Define MACROES
**********************************************************************/
#define CTRL_PERIOD 0.02f
/**********************************************************************
Include Libraries
**********************************************************************/
#include <mbed.h>
#include <ros/ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Bool.h>
#include <math.h>
#include <rotary_encoder.h>
/**********************************************************************
Declare variables
**********************************************************************/
bool ignition_flag = false; //花火
bool injection_flag = false;//射出
bool collect_flag = false;//回収
bool state = false;//ピストンが縮んでるか否か


bool a = true; //射出機構、どこまで引いたかの判定
float enc;　// どんだけ回ったか

float front_back;　//前後
float right_left; //旋回


Timer ControlTicker;
dc_motor_1 M_r( A6, A11, 1);//右車輪
dc_motor_1 M_l( D3, D8, 1 ); //左車輪
AnalogOut collect();//回収機構の電磁弁
AnalogOut ignition(A3);//花火の電磁弁
DigitalOut led0(A4);//花火
rotary_encoder ENC_inject(D4,D5,NC,50,rotary_encoder::X4_ENCODING);
<<<<<<< HEAD
=======
// rotary_encoder ENC_M_l();//左舎利のエンコーダ
// rotary_encoder ENC_M_r();//右車輪のエンコーダ
>>>>>>> 4ab942debafc345dcde667828ae564eea2f31e84

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
  ros::NodeHandle nh; //nodehandleをnhという名前でつかう
  ros::Rate       loop_rate(1.0 / CTRL_PERIOD); //50ｈｚでまわす
  ros::Subscriber sub_function_flag = nh.subscribe("inject_flag",1,cb_function); //topic名"inject_flag"でcb_functionを実行 
  ros::Subscriber sub_directions = nh.subscribe("direction_array",1,cb_directions);//topic名"inject_flag"でcb_functionを実行 

  while (ros::ok()) {
    ros::spinOnce();
    //常に射出の待機状態にする
    //最初だけ回る、射出の際は一周するので初期位置に戻ってくる
    enc = 0; //射出モータ
    while(a){
      if(enc>1.57){
        Minject.drive(0); //1.57以上になったら回転を止める
        a =false;//whileの判定
        break;
      }
      Minject.drive(10);//10でまわす
      enc = ENC_inject.getRad();//回転角の読み取り
    }

    Move(front_back, right_left);// 動く
    
    //回収
    if(collect_flag){
      Collect()
    }
    //射出
    else if(injection_flag){
      Injection();
    }
    //花火
    else if(ignition_flag){
      Ignition();
    }

    loop_rate.sleep();

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
  else{
    ignition_flag = false;
    injection_flag = false;
    collect_flag = false;
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
<<<<<<< HEAD

void check_state(){
  ROSINFO("%d",front_back);
  ROSINFO("%d",right_left);
  ROSINFO("%d",ignition_flag);
  ROSINFO("%d",injection_flag);
  ROSINFO("%d",collect_flag);
  ROSINFO("%d",a);
  ROSINFO("%d",state);
}
=======
>>>>>>> 4ab942debafc345dcde667828ae564eea2f31e84
