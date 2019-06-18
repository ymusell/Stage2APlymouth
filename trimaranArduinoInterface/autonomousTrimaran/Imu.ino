
void setupImu(){
 accelgyro.initialize();
 nh.advertise(pubImu);
 nh.subscribe(imuSub);
 Serial.println("Testing device connections...");
 Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
 delay(1000);
}


void updateImu(){
  getAccel_Data();
  getGyro_Data();
  getCompassDate_calibrated(); // compass data has been calibrated here 
  getHeading();       //before we use this function we should run 'getCompassDate_calibrated()' frist, so that we can get calibrated data ,then we can get correct angle .          
  getTiltHeading();
  publishImu();           
}


void publishImu(){
  //nh.loginfo("envoie imu ...");
  imuMsgs.linear_acceleration.x = Axyz[0];
  imuMsgs.linear_acceleration.y = Axyz[1];
  imuMsgs.linear_acceleration.z = Axyz[2];
  imuMsgs.angular_velocity.x = Gxyz[0];
  imuMsgs.angular_velocity.y = Gxyz[0];
  imuMsgs.angular_velocity.z = Gxyz[0];
  imuMsgs.orientation.x = Mxyz[0];
  imuMsgs.orientation.y = Mxyz[1];
  imuMsgs.orientation.z = Mxyz[2];
  pubImu.publish(&imuMsgs);
  
}

/********** send calibration in imuCalib suscriber to calibrate the IMU *********************/
/* the parametre will be write in the node rosserial, you can change them in configIMU.h for the time*/ 

void imuCallBack(const std_msgs::String& cmd){
  if (cmd.data == "Calibrate"){
    Mxyz_init_calibrated();
  }
  else{
    nh.loginfo("To calibrate imu, send 'calibrate' to the topic imuCalibrate");
  }
}



/********************************************************************************************/
/* Fonctions pour le fonctionnement de l'imu */


void getHeading(void)
{
  heading=180*atan2(Mxyz[1],Mxyz[0])/PI;
  if(heading <0) heading +=360;
}

void getTiltHeading(void)
{
  float pitch = asin(-Axyz[0]);
  float roll = asin(Axyz[1]/cos(pitch));
  float xh = Mxyz[0] * cos(pitch) + Mxyz[2] * sin(pitch);
  float yh = Mxyz[0] * sin(roll) * sin(pitch) + Mxyz[1] * cos(roll) - Mxyz[2] * sin(roll) * cos(pitch);
  float zh = -Mxyz[0] * cos(roll) * sin(pitch) + Mxyz[1] * sin(roll) + Mxyz[2] * cos(roll) * cos(pitch);
  tiltheading = 180 * atan2(yh, xh)/PI;
  if(yh<0)    tiltheading +=360;
}



void Mxyz_init_calibrated ()
{
  
  nh.loginfo("Before using 9DOF,we need to calibrate the compass frist,It will takes about 2 minutes.");
  nh.loginfo("  ");
  nh.loginfo("During  calibratting ,you should rotate and turn the 9DOF all the time within 2 minutes.");
  nh.loginfo("  ");
  nh.loginfo("Wait 5s");
  nh.loginfo("  ");
  nh.loginfo("ready");
  nh.loginfo("Sample starting......");
  nh.loginfo("waiting ......");
  
  get_calibration_Data ();
  
  nh.loginfo("     ");
  nh.loginfo("compass calibration parameter ");
  nh.loginfo(" %f ",mx_centre);
  nh.loginfo(my_centre);
  nh.loginfo(mz_centre);
}


void get_calibration_Data ()
{
    for (int i=0; i<sample_num_mdate;i++)
      {
      get_one_sample_date_mxyz();
      /*
      Serial.print(mx_sample[2]);
      Serial.print(" ");
      Serial.print(my_sample[2]);                            //you can see the sample data here .
      Serial.print(" ");
      Serial.println(mz_sample[2]);
      */
      if (mx_sample[2]>=mx_sample[1])mx_sample[1] = mx_sample[2];     
      if (my_sample[2]>=my_sample[1])my_sample[1] = my_sample[2]; //find max value      
      if (mz_sample[2]>=mz_sample[1])mz_sample[1] = mz_sample[2];   
      
      if (mx_sample[2]<=mx_sample[0])mx_sample[0] = mx_sample[2];
      if (my_sample[2]<=my_sample[0])my_sample[0] = my_sample[2];//find min value
      if (mz_sample[2]<=mz_sample[0])mz_sample[0] = mz_sample[2];
            
      }
      
      mx_max = mx_sample[1];
      my_max = my_sample[1];
      mz_max = mz_sample[1];      
          
      mx_min = mx_sample[0];
      my_min = my_sample[0];
      mz_min = mz_sample[0];
 
      mx_centre = (mx_max + mx_min)/2;
      my_centre = (my_max + my_min)/2;
      mz_centre = (mz_max + mz_min)/2;  
  
}


void get_one_sample_date_mxyz()
{   
    getCompass_Data();
    mx_sample[2] = Mxyz[0];
    my_sample[2] = Mxyz[1];
    mz_sample[2] = Mxyz[2];
} 


void getAccel_Data(void)
{
  accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
  Axyz[0] = (double) ax / 16384;
  Axyz[1] = (double) ay / 16384;
  Axyz[2] = (double) az / 16384; 
}

void getGyro_Data(void)
{
  accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
  Gxyz[0] = (double) gx * 250 / 32768;
  Gxyz[1] = (double) gy * 250 / 32768;
  Gxyz[2] = (double) gz * 250 / 32768;
}

void getCompass_Data(void)
{
  I2C_M.writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01); //enable the magnetometer
  delay(10);
  I2C_M.readBytes(MPU9150_RA_MAG_ADDRESS, MPU9150_RA_MAG_XOUT_L, 6, buffer_m);
  
    mx = ((int16_t)(buffer_m[1]) << 8) | buffer_m[0] ;
  my = ((int16_t)(buffer_m[3]) << 8) | buffer_m[2] ;
  mz = ((int16_t)(buffer_m[5]) << 8) | buffer_m[4] ;  
  
  Mxyz[0] = (double) mx * 1200 / 4096;
  Mxyz[1] = (double) my * 1200 / 4096;
  Mxyz[2] = (double) mz * 1200 / 4096;
}

void getCompassDate_calibrated ()
{
  getCompass_Data();
  Mxyz[0] = Mxyz[0] - mx_centre;
  Mxyz[1] = Mxyz[1] - my_centre;
  Mxyz[2] = Mxyz[2] - mz_centre;  
}
