#include <SPI.h>                                          // Подключаем библиотеку  для работы с шиной SPI
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>                                         // Подключаем библиотеку  для работы с nRF24L01+
#include <stdio.h>

RF24           radio(9, 10);                              // Создаём объект radio   для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
char            data[3];                                   // Создаём массив для приёма данных

int serialWrite(char c, FILE *f) {
Serial.write(c); // а это уже стандартная шняга ардуино, которая будет печатать в com порт
return 0;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("by S.D.");
    stdout = stderr = fdevopen(serialWrite, NULL);
    
    delay(1000);
    pinMode(5, OUTPUT);
    
    radio.begin();                                        // Инициируем работу nRF24L01+
    radio.setChannel(80);                                  // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_250KBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_MAX);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    
    radio.openReadingPipe (1, 0x1234567890LL);            // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
    radio.openWritingPipe (0x1234567891LL);
    radio.printDetails();

    radio.startListening();
}
void loop()
{
    if(radio.available())
    {                                // Если в буфере имеются принятые данные
        radio.read(data, sizeof(data));                  // Читаем данные в массив data и указываем сколько байт читать
        Serial.print("data: ");
        Serial.print(data[0]);
        Serial.print(", ");
        Serial.println(data[1]);
        digitalWrite(5, data[1]);

        radio.stopListening();
        delay(1);
        sprintf(data, "sd");
        radio.write(data, sizeof(data));
        radio.startListening();
        
    } /*else
        Serial.println("No data");*/
    //delay(100);
    
//    radio.printDetails();
//    radio.print_status(radio.get_status());
}
