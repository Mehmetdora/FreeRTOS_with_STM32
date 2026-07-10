/*
 * I2C_Driver.c
 *
 *  Created on: Jul 7, 2026
 *      Author: mehmet_dora
 */

#include "I2C_Driver.h"
#include "stm32f4xx.h"






void I2C1_init(void){

	// I2c kullanmak için GPIO pinleri AF olarak ayarlanmalıdır

	RCC->APB1ENR |= (1UL << 21);	// I2c1 enable clock
	RCC->AHB1ENR |= (1UL << 1);		// GPIOB clock enable



	GPIOB->MODER &= ~(3UL << (2*8));
	GPIOB->MODER |=  (2UL << (2*8)); // AF

	GPIOB->MODER &= ~(3UL << (2*9));
	GPIOB->MODER |=  (2UL << (2*9)); // AF

	GPIOB->AFR[1] &= ~(0xF << (4*(8-8)));
	GPIOB->AFR[1] |=  (4   << (4*(8-8)));

	GPIOB->AFR[1] &= ~(0xF << (4*(9-8)));
	GPIOB->AFR[1] |=  (4   << (4*(9-8)));


	GPIOB->OTYPER |= (1UL << 8);	// PB8-9 open drain
	GPIOB->OTYPER |= (1UL << 9);

	GPIOB->PUPDR &= ~(3UL << (2*8));
	GPIOB->PUPDR |=  (1UL << (2*8)); // pull-up

	GPIOB->PUPDR &= ~(3UL << (2*9));
	GPIOB->PUPDR |=  (1UL << (2*9));



	// I2C peripheral
	I2C1->CR1 |= (1UL << 15);	// reset state enable
	I2C1->CR1 &= ~(1UL << 15);	// reset state disable

	// 42 MHz için -> 101010
	I2C1->CR2 |= 42UL;

	// SCL in clock un high ve low sürelerinin belirlenmesi, duty-cycle ayarı
	// CCR = 42 MHz(APB1 freq.) / (2 * 100 kHz)
	I2C1->CCR = 210;



	/*
	 * I2C hattı opendrain ve pull-up olduğundan hemen low->high a geçemez,
	 * bu geçiş süresine TRise denir
	 * Bu süre ayarlanarak donanıma haber verilir.
	 */
	I2C1->TRISE = 43;


	I2C1->CR1 |= (1UL << 0);	// I2C1 Enable

}



void I2C1_ByteRead(char saddr, char maddr, uint8_t* data){
	/*
	saddr → Slave cihaz adresi (örneğin sensör: 0x68)
	maddr → Okunacak cihazın memory adresi (örneğin: 0x75)
	data → Okunan verinin yazılacağı pointer
	*/


	volatile int tmp;
	// bu değişken ile SR2 register okunarak ADDR flag temizlenmiş olunacak
	// volatile kullanılır çünkü compiler kodu optimize ederken bu değişkeni kaldırabilir




	while(I2C1->SR2 & (1UL << 1));
	// Eğer hat busy ise bekle, değilse veri okumaya başla
	// aynı anda 2 master çakışmasın diye







	// generate start
	I2C1->CR1 |= (1UL << 8);
	// Start condition oluşturulur, master biz ise bu start ı gönderebiliriz
	// Bus iletişimi başlar

	while(!(I2C1->SR1 & (1UL << 0)));
	// wait for enable start condition, wait for start bit flag







	// transmit slave adress + write
	I2C1->DR |= (saddr << 1);
	// Slave adresi gönderiliyor, write modunda olması gerektiği için de lsb 0 olmalı
	//LSB = 0 → WRITE mode
	// 0. bit write-read modunu belirliyor

	// wait for addr flag set
	while(!(I2C1->SR1 & (1UL << 1)));
	// adres gönderildi mi diye beklenir
	// flag oluşması ile slave cihaz adresi almış olur ve ACK göndermiş olur

	// flag clear
	tmp = I2C1->SR2;
	// ADDR flag temizlemek için SR2 değerinin okunması gerekir









	// send memory address
	I2C1->DR = maddr;
	// Slave cihaza hangi adresi okumak istediğini gönder

	// wait until trasmitter empty
	while(!(I2C1->SR1 & (1UL << 7)));	// TxE
	// Verinin gönderildiğini ve DR ın boşaldığını bekle








	// generate restart, this time for reading data
	I2C1->CR1 |= (1UL << 8);

	// wait for start generation
	while(!(I2C1->SR1 & (1UL << 0)));






	// transmit slave address + read
	I2C1->DR = (saddr << 1) | 1 ;
	// Verinin okunacağı slave cihaz adresi gönderilir
	// Veri okuması yapılacağı için lsb -> 1 yapılarak read modu seçilir

	// wait until addr flag is set
	while(!(I2C1->SR1 & (1UL << 1)));
	// Slave cihaz read adresini aldı ve ACK gönderdiğin bekle

	// disable ACK
	I2C1->CR1 &= ~(1UL << 10);
	// Tek bir byte okunacağından ACK gönderilmez
	// bu nedenle slave NACK alır ve iletişimi durdurur






	// clear addr flag
	tmp = I2C1->SR2;
	// ADDR flag temizlenir


	// Generate stop after data received
	I2C1->CR1 |= (1UL << 9);
	// veri alındıktan sonra hat serbest bırakılır

	// wait until RXNE flag is set
	while(!(I2C1->SR1 & (1UL << 6)));
	// DR a veri geldiğinde flag set olur


	// read data
	*data++ = I2C1->DR;


}



void I2C1_BurstRead(char saddr, char maddr, int n, uint8_t* data){

	/*
	 * Bu fonksiyon ile veriler okunurken bir kerelik bir adresten okunması yerine
	 * bir adresten başlanarak cihaz üzerindeki birden fazla adresten tek seferde
	 * verinin alınmasını sağlar, bir kerelik okuma.
	 *
	 * Yani aslında çoğu sensör veya cihaz auto-increment ile bir memory adresinden
	 * ACK ile veri okunduğunda kendisi bu adresi artttırır ki sonraki okumada
	 * başka bir register(memory) değeri okunsun. Bu fonksiyonda da maddr
	 * adresinden okunmaya başlanarak n kadar sonraki adrese kadar arka arkaya veriler
	 * alınır.
	 *
	 * Fakat her cihaz bu auto-increment özelliğini desteklemez , bu kontrol edilmelidir.
	 *
	 */

	volatile int tmp;

	while(I2C1->SR2 & (1UL << 1));
	// Eğer hat busy ise bekle, değilse veri okumaya başla
	// aynı anda 2 master çakışmasın diye







	// generate start
	I2C1->CR1 |= (1UL << 8);
	// Start condition oluşturulur, master biz ise bu start ı gönderebiliriz
	// Bus iletişimi başlar
	while(!(I2C1->SR1 & (1UL << 0)));
	// wait for enable start condition, wait for start bit flag





	// transmit slave adress + write
	I2C1->DR = (saddr << 1);
	// Slave adresi gönderiliyor, write modunda olması gerektiği için de lsb 0 olmalı
	//LSB = 0 → WRITE mode
	// 0. bit write-read modunu belirliyor

	// wait for addr flag set
	while(!(I2C1->SR1 & (1UL << 1)));
	// adres gönderildi mi diye beklenir
	// flag oluşması ile slave cihaz adresi almış olur ve ACK göndermiş olur

	// flag clear
	tmp = I2C1->SR2;
	// ADDR flag temizlemek için SR2 değerinin okunması gerekir





	// send memory address
	I2C1->DR = maddr;
	// Slave cihaza hangi adresi okumak istediğini gönder

	// wait until trasmitter empty
	while(!(I2C1->SR1 & (1UL << 7)));	// TxE
	// Verinin gönderildiğini ve DR ın boşaldığını bekle




	// generate restart, this time for reading data
	I2C1->CR1 |= (1UL << 8);

	// wait for start generation
	while(!(I2C1->SR1 & (1UL << 0)));




	// transmit slave address + read
	I2C1->DR = (saddr << 1) | 1 ;
	// Verinin okunacağı slave cihaz adresi gönderilir
	// Veri okuması yapılacağı için lsb -> 1 yapılarak read modu seçilir

	// wait until addr flag is set
	while(!(I2C1->SR1 & (1UL << 1)));
	// Slave cihaz read adresini aldı ve ACK gönderdiğin bekle

	// flag clear
	tmp = I2C1->SR2;

	// enable ACK
	I2C1->CR1 |= (1UL << 10);




	while(n > 0U){


		if(n == 1U){
			// disable ACK
			I2C1->CR1 &= ~(1UL << 10);
			// Tek bir byte okunacağından ACK gönderilmez
			// bu nedenle slave NACK alır ve iletişimi durdurur

			// Generate stop after data received
			I2C1->CR1 |= (1UL << 9);
			// veri alındıktan sonra hat serbest bırakılır

			// wait until RXNE flag is set
			while(!(I2C1->SR1 & (1UL << 6)));
			// DR a veri geldiğinde flag set olur


			// read data
			*data++ = I2C1->DR;

			break;



		}else{

			// wait until RXNE flag is set
			while(!(I2C1->SR1 & (1UL << 6)));
			// DR a veri geldiğinde flag set olur


			// read data
			*data++ = I2C1->DR;

			n--;

		}



	}
}



void I2C1_BurstWrite(char saddr, char maddr, int n, uint8_t* data){

	// Bir slave cihaza belirtilen adresten başlayarak n byte veri yazar.
	// Bir ekrana yada bir sensöre config verilerini göndermek için kullanılabilir.

	volatile uint32_t tmp;

	while(I2C1->SR2 & (1UL << 1));
	// Eğer hat busy ise bekle, değilse veri okumaya başla
	// aynı anda 2 master çakışmasın diye







	// generate start
	I2C1->CR1 |= (1UL << 8);
	// Start condition oluşturulur, master biz ise bu start ı gönderebiliriz
	// Bus iletişimi başlar
	while(!(I2C1->SR1 & (1UL << 0)));
	// wait for enable start condition, wait for start bit flag





	// transmit slave adress + write
	I2C1->DR = (saddr << 1);
	// Slave adresi gönderiliyor, write modunda olması gerektiği için de lsb 0 olmalı
	//LSB = 0 → WRITE mode
	// 0. bit write-read modunu belirliyor

	// wait for addr flag set
	while(!(I2C1->SR1 & (1UL << 1)));
	// adres gönderildi mi diye beklenir
	// flag oluşması ile slave cihaz adresi almış olur ve ACK göndermiş olur

	// flag clear
	tmp = I2C1->SR2;
	// ADDR flag temizlemek için SR2 değerinin okunması gerekir





	// wait until trasmitter empty
	while(!(I2C1->SR1 & (1UL << 7)));	// TxE
	// Verinin gönderildiğini ve DR ın boşaldığını bekle

	// send memory address
	I2C1->DR = maddr;
	// Slave cihaza hangi adresi okumak istediğini gönder




	for(int i = 0; i<n; i++){
		// wait until trasmitter empty
		while(!(I2C1->SR1 & (1UL << 7)));	// TxE
		// Verinin gönderildiğini ve DR ın boşaldığını bekle

		// read data
		I2C1->DR = *data++;



	}


	// wait until transfer finished
	while(!(I2C1->SR1 & (1UL << 2)));
	// RXNE yerine BTF(byte transfer finished) ile kontrol edilir


	// Generate stop after data received
	I2C1->CR1 |= (1UL << 9);
	// veri alındıktan sonra hat serbest bırakılır
}





