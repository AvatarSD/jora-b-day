#include <SPI.h>                                          // Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>                                         // Подключаем библиотеку для работы с nRF24L01+
#include <stdio.h>

RF24           radio(9, 10);                              // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
char            data[3];                                   // Создаём массив для приёма данных

int serialWrite(char c, FILE *) {
    Serial.write(c); // а это уже стандартная шняга ардуино, которая будет печатать в com порт
    return 0;
}

void setup()
{
  
    Serial.begin(115200);
    stdout = stderr = fdevopen(serialWrite, NULL);
    
    pinMode(5, INPUT);
  
    radio.begin();                                        // Инициируем работу nRF24L01+
    radio.setChannel(80);                                  // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_250KBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_MAX);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x1234567890LL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
    radio.openReadingPipe (1, 0x1234567891LL);
    data[2] = 0;
    Serial.println("by S.D.");
    radio.printDetails();
}

void loop()
{
    data[0] = '!';                             // считываем показания Trema слайдера с вывода A1 и записываем их в 0 элемент массива data
    data[1] = digitalRead(5);                           // считываем показания Trema потенциометра с вывода A2 и записываем их в 1 элемент массива data
    radio.write(data, sizeof(data));                     // отправляем данные из массива data указывая сколько байт массива мы хотим отправить. Отправить данные можно с проверкой их доставки: if( radio.write(&data, sizeof(data)) ){данные приняты приёмником;}else{данные не приняты приёмником;}
    radio.startListening();

    Serial.print(data[1]);
    Serial.println(" <- data, status: ");
    
    //radio.print_status(radio.get_status());
    //radio.printDetails();

    for(uint16_t i = 0; i< 10; i++)
    {
        delay(1);
        if(radio.available())
        { 
            radio.read(data, sizeof(data));
            if(memcmp(data, "sd", 2))
                printf("Error reply: %02X:%02X:%02X\n", data[0], data[1], data[2]);
            else
                printf("Ok!\n");
            break;
        }
        else if(i == 9)
            printf("No reply =(\n");
    }
    radio.stopListening();
}
