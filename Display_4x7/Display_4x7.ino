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

void setup() {
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
  pinMode(third,OUTPUT);
  pinMode(fourth,OUTPUT);

}

void loop() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(DP, HIGH);
  
  digitalWrite(first,LOW);
  digitalWrite(second,LOW);
  digitalWrite(third,LOW);
  digitalWrite(fourth,LOW);
  delay(1000);
}
