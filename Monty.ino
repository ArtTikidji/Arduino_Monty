/*=== Monty ===*/  

#define TRG  A2
#define ECHO A3
#define ENA A0
#define ENB A1
#define IN1 10
#define IN2 11
#define IN3 5
#define IN4 6
#define ERR 13
#define SRV 3


void setup() {
  pinMode (TRG, OUTPUT);    //Эхолокатор (передача) см. long distance
  pinMode (ECHO, INPUT);    //Эхолокатор (прием) см. long distance
  analogWrite(IN1, 0);      //IN1-IN4 - моторы вперед/назад (см.void mtrctrl)
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  pinMode(ENA, OUTPUT);   //ENA,ENB - правое и левое колесо 
  pinMode(ENB, OUTPUT); 
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  pinMode(ERR, OUTPUT);   //ERR - светодиод
  digitalWrite(ERR, LOW); 
  analogWrite (SRV, 192);   //SRV - сервопривод (192 - середина (зависит от 
  //сервопривода))
  randomSeed(A4);
}

// замер расстояния, параметр n, определяет длинну УЗ волны 
// используемую для замера расстояния. Данная модиффикация
// повышает вероятность обнаружения узких предметов, таких как ножки стульев.
long distance (int n) {    
  digitalWrite(TRG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRG, HIGH);
  delayMicroseconds(n*n);
  digitalWrite(TRG, LOW);
  long duration = pulseIn(ECHO, HIGH);
  return duration / 29 / 2;
}

// подпрограмма управляющая моторами
void mtrctrl(int lf, int lb, int rf, int rb, int enl, int enr){ 
  if ( ( lb>0 && lf>0) || ( rb>0 && rf>0 ) ){   // проверка на наличие сбоя
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
    return;
  }
  digitalWrite(ENA, (enr > 0) ? HIGH : LOW);
  digitalWrite(ENB, (enl > 0) ? HIGH : LOW);
  
  analogWrite(IN4, lf);
  analogWrite(IN3, lb);
  analogWrite(IN1, rf);
  analogWrite(IN2, rb);
}

// Робот отъезжяет назад и вращает УЗ дачиком расстояния, 
// определяя с какой стороны есть препятствия
// Если препятствия есть с обеих сторон, робот отезжяет еще назад 
// Возвращает 0 -- если нет припятствия слева, 1 -- если справа
boolean servo() {
  s:
  boolean r;
  boolean l;
  mtrctrl(0, 150, 0, 100, 1, 1);
  delay(300);
  mtrctrl(0, 0, 0, 0, 1, 1);
  analogWrite (SRV, 195);
  delay(500);
  analogWrite (SRV, 250);
  if ((distance(2) >30) && (distance(3) > 30) && (distance(4) > 30))  r=1;
  else r=0;
  delay(500);
  analogWrite (SRV, 140);
  if ((distance(2) > 30) && (distance(3) > 30) && (distance(4) > 30))   l=1;
  else l=0;
  delay(500);
  analogWrite(SRV, 192);
  delay(500);
  if (r && l) { return random(1); }
  else {
   if (!r && !l) {
      goto s;
   } 
   else { 
    if (l > r) return 0;
    else return 1;
   }
  }
} 

void loop() {
  delay(10);
  if ((distance(2) > 30) && (distance(3) > 30) && (distance(4) > 30)) {
    // если перед роботом нет препятствий -- ехать вперед
    mtrctrl(150, 0, 104, 0, 1, 1);
  } 
  else {
    // если перед роботом есть препятствие 
    delay(500);
    if (servo() ) {
      mtrctrl(150, 0, 0, 104, 1, 1);
      delay(227);
      mtrctrl(0, 0, 0, 0, 1, 1);
    } 
    else {
      mtrctrl(0, 150, 140, 0, 1, 1);
      delay(199);
      mtrctrl(0, 0, 0, 0, 1, 1);
    }
    delay(500);
    delay(1000);
  } 
} 
