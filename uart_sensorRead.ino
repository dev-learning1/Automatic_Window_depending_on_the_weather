int Button_O=4;
int Button_C=5;
int Rain=A0;
int CDS=A1;
int IR=A2;

String Analog_val;

void setup(){
  pinMode(Rain,INPUT);
  pinMode(CDS,INPUT);
  pinMode(IR,INPUT);
  pinMode(Button_O,INPUT);
  pinMode(Button_C,INPUT);
  Serial.begin(115200);
}

void loop(){
  int Botton_O_val = digitalRead(Button_O);
  int Botton_C_val = digitalRead(Button_C);
  int Rain_val = analogRead(Rain);
  int CDS_val = analogRead(CDS);
  int IR_val = analogRead(IR);
  
  Analog_val += String(Botton_O_val);
  Analog_val += "O";
  Analog_val += String(Botton_C_val);
  Analog_val += "C"; 
  Analog_val += String(Rain_val);
  Analog_val += "R";
  Analog_val += String(CDS_val);
  Analog_val += "D";
  Analog_val += String(IR_val);
  Analog_val += "I";

  Serial.println(Analog_val);
  Analog_val="";
  delay(300);
}
