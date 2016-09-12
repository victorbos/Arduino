float temp;
int tempPin = 0;

int A=6;
int B=7;
int C=8;
int D=9;
int E=10;
int F=11;
int G=12;

int DP=13;

int first=2;
int second=3;
int third=4;
int fourth=5;

int tempDelay = 10000;
long lastTime;

int d = 3;
String tempString;
String digit1;
String digit2;
String digit3;
String digit4;

void setup() {
  setupDisplay();
  allOff();
  lastTime=millis() + tempDelay;
}

void loop() {
  if (millis() - lastTime  >= tempDelay) {
    lastTime = millis();
    temp = analogRead(tempPin) ;
    temp = temp *  0.48828125;

    tempString = String((int) (temp * 100)) ;
    digit1 = tempString.substring(0, 1);
    digit2 = tempString.substring(1, 2);
    digit3 = tempString.substring(3, 4);
    digit4 = tempString.substring(5);
  }

  digitalWrite(first, LOW);
  digitalWrite(second, HIGH);
  digitalWrite(third, HIGH);
  digitalWrite(fourth, HIGH);
  digitalWrite(DP, LOW);
  makeDigit(digit1);
  delay(d);

  digitalWrite(first, HIGH);
  digitalWrite(second, LOW);
  digitalWrite(third, HIGH);
  digitalWrite(fourth, HIGH);
  digitalWrite(DP, HIGH);
  makeDigit(digit2);
  delay(d);

  digitalWrite(first, HIGH);
  digitalWrite(second, HIGH);
  digitalWrite(third, LOW);
  digitalWrite(fourth, HIGH);
  digitalWrite(DP, LOW);
  makeDigit(digit3);
  delay(d);

  digitalWrite(first, HIGH);
  digitalWrite(second, HIGH);
  digitalWrite(third, HIGH);
  digitalWrite(fourth, LOW);
  digitalWrite(DP, LOW);
  makeDigit(digit3);
  delay(d);
}

void setupDisplay() {
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(DP, OUTPUT);

  pinMode(first, OUTPUT);
  pinMode(second, OUTPUT);
  pinMode(third, OUTPUT);
  pinMode(fourth, OUTPUT);
}

void allOff() {
  digitalWrite(first, HIGH);
  digitalWrite(second, HIGH);
  digitalWrite(third, HIGH);
  digitalWrite(fourth, HIGH);
}

void makeDigit(String digit) {
  if (digit == "0")
    zero();
  else if (digit == "1")
    one();
  else if (digit == "2")
    two();
  else if (digit == "3")
    three();
  else if (digit == "4")
    four();
  else if (digit == "5")
    five();
  else if (digit == "6")
    six();
  else if (digit == "7")
    seven();
  else if (digit == "8")
    eight();
  else if (digit == "9")
    nine();
}

void zero() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, LOW);
}

void one() {
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
}

void two() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
}

void three() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
}

void four() {
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void five() {
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void six() {
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void seven() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
}

void eight() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}


void nine() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

