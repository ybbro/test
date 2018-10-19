#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mpu6050/mpu6050.h"

#define UART1_BAUD_RATE 115200L

#include "uart/uart.h"

int micros = 0;

#define ROTOR_L_U OCR3B
#define ROTOR_R_U OCR1B
#define ROTOR_L_D OCR3A
#define ROTOR_R_D OCR3C

/* Motor Gain 을 설정합니다. 모터마다 출력이 조금씩 다르기 때문에 테스트에 따라 게인을 설정합니다. */
#define ROTOR_L_U_GAIN 50
#define ROTOR_R_U_GAIN 50
#define ROTOR_L_D_GAIN 50
#define ROTOR_R_D_GAIN 50

char strbuf[128];
char retbuf[128];
int bufpos = 0;


/* User Command Input */
char* GetLine()
{
	int c = 0;
	while (1)
	{
		int c = uart1_getc();
		if (c & UART_NO_DATA)
			return 0;

		/* if(c & UART_FRAME_ERROR)
			return 0;
		if(c & UART_OVERRUN_ERROR)
			return 0;
		if(c & UART_BUFFER_OVERFLOW)
			return 0; */

		strbuf[bufpos++] = c;

		if (strbuf[bufpos - 1] == '\n')
		{
			memcpy(retbuf, strbuf, bufpos);
			retbuf[bufpos - 1] = '\0';
			bufpos = 0;
			return retbuf;
		}
	}
	return 0;
}

int16_t ax = 0;
int16_t ay = 0;
int16_t az = 0;
int16_t gx = 0;
int16_t gy = 0;
int16_t gz = 0;
double axg = 0;
double ayg = 0;
double azg = 0;
double gxds = 0;
double gyds = 0;
double gzds = 0;
long *ptr = 0;
double qw = 1.0f;
double qx = 0.0f;
double qy = 0.0f;
double qz = 0.0f;
double roll = 0.0f;
double pitch = 0.0f;
double yaw = 0.0f;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

void readAccelGyro() { // mpu6050으로부터 데이터 받음 -> x,y,z 3축에 대한 가속도, 각속도 값
	mpu6050_getRawData(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);
}

float dt;
float accel_angle_x, accel_angle_y;    //accel_angle_z; Z방향 각가속도 계산 X
float filtered_angle_x, filtered_angle_y, filtered_angle_z; // 글로벌 변수는 자동으로 0으로 됨

extern float roll_output, pitch_output, yaw_output; //yaw_output 이거 현재 사용 x -> yaw 측정 불가
float motorA_speed, motorB_speed, motorC_speed, motorD_speed;

float baseAcX, baseAcY, baseAcZ;
float baseGyX, baseGyY, baseGyZ;

void calibAccelGyro() {
	float sumAcX = 0, sumAcY = 0, sumAcZ = 0;
	float sumGyX = 0, sumGyY = 0, sumGyZ = 0;

	readAccelGyro(); // 자이로에서 3축에 대한 가속도, 각속도 값을 받음. 이 값들로 초기화를 함 

	for (int i = 0; i < 10; i++) { // 각각 100ms의 딜레이로 10번의 데이터를 수집하여(1초 소요) sumXXX 변수에 더해줌 -> 스위치를 켠 후 1초간 평형을 유지하는것이 중요
		readAccelGyro();
		sumAcX += AcX; sumAcY += AcY; sumAcZ += AcZ;
		sumGyX += GyX; sumGyY += GyY; sumGyZ += GyZ;
		_delay_ms(100);
	}

	baseAcX = sumAcX / 10; // 위에 더한 값들에 평균을 내어줌
	baseAcY = sumAcY / 10;
	baseAcZ = sumAcZ / 10;

	baseGyX = sumGyX / 10;
	baseGyY = sumGyY / 10;
	baseGyZ = sumGyZ / 10;
}

unsigned long t_now;
unsigned long t_prev;

void initDT() {
	TCCR1B |= _BV(CS10); // 1분주율(코드비전에서 _BV는 해당 비트를 1로 설정하라는 명령어) 1<< CS10
	TIMSK |= _BV(TOIE1); // 타이머/카운터1 오버플로우 인터럽트 인에이블
	micros = 0;
}

void calcDT() {
	dt = ((float)micros / ( 1000000.0 / 16.0 )); // 처음의 initDT() 이후(타이머 인터럽트가 구동)에 calDT()가 나올때마다 그 동안 걸린 시간을 계산
	micros = 0; // 다시 초기화하고 다음 calDT() 가 나올 때까지 걸린 시간을 저장하기 위한 준비
}

ISR(TIMER1_OVF_vect) {
	micros += 1; // *16 ???? 16 뭐임 -> 1/16*1*65536 = 4096us마다 +1씩
}


void calcAccelYPR() {
	float accel_x, accel_y, accel_z;
	float accel_xz, accel_yz;
	const float RADIANS_TO_DEGREES = 180 / 3.14159; // 라디안 -> 도

	accel_x = AcX - baseAcX; // 현재 자이로에서 읽어들인 가속도에서 초기 가속도를 빼서 현재의 각 축방향 가속도를 구함
	accel_y = AcY - baseAcY;
	accel_z = AcZ + (16384 - baseAcZ); // 2^14=16384 인데 왜 쓰는가? -> MPU6050 데이터시트 13 페이지 참조 -> ADC Word Length = 0 (민감도 최대치)일 때 중력 가속도를 보정해주기 위해 쓰는 값

	accel_yz = sqrt(pow(accel_y, 2) + pow(accel_z, 2)); // YZ 평면 방향 가속도
	accel_angle_y = atan(-accel_x / accel_yz) * RADIANS_TO_DEGREES; // 오일러 각에 의한 roll 값 계산

	accel_xz = sqrt(pow(accel_x, 2) + pow(accel_z, 2));
	accel_angle_x = atan( accel_y / accel_xz) * RADIANS_TO_DEGREES;
	 
	//accel_angle_z = 0; // 0이고 아예 쓰이지 않음.. Z방향 각가속도는 무시
}

float gyro_x, gyro_y, gyro_z;

void calcGyroYPR() {
	const float GYROXYZ_TO_DEGREES_PER_SEC = 131; // 데이터시트 12페이지 참고 : Gyroscope ADC Word Length 

	gyro_x = (GyX - baseGyX) / GYROXYZ_TO_DEGREES_PER_SEC; // 처음에 평형 상태에서 구한 각속도 값과 현재의 각속도 값의 차이를 단위만 변환 -> 현재의 각속도
	gyro_y = (GyY - baseGyY) / GYROXYZ_TO_DEGREES_PER_SEC;
	gyro_z = (GyZ - baseGyZ) / GYROXYZ_TO_DEGREES_PER_SEC;

	//  gyro_angle_x += gyro_x * dt; // 이거 왜 지운거지? -> 안씀. 바로 밑의 calcFilteredYPR()에서 현재 각도 계산
	//  gyro_angle_y += gyro_y * dt;
	//  gyro_angle_z += gyro_z * dt;
}

void calcFilteredYPR() {
	const float ALPHA = 0.96;
	float tmp_angle_x, tmp_angle_y, tmp_angle_z;

	tmp_angle_x = filtered_angle_x + gyro_x * dt;
	tmp_angle_y = filtered_angle_y + gyro_y * dt;
	tmp_angle_z = filtered_angle_z + gyro_z * dt;

	/* 간단한 선형 필터를 적용하여 값을 안정화합니다. */ // ALPHA 는 기울기
	/* Edu
	filtered_angle_x = ALPHA * tmp_angle_x + (1.0-ALPHA) * accel_angle_x;
	filtered_angle_y = 0; // 값을 지정해야 합니다.
	filtered_angle_z = 0; // 값을 지정해야 합니다.
	*/

	/* Sample */
	filtered_angle_x = ALPHA * tmp_angle_x + (1.0 - ALPHA) * accel_angle_x;
	filtered_angle_y = ALPHA * tmp_angle_y + (1.0 - ALPHA) * accel_angle_y;
	filtered_angle_z = tmp_angle_z; // Accel 값으로는 Yaw 변화 감지가 불가능합니다. 지자기계 등 추가필요. -> YAW 잡는 방법 X -> 그냥 실험적으로 잡아보자 -> 아니 씨바 이따구로 하면 뒤에 계산 어떻게 함?
}

float base_roll_target_angle;
float base_pitch_target_angle;
float base_yaw_target_angle;

extern float roll_target_angle;
extern float pitch_target_angle;
extern float yaw_target_angle;

void initYPR() {

	for (int i = 0; i < 10; i++) {
		readAccelGyro(); // 자이로에서 각 축방향 가속도, 각속도 값을 읽어옴
		calcDT(); // 여기까지 걸린 시간을 계산 dt 및 micros 초기화
		calcAccelYPR(); // 가속도 측정
		calcGyroYPR(); // 각속도 측정
		calcFilteredYPR(); // 현재의 x,y방향 각도를 구합니다.

		base_roll_target_angle += filtered_angle_y; // 각 축방향 목표 각도
		base_pitch_target_angle += filtered_angle_x;
		base_yaw_target_angle += filtered_angle_z;

		_delay_ms(100);
	}

	base_roll_target_angle /= 10;
	base_pitch_target_angle /= 10;
	base_yaw_target_angle /= 10;

	roll_target_angle = base_roll_target_angle;
	pitch_target_angle = base_pitch_target_angle;
	yaw_target_angle = base_yaw_target_angle;
}

float throttle = 0;
float motorA_speed, motorB_speed, motorC_speed, motorD_speed;

/*1. 블루투스 4.0 (HM - 10) 의 STATE 핀이 블루투스가 연결 되었는지 아닌지를 판정 가능한 신호를 내어주나, 현재 드론에는 STATE 핀이 연결되어있지 않아 블루투스 자체만으로는 판정 불가

2. 실력이 뛰어난 파일럿에게 현재의 나인플러스 드론보다 더 조작감이 좋은 드론으로 근처에서 위치를 유지해보기를 요청
   -> 위치를 유지할 때 가장 입력이 뜸할 것이라 가정하고 어느 정도의 간격으로 명령을 입력하는지 관찰

3. 관찰 결과 : 위치를 유지할 때에도 컨트롤러를 거의 끊임없이 조작해줌.
               따라서 3초 정도 컨트롤러의 입력이 없을 경우는 연결이 끊어진 것(혹은 컨트롤러에서 손을 놓음)이라 판단하고 모터의 출력을 서서히 낮추기로 함*/



void calcMotorSpeed() {
	/* Roll 은 좌우 기우뚱, Pitch는 앞뒤 기우뚱, Yaw는 빙글빙글. */
	/* 따라서 Roll, Pitch, Yaw 에 유의하여야 합니다. */
	/* Roll 은 좌우 반전, Pitch는 전후 반전, Yaw는 대각끼리 일정해야 합니다. */

	/* Education */
	// 앞뒤 끄떡, 회전은 이렇게 합니다. 좌우로 끄떡끄떡은 어떻게 해야 할까요? -> roll 연산에 대한 결과값을 가감산해줌
	
	motorA_speed = (throttle == 0) ? 0:
	throttle  + pitch_output + roll_output + ROTOR_L_U_GAIN; //+ yaw_output 삭제
	motorB_speed = (throttle == 0) ? 0:
	throttle + pitch_output - roll_output + ROTOR_R_U_GAIN;//- yaw_output 삭제
	motorC_speed = (throttle == 0) ? 0:
	throttle - pitch_output + roll_output + ROTOR_L_D_GAIN; //- yaw_output 삭제
	motorD_speed = (throttle == 0) ? 0:
	throttle  - pitch_output - roll_output + ROTOR_R_D_GAIN; //+ yaw_output 삭제

	if(motorA_speed < 0) motorA_speed = 0; 
	if(motorA_speed > 255) motorA_speed = 255;
	if(motorB_speed < 0) motorB_speed = 0;
	if(motorB_speed > 255) motorB_speed = 255;
	if(motorC_speed < 0) motorC_speed = 0;
	if(motorC_speed > 255) motorC_speed = 255;
	if(motorD_speed < 0) motorD_speed = 0;
	if(motorD_speed > 255) motorD_speed = 255;
	

	/* Sample 
	motorA_speed = (throttle == 0) ? 0 :
	               throttle + yaw_output + roll_output + pitch_output + ROTOR_L_U_GAIN;
	motorB_speed = (throttle == 0) ? 0 :
	               throttle - yaw_output - roll_output + pitch_output + ROTOR_R_U_GAIN;
	motorC_speed = (throttle == 0) ? 0 :
	               throttle - yaw_output + roll_output - pitch_output + ROTOR_L_D_GAIN;
	motorD_speed = (throttle == 0) ? 0 :
	               throttle + yaw_output - roll_output - pitch_output + ROTOR_R_D_GAIN;

	if (motorA_speed < 0) motorA_speed = 0;
	if (motorA_speed > 255) motorA_speed = 255;
	if (motorB_speed < 0) motorB_speed = 0;
	if (motorB_speed > 255) motorB_speed = 255;
	if (motorC_speed < 0) motorC_speed = 0;
	if (motorC_speed > 255) motorC_speed = 255;
	if (motorD_speed < 0) motorD_speed = 0;
	if (motorD_speed > 255) motorD_speed = 255;*/
}

enum {
	HEAD1,  HEAD2,  HEAD3,  DATASIZE, CMD,
	ROLL,   PITCH,  YAW,    THROTTLE,
	AUX,    CRC,    PACKETSIZE,
};

uint8_t mspPacketBuf[128];

char* getMSPPacket()
{
	int c = 0;
	while (1)
	{
		int c = uart1_getc();
		if (c & UART_NO_DATA) // 사용 가능한 데이터가 없을 경우 -> '입력 없음' 이 포함
			return 0;

		/* if(c & UART_FRAME_ERROR)
			return 0;
		if(c & UART_OVERRUN_ERROR)
			return 0;
		if(c & UART_BUFFER_OVERFLOW)
			return 0; */
		if (c != '$' && bufpos == 0)
			continue;

		strbuf[bufpos++] = c; // 배열 내에 하나씩 

		if (bufpos == PACKETSIZE)
		{
			memcpy(retbuf, strbuf, bufpos);
			bufpos = 0;
			return retbuf;
		}
	}
	return 0;
}

int checkMspPacket() { 
	static uint32_t cnt;
	char* mspPacket = NULL;
	if ((mspPacket = getMSPPacket())) { // getMSPPacket()가 retbuf를 리턴할 경우(데이터 버퍼가 쌓여서 정상적으로 보낼 준비가 됨) 이걸 mspPacket에 값을 대입하고, 대입된 값이 0이 아닐 경우(조건문 내에 아무런 비교문이 없을 경우 !=0 생략) 안의 내용을 수행
		if (mspPacket[CMD] == 150) {
			throttle = mspPacket[THROTTLE];
			
			if (mspPacket[PITCH] - 125 == 0)
				pitch_target_angle = base_pitch_target_angle;
			else if (mspPacket[PITCH] - 125 < 0)
				pitch_target_angle = base_pitch_target_angle + 15;
			else
				pitch_target_angle = base_pitch_target_angle - 15;

			if (mspPacket[ROLL] == 125)
				roll_target_angle = base_roll_target_angle;
			else if (mspPacket[ROLL] < 125)
				roll_target_angle = base_roll_target_angle - 15;
			else
				roll_target_angle = base_roll_target_angle + 15;
		}
	}
	// 여기서부터 추가 구문
	else // 정상적으로 통신 데이터가 나오지 않을 경우
	return mspPacket;
}

#define THROTTLE_MAX 255
#define THROTTLE_MIN 0

void initMotorSpeed() {
	ROTOR_L_U = THROTTLE_MIN;
	ROTOR_R_U = THROTTLE_MIN;
	ROTOR_L_D = THROTTLE_MIN;
	ROTOR_R_D = THROTTLE_MIN;
}

void updateMotorSpeed() { 
	ROTOR_L_U = motorA_speed * 4;
	ROTOR_R_U = motorB_speed * 4;
	ROTOR_L_D = motorC_speed * 4;
	ROTOR_R_D = motorD_speed * 4;
}

void dualPID(float target_angle, float angle_in, float rate_in, float stabilize_kp, float stabilize_ki, float rate_kp,
             float rate_ki, float rate_kd, float pre_rate_error, float *stabilize_iterm, float *rate_iterm, float *output) { // stabilize_kp, stabilize_ki 실험적으로 변형하여야함 // rate : 각속도 
	float angle_error;
	float desired_rate;
	float rate_error;
	float stabilize_pterm, rate_pterm;

	angle_error = target_angle - angle_in;
	

	stabilize_pterm  = stabilize_kp * angle_error; // 각도의 차이에 대함 PI 제어 계산
	(*stabilize_iterm) += stabilize_ki * angle_error * dt; // 각도 오차를 누적 -> 각도 오차가 커질 경우 더 빠르게 원 상태로 돌아가게 하는데 쓰임

	desired_rate = stabilize_pterm; // 왜??? 목표 각속도=각도 차이*Kp????(단위 무시하고) 일단 식만 놓고 보자면 각도 차이가 크게 날수록 목표 각속도는 올라감 -> 여기서 K 계수들만 바꿔주면 적절하게 잡을 수 있음

	rate_error = desired_rate - rate_in;

	rate_pterm  = rate_kp * rate_error;
	(*rate_iterm) += rate_ki * rate_error * dt;
	rate_dterm = rate_kd * (rate_error-pre_rate_error)/dt;
	pre_rate_error=rate_error;

	(*output) = rate_pterm + (*rate_iterm) + rate_dterm + (*stabilize_  iterm);
}

/* PID 값을 세팅합니다. */

float roll_pre_rate_error;
float pitch_pre_rate_error;
float yaw_pre_rate_error;

float roll_target_angle = 0.0;
float roll_angle_in;
float roll_rate_in;
float roll_stabilize_kp = 0.8;
float roll_stabilize_ki = 0.2;
float roll_rate_kp = 1;
float roll_rate_ki = 0;
float roll_rate_kd = 1;
float roll_stabilize_iterm;
float roll_rate_iterm;
float roll_output;

float pitch_target_angle = 0.0;
float pitch_angle_in;
float pitch_rate_in;
float pitch_stabilize_kp = 1;
float pitch_stabilize_ki = 0.1;
float pitch_rate_kp = 1;
float pitch_rate_ki = 0;
float pitch_rate_kd = 1;
float pitch_stabilize_iterm;
float pitch_rate_iterm;
float pitch_output;

float yaw_target_angle = 0.0;
float yaw_angle_in;
float yaw_rate_in;
float yaw_stabilize_kp = 0.6;
float yaw_stabilize_ki = 0.3;
float yaw_rate_kp = 1;
float yaw_rate_ki = 0;
float yaw_rate_kd = 1;
float yaw_stabilize_iterm;
float yaw_rate_iterm;
float yaw_output;

void calcYPRtoDualPID() {
	roll_angle_in = filtered_angle_y;
	roll_rate_in = gyro_y;
	dualPID(  roll_target_angle,
	          roll_angle_in,
	          roll_rate_in,
	          roll_stabilize_kp,
	          roll_stabilize_ki,
	          roll_rate_kp,
	          roll_rate_ki,
			  roll_rate_kd,
			  roll_pre_rate_error,
	          &roll_stabilize_iterm,
	          &roll_rate_iterm,
	          &roll_output);

	pitch_angle_in = filtered_angle_x;
	pitch_rate_in = gyro_x;
	dualPID(pitch_target_angle,
	        pitch_angle_in,
	        pitch_rate_in,
	        pitch_stabilize_kp,
	        pitch_stabilize_ki,
	        pitch_rate_kp,
	        pitch_rate_ki,
			pitch_rate_kd,
			pitch_pre_rate_error,
	        &pitch_stabilize_iterm,
	        &pitch_rate_iterm,
	        &pitch_output);

	yaw_angle_in = filtered_angle_z;
	yaw_rate_in = gyro_z;
	dualPID(  yaw_target_angle,
	          yaw_angle_in,
	          yaw_rate_in,
	          yaw_stabilize_kp,
	          yaw_stabilize_ki,
	          yaw_rate_kp,
	          yaw_rate_ki,
			  yaw_rate_kd,
			  yaw_pre_rate_error,
	          &yaw_stabilize_iterm,
	          &yaw_rate_iterm,
	          &yaw_output);
}

/*void getSensorYPR() // 이 함수를 쓰는 구간 x
{
	if (mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz))
	{
		mpu6050_getRollPitchYaw(qw, qx, qy, qz, &filtered_angle_x, &filtered_angle_y, &filtered_angle_z);
		// MPU 6050 Data Input 
	}
}*/


int main(void)
{
	PORTA = 0xFF; //led
	DDRA = 0xff;
	int i = 0x01;

	DDRB = 0xFF; // PB6(OC1B) : RIGHT UP(중요 : 넷 중 하나만 B포트), PB7 : 스피커(추가 과제?)
	DDRE = 0xFF; // PE3(OC3A) : LEFT DOWN, PE4(OC3B) : LEFT UP, PE5(OC3C) : RIGHT DOWN, INT6, INT7(외부 인터럽트도 추가 과제 예상)

	/* 모터 PWM 초기화 */
	// 모든 모터에 대해 Fast PWM(10비트-> 0~1023까지), 64분주율
	TCCR1A = 0b00100011; // OC1B 에 대해서만 OC1B 출력 클리어
	TCCR1B = 0b00001011;

	TCCR3A = 0b10101011; // OC3A, OC3B, OC3C 에 대해서 OCnx 출력 클리어
	TCCR3B = 0b00001011;

	// 위에서부터 OCR3B,OCR1B,OCR3A,OCR3C 값이며 시작하면서 초기화 -> 모터의 급작스러운 동작을 방지
	ROTOR_L_U = 0;
	ROTOR_R_U = 0;
	ROTOR_R_D = 0;
	ROTOR_L_D = 0;
	/* ------------ */

	char* strbuf;
	char outbuf[128];

	mpu6050_init(); // mpu6050 센서 초기화
	_delay_ms(2050); // 걸리는 시간 50ms + 2s -> 시작시 2초의 시간을 더 지연시켜 이후의 기본 좌표를 만드는데 손을 떼면서 생기는 떨림을 방지

	calibAccelGyro(); // 초기에 각 축방향 가속도, 각속도를 수집하여 기초값을 만듦
	initDT(); // 타이머/카 .,
	//|운터1 오버플로우 인터럽트 시작
	initYPR(); // 회전 운동에 대한 측정을 위한 초기 설정
	initMotorSpeed(); // 모든 모터의 출력을 0으로 만듦 -> ?????? 이거 위에서 해준건데
	
	//uart_init(UART_BAUD_SELECT(UART1_BAUD_RATE, F_CPU));
	uart1_init(UART_BAUD_SELECT(UART1_BAUD_RATE, F_CPU)); // 위의 UART1_BAUD_RATE 를 이용하여 UART1 초기화

	sei();

	PORTA = 0x00; // 준비가 완료되면 LED 끔
	

	while (1)
	{
		readAccelGyro(); // 자이로에서 데이터 받음

		calcDT(); // 루프가 1회 도는 동안 걸린 시간 dt를 구함
		calcAccelYPR(); // roll & pitch 계산 
		calcGyroYPR(); // 현재의 각속도 측정
		calcFilteredYPR(); // 선형 필터를 사용하여 각 방향 각도를 구함

		calcYPRtoDualPID();
		calcMotorSpeed();

		while (checkMspPacket()); // to clear buffer

		updateMotorSpeed();// 0~255 * 4 -> 0~1020
	}
}
