#define VibratingLed 4
#define OnLed 3
#define Alarm 2
int setval=0;
int valorbase;
bool validation1;
bool validation2;
int readOut1;int readOut2;int readOut3;int readOut4;
int sum1;int sum2;int sum3;
int X;
bool emptyTank = false;
bool EmptyBarrel = false;
unsigned long firstpump;
unsigned long lastpump;
byte calibrationtotal = 0;
byte CalibrationTolerance =3;
byte sensitivity=7;
byte highspike = 0;
byte lowspike = 0;
int average;
unsigned long currenttime;
unsigned long checktime;
bool checkaverage = false;
byte bip=0;
int gap;
long tolerance;
void setup() {
pinMode(2,OUTPUT);pinMode(3,OUTPUT);pinMode(4,OUTPUT);
  Serial.begin(9600);
}

void loop() {
 
 while(bip==0){ 

      Serial.println("Starting");                                
    for(byte DeviceTurnedOn=0;DeviceTurnedOn<3;DeviceTurnedOn++){                     /* This function will beep the allarm 3 times to let us know the device was switched on. */
     digitalWrite(Alarm,HIGH);delay(80);digitalWrite(Alarm,LOW);delay(50);}  
    

    
    delay(30000); 
  digitalWrite(OnLed,HIGH);                                                          /* 30 seconds after the device being turned on, it will start executing it's function.
                                                                                        A green led will turn on at that moment, and the alarm beeps 10 times. */
  
     for(byte StartOperation=0;StartOperation<10;StartOperation++){  
     digitalWrite(Alarm,HIGH);delay(20);digitalWrite(Alarm,LOW);delay(20);}
      bip++;
  }

  
 if(setval==0){                                                                     /* This will read and set a base voltage value from the adxl335.
                                                                                       The program will take multiple readings of the adxl335, and compare them to each other.
                                                                                       If they are all within a certain margin, a base level will be set.
                                                                                       If the device is vibrating too much, the program will repeat the process up to 50 times.
                                                                                       If after 50 tries it still can't get homogeneous readings, the alarm goes off.  */
  delay(1);Serial.println("Setting base value");              
 readOut1 = analogRead(A5); delay(1);                    
  Serial.print("readOut1 ");Serial.println( readOut1);
 readOut2 = analogRead(A5); delay(1);
  Serial.print("readOut2 ");Serial.println( readOut2);
   sum1= readOut1+ readOut2;
   
  if(readOut1-readOut2>-CalibrationTolerance&&readOut1-readOut2<CalibrationTolerance){ Serial.print("sum1 ");Serial.println(sum1);validation1=true;}
else{validation1=false;Serial.println("readOut1 and readOut2 too far apart! ");}
 
  
   readOut3 = analogRead(A5); delay(1);
  Serial.print("readOut3 ");Serial.println( readOut3);
    readOut4 = analogRead(A5); delay(1);
  Serial.print(" readOut4 ");Serial.println( readOut4);
 sum2= readOut3+ readOut4;

if( readOut3- readOut4>-CalibrationTolerance&& readOut3- readOut4<CalibrationTolerance){ Serial.print("sum2 ");Serial.println(sum2);validation2=true;}
else{validation2=false;Serial.println(" readOut3 and readOut4 too far apart!");}

 
  
  if(validation1&&validation2==true){
  sum3 = sum1+sum2;
  Serial.print("sum3 ");Serial.println(sum3);
  valorbase=sum3/4;
  Serial.print("Base value is set to ");Serial.println(valorbase);
  Serial.print("sensitivity ");Serial.println(sensitivity);average=valorbase;
  validation1=false;validation2=false;checkaverage=true;checktime=millis();calibrationtotal++;Serial.print("calibration attempts total ");Serial.println(calibrationtotal);
  setval=1;delay(2000);}
  else{valorbase=900;setval=0;calibrationtotal++;Serial.print("calibration attempts total ");Serial.println(calibrationtotal);}
  }

  while(calibrationtotal>50){Serial.println("Failed calibration!");digitalWrite(Alarm,HIGH);delay(800);digitalWrite(Alarm,LOW);delay(10000);}

  
if(setval==1){
  


if(checkaverage==true){                                                                                     /* The adxl335 feels acceleration. Which is indistinguishable from gravity.
                                                                                                               As a result, the adxl335 feels tilt. So we need to recalibrate our reference voltage every so often.
                                                                                                               I arbitrarily chose to do so every 30 seconds. */
  Serial.println("Comparing current Analog5 readouts to base value");
   readOut1 = analogRead(A5); delay(1);
  Serial.print(" readOut1 ");Serial.println( readOut1);
 readOut2 = analogRead(A5); delay(1);
  Serial.print(" readOut2 ");Serial.println( readOut2);
 sum1= readOut1+ readOut2;
 
 if( readOut1- readOut2>-CalibrationTolerance&& readOut1- readOut2<CalibrationTolerance){ Serial.print("sum1 ");Serial.println(sum1);validation1=true;}
else{validation1=false;Serial.println(" readOut1 and readOut2 too far apart!");checkaverage=1;}
delay(10);


   readOut3 = analogRead(A5); delay(1);
  Serial.print(" readOut3 ");Serial.println( readOut3);
   readOut4 = analogRead(A5); delay(1);
  Serial.print(" readOut4 ");Serial.println( readOut4);
  sum2= readOut3+ readOut4;
  
if( readOut3- readOut4>-CalibrationTolerance&& readOut3- readOut4<CalibrationTolerance){ Serial.print("sum2 ");Serial.println(sum2);validation2=true;}
else{validation2=false;Serial.println(" readOut3 and readOut4 too far apart!");checkaverage=true;}

 
  
  if(validation1&&validation2==true){ sum3 = sum1+sum2; average = sum3/4; Serial.print("Average readouts = ");Serial.println(average);
  Serial.print("Base value = ");Serial.println(valorbase);checkaverage=false;Serial.println("Average readouts ARE within tolerance!");checktime=millis();}
  else{validation1=false;validation2=false;}
   if(average-valorbase>3||average-valorbase<-3){emptyTank = false;Serial.println("Average readouts are NOT within tolerance!");setval=0;checkaverage=false;}
  }
 
   if(checktime+30000<currenttime){Serial.println("Recalibrating");checkaverage=true;calibrationtotal=0;}



  currenttime=millis();
  X = analogRead(A5);
 
  
if(X>sensitivity+valorbase){Serial.println("Higher than base value detected!");highspike++;Serial.println(highspike);}    /* Checks if the adxl335 is producing voltages high enough above, and below our base value. Meaning, it's vibrating. */
if(X<valorbase-sensitivity){Serial.println("Lower than base value detected!");lowspike++;Serial.println(lowspike);}



 if(lowspike-highspike>100){setval=0;Serial.println("New angle! Low is higher");lowspike=0;}                              /* Checks for a constant increment of the highspike or lowspike variable. Meaning, the device was tilted. */
  if(highspike-lowspike>100){setval=0;Serial.println("New angle! Higher is higher");highspike=0;}

 if(highspike&&lowspike>1){emptyTank = true; firstpump=millis();lastpump=millis();Serial.println("Pump started! Filling tank up!");     /*A vibration was detected. So, the pump should be working/vibrating. */ 
 digitalWrite(VibratingLed,HIGH);highspike=0;lowspike=0;delay(100);digitalWrite(VibratingLed,LOW);} 

 
 while(emptyTank==true){                                                                                                           /*   While the tank is empty: */ 
  X = analogRead(A5);
  currenttime=millis();



 if(X>sensitivity+valorbase){Serial.println("Higher than base value detected!");highspike++;Serial.println(highspike);}               /* Check if the pump is working/vibrating */
 if(X<valorbase-sensitivity){Serial.println("Lower than base value detected!");lowspike++;Serial.println(lowspike);}
 if(highspike&&lowspike>1){

  
   gap=currenttime-lastpump;  Serial.print("Pump stroke interval ");Serial.println(gap);
   
  lastpump=millis();Serial.println("Vibration detected!");digitalWrite(VibratingLed,HIGH);delay(300);digitalWrite(VibratingLed,LOW); /*If a vibration is detected, the red led lights up for 300 milliseconds. */   
  highspike=0;lowspike=0;}   
 
  if(lowspike-highspike>50){setval=0;Serial.println("New angle! Low is higher!");lowspike=0;emptyTank=false;}                                   /* Checks again if the device was tilted. */
  if(highspike-lowspike>50){setval=0;Serial.println("New angle! Higher is higher!");highspike=0;emptyTank=false;}



   

   currenttime=millis();
  
  if(currenttime-lastpump>5000){emptyTank = false;Serial.println("Tank is full");setval=0;calibrationtotal=0;break;                        /* If no vibration is detected for 5 seconds, it assumes the tank is full. */
}


                                                                                                                /* Checks if the frequency between vibrations is longer than 450 milliseconds. Slower vibrations means the pump is pumping
                                                                                                                thick material. So we allow it 180 seconds to fill the tank. But if the pump is going fast, it's possible it may just be pumping air.
                                                                                                                So it's only allowed to do so for 120 seconds. */
  if(gap>450){tolerance=180000;}                                                                                             
  else{tolerance=120000;}





                                                                                                                              /*If the pump vibrates for longer than it is allowed to, the alarm goes off in an intermitent manner
                                                                                                                              because the damn thing is too loud. */
  if(lastpump>firstpump+tolerance){EmptyBarrel=true;lastpump=firstpump;}                                                            
  while(EmptyBarrel==true){Serial.println("Change PIB barrel!");digitalWrite(Alarm,HIGH);delay(400);digitalWrite(Alarm,LOW);delay(2500);
  for(int beep=0;beep<12;beep++){digitalWrite(Alarm,HIGH);delay(25);digitalWrite(Alarm,LOW);delay(2500);}
  }
  }

}
}

