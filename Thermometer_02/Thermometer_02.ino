float temp;
const int tempPin = 0;

const int digitPins[4] = {2, 3, 4, 5};
const int segmentPins[7] = {6, 7, 8, 9, 10, 11, 12} ;
const int DP = 13;

const int tempDelay = 30000;
long lastTime;

const  int d = 3;

String tempString;
int digitValue[4];

const char* digits[10] = {
  "1111110",
  "0111000",
  "1101101",
  "1111001",
  "0110011",
  "1011011",
  "1011111",
  "1110000",
  "1111111",
  "1111011"
} ;

void setup() {
  setupDisplay();
  lastTime = millis() + tempDelay;
  //  Serial.begin(9600);
}

void loop() {
  if (millis() - lastTime  >= tempDelay) {
    lastTime = millis();
    temp = analogRead(tempPin) ;
    temp = temp *  0.48828125;

    tempString = String((int) (temp * 100)) ;
    digitValue[0] = tempString.substring(0, 1).toInt();
    digitValue[1] = tempString.substring(1, 2).toInt();
    digitValue[2] = tempString.substring(3, 4).toInt();
    digitValue[3] = tempString.substring(5).toInt();
  }

  update();
}

void setupDisplay() {
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
  }
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  pinMode(DP, OUTPUT);
}

void allOff() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], HIGH);
  }
}

void update() {
  for (int i = 0; i < 4; i++) {
    allOff();
    digitalWrite(digitPins[i], LOW);
    makeDigit(digitValue[i]);
    makeDP(i);
    delay(d);
  }
}

void makeDigit(int digit) {
  String digitString = digits[digit];
  for (int i = 0; i < 7; i++) {
    if (digitString.substring(i, i + 1) == "0") {
      digitalWrite(segmentPins[i], LOW);
    } else {
      digitalWrite(segmentPins[i], HIGH);
    }
  }
}

void makeDP(int pos) {
  if (pos == 1) {
    digitalWrite(DP, HIGH);
  } else {
    digitalWrite(DP, LOW);
  }
}

