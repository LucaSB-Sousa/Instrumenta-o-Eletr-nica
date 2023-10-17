#include<Wire.h>
 
//Endereco I2C do MPU6050
const int MPU=0x68;  

//Variaveis para armazenar valores dos sensores
int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
   
  //Inicializa o acelerômetro
  Wire.write(0); 
  Wire.endTransmission(true);
    
}

void loop()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  
  //Solicita os dados do sensor
  Wire.requestFrom(MPU,14,true);  
  
  //Armazena o valor dos sensores e converte
  AcX=Wire.read()<<8|Wire.read();      
  AcY=Wire.read()<<8|Wire.read(); 
  AcZ=Wire.read()<<8|Wire.read(); 
  Tmp=Wire.read()<<8|Wire.read(); 
  GyX=Wire.read()<<8|Wire.read(); 
  GyY=Wire.read()<<8|Wire.read(); 
  GyZ=Wire.read()<<8|Wire.read(); 

   
  //Mostra os valores na serial
  Serial.print("AcX = "); 
  Serial.print(AcX);
  Serial.print(" | Y = "); 
  Serial.print(AcY);
  Serial.print(" | Z = "); 
  Serial.print(AcZ);
  Serial.print(" | Gir. X = "); 
  Serial.print(GyX);
  Serial.print(" | Y = "); 
  Serial.print(GyY);
  Serial.print(" | Z = "); 
  Serial.print(GyZ);
  Serial.print(" | Temp = "); 
  Serial.println(Tmp/340.00+36.53);
    
  //Aguarda 300 ms antes de reiniciar o processo
  delay(500);
}