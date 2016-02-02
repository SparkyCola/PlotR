/*
 * PlotterAxis.cpp - Library for plotter with stepper-motor-driven axis.
 * Created by Mattis Männel aka SparkyCola, October 8, 2015.
 */

//only for autocomplete & hightlighting in qt
#if QT_VERSION <= 0x040000

//#include "/home/mattis/.arduino-1.6.7/hardware/arduino/avr/cores/arduino/Arduino.h"
#endif

#include "plotteraxis3.h"

PlotterAxis3::PlotterAxis3(int NumberOfSteps, int DirectionPin,int StepPin, int SleepPin, int ForklightPin)
{
    numberOfSteps = NumberOfSteps;
    directionPin = DirectionPin;
    stepPin = StepPin;
    sleepPin = SleepPin;
    forklightPin = ForklightPin;

    position = 0;
    maximum = 0;
    minimum = 0;
    setSpeed(500);
    setMaximum(2000);
    disableMicrostepping = true;
    setMicrostepping(1);
    enableReversedDircetion(false);

    pinMode(DirectionPin, OUTPUT);
    pinMode(StepPin, OUTPUT);
    pinMode(SleepPin, OUTPUT);

    digitalWrite(SleepPin, LOW);   //sleep
    sleeping = true;

    pinMode(ForklightPin, INPUT_PULLUP);       //set forklightPin as input with pull-up resitors
    //digitalWrite(forklightPin, HIGH);   //activate pull-up resistors for forklightPin
}

long PlotterAxis3::getPosition()
{
    return position;
}

void PlotterAxis3::setPosition(long PositionToMove)
{
    Serial.print("...");
    if(PositionToMove >= 0)
        for(long i = 0; i < PositionToMove; i++){
            move(1);
            Serial.println(i+1);
        }
    else
        for(long i = 0; i < PositionToMove * -1; i++){
            move(-1);
            Serial.println(i+1);
        }
    Serial.println("{done}");
    //position = position_to_move;
}

int PlotterAxis3::move(long StepsToMove)
{  
    StepsToMove = StepsToMove * steppingFactor;
    if(position + StepsToMove > maximum || position + StepsToMove < minimum)
        return 1;
   /* Serial.print("\nstepsToMove: ");  Serial.println(StepsToMove);
    Serial.print("stepDelay: ");    Serial.println(stepDelayTime);
    Serial.print("speed: ");    Serial.println(speed);
    Serial.print("microStepping: ");    Serial.println(microsteppingFactor);
    Serial.print("numberOfSteps: ");    Serial.println(numberOfSteps);*/
    if(StepsToMove >= 0) //fwd
    {
        setDirection(0);
        for(long i = 0; i < StepsToMove; i++)
        {
            step();
            position++;
            delayMicroseconds(stepDelayTime);
        }
    }
    else    //rwd
    {
        StepsToMove = -StepsToMove;
        setDirection(1);
        for(long i = 0; i < StepsToMove; i++)
        {
            step();
            position--;
            delayMicroseconds(stepDelayTime);
        }
    }

    return 0;
}

void PlotterAxis3::reset()
{
    wakeUp();
    setDirection(1);
    while(!digitalRead(forklightPin))
    {
        step();
        delayMicroseconds(stepDelayTime);
    }
    position = 0;
    sleep();
}

void PlotterAxis3::setSpeed(long Speed)
{
    speed = Speed;
    stepDelayTime = 60L * 1000L * 1000L / (long)(numberOfSteps * speed * microsteppingFactor);
}

long PlotterAxis3::getSpeed()
{
    return speed;
}

long PlotterAxis3::getMaximum()
{
    return maximum;
}

void PlotterAxis3::setMinimum(long Minimum)
{
    minimum = Minimum;
}

void PlotterAxis3::setMaximum(long Maximum)
{
    maximum = Maximum;
}

void PlotterAxis3::setMicrostepping(int MicrosteppingFactor)
{
    microsteppingFactor = MicrosteppingFactor;
    stepDelayTime = 60L * 1000L * 1000L / (long)(numberOfSteps * speed * microsteppingFactor);
    if(disableMicrostepping)
        steppingFactor = microsteppingFactor;
    //Serial.print("µSteppingFactor: ");  Serial.println(microsteppingFactor);
    //Serial.print("steppingFactor: ");  Serial.println(steppingFactor);
}

String PlotterAxis3::getMicroStepping()
{
    String returnString = "microSteppingFactor: " + String(microsteppingFactor) + " SteppingFactor: " + String(steppingFactor);
    return returnString;
}

void PlotterAxis3::enableMicrostepping(bool EnableMicrostepping)
{
    disableMicrostepping = !EnableMicrostepping;
    if(disableMicrostepping)
        steppingFactor = microsteppingFactor;
    else
        steppingFactor = 1;
}

void PlotterAxis3::enableReversedDircetion(bool Reverse)
{
    reverseDirection = Reverse;
}

void PlotterAxis3::sleep()
{
    digitalWrite(sleepPin, LOW);    //sleep on
    delayMicroseconds(100);
    sleeping = true;
}

void PlotterAxis3::wakeUp()
{
    digitalWrite(sleepPin, HIGH);   //sleep off
    delayMicroseconds(100);
    sleeping = false;
}

bool PlotterAxis3::isSleeping()
{
    return sleeping;
}

int PlotterAxis3::version()
{
    return 1;
}

void PlotterAxis3::step()
{
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);
}

void PlotterAxis3::setDirection(bool Direction)
{
    if(!reverseDirection)
        digitalWrite(directionPin, Direction);
    else
        digitalWrite(directionPin, !Direction);
    delayMicroseconds(100);
}