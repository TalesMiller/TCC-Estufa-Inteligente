#include <DHT.h>
#include <DHT_U.h>

#define pinUmidadeSolo A0 // pino que está conectado o Sensor de Umidade do solo
#define pinLuz A1         // pino que está conectado o sensor de luminosidade
#define DHTPIN A2         // pino que está conectado o DHT

#define DHTTYPE DHT11 // Define o Tipo do DHT para o 11
#define pinAgua 13    // pino onde está conectado o sistema de irrigação
#define pinLampada 12 // pino onde está conectado a lampada
#define pinGas 7      // pino que está conectado o sensor de Gáses

DHT dht(DHTPIN, DHTTYPE);

// Definição das variaveis
float tempAmb, umidadeAmb, gasAmb;
int umidadeSolo, luminosidade;
int tempoLuz = 0, luzLigada = 0;
int portaAbrindo = 0, portaFechando = 0;
int GasAlto = 0, TempAlta = 0;

void setup()
{
    pinMode(pinUmidadeSolo, INPUT); // Define que o "pinUmidadeSolo" sera uma entrada de dados.
    pinMode(pinLuz, INPUT);         // Define que o "pinLuz" sera uma entrada de dados.
    pinMode(pinGas, INPUT);         // Define que o "pinGas" sera uma entrada de dados.

    pinMode(pinAgua, OUTPUT);    // Define que o "pinAgua" sera uma saída de dados.
    pinMode(pinLampada, OUTPUT); // Define que o "pinLampada" sera uma saída de dados.
    Serial.begin(9600);
    Serial.println("DHTxx test!");
    dht.begin();
}
void testDHT()
{
    umidadeAmb = dht.readHumidity();
    tempAmb = dht.readTemperature();
    gasAmb = digitalRead(pinGas);

    if (isnan(umidadeAmb) || isnan(tempAmb)) // Verifica se o DHT esta funcionando.
    {
        Serial.println("Falha na leitura do DHT");
    }
    if (gasAmb == HIGH && GasAlto == 0)
    {
        digitalWrite(pinLampada, HIGH); // Liga a lampada.
        GasAlto++;                      // Inicia o contador de ciclos do motor
    }
    else if (GasAlto != 0)
    {
        if (GasAlto % 2 == 0)
        {
            digitalWrite(pinLampada, LOW); // Desliga a lampada.
        }
        else
        {
            digitalWrite(pinLampada, HIGH);
        }
        GasAlto++;
        if (GasAlto > 120)
        {
            GasAlto = 0;
        }
    }
    if (tempAmb >= 26 && TempAlta == 0) // Verifica se está muito quente ou com Gases tóxicos.
    {
        digitalWrite(pinLampada, HIGH); // Liga a lampada.
        TempAlta++;                     // Inicia o contador de ciclos para teperatura
    }
    else if (TempAlta != 0)
    {

        if (TempAlta % 8 == 0)
        {
            digitalWrite(pinLampada, HIGH); // Desliga a lampada.
        }
        else
        {
            if (TempAlta % 4 == 0)
            {
                digitalWrite(pinLampada, LOW); // Desliga a lampada
            }
        }
        TempAlta++;
        if (TempAlta > 120)
        {
            TempAlta = 0;
            digitalWrite(pinLampada, LOW); // Desliga a lampada.
        }
    }
}

void LDR()
{
    luminosidade = analogRead(pinLuz);

    if (luminosidade >= 300 && luzLigada == -1) // Espera para se inicializar o amanhecar para começar as leituras novamente
    {
        luzLigada = 0;
        tempoLuz = 0; // Reseta o tempo que teve luz natural.
    }
    else if (luminosidade >= 125 && luzLigada == 0) // Observar qntidade de luminosidade.
    {
        tempoLuz++; // Contador de tempo de luz recebida.
    }
    else
    {
        if (tempoLuz < 2400) // Verifica se a região teve 10hrs diarias de luz
        {
            digitalWrite(pinLampada, HIGH); // Liga a lampada.
        }
        luzLigada++;          // Contador de qnto tempo a luz se manteve ligada
        if (luzLigada > 120) // 30 min que a lampada se mantem ligada intermitentemente
        {
            digitalWrite(pinLampada, LOW); // Desliga a lampada
            luzLigada = -1;
        }
    }
}

void Solenoide()
{
    umidadeSolo = analogRead(pinUmidadeSolo);

    if (umidadeSolo >= 650) // Verifica se o solo esta muito seco. *Alterar os valores de controle
    {
        digitalWrite(pinAgua, HIGH); // Liga a irrigação.
    }
    if (umidadeSolo <= 450) // Verifica se o Solo esta muito umido.
    {
        digitalWrite(pinAgua, LOW); // Desliga a irrigação.
    }
}

void printador()
{
    Serial.print("Temperatura Ambiente: ");
    Serial.println(tempAmb);
    Serial.print("Umidade Ambiente: ");
    Serial.println(umidadeAmb);
    Serial.print("Gás Ambiente: ");
    Serial.println(gasAmb);
    Serial.print("Luz Ambiente: ");
    Serial.println(luminosidade);
    Serial.print("Umidade do Solo: ");
    Serial.println(umidadeSolo);
}

void loop()
{
    testDHT();
    LDR();
    Solenoide();
    printador();

    delay(15000);
}
