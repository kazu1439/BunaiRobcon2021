/**********************************************************************
   対向二輪の運動モデル
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

std::vector<float> vx_vy = {0.0f, 0.0f}; //vx, vy

float ruto2 = 1.41421356; //車輪と車体中心までの距離
std_msgs::Float32MultiArray msg_two_speed; //vR,vLを送る
float mata[2] = {0.0f, 0.0f}; //
/**********************************************************************
   Proto_type_Declare functions(受け取る関数)
**********************************************************************/
void two_speed_cb(const std_msgs::Float32MultiArray::ConstPtr &msg);
/**********************************************************************
   Main
**********************************************************************/

int main(int argc, char **argv)
{
   ros::init(argc, argv, "actionmodel_two");
   ros::NodeHandle nh;
   ros::Subscriber sub_second = nh.subscribe("joy_content", 100, two_speed_cb);         //joyコンの値を受け取る
   ros::Publisher pub_float = nh.advertise<std_msgs::Float32MultiArray>("vr_vl", 300);
   ros::Rate loop_rate(1.0f / CYCLE_PERIOD);
   msg_two_speed.data.resize(2);

   while (ros::ok())
   {
      ros::spinOnce();                                  //callback関数を読み込む  
      Eigen::Matrix<float, 2, 1> x;                                  //車体のy速度、ω
      x << vx_vy[0],
           vx_vy[1];
      Eigen::Matrix<float, 2, 2> A;                                       //運動モデルの行列式のやつ
      A << 1.0/ruto2, -1.0/ruto2,
      1.0/ruto2, 1.0/ruto2;
      Eigen::Matrix<float, 2, 1> result;
      result << 0,
      0;
      result = A * x;
      for(int i = 0; i < 2; i++){
          mata[i] = result(i, 0);
          msg_two_speed.data[i] = mata[i];
      }
       pub_float.publish(msg_two_speed);

       ROS_INFO("%f, %f", vx_vy[0], vx_vy[1]);

            
      loop_rate.sleep();                                //処理が制御周期より早く終わった時，ここで止まって残った時間を使う
   }
   return 0; 
}

/**********************************************************************
   Functions <Call Back>
**********************************************************************/

void two_speed_cb(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
    vx_vy = msg->data;

}
