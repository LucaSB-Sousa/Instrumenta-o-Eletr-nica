#include <Arduino.h>
#include <TinyMPU6050.h>

const int medidas = 100; //quantidade de coletas para calibracao
const int qt = 36; //quantidade de coletas para calcular erro
const int lin = 15; //quantidade de linhas printadas na serial
const int line = 255-lin;

byte qtline = 0;
float AcelXoffset, AcelYoffset, AcelZoffset, GanhoX, GanhoY, GanhoZ;
float Xp, Xn, Yp, Yn, Zp, Zn;

MPU6050 mpu (Wire);

void setup() {

  // Initialization
  mpu.Initialize();

  // Calibration
  Serial.begin(9600);
  Serial.println("=====================================");
  Serial.println("iniciando Calibração...");
  calibrar(); 
  Serial.println("Calibracao efetuada com sucesso!");
}

void loop() {
  acelerometro(); // Funcao que printa valores constantemente
}


void acelerometro(){// Printa os dados lidos na serial
  mpu.Execute();
  
  if (qtline == 0){ // printa a qt de linhas definida no escopo
    Serial.println("\n|     Raw X     |     Raw Y     |     Raw Z     |       X       |       Y       |       Z       |     ang YZ    |     ang XZ    |     ang XY    |");
    qtline = line;    
  }

  if ((millis()%250)==0){
    // Serial.println("| %d | %d | %d | %f m/s² | %f m/s² | %f m/s² |",mpu.GetRawAccX(),mpu.GetRawAccY(), mpu.GetRawAccZ(), mpu.GetAccX(), mpu.GetAccY(), mpu.GetAccZ());
    Serial.print("| ");
    PrintAcelBefore();
    PrintAcelAfter();
    PrintAng();
    qtline ++;
  }
}

void calibrar(){// calibra os eixos do acelerometro
  Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
  while(true){
    if (Serial.available()){
      String comando = Serial.readStringUntil('\n');
      if (comando == "Xp"){  
        Xp = 0;  //inicializa o somatório
        Serial.print("Xp = [");
        for (int i = 0; i< medidas; i++){
          mpu.Execute(); //nova coleta dos valores em cada eixo.
          Serial.print(mpu.GetAccX());
          Serial.print(", ");
          Xp += mpu.GetAccX(); //valor do eixo X
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Xp COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      } 
      else if (comando == "Xn"){  
        Xn = 0;   //inicializa o somatório
        Serial.print("Xn = [");
        for (int i = 0; i< medidas; i++){
          mpu.Execute();  //nova coleta dos valores em cada eixo
          Serial.print(mpu.GetAccX());
          Serial.print(", ");
          Xn += mpu.GetAccX(); //valor do eixo X
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Xn COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
      else if (comando == "Yp"){  
        Yp = 0;  //inicializa o somatório
        Serial.print("Yp = [");
        for (int i = 0; i< medidas; i++){
          mpu.Execute();  //nova coleta dos valores em cada eixo
          Serial.print(mpu.GetAccY());
          Serial.print(", ");
          Yp += mpu.GetAccY(); //valor do eixo Y
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Yp COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
      else if (comando == "Yn"){  
        Yn = 0;  //inicializa o somatório
        Serial.print("Yn = [");
        for (int i = 0; i< medidas; i++){
          mpu.Execute();  //nova coleta dos valores em cada eixo
          Serial.print(mpu.GetAccY()); //valor do eixo Y
          Serial.print(", ");
          Yn += mpu.GetAccY();
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Yn COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
      else if (comando == "Zp"){  
        Zp = 0;  //inicializa o somatório
        Serial.print("Zp = [");
        for (int i = 0; i< medidas; i++){
          mpu.Execute();  //nova coleta dos valores em cada eixo.
          Serial.print(mpu.GetAccZ());
          Serial.print(", ");
          Zp += mpu.GetAccZ(); //valor do eixo Z
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Zp COLETADOS #######");
        Serial.println("Digite o eixo que será calibrado. Ex: Xp, Xn, Yp, Yn, Zp, Zn ou EXIT");
      }
      else if (comando == "Zn"){  
        Zn = 0; //inicializa o somatório
        Serial.print("Zn = [");
        for (int i = 0; i< medidas; i++){
          mpu.Execute();  //nova coleta dos valores em cada eixo.
          Serial.print(mpu.GetAccZ());
          Serial.print(", ");
          Zn += mpu.GetAccZ();  //valor do eixo Z
          delay(2);
        }
        Serial.println("]");
        Serial.println("##### DADOS Zn COLETADOS #######");
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


// Eixos calibrados
float getX(){
  float X = mpu.GetAccX()/GanhoX - AcelXoffset;
  return(X);
}

float getY(){
  float Y = mpu.GetAccY()/GanhoY - AcelYoffset;
  return(Y);
}

float getZ(){
  float Z = mpu.GetAccZ()/GanhoZ - AcelZoffset;
  return(Z);
}


// Ângulos em graus
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

//funcoes de print
void PrintAcelBefore(){// printa eixos nao calibrados
  Serial.print(mpu.GetAccX()); 
  Serial.print(" g \t| ");
  Serial.print(mpu.GetAccY());
  Serial.print(" g \t| ");  
  Serial.print(mpu.GetAccZ());
  Serial.print(" g \t| ");  
}

void PrintAcelAfter(){ //printa eixos calibrados
  Serial.print(getX());
  Serial.print(" g \t| ");  
  Serial.print(getY());
  Serial.print(" g \t| ");
  Serial.print(getZ());
  Serial.print(" g \t| ");
}

void PrintAng(){ //printa angulos
  Serial.print(ang(getY(), getZ()));
  Serial.print("◦ YZ \t| ");
  Serial.print(ang(getX(), getZ()));
  Serial.print("◦ XZ \t| ");
  Serial.print(ang(getX(), getY()));
  Serial.println("◦ XY \t| ");
}
