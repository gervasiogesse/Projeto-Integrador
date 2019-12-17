/*
 * Autores: Gervasio Gesse Junior
 *          Iris menezes Barbosa
 * Titulo: Projeto Integrador 6 bimestre Univesp Polo Jandira
 * Data: 23 de Novembro de 2019 
 * Resumo: Semáforo com intervalo de tempo variável com 
 * sinalização sonora para ficlitar a travessia deficientes 
 * visuais e pessoas com mobilidade reduzida.
 */
 #include <Arduino_FreeRTOS.h>
 #include <semphr.h>
 #include "string.h"
 #include <SoftwareSerial.h>

//Define a saida para os leds
const int vermelho = 4;
const int amarelo = 5;
const int verde = 6;
const int p_vermelho = 7;
const int p_verde = 8;
const int botao = 2;


//Define os pinos para a serial   
SoftwareSerial mySerial(10, 11); // RX, TX 

// Define a estrutura do semaforo
SemaphoreHandle_t xSerialSemaphore;

// Define buffer
char bufferPC[64] = {'\0'};
char bufferBT[64] = {'\0'};


// Define o tempo em segundos para os estados do semaforo
int t_vermelho = 1, t_amarelo = 1, t_verde = 1, t_adicional;

// Define as tasks
void TaskBlink( void *pvParameters );
void TaskSemaforoPrincipal( void *pvParameters );
void TaskComSerial( void *pvParameters );
void TaskBluetoothSerial( void *pvParameters );

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  mySerial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

    if ( xSerialSemaphore == NULL )  // Verifica se o semaforo existe e se não existir cria
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Cria um semaforo mutex
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Libera o semaforo
  }

    if ( xSerialSemaphore != NULL )
  {
    //Cria as tasks de comunicacao
    xTaskCreate(TaskComSerial, "ComSerial", 128, NULL, 2, NULL );
    xTaskCreate(TaskBluetoothSerial, "BluetoothSerial", 128, NULL, 2, NULL );
        
  }
  else
  {
    Serial.println("**** Erro ao criar semaforo!");
  }

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *)"Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskSemaforoPrincipal
    ,  (const portCHAR *) "SemaforoPrincipal"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );

}

void loop() {
  //Em branco. Tudo é feito pelas tasks do SO 
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters)  // Task blink para teste.
{
  (void) pvParameters;
  // inicializa digital LED_BUILTIN interno no pino 13 como output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(botao, INPUT_PULLUP);

  for (;;) // A Task shall never return or exit.
  {
    if(digitalRead(botao) == 0){
      t_vermelho = 10;
      digitalWrite(LED_BUILTIN, HIGH);
    } else{
    digitalWrite(LED_BUILTIN, HIGH);   // liga o led interno
    vTaskDelay( 200 / portTICK_PERIOD_MS ); // aguarda um segundo
    digitalWrite(LED_BUILTIN, LOW);    // desliga o led interno
    vTaskDelay( 4800 / portTICK_PERIOD_MS ); // aguarda um segundo
    }
  }
}

void TaskComSerial(void *pvParameters)
{
  (void) pvParameters;
  char caractere;
  int i=0;
  for (;;)
  {
    //Trocar portMAX_DELAY por ( TickType_t ) 5 para ticks e deposi desistir
    if ( xSemaphoreTake( xSerialSemaphore,  ( TickType_t ) 5 ) == pdTRUE )
    {
  
      // Enquanto receber algo pela serial
      while(Serial.available() > 0) {
        // Lê byte da serial PC
        caractere = Serial.read();
        Serial.print("Recebi: ");
        Serial.println(caractere);
        // Concatena valores
        bufferPC[i] = caractere;
        i++;
        bufferPC[i]= '\0';
        // Aguarda buffer serial ler próximo caractere
        vTaskDelay(1);
      }
      i=0;
      if(bufferBT[0] != '\0'){
        Serial.println(bufferBT);
        if(strncmp(bufferBT, "+DISC:SUCCESS", 13) == 0){
         Serial.println("DesConectado");
        }
        if(strncmp(bufferBT, "OK", 2) == 0){
         Serial.println("Conectado");
         t_vermelho = 10;
        }
      }
      bufferBT[0] = '\0'; 
      xSemaphoreGive( xSerialSemaphore );

    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskBluetoothSerial(void *pvParameters)
{
  (void) pvParameters;
  char caractere;
  int i=0;
  for (;;)
  {
    //Trocar portMAX_DELAY por ( TickType_t ) 5 para ticks e deposi desistir
    if ( xSemaphoreTake( xSerialSemaphore,  ( TickType_t ) 5 ) == pdTRUE )
    {
  
      // Enquanto receber algo pela serial
      while(mySerial.available() > 0) {
        // Lê byte da serial
        caractere = mySerial.read();
        Serial.print("Recebi BT: ");
        Serial.println(caractere);
        // Concatena valores
        bufferBT[i] = caractere;
        i++;
        bufferBT[i]= '\0';
        // Aguarda buffer serial ler próximo caractere
        vTaskDelay(1);
      }
      i=0;
      if(bufferPC[0] != '\0'){
        mySerial.println(bufferPC);
        Serial.println(bufferPC);
      }
      bufferPC[0] = '\0'; 
      xSemaphoreGive( xSerialSemaphore );

    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}



void TaskSemaforoPrincipal(void *pvParameters)  // Task blink para teste.
{
  (void) pvParameters;
  // inicializa os pinos do leds que representam os estados.
  pinMode(verde, OUTPUT);
  pinMode(amarelo, OUTPUT);
  pinMode(vermelho, OUTPUT);
  pinMode(p_vermelho, OUTPUT);
  pinMode(p_verde, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {

    digitalWrite(vermelho, LOW); 
    digitalWrite(verde, HIGH);   // liga o led verde para carro
    digitalWrite(p_vermelho, HIGH); //liga o led vermelho para pedestres
    digitalWrite(p_verde, LOW);
    vTaskDelay( t_verde * 1000 / portTICK_PERIOD_MS ); // Tempo do estado aberto carro
    digitalWrite(verde, LOW);
    digitalWrite(amarelo, HIGH);
    vTaskDelay( t_amarelo * 1000 / portTICK_PERIOD_MS ); // Tempo do estado amarelo carro
    digitalWrite(amarelo, LOW);  
    digitalWrite(p_vermelho, LOW);
    digitalWrite(p_verde, HIGH);
    digitalWrite(vermelho, HIGH);  
    vTaskDelay( t_vermelho * 1000 / portTICK_PERIOD_MS ); // Tempo do estado vermelho carro
    // Desliga flag
    t_vermelho = 1;
  }
}
