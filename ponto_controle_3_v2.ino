#include <Wire.h>   //include the Wire library

float RateRoll, RatePitch, RateYaw;   //variáveis globais

float AccX, AccY, AccZ;
float AngleRoll, AnglePitch, Gx_p =0, Gy_p =0, Gz_p =0,Gx_a =0, Gy_a =0, Gz_a =0,ax =0,ay=0,az=0; 

float AcelXoffset, AcelYoffset, AcelZoffset, GanhoX, GanhoY, GanhoZ;
float Xp = 103.67, Xn = -95.91, Yp = 99.78, Yn = -101.18, Zp = 119.98, Zn = -82.97;
float current_timer = 0;
/*
103.67
-95.91
99.78
-101.18
119.98
-82.97
*/

const int medidas = 100;

float RateCalibrationRoll = 0.0, RateCalibrationPitch = 0.0, RateCalibrationYaw = 0.0, previous_timer = 0.0, delta_timer = 0.0;
int RateCalibration;
int calibra = 1; // variavel que habilita a rotina de coleta dos cados de calibração

float LoopTimer;

void gyro_signal(void){
  Wire.beginTransmission(0x68);   // Inicia a comunicação I2C
  Wire.write(0x1A);  //Liga o filtro passa-baixas
  Wire.write(0x05);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x68); // Configuração do Giroscópio
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x68); // Configuração do Acelerometro
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x68);   //Acessa os registradores que guardam as leituras do giroscópio
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);
  int16_t GyroX=Wire.read()<<8|Wire.read(); // Lê as medidas no eixo X
  int16_t GyroY=Wire.read()<<8|Wire.read(); // Lê as medidas no eixo Y
  int16_t GyroZ=Wire.read()<<8|Wire.read(); // Lê as medidas no eixo Z

  Wire.beginTransmission(0x68);   //Acessa os registradores que guardam as leituras do acelerômetro
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);
  int16_t AccXLSB=Wire.read()<<8|Wire.read(); // Lê as medidas no eixo X
  int16_t AccYLSB=Wire.read()<<8|Wire.read(); // Lê as medidas no eixo Y
  int16_t AccZLSB=Wire.read()<<8|Wire.read(); // Lê as medidas no eixo Z

  RateRoll=(float)GyroX/131;   //Converte as medidas em º/s
  RatePitch=(float)GyroY/131;   //Converte as medidas em º/s
  RateYaw=(float)GyroZ/131;   //Converte as medidas em º/s

  AccX=(float)AccXLSB/4096;
  AccY=(float)AccYLSB/4096;
  AccZ=(float)AccZLSB/4096;

  AngleRoll=atan(AccY/sqrt(AccY)/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
  AnglePitch=-atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
   // calibra_gyro();
}
//-------------------------------------------------------------------------

// Feito agora
void calibra_gyro()
{
  if (calibra == 1)
  {
      for(RateCalibration=0;RateCalibration<100;RateCalibration++){  //Realiza a calibração das medidas
          gyro_signal();
          RateCalibrationRoll+=RateRoll;
          RateCalibrationPitch+=RatePitch;
          RateCalibrationYaw+=RateYaw;
          delay(1);
        }
          RateCalibrationRoll/=100;
          RateCalibrationPitch/=100;
          RateCalibrationYaw/=100;
    calibra = 0;
  } 
    RateRoll-=RateCalibrationRoll;
    RatePitch-=RateCalibrationPitch;
    RateYaw-=RateCalibrationYaw;
}


void gyro_angle ()
  {
       //pega o tempo atual em segundos
      
      Gx_p=Gx_a;
      Gy_p=Gy_a;
      Gz_p=Gz_a;
      previous_timer = current_timer;
      current_timer = millis()/1000;
      delta_timer = current_timer - previous_timer; // variação de tempo
      gyro_signal();  //coleta uma nova amostra
      Gx_a = RateRoll;
      Gy_a = RatePitch;
      Gz_a = RateYaw;
      ax = ax + delta_timer*(Gx_a+Gx_p)/2;
      ay = ay + delta_timer*(Gy_a+Gy_p)/2;
      az = az + delta_timer*(Gz_a+Gz_p)/2;
       // atualiza o timer
      Serial.print("| x:");
      Serial.print(ax);
      Serial.print("| y:");
      Serial.print(ay);
      Serial.print("| z:");
      Serial.print(az);
      Serial.print("| \n");
      }

//-------------------------------------------------------------------------


void calibrar(){
  Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
  while(true){
    if(Serial.available()){
      String comando = Serial.readStringUntil('\n');
      if(comando == "Xp"){
        Xp=0;
        Serial.print("Xp=[");
        for(int i = 0; i < medidas; i++){
          gyro_signal();
          Serial.print(AccX);
          Serial.print(", ");
          Xp+=AccX;
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Xp COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
      else if(comando == "Xn"){
        Xn=0;
        Serial.print("Xn=[");
        for(int i = 0; i < medidas; i++){
          gyro_signal();
          Serial.print(AccX);
          Serial.print(", ");
          Xn+=AccX;
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Xn COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
      else if(comando == "Yp"){
        Yp=0;
        Serial.print("Yp=[");
        for(int i = 0; i < medidas; i++){
          gyro_signal();
          Serial.print(AccY);
          Serial.print(", ");
          Yp+=AccY;
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Yp COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
       }
       else if(comando == "Yn"){
        Yn=0;
        Serial.print("Yn=[");
        for(int i = 0; i < medidas; i++){
          gyro_signal();
          Serial.print(AccY);
          Serial.print(", ");
          Yn+=AccY;
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Yn COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
       }
       else if(comando == "Zp"){
        Zp=0;
        Serial.print("Zp=[");
        for(int i = 0; i < medidas; i++){
          gyro_signal();
          Serial.print(AccZ);
          Serial.print(", ");
          Zp+=AccZ;
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Zp COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
       }
       else if(comando == "Zn"){
        Zn=0;
        Serial.print("Zn=[");
        for(int i = 0; i < medidas; i++){
          gyro_signal();
          Serial.print(AccZ);
          Serial.print(", ");
          Zn+=AccZ;
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Zp COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
      else if (comando == "EXIT"){ 
        Serial.println("##### CALIBRACAO ENCERRADA #######");
        break;
      }
      else{
        Serial.println("##### COMANDO INVALIDO #####");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
    }
  }
  //***** CALCULO DOS OFSSET's E GANHOS ******//
  AcelXoffset = (Xp+Xn)/(2*medidas);
  AcelYoffset = (Yp+Yn)/(2*medidas);
  AcelZoffset = (Zp+Zn)/(2*medidas);
  GanhoX = Xp/medidas - AcelXoffset;
  GanhoY = Yp/medidas - AcelYoffset;
  GanhoZ = Zp/medidas - AcelZoffset;

  Serial.println(Xp);
  Serial.println(Xn);
  Serial.println(Yp);
  Serial.println(Yn);
  Serial.println(Zp);
  Serial.println(Zn);

  // Printa os parametros de calibração na serial
  Serial.println("Ganho X = "+(String)GanhoX +"\t Offset X = "+(String)AcelXoffset);
  Serial.println("Ganho Y = "+(String)GanhoY +"\t Offset Y = "+(String)AcelYoffset);
  Serial.println("Ganho Z = "+(String)GanhoZ +"\t Offset Z = "+(String)AcelZoffset);
}

void setup(){
    Serial.begin(9600);
    Wire.setClock(400000);
    Wire.begin();
    delay(250);
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();
    for(RateCalibration=0;RateCalibration<100;RateCalibration++){  //Realiza a calibração das medidas
      gyro_signal();
      RateCalibrationRoll+=RateRoll;
      RateCalibrationPitch+=RatePitch;
      RateCalibrationYaw+=RateYaw;
      delay(1);
    }
      RateCalibrationRoll/=100;
      RateCalibrationPitch/=100;
      RateCalibrationYaw/=100;
    Serial.println("=====================================");
    Serial.println("iniciando Calibração...");
    calibrar(); 
    Serial.println("Calibracao efetuada com sucesso!");
    //calibra_gyro();
    //current_timer = millis()/1000;

}

float getX(){
  gyro_signal();
  float X = AccX/GanhoX - AcelXoffset;
  return(X);
}

float getY(){
  float Y = AccY/GanhoY - AcelYoffset;
  return(Y);
}

float getZ(){
  float Z = AccZ/GanhoZ - AcelZoffset;
  return(Z);
}

float ang(float Y, float Z){
  float ZY = atan(Y/Z)*57.2958;
  if (Y < 0 ){ // Y negativo
    if (Z < 0){ // Y- Z-
      return (180 - ZY);
    }
    else if(Z == 0){ // Y- Z0
      return (90);
    }
    else { // Y- Z+
      return (-ZY); 
    }
  }
  else if(Y > 0){ // Y positivo
    if (Z < 0){ // Y+ Z-
      return (-ZY +180);
    }
    else if(Z == 0){ // Y+ Z0
      return (270);
    }
    else { // Y+ Z+
      return (360-ZY); 
    }
  }
  else{ // Y zerado
    return (180);
  }
}

void loop(){
    gyro_signal();
 
    Serial.print(ang(getY(), getZ()));
    Serial.print("◦ YZ \t| ");
    Serial.print(ang(getX(), getZ()));
    Serial.print("◦ XZ \t| ");
    Serial.print(ang(getX(), getY()));
    Serial.println("◦ XY \t| ");
    //gyro_angle();
}
