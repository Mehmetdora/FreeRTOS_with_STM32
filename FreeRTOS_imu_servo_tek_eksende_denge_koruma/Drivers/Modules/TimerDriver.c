/*
 * TimerDriver.c
 *
 *  Created on: Jul 7, 2026
 *      Author: mehmet_dora
 */


/*
 * PWMDriver.c
 *
 *  Created on: 20 Şub 2026
 *      Author: mehmet_dora
 */


#include "TimerDriver.h"
#include "stm32f4xx_hal.h"


#define SERVO_MIN_ANGLE      0.0f
#define SERVO_MAX_ANGLE      180.0f

#define SERVO_MIN_PULSE_US   1000.0f
#define SERVO_MAX_PULSE_US   2000.0f




void PWMDriver_init(void){


	// ENABLE GPIOA CLOCK
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	// RCC->AHB1ENR |= (1UL << 0);		// Üstteki ile aynı şey


	// ENABLE TIMER2 CLOCK
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	// RCC->APB1ENR |= (1UL << 0);		// Üstteki ile aynı şey




	// PORTu ALTERNATİF FONK OLARAK AYARLA
	GPIOA->MODER &= ~(3UL << (2*6));   // temizle
	GPIOA->MODER |= (2UL << (2*6));
	// TIM2 destekleyen bir pin seçilmeli
	// PA0 datasheet içindeki alternate fonk. tablosunda desteklediği için kullanılabilir


	// SEÇİLEN PİN İÇİN AF BİLGİSİNİ TANIMLAMA
	GPIOA->AFR[0] &= ~(0xF << (4*6));  // AF bits temizle
	GPIOA->AFR[0] |= (2UL << (4*6));
	// Buradaki AFR[0] seçilmesinin nedeni PA0 pininin numarası 8'den küçük olduğu için AFR[0] seçildi
	// eğer 8den büyük olsaydı AFR[1] seçilmesi gerekirdi.
	// AFR register içinde her pin için AF seçimi 4 bit ile yapıldığı için 4*0 yapılarak aslında ilk 4'lük seçildi.



	// CLOCK SİNYALİ AYARLAMA
	TIM3->PSC = 84-1;

	TIM3->ARR = 20000-1;		// SERVO KONTROLLERİ İÇİN BU DEĞERİN AYARLANMASI GEREKİR HZ DEĞERİ




	// TIM3 HANGİ MOD İLE ÇALIŞACAĞI

	// CH1/CH2 ---> CCMR1 ile ayarlanır, CCMR1 içinde CH1 ve CH2 içiden ayarlama bitleri 1 ve 2 ile buna göre ayrılır
	// CH3/CH4 ---> CCMR2 ile ayarlanır, aynı şekilde CH2 ve CH3 için de 3 ve 4 olarak ayrılır.
	TIM3->CCMR1 &= ~(7 << 4);	// öncesinde ilgili yerin clear edilmesi.
	TIM3->CCMR1 |= (6UL << 4);	// buradaki 6UL -> 110 demektir , bu ise PWM 1 modunu seçmek anlamına geliyor.
	// PWM modu şuna göre karşılaştırır
	/*
		CNT < CCR1 → output HIGH
		CNT ≥ CCR1 → output LOW
	 */



	// TIM3 PWM PRELOAD ENABLE
	TIM3->CCMR1 |= (1UL << 3);	// 3. bitteki preload ayarını enbable yaparak sıfırdan başlamasını sağla



	// TIM3 İÇİN CAPTURE/COMPARE MODUNU ENABLE YAP
	TIM3->CCER |= (1UL << 0);	// 0. bit C/C için enable ayarı içindir, set edildi.






	// TIM3 ARR İÇİN PRELOAD ENABLE
	TIM3->CR1 |= (1UL << 7);


	// UPDATE EVENT GENERATION eNABLE YAP
	// önceki counter değerlerinin sıfırlanması ile tüm counter'ların 0'dan saymaya başlamasını sağlar.
	// yani en baştan tüm timer'ı başlat
	TIM3->EGR |= (1UL << 0);	// hemen event üret ve PWM counter'ları sıfırlanarak başlasın


	Servo_SetAngle(90.0f);	// Başlangıç açısı


	// START TİMER COUNTİNG
	PWMDriver_on();	// 0. bit CEN dir ve tüm timer'ın saymaya başlamasını enable yapar. Sistemi başlatır.



}


float clamp_float(float value, float min, float max)
{
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

void Servo_SetAngle(float angle)
{
    float pulse_us;

    /*
     * Açıyı güvenli aralıkta tut.
     */
    angle = clamp_float(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

    /*
     * 0-180 dereceyi 1000-2000 us pulse aralığına dönüştür.
     *
     * angle = 0   → 1000 us
     * angle = 90  → 1500 us
     * angle = 180 → 2000 us
     */
    pulse_us = SERVO_MIN_PULSE_US +
               ((angle / 180.0f) * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US));

    /*
     * Eğer timer 1 us çözünürlükte ayarlandıysa CCR doğrudan pulse_us olur.
     * TIM3 CH1 örneği:
     */
    TIM3->CCR1 = (uint32_t)pulse_us;
}

float limit_step(float target, float current, float max_step)
{
    if(target > current + max_step) return current + max_step;
    if(target < current - max_step) return current - max_step;
    return target;
}


void Servo_SetPulseUs(uint16_t pulse_us)
{
    if(pulse_us < 1000)
    {
        pulse_us = 1000;
    }

    if(pulse_us > 2000)
    {
        pulse_us = 2000;
    }

    TIM3->CCR1 = pulse_us;
}

void Servo_SetNeutral(void)
{
    Servo_SetAngle(90.0f);
}


void PWMDriver_on(void){
	TIM3->CR1 |= (1UL << 0);
}


void PWMDriver_off(void){
	TIM3->CR1 &= ~(1UL << 0);
}


