/*
 * Autores: Gervasio Gesse Junior
 *          Iris menezes Barbosa
 * Titulo: Projeto Integrador 6 bimestre Univesp Polo Jandira
 * Data: 23 de Novembro de 2019 
 * Resumo: Semáforo com intervalo de tempo variável com 
 * sinalização sonora para ficlitar a travessia deficientes 
 * visuais e pessoas com mobilidade reduzida.
 */

//Define a saida para os leds
const int verde = 3;
const int botao = 4;
int val;
unsigned long startTime = 0;

 
void setup() {
  // inicializa o arduino
  //define que o pino verde como saida
  pinMode(verde, OUTPUT); 
  // define que o pino botão como entrada com pullup interno ativo
  pinMode ( botao, INPUT_PULLUP);   

}

void loop() {

  startTime = millis();
  
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); 

  val = digitalRead(botao);
  if(val == 0 && (millis() - startTime) > 500) {
    digitalWrite(verde, !digitalRead(verde));
  }

}
