#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySerial(A1, A0); // RX, TX
DFRobotDFPlayerMini player;

String alwaysDisplay = "00:00";
String musica[11] = {
  "'Tis a pity she was a whore - D. Bowie",
  "Ghost Town - Ye",
  "Operator - Jim Croce",
  "ALIVE - Ye",
  "Preacher Man - Ye",
  "The Man Who Sold the World - Midge Ure",
  "Oh Lord - Foxy Shazam",
  "Ridin' Solo - Jason Derulo",
  "Big in Japan - Alphaville",
  "Unbound (The Wild Ride) - Avenged 7fold",
  "Lose Yourself to Dance - Daft Punk"
};

LiquidCrystal lcd(2,3,7,6,5,4);

String input;
int x, y = 0, z = 0, d1 = 0, d2 = 0, d3 = 0, d4 = 0, estado = 0;
unsigned long tempoInicio = 0;
int musicLen = sizeof(musica) / sizeof(musica[0]);

// Função para exibir texto ocupando 2 linhas (20x4)
void printDuasLinhas(String texto) {
  lcd.clear();
  if (texto.length() <= 20) {
    // Se couber em uma linha só
    lcd.setCursor(0, 0);
    lcd.print(texto);
  } else if (texto.length() <= 40) {
    // Se couber em duas linhas
    String linha1 = texto.substring(0, 20);
    String linha2 = texto.substring(20);
    lcd.setCursor(0, 0);
    lcd.print(linha1);
    lcd.setCursor(0, 1);
    lcd.print(linha2);
  } else {
    // Se for muito longo, corta o excesso
    String linha1 = texto.substring(0, 20);
    String linha2 = texto.substring(20, 40);
    lcd.setCursor(0, 0);
    lcd.print(linha1);
    lcd.setCursor(0, 1);
    lcd.print(linha2);
  }
}

void setup() {
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(10, INPUT);
  lcd.begin(20,4);
  lcd.clear();
  lcd.print("-------JukeBox------");
  lcd.setCursor(0,2);
  lcd.print("  Insira uma moeda");

  mySerial.begin(9600);
  if (!player.begin(mySerial)) {
    lcd.clear();
    lcd.print("Erro DFPlayer!");
    while(true);
  }
  player.volume(10);
}

void loop() {
  switch(estado) {
    case 0:
      if (digitalRead(10) == 0) {
        lcd.clear();
        delay(200);
        input = "Selecione uma musica";
        lcd.setCursor(0,1);
        lcd.print(input);
        estado = 2;
        delay(200);
      }
    break;
   
    case 2:
      lcd.setCursor(0,1);
      lcd.print(input);
      delay(1000);
      y += 1;
      delay(700);
      lcd.clear();
      if (y == 1) {
        y = 0;
        z = 0;
        lcd.clear();
        estado = 3;
      }
    break;
   
    case 3:
      if (digitalRead(A4)) {
        lcd.clear();  
        estado = 10;
      }

      if (digitalRead(A3) || z == 0) {
        if (y < musicLen-1 && z == 1) y++;
        else y = 0;
        z = 1;
        lcd.clear();
        input = musica[y];
        printDuasLinhas(input);
        alwaysDisplay = "Track: " + String(y+1);
        lcd.setCursor(0,2); // exibe o "Track:" na 3ª linha
        lcd.print(alwaysDisplay);
        delay(200);
      }
    break;

    case 10: // Preparar pra tocar
      input = musica[y];
      printDuasLinhas(input);
      lcd.setCursor(0,2);
      lcd.print("Reproduzindo...");
      delay(100);
      player.playFolder(1, y + 1); // toca o arquivo correspondente (001.mp3 = música 1)
      tempoInicio = millis();
      estado = 12;
    break;

    case 12: // Tocando música
      // Atualiza o tempo decorrido no LCD
      unsigned long segundos = (millis() - tempoInicio) / 1000;
      int minutos = segundos / 60;
      int seg = segundos % 60;
      alwaysDisplay = String(minutos) + ":" + (seg < 10 ? "0" + String(seg) : String(seg));
      lcd.setCursor(0,3);
      lcd.print(alwaysDisplay + "   ");
      if (player.available()) {
        if (player.readType() == DFPlayerPlayFinished) {
          lcd.clear();
          lcd.print("Musica encerrada!");
          delay(1500);
          estado = 0;
          lcd.clear();
          lcd.print("-------JukeBox------");
          lcd.setCursor(0,1);
          lcd.print("  Insira uma moeda");
        }
      }

      if (digitalRead(A3)) {
        player.stop();
        lcd.clear();  
        lcd.setCursor(0,0);
        lcd.print("-------JukeBox------");  
        lcd.setCursor(0,2);
        lcd.print("  Insira uma moeda");
        estado = 0;
        delay(200);
      }
    break;
  }
  
}
