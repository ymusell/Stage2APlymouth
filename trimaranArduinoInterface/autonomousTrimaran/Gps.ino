
void setupGps(){
   nh.advertise(pubGps);
   Serial2.begin(9600);
   Serial2.setTimeout(10);

}


void updateGps(){
  testGPS();
}

void publishGps(){
  
  gpsMsg.header.stamp.sec = millis()/1000;
  gpsMsg.status.header.stamp.sec = gps.time.value();
  gpsMsg.latitude = gps.location.lat();
  gpsMsg.longitude = gps.location.lng();
  gpsMsg.altitude = gps.altitude.meters();
  gpsMsg.track = gps.course.deg();
  gpsMsg.speed = gps.speed.mps();
  //gpsMsg.status.satellites_used = gps.satellites.values();
  gpsMsg.hdop = gps.hdop.value();
  gpsMsg.time = gps.date.value();
  pubGps.publish(&gpsMsg);
}


void testGPS(){
  //unsigned long t0 = millis();
  String buf;
  while (Serial2.available()>0){
    buf = Serial2.readStringUntil('\n');
    //Serial.println(buf);
    if (isTrame(buf)){
      if (isGPSGPRMC(buf) == 1 ){//|| isGPSGPGGA(buf) == 1){
        //Serial.println(buf);
        for(int i = 0; i<buf.length(); i++){
          gps.encode(buf[i]);
        }
        publishGps();
        
        
      }
    }
    
    
  }
  
  //Serial.print("temps : ");
  //Serial.println(millis()-t0);
}


int isTrame(String trameGPS){
  int count = 1;
  int res = 0;
  char currentchar = trameGPS[count];
  //Serial.println(trameGPS);
  int len = trameGPS.length();
  while (currentchar != '*' && count<len)
  {
    if (currentchar != ','){
      res = res ^ currentchar;
    }
    count++;
    currentchar = trameGPS[count];
  }
  char check[2];
  check[0] = trameGPS[count+1];
  check[1] = trameGPS[count+2];
  int test  = strtol(check,NULL,16);
  if (test == res){
    return 1;
  }
  else
  {
    /*
    Serial.print(" error : ");
    Serial.print(test);
    Serial.print(" ");
    Serial.println(trameGPS);
    */
    return 0;
  }
}

int isGPSGPRMC( String trameGPS){
  if(trameGPS[0] == '$' && trameGPS[1] == 'G' && trameGPS[2] == 'P' && trameGPS[3] == 'R' && trameGPS[4] == 'M' && trameGPS[5] == 'C')
    {
    return 1;
    }
  else
    return 0;
}


int isGPSGPGGA( String trameGPS) {
  if(trameGPS[0] == '$' && trameGPS[1] == 'G' && trameGPS[2] == 'P' && trameGPS[3] == 'G' && trameGPS[4] == 'G' && trameGPS[5] == 'A')
    {
    return 1;
    }
  else
    return 0;
}
