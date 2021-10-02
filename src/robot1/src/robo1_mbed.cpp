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
bool inject_flag = false;
bool a = true;
float enc = 0;
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
AnalogOut ignition_flag(A3);//花火の電磁弁
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
  geometry_msgs::Twist vel;

  ros::init(argc, argv, "robo_1");
  ros::NodeHandle nh;
  ros::Rate       loop_rate(1.0 / CTRL_PERIOD);
  ros::Subscriber sub_function_flag = nh.subscribe("inject_flag",1,cb_function);
  ros::Subscriber sub_directions = nh.subscribe("direction_array",1,cb_directions);



  ros::Publisher  pub_vel = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

  while (ros::ok()) {
    ros::spinOnce();
    Move(front_back, right_left);
    
    //回収
    if(collect_flag){
      Collect();
    }
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
  // ignition_flag = array->data[0];
  if (array->data[0]){
    ignition_flag= !ignition_flag;
  }
  // injection_flag = array->data[1];
  else if (array->datra[1]){
    injection_flag = !injection_flag;
  }
  // collect_flag = array->data[2];
  else if (array->data[2]){
    collect_flag = !collect_flag;
  } 

}

void Move(float front_back, float right_left){
  if(front_back>0){
    M_r.drive(front_back);
    M_l.drive(front_back);
  }
  else if(front_back < 0){
    M_r.drive(front_back);
    M_l.drive(front_back);
  }

  else if(right_left>0){
    M_r.drive(-right_left);
    M_l.drive(right_left);
  }

  else if (right_left < 0){
    M_r.drive(-right_left);
    M_l.drive(right_left);
  }
}

void Ignition(){
  //ピストンを伸ばす→待期(とりあえず5秒)→ピストンを戻す→LEDを付ける
  ignition_flag = 1;
  ros::Duration(5.0).sleep();
  ignition_flag = 0;
  led0 = 1;
}

void Injection(){
    float enc=0;
      // INJECTION--
    // 射出、引いた状態まで、1.57は変更すると思う
    while(a){
        if(enc>1.57){
          Minject.drive(0);
          a =false;
          break;
        }
        ros::spinOnce();
        Minject.drive(10);
        enc = ENC_inject.getRad();
        ROS_INFO("enc%f",enc);
        loop_rate.sleep();
    }


    // encを0に戻す。
    enc = 0;

    ROS_INFO("set enc 0");
    // 発射の信号が送られてきたら
    if(inject_flag){
        while(enc < 6.28){
            //motor回転
            ros::spinOnce();
            Minject.drive(10);
            enc = ENC_inject.getRad();
            ROS_INFO("enc%f",enc);
            loop_rate.sleep();
        }
        ROS_INFO("done");
        //発射の合図を待つ、エンコーダをリセット、初期状態に戻すためにa=true
        inject_flag = false;
        enc = 0;
        a = true;
    }
    // INJECTION--
}

void Collect