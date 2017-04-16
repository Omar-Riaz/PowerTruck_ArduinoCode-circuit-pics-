#define INT 3
#define POL 4
#define CLR 5
#define LED 6


//I GOT SOME OF THE CODE FOR THE COLOUMB COUNTER FROM SPARKFUN. HERE IS THEIR LICENSE
/*  LICENSE:
  
  Our example code uses the "beerware" license. You can do anything
  you like with this code. No really, anything. If you find it useful
  and you meet one of us in person someday, consider buying us a beer.
*/


double battery_mAh = 0.0; // milliamp-hours (mAh)
double battery_percent = 0.0;  // state-of-charge (percent)

// Global variables:

double ah_quanta = 0.17067759; // mAh for each INT
double percent_quanta; // calculate below

double average = 0.0;
float sum = 0;
int lightDelay = 0;
double n = 1.0;
bool lightOff = true;
float likelihoodInc = 0.0;
int previous_percent_charged = -1;
int percent_charged = 0;






//the android application will send one of these numbers to indicate to the arduino what to do regarding the light
//2 light off
//3 light on
//4 no blink
//5 slow blink
//6 medium blink
//7 fast blink
                  //recieving a 4,5,6 and 7 if the light is to be kept off does nothing 
void Lights(int input, int delayInput){
  if(input == 2){
    digitalWrite(LED, LOW);
    lightOff = true;
    return;
  } else if(input == 3){
    //digitalWrite(LED, HIGH);
    lightOff = false;                        
  } else if(input == 4){
    lightDelay = 5000;
  } else if(input == 5){
    lightDelay = 550;
  } else if(input == 6){
    lightDelay = 250;
  } else if(input == 7){
    lightDelay = 80;
  } else {
    Blink(lightDelay);
  }
}






void Blink(int input){
  if(lightDelay == 5000 && !lightOff)  {digitalWrite(LED, HIGH);}     //in this condition, we basically want the light consistently on
  else if(!lightOff){                       //otherwise, have the light blink at the specified rate
    digitalWrite(LED, HIGH);
    delay(input);
    digitalWrite(LED, LOW);
    delay(input);
  }
}






void setup()
{
  // Set up I/O pins:
  pinMode(INT,INPUT);
  pinMode(POL,INPUT);
  pinMode(CLR,OUTPUT);
  digitalWrite(CLR,HIGH);
  pinMode(LED, OUTPUT);

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);  
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  // Enable serial output:
  Serial.begin(9600);

  // One INT is this many percent of battery capacity:
  percent_quanta = 1.0/(battery_mAh/1000.0*5859.0/100.0);
}









void loop()
{
  static long int currTime, lasttime;
  double mA;
  
  if(Serial.available()){                                    //if not the case, we know for sure that the app's inputStream isn't on. Therfefore, we might be revieving something from the outputStream...
    int input = (int) Serial.read();
    Serial.println(input);
    Lights(input, lightDelay);
  }
  else if (digitalRead(INT)==0) // INT has gone low. signal that data has been sent from coloumb counter
  {

    lasttime = currTime;
    currTime = micros();

    battery_mAh += ah_quanta;
    battery_percent += percent_quanta;

    // Reset INT pin
    digitalWrite(CLR,LOW);
    delayMicroseconds(40); // CLR needs to be low > 20us
    digitalWrite(CLR,HIGH);

    //calculate time elapsed since the arduino was turned on
    sum = sum + (currTime-lasttime)/1000000.000;

    //every five samples, update the battery percentage
    if(n >= 5){
      average = sum / 5.000;
      //OVER HERE, I USE A FUNCTION OF BATTERY PERCENTAGE WITH RESPECT TO THE TIME DIFFERENCE BETWEEN ONE "PERCENT_QUANTA" (initialized in setup method), which is returned by the coloumb counter.
      //THE FUNCTION WAS MADE BY CHARGING A BATTERY PACK FROM EMPTY TO FULL WHILE LOGGING THE VALUE OF TIME DIFFERENCE AT EACH PERCENTAGE INTO A TEXT FILE. THEN I USED EXCEL TO MAKE A POLYNOMAIL FUCNTION BASED OFF OF THIS COLLECTED DATA 
      percent_charged = -0.1839*pow(average, 6) + 5.9492*pow(average, 5) - 75.085*pow(average, 4) + 467.12*pow(average, 3) - 1491.1*pow(average, 2) + 2299.8*(average) - 1241 + likelihoodInc;
      String percent_charged_s = String(percent_charged);
      percent_charged_s.trim();
        Serial.print(0);
        Serial.println(percent_charged_s); 
        previous_percent_charged = percent_charged;
        likelihoodInc = 0.0;

      sum = 0.00000;
      n = 1.000000;
    }else{
      n++;
      if(previous_percent_charged >= percent_charged){
        likelihoodInc+=0.01;
      }
    }
  }else{
    Lights(0, lightDelay);          //Operate a light
  }
}


