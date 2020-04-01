#include <ArduinoSTL.h>
#include<math.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#define a1_size 15
#define fall_size 15

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2

Adafruit_MPU6050 mpu;
float fall[20],a1[20];

void setup(void) {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,INPUT);
//  while (!Serial)
 //   delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {
 // updateSerial();
  // put your main code here, to run repeatedly:
  if(digitalRead(8))
  {
    digitalWrite(4,HIGH);
    delay(3000);
    digitalWrite(4,LOW);
  }
  sensors_event_t a;
  int i,j,k;
  int counter;
  float maximum_no,tmp;
 
  float avg=0;
  float dev,dev1;
  for(i=0;i<a1_size;i++)
  {
    mpu.getAccelerometerRed(&a);
    avg=0;
    avg=avg+pow(a.acceleration.x,2);
    avg=avg+pow(a.acceleration.y,2);
    avg=avg+pow(a.acceleration.z,2);
    avg=sqrt(avg);
    if(avg<2.5)
    {
      maximum_no=0; 
      tmp=0;
      for(counter=0;counter<70;counter++)
      {
        k=0;
        Serial.println(counter);
        while(k!=fall_size)
        {
          fall[k]=avg;
          if(i<15)
          {
            dev1=fabs(9.8-avg);
            a1[i]=dev1;
          }
          else
          {
            i=0;
            state(a1);
            dev1=fabs(9.8-avg);
            a1[i]=dev1;
          }
       // delay(133);
          mpu.getAccelerometerRed(&a);
          avg=0;
          avg=avg+pow(a.acceleration.x,2);
          avg=avg+pow(a.acceleration.y,2);
          avg=avg+pow(a.acceleration.z,2);
          avg=sqrt(avg); 
          k++;
          i++;
       }
       tmp=findmax(fall);
       if(tmp>maximum_no)
       {
          maximum_no=tmp;
       }
       Serial.println("Cleared");
      }
      isfall(maximum_no);
      Serial.println("here");   
    }
    else{
      dev1=fabs(9.8-avg);
      a1[i]=dev1;
    
    }
  }
  state(a1);
}
void state(float a1[20])
{
  int i;
  float ans=0;
  for(i=0;i<a1_size;i++)
  {
    ans=ans+a1[i];
  }
  ans=ans/a1_size;
  Serial.println(ans);
  if(ans>=4.35)
  {
    Serial.println("Activity is:Running");
    digitalWrite(7,HIGH);
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
  }
  else if(ans>=1.50 && ans<4.35)
  {
    Serial.println("Activity is:Walking"); 
    digitalWrite(6,HIGH);
    digitalWrite(5,LOW);
    digitalWrite(7,LOW);
  }
  else{
    Serial.println("Activity is:Steady");
    digitalWrite(5,HIGH);
    digitalWrite(7,LOW);
    digitalWrite(6,LOW);
  }
}

float findmax(float fall[20])
{
  float maximum,temp;
  int i,j;
  for(i=0;i<fall_size;i++)
  {
    for(j=1;j<fall_size-i;j++)
    {
      if(fall[j]<fall[j-1])
      {
        temp=fall[j];
        fall[j]=fall[j-1];
        fall[j-1]=temp;
      }
    }
  }
  maximum=fall[fall_size-1];
  return maximum;
}

void isfall(float maximum_no)
{
  
  Serial.print(maximum_no);
  if(maximum_no>88)
  {
    Serial.println("FALL DETECTED!!!");
    digitalWrite(4,HIGH);
    while(digitalRead(8)==0);
    digitalWrite(4,LOW);
    call_person();
    msg_send();
  }
  else if(maximum_no<50 && maximum_no>30)
  {
    //Serial.println("Soft landing");
  }
  else
  {
    Serial.println("---NO FALL---");
  }
}

void call_person()
{
  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, i t will back to OK
  updateSerial();
  
  mySerial.println("ATD+ +919403192318;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  updateSerial();
  delay(20000); // wait for 20 seconds...
  mySerial.println("ATH"); //hang up
  updateSerial(); 
}


void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void msg_send()
{
  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+91xxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("Hi, FALL DETECTED!"); //text content
  updateSerial();
  mySerial.write(26);
}
