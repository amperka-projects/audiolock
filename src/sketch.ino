#define CODE_LEN 5

void setup()
{
    Serial.begin(9600);
    pinMode(10, OUTPUT);
}

bool code[CODE_LEN] = { 1, 0, 0, 1, 1 };
bool input[CODE_LEN] = { 0, 0, 0, 0, 0 };

void push(bool b)
{
    int i;

    for(i = 0; i < CODE_LEN-1; i++)
        input[i] = input[i+1];

    input[CODE_LEN-1] = b;

    for(i = 0; i < CODE_LEN; i++)
        if(code[i] != input[i]) return;

    for(i = 0; i < CODE_LEN; i++)
        input[i] = 0;

    tone(10, 500, 100);
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
}

