#include <Servo.h>

#define KNOCK_PIN A0
#define BTN_PIN 3
#define BUZZER_PIN 9
#define LED_PIN 8

#define KNOCK_HIGH 300
#define KNOCK_TIMEOUT 1000

#define BTN_SHORT_MIN 100
#define BTN_SHORT_MAX 2000
#define BTN_LONG_MIN 3000 
#define BTN_LONG_MAX 10000

#define OPEN 107
#define OPEN2 103
#define CLOSE 85
#define CLOSE2 88
#define CODE_LEN 5


enum State
{
    OPENED,
    CLOSED,
    SETPWD,
    CNFPWD,
    INIT
};


bool code[CODE_LEN] = { 1, 0, 1, 0, 1 };
bool input[CODE_LEN] = { 0, 0, 0, 0, 0 };
unsigned int knockLevel = 0;
State state;
Servo srv;

void setup()
{
    Serial.begin(115200);

    pinMode(BTN_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);

    state = INIT;
    Serial.println("Started.");
}

bool btn(void)
{
    return digitalRead(BTN_PIN);
}

bool getknk(void)
{
    bool b1, b2;

    while(1)
    {
        b1 = analogRead(KNOCK_PIN) > knockLevel;
        delay(1);
        b2 = analogRead(KNOCK_PIN) > knockLevel;

        if(b1 == b2) return b1;
        delay(1);
    }

    return false;
}

char getch_knk(void)
{
    unsigned long tm = millis();

    while(!getknk() and millis()-tm < KNOCK_TIMEOUT);

    if(millis()-tm >= KNOCK_TIMEOUT) return -1;

    tm = millis();

    while(getknk());

    while(!getknk() and millis()-tm < KNOCK_HIGH);

    tm = millis()-tm;

    while(getknk());

    if(tm < KNOCK_HIGH) return 0;

    return 1;
}

char readCode(void)
{
    int i;
    char ch;

    for(i = 0; i < CODE_LEN; i++)
    {
        ch = getch_knk();

        if(ch < 0) return -1;

        input[i] = ch;
        Serial.print((int)ch);
    }

    Serial.println("");

    for(i = 0; i < CODE_LEN; i++)
    {
        if(input[i] != code[i]) return 0;
    }


    return 1;
}

void readPwd(void)
{
    int i;
    char ch;

    for(i = 0; i < CODE_LEN; i++)
    {
        while((ch = getch_knk()) < 0);
        input[i] = ch;

        Serial.print((int)ch);
    }
}

void ledOn(void) { digitalWrite(LED_PIN, HIGH); }
void ledOff(void) { digitalWrite(LED_PIN, LOW); }

void open(void)
{

    srv.attach(5);
    srv.write(OPEN);
    delay(500);
    srv.write(OPEN2);
    delay(100);
    srv.detach();

    state = OPENED;
    Serial.println("Opened.");
}

void close(void)
{

    srv.attach(5);
    srv.write(CLOSE);
    delay(500);
    srv.write(CLOSE2);
    delay(100);
    srv.detach();

    state = CLOSED;
    Serial.println("Closed.");
}


void loop()
{
    int i;
    char ch;
    unsigned long tm;

    knockLevel = analogRead(A1);

    switch(state)
    {
    case OPENED:
        while(btn());
        tm = millis();
        while(!btn());
        tm = millis()-tm;

        if(tm > BTN_SHORT_MIN and tm < BTN_SHORT_MAX) close();
        else if(tm > BTN_LONG_MIN and tm < BTN_LONG_MAX) state = SETPWD, ledOn();
        break;

    case CLOSED:
        ch = readCode();

        if(ch < 0)
        {
            Serial.println("FAIL");
            break;
        }

        if(ch == 1) open();
        else
        {
            tone(BUZZER_PIN, 500, 200);
            delay(200);
            tone(BUZZER_PIN, 500, 200);
            delay(200);
            tone(BUZZER_PIN, 500, 200);
            delay(200);
        }
        break;

    case SETPWD:
        Serial.print("Input code:");
        readPwd();

        for(i = 0; i < CODE_LEN; i++) code[i] = input[i];

        Serial.println("");
        state = CNFPWD;
        break;

    case CNFPWD:
        Serial.print("Confirm code:");
        readPwd();

        for(i = 0; i < CODE_LEN; i++) if(code[i] != input[i]) break;

        Serial.println("");
        if(i == CODE_LEN) ledOff(), state = OPENED;
        else state = SETPWD;

        break;

    case INIT: open(); break;
    }
}

