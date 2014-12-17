#include <Wire.h>
#include <Multiservo.h>


#define OPEN 112
#define OPEN2 109
#define CLOSE 91
#define CLOSE2 93
#define CODE_LEN 5


bool code[CODE_LEN] = { 1, 0, 0, 1, 1 };
bool input[CODE_LEN] = { 0, 0, 0, 0, 0 };

Multiservo srv;


void setup()
{
    Serial.begin(9600);
    srv.attach(2);
}

void open(void)
{
  srv.write(OPEN);
  delay(300);
  srv.write(OPEN2);
}

void close(void)
{
  srv.write(CLOSE);
  delay(300);
  srv.write(CLOSE2);
}

void push(bool b)
{
    int i;
    
    Serial.println(b);

    for(i = 0; i < CODE_LEN-1; i++)
        input[i] = input[i+1];

    input[CODE_LEN-1] = b;

    for(i = 0; i < CODE_LEN; i++)
        if(code[i] != input[i]) return;

    for(i = 0; i < CODE_LEN; i++)
        input[i] = 0;

    open();
    delay(1000);
    
    Serial.println("Started");
}

void loop()
{
    static bool shock = false;
    static unsigned long last_time = millis();

    if(analogRead(A0) > 900)
    {
        unsigned long m = millis();

        if(millis()-last_time < 700 && shock)
        {
            shock = false;

            push(0);
        }
        else shock = true;

        last_time = m;

        delay(50);
    }

    if(millis()-last_time > 700 && shock)
    {
        last_time = millis();
        shock = false;

        push(1);
    }
    
    if(!digitalRead(3))
    {
      close();
      delay(1000);
    }
}

