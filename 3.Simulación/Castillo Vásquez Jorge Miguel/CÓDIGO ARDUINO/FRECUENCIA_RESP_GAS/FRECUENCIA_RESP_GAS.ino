#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 13  
//Adafruit_SSD1306 display(OLED_RESET);  //OLED 4 PINES
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
#define SSD1306_128_64

int sensor=A1;
int lecturaSensor=0;

byte data[128] ;  // Para guardar las lecturas
byte indexs = 0 ;
byte anterior;

///// FRECUENCIOMETRO ///
long rango = 5; // este es el rongo por el que se disparará la salida 2 y pasa a estado lógico 1
long ultimamedicion; // contiene el valor de la última medición que disparó a lógico 1, la salida 2
int ciclo = 0; // 1=alto 0=bajo
int cambiodeciclo = 0;
int picodetension;
int valledetension = 1023;
long contadorciclo;
///////********************/////////
boolean estadoBPM = true;  //guarda el estado del led (encendido o apagado)
boolean estadoLed = true;  //guarda el estado del led (encendido o apagado)
int intervaloEncendido = 1; // tiempo que esta encendido el led
int IntervaloApagado = 1;  // tiempo que esta apagado el led

int IntervaloBPM = 10000;  // tiempo que esta apagado el led
int IntervaloBPM2 = 1;  // tiempo que esta apagado el led

unsigned long tiempoAnteriorEncendido = 0;  //guarda tiempo de referencia para comparar
unsigned long tiempoAnteriorApagado = 0;  //guarda tiempo de referencia para comparar
unsigned long tiempoAnteriorBPM = 0;  //guarda tiempo de referencia para comparar
unsigned long tiempoAnteriorBPM2 = 0;  //guarda tiempo de referencia para comparar
///******************************************************+
int pulsos = 0;
int pulsos2 = 0;
int senal = 0;
//******************************************************************************************//
int buzzer=7;

void setup()   
{                
  Serial.begin(9600);
  //display.begin(SSD1306_SWITCHCAPVCC);
   display.begin(SSD1306_SWITCHCAPVCC,0X3C);  // //Inicializa pantalla con en la dirección 0x3D para la conexión I2C. 0x02 
   display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
 pinMode(buzzer, OUTPUT);
  pinMode(8, OUTPUT);
 display.clearDisplay();
 for (int i =0 ; i<128 ; i++)    // Ponemso a 0 el array
      data[i] = 0 ;
}

void loop()
{
  display.clearDisplay();
  indexs = indexs % 128;
  lecturaSensor=analogRead(sensor);
  lecturaSensor=lecturaSensor*7;
  Serial.println(lecturaSensor);
  data[indexs] = 64 * lecturaSensor/1024;
  indexs++; 
  byte i = indexs ;

for (int x = 0 ;  x <128; x++)
    {
   
       i = i % 128;
       // display.drawPixel( x, data[i++], 1);
       display.drawLine(x, data[i], x, anterior, 1);
      anterior=data[i];
       i++;
     if(x==0)  //quita que se repita la linea 0 y 128
        {
         display.clearDisplay();
        }
     }
     ////// BPM ////// 
  display.setCursor(0,5);
  display.print("F.RESP=");
 // display.setCursor(10,20);
 display.setCursor(90,5);
  display.print(pulsos2*6);
  display.display();
//************************* CALCULO FRECUENCIA RESPIRATORIA  **********************************************//
    senal=lecturaSensor; //guardamos el valor en la variable senal
  /////////***********************************************************//////////////////
   if (senal >= ( ultimamedicion+30) ) // La salida 2 pasa a 1 logico si la tensión medida en la entrada analógica 0 es mayor que la anterior lectura + latensión de RANGO
   {
 ultimamedicion = senal;  // SE ASIGANA A LA VARIABLE ULTIMAMEDICION EL VALOR LEIDO POR LA ENTRADA ANALÓGICA CERO 
     ciclo=1;
     if (senal>picodetension) // SI LA TENSIÓN MEDIDA POR LA ENTRADA CERO, ES LA MAYOR DETECTADA, SE ASIGNA A LA VARIABLE PICODETENSIÓN EL VALOR LEIDO POR AL ENTRADA CERO ANALÓGICA 
     {
     picodetension=senal; // SE ASIGNA EL VALOR LEIDO POR LA ENTRADA CERO ANALÓGICA A LA VARIABLE PICODETENSIÓN.
     }
   }
     
   if (senal <= ( ultimamedicion-30))  // La salida 2 pasa a 1 lógico si la tensión medida en la entrada analógica 0 es menor que la anterior lectura - la tensión de RANGO
   {
     ultimamedicion = senal; // SE ASIGNA A LA VARIABLE ULTIMAMEDICIÓN LA LECTURA MEDIDA POR EL PUERTO ANALÓGICO CERO
     ciclo=0; // EL CICLO SE PONE A CERO, ES DECIR, QUE EL VOLTAJE EMPIEZA A BAJAR DESDE EL PICO DE TENSIÓN MÁS ALTA
     if (senal<valledetension) // SE CUMPLE LA CONDICIÓN SI LA TENSIÓN DETECTADA POR EL PUERTO ANALÓGICO CERO ES MENOR QUE LA CONTENIDA EN LA VARIABLE VALLEDETENSIÓN
     {
     valledetension=senal; //Se asigna a la variable valledetensión el valor medido por la entrada analógica cero
     }
   }

  ////////***********************************************************///////////////////
  //////////////////// 1 minuto ////////////
   if(millis()-tiempoAnteriorBPM>=IntervaloBPM){  //si ha transcurrido el periodo programado
      lecturaSensor=analogRead(sensor);
    estadoBPM=false;
    pulsos2=pulsos;
     tiempoAnteriorBPM=millis();  //guarda el tiempo actual para comenzar a contar el tiempo apagad
    pulsos=0;   
  }
  if(millis()-tiempoAnteriorBPM2>=IntervaloBPM2){  //si ha transcurrido el periodo programado
      lecturaSensor=analogRead(sensor);
    estadoBPM=true;
 //   pulsos2=(pulsos2+pulsos2)/2;
    //Serial.print(pulsos2*4);
    tiempoAnteriorBPM2=millis();  //guarda el tiempo actual para comenzar a contar el tiempo apagado
  }
    ///////////////////////////////////////
  if((millis()-tiempoAnteriorEncendido>=intervaloEncendido)&&estadoLed==true && ciclo==0)
  {  //si ha transcurrido el periodo programado

    estadoLed=false;  //actualizo la variable para apagar el led
    picodetension=senal;//SE ASIGNA A LA VARIABLE PICODETENSIÓN EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO
    valledetension=senal;//SE ASIGNA A LA VALLEDETENSION EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO  
    digitalWrite(8,HIGH);  //apago el led
    tone(buzzer, 2500);
    tiempoAnteriorApagado=millis();  //guarda el tiempo actual para comenzar a contar el tiempo apagado
  }
  
  if((millis()-tiempoAnteriorApagado>=IntervaloApagado)&&estadoLed==false && ciclo==1) //PULSO
  {  //si ha transcurrido el periodo programado
  
    pulsos++;
    picodetension=senal;//SE ASIGNA A LA VARIABLE PICODETENSIÓN EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO
    valledetension=senal;//SE ASIGNA A LA VALLEDETENSION EL VALOR DE LA TENSIÒN LEIDA POR EL PUERTO ANALÓGICO CERO
    estadoLed=true;  //actualizo la variable para encender el led 
    
  noTone(buzzer); //apaga  buzzer
    digitalWrite(2,LOW);  //enciendo el led
    /////////////////////////////// 
    tiempoAnteriorEncendido=millis();  //guarda el tiempo actual para comenzar a contar el tiempo encendido
  }
    
delay(80);
}
