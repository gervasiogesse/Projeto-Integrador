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

//Define a saida para os leds
const int vermelho = 4;
const int amarelo = 5;
const int verde = 6;
const int p_vermelho = 7;
const int p_verde = 8;
const int botao = 2;


// Define as tasks
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
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
    TaskDigitalRead
    ,  (const portCHAR *) "BotaoRead"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  xTaskCreate(
    TaskSemaforoPrincipal
    ,  (const portCHAR *) "SemaforoPrincipal"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
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

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(LED_BUILTIN, HIGH);   // liga o led interno
    vTaskDelay( 200 / portTICK_PERIOD_MS ); // aguarda um segundo
    digitalWrite(LED_BUILTIN, LOW);    // desliga o led interno
    vTaskDelay( 4800 / portTICK_PERIOD_MS ); // aguarda um segundo
  }
}

void TaskDigitalRead(void *pvParameters)  //Task para ler o botao.
{
  (void) pvParameters;
  int val, oldVal=1;
  unsigned long startTime = 0;
  int brilho = 0;
  //define o pino verde como saida
  pinMode(verde, OUTPUT); 
  // define o pino botão como entrada e com pullup interno ativo
  pinMode ( botao, INPUT_PULLUP);

  for (;;)
  {
    startTime = millis();
    val = digitalRead(botao);
    //while(val == 0 && (millis() - startTime) > 500) && oldVal == 1 {
    while(val == 0){
      Serial.println(millis() - startTime);
      vTaskDelay(pdMS_TO_TICKS(1000));
      val = digitalRead(botao);
      }
    vTaskDelay(1);  // 1 tick delay (15ms)
  }
}



void TaskSemaforoPrincipal(void *pvParameters)  // Task blink para teste.
{
  (void) pvParameters;
  // Define o tempo em segundos para os estados do semaforo
  int t_vermelho = 10, t_amarelo = 5, t_verde = 10;
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
  }
}
