

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU9250.h"
#include <sensor_msgs/Imu.h>
#include <std_msgs/Bool.h>
#include <sensor_msgs/MagneticField.h>

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU9250 accelgyro;
I2Cdev   I2C_M;

uint8_t buffer_m[6];


int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t   mx, my, mz;


float Axyz[3];
float Gxyz[3];
float Mxyz[3];


#define sample_num_mdate  5000      

// pour charger une calibration il faut changer les valeurs des centres ici 
static float mx_centre = 0;
static float my_centre = 0;
static float mz_centre = 0;

sensor_msgs::Imu imuMsgs;
ros::Publisher pubImu("ardu_send_imu",&imuMsgs);


sensor_msgs::MagneticField magMsgs;
ros::Publisher pubMag("ardu_send_mag",&magMsgs);
