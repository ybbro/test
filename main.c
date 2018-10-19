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

/* Motor Gain �� �����մϴ�. ���͸��� ����� ���ݾ� �ٸ��� ������ �׽�Ʈ�� ���� ������ �����մϴ�. */
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

void readAccelGyro() { // mpu6050���κ��� ������ ���� -> x,y,z 3�࿡ ���� ���ӵ�, ���ӵ� ��
	mpu6050_getRawData(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);
}

float dt;
float accel_angle_x, accel_angle_y;    //accel_angle_z; Z���� �����ӵ� ��� X
float filtered_angle_x, filtered_angle_y, filtered_angle_z; // �۷ι� ������ �ڵ����� 0���� ��

extern float roll_output, pitch_output, yaw_output; //yaw_output �̰� ���� ��� x -> yaw ���� �Ұ�
float motorA_speed, motorB_speed, motorC_speed, motorD_speed;

float baseAcX, baseAcY, baseAcZ;
float baseGyX, baseGyY, baseGyZ;

void calibAccelGyro() {
	float sumAcX = 0, sumAcY = 0, sumAcZ = 0;
	float sumGyX = 0, sumGyY = 0, sumGyZ = 0;

	readAccelGyro(); // ���̷ο��� 3�࿡ ���� ���ӵ�, ���ӵ� ���� ����. �� ����� �ʱ�ȭ�� �� 

	for (int i = 0; i < 10; i++) { // ���� 100ms�� �����̷� 10���� �����͸� �����Ͽ�(1�� �ҿ�) sumXXX ������ ������ -> ����ġ�� �� �� 1�ʰ� ������ �����ϴ°��� �߿�
		readAccelGyro();
		sumAcX += AcX; sumAcY += AcY; sumAcZ += AcZ;
		sumGyX += GyX; sumGyY += GyY; sumGyZ += GyZ;
		_delay_ms(100);
	}

	baseAcX = sumAcX / 10; // ���� ���� ���鿡 ����� ������
	baseAcY = sumAcY / 10;
	baseAcZ = sumAcZ / 10;

	baseGyX = sumGyX / 10;
	baseGyY = sumGyY / 10;
	baseGyZ = sumGyZ / 10;
}

unsigned long t_now;
unsigned long t_prev;

void initDT() {
	TCCR1B |= _BV(CS10); // 1������(�ڵ�������� _BV�� �ش� ��Ʈ�� 1�� �����϶�� ��ɾ�) 1<< CS10
	TIMSK |= _BV(TOIE1); // Ÿ�̸�/ī����1 �����÷ο� ���ͷ�Ʈ �ο��̺�
	micros = 0;
}

void calcDT() {
	dt = ((float)micros / ( 1000000.0 / 16.0 )); // ó���� initDT() ����(Ÿ�̸� ���ͷ�Ʈ�� ����)�� calDT()�� ���ö����� �� ���� �ɸ� �ð��� ���
	micros = 0; // �ٽ� �ʱ�ȭ�ϰ� ���� calDT() �� ���� ������ �ɸ� �ð��� �����ϱ� ���� �غ�
}

ISR(TIMER1_OVF_vect) {
	micros += 1; // *16 ???? 16 ���� -> 1/16*1*65536 = 4096us���� +1��
}


void calcAccelYPR() {
	float accel_x, accel_y, accel_z;
	float accel_xz, accel_yz;
	const float RADIANS_TO_DEGREES = 180 / 3.14159; // ���� -> ��

	accel_x = AcX - baseAcX; // ���� ���̷ο��� �о���� ���ӵ����� �ʱ� ���ӵ��� ���� ������ �� ����� ���ӵ��� ����
	accel_y = AcY - baseAcY;
	accel_z = AcZ + (16384 - baseAcZ); // 2^14=16384 �ε� �� ���°�? -> MPU6050 �����ͽ�Ʈ 13 ������ ���� -> ADC Word Length = 0 (�ΰ��� �ִ�ġ)�� �� �߷� ���ӵ��� �������ֱ� ���� ���� ��

	accel_yz = sqrt(pow(accel_y, 2) + pow(accel_z, 2)); // YZ ��� ���� ���ӵ�
	accel_angle_y = atan(-accel_x / accel_yz) * RADIANS_TO_DEGREES; // ���Ϸ� ���� ���� roll �� ���

	accel_xz = sqrt(pow(accel_x, 2) + pow(accel_z, 2));
	accel_angle_x = atan( accel_y / accel_xz) * RADIANS_TO_DEGREES;
	 
	//accel_angle_z = 0; // 0�̰� �ƿ� ������ ����.. Z���� �����ӵ��� ����
}

float gyro_x, gyro_y, gyro_z;

void calcGyroYPR() {
	const float GYROXYZ_TO_DEGREES_PER_SEC = 131; // �����ͽ�Ʈ 12������ ���� : Gyroscope ADC Word Length 

	gyro_x = (GyX - baseGyX) / GYROXYZ_TO_DEGREES_PER_SEC; // ó���� ���� ���¿��� ���� ���ӵ� ���� ������ ���ӵ� ���� ���̸� ������ ��ȯ -> ������ ���ӵ�
	gyro_y = (GyY - baseGyY) / GYROXYZ_TO_DEGREES_PER_SEC;
	gyro_z = (GyZ - baseGyZ) / GYROXYZ_TO_DEGREES_PER_SEC;

	//  gyro_angle_x += gyro_x * dt; // �̰� �� �������? -> �Ⱦ�. �ٷ� ���� calcFilteredYPR()���� ���� ���� ���
	//  gyro_angle_y += gyro_y * dt;
	//  gyro_angle_z += gyro_z * dt;
}

void calcFilteredYPR() {
	const float ALPHA = 0.96;
	float tmp_angle_x, tmp_angle_y, tmp_angle_z;

	tmp_angle_x = filtered_angle_x + gyro_x * dt;
	tmp_angle_y = filtered_angle_y + gyro_y * dt;
	tmp_angle_z = filtered_angle_z + gyro_z * dt;

	/* ������ ���� ���͸� �����Ͽ� ���� ����ȭ�մϴ�. */ // ALPHA �� ����
	/* Edu
	filtered_angle_x = ALPHA * tmp_angle_x + (1.0-ALPHA) * accel_angle_x;
	filtered_angle_y = 0; // ���� �����ؾ� �մϴ�.
	filtered_angle_z = 0; // ���� �����ؾ� �մϴ�.
	*/

	/* Sample */
	filtered_angle_x = ALPHA * tmp_angle_x + (1.0 - ALPHA) * accel_angle_x;
	filtered_angle_y = ALPHA * tmp_angle_y + (1.0 - ALPHA) * accel_angle_y;
	filtered_angle_z = tmp_angle_z; // Accel �����δ� Yaw ��ȭ ������ �Ұ����մϴ�. ���ڱ�� �� �߰��ʿ�. -> YAW ��� ��� X -> �׳� ���������� ��ƺ��� -> �ƴ� ���� �̵����� �ϸ� �ڿ� ��� ��� ��?
}

float base_roll_target_angle;
float base_pitch_target_angle;
float base_yaw_target_angle;

extern float roll_target_angle;
extern float pitch_target_angle;
extern float yaw_target_angle;

void initYPR() {

	for (int i = 0; i < 10; i++) {
		readAccelGyro(); // ���̷ο��� �� ����� ���ӵ�, ���ӵ� ���� �о��
		calcDT(); // ������� �ɸ� �ð��� ��� dt �� micros �ʱ�ȭ
		calcAccelYPR(); // ���ӵ� ����
		calcGyroYPR(); // ���ӵ� ����
		calcFilteredYPR(); // ������ x,y���� ������ ���մϴ�.

		base_roll_target_angle += filtered_angle_y; // �� ����� ��ǥ ����
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

/*1. ������� 4.0 (HM - 10) �� STATE ���� ��������� ���� �Ǿ����� �ƴ����� ���� ������ ��ȣ�� �����ֳ�, ���� ��п��� STATE ���� ����Ǿ����� �ʾ� ������� ��ü�����δ� ���� �Ұ�

2. �Ƿ��� �پ ���Ϸ����� ������ �����÷��� ��к��� �� ���۰��� ���� ������� ��ó���� ��ġ�� �����غ��⸦ ��û
   -> ��ġ�� ������ �� ���� �Է��� ���� ���̶� �����ϰ� ��� ������ �������� ����� �Է��ϴ��� ����

3. ���� ��� : ��ġ�� ������ ������ ��Ʈ�ѷ��� ���� ���Ӿ��� ��������.
               ���� 3�� ���� ��Ʈ�ѷ��� �Է��� ���� ���� ������ ������ ��(Ȥ�� ��Ʈ�ѷ����� ���� ����)�̶� �Ǵ��ϰ� ������ ����� ������ ���߱�� ��*/



void calcMotorSpeed() {
	/* Roll �� �¿� ����, Pitch�� �յ� ����, Yaw�� ���ۺ���. */
	/* ���� Roll, Pitch, Yaw �� �����Ͽ��� �մϴ�. */
	/* Roll �� �¿� ����, Pitch�� ���� ����, Yaw�� �밢���� �����ؾ� �մϴ�. */

	/* Education */
	// �յ� ����, ȸ���� �̷��� �մϴ�. �¿�� ���������� ��� �ؾ� �ұ��? -> roll ���꿡 ���� ������� ����������
	
	motorA_speed = (throttle == 0) ? 0:
	throttle  + pitch_output + roll_output + ROTOR_L_U_GAIN; //+ yaw_output ����
	motorB_speed = (throttle == 0) ? 0:
	throttle + pitch_output - roll_output + ROTOR_R_U_GAIN;//- yaw_output ����
	motorC_speed = (throttle == 0) ? 0:
	throttle - pitch_output + roll_output + ROTOR_L_D_GAIN; //- yaw_output ����
	motorD_speed = (throttle == 0) ? 0:
	throttle  - pitch_output - roll_output + ROTOR_R_D_GAIN; //+ yaw_output ����

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
		if (c & UART_NO_DATA) // ��� ������ �����Ͱ� ���� ��� -> '�Է� ����' �� ����
			return 0;

		/* if(c & UART_FRAME_ERROR)
			return 0;
		if(c & UART_OVERRUN_ERROR)
			return 0;
		if(c & UART_BUFFER_OVERFLOW)
			return 0; */
		if (c != '$' && bufpos == 0)
			continue;

		strbuf[bufpos++] = c; // �迭 ���� �ϳ��� 

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
	if ((mspPacket = getMSPPacket())) { // getMSPPacket()�� retbuf�� ������ ���(������ ���۰� �׿��� ���������� ���� �غ� ��) �̰� mspPacket�� ���� �����ϰ�, ���Ե� ���� 0�� �ƴ� ���(���ǹ� ���� �ƹ��� �񱳹��� ���� ��� !=0 ����) ���� ������ ����
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
	// ���⼭���� �߰� ����
	else // ���������� ��� �����Ͱ� ������ ���� ���
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
             float rate_ki, float rate_kd, float pre_rate_error, float *stabilize_iterm, float *rate_iterm, float *output) { // stabilize_kp, stabilize_ki ���������� �����Ͽ����� // rate : ���ӵ� 
	float angle_error;
	float desired_rate;
	float rate_error;
	float stabilize_pterm, rate_pterm;

	angle_error = target_angle - angle_in;
	

	stabilize_pterm  = stabilize_kp * angle_error; // ������ ���̿� ���� PI ���� ���
	(*stabilize_iterm) += stabilize_ki * angle_error * dt; // ���� ������ ���� -> ���� ������ Ŀ�� ��� �� ������ �� ���·� ���ư��� �ϴµ� ����

	desired_rate = stabilize_pterm; // ��??? ��ǥ ���ӵ�=���� ����*Kp????(���� �����ϰ�) �ϴ� �ĸ� ���� ���ڸ� ���� ���̰� ũ�� ������ ��ǥ ���ӵ��� �ö� -> ���⼭ K ����鸸 �ٲ��ָ� �����ϰ� ���� �� ����

	rate_error = desired_rate - rate_in;

	rate_pterm  = rate_kp * rate_error;
	(*rate_iterm) += rate_ki * rate_error * dt;
	rate_dterm = rate_kd * (rate_error-pre_rate_error)/dt;
	pre_rate_error=rate_error;

	(*output) = rate_pterm + (*rate_iterm) + rate_dterm + (*stabilize_  iterm);
}

/* PID ���� �����մϴ�. */

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

/*void getSensorYPR() // �� �Լ��� ���� ���� x
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

	DDRB = 0xFF; // PB6(OC1B) : RIGHT UP(�߿� : �� �� �ϳ��� B��Ʈ), PB7 : ����Ŀ(�߰� ����?)
	DDRE = 0xFF; // PE3(OC3A) : LEFT DOWN, PE4(OC3B) : LEFT UP, PE5(OC3C) : RIGHT DOWN, INT6, INT7(�ܺ� ���ͷ�Ʈ�� �߰� ���� ����)

	/* ���� PWM �ʱ�ȭ */
	// ��� ���Ϳ� ���� Fast PWM(10��Ʈ-> 0~1023����), 64������
	TCCR1A = 0b00100011; // OC1B �� ���ؼ��� OC1B ��� Ŭ����
	TCCR1B = 0b00001011;

	TCCR3A = 0b10101011; // OC3A, OC3B, OC3C �� ���ؼ� OCnx ��� Ŭ����
	TCCR3B = 0b00001011;

	// ���������� OCR3B,OCR1B,OCR3A,OCR3C ���̸� �����ϸ鼭 �ʱ�ȭ -> ������ ���۽����� ������ ����
	ROTOR_L_U = 0;
	ROTOR_R_U = 0;
	ROTOR_R_D = 0;
	ROTOR_L_D = 0;
	/* ------------ */

	char* strbuf;
	char outbuf[128];

	mpu6050_init(); // mpu6050 ���� �ʱ�ȭ
	_delay_ms(2050); // �ɸ��� �ð� 50ms + 2s -> ���۽� 2���� �ð��� �� �������� ������ �⺻ ��ǥ�� ����µ� ���� ���鼭 ����� ������ ����

	calibAccelGyro(); // �ʱ⿡ �� ����� ���ӵ�, ���ӵ��� �����Ͽ� ���ʰ��� ����
	initDT(); // Ÿ�̸�/ī .,
	//|����1 �����÷ο� ���ͷ�Ʈ ����
	initYPR(); // ȸ�� ��� ���� ������ ���� �ʱ� ����
	initMotorSpeed(); // ��� ������ ����� 0���� ���� -> ?????? �̰� ������ ���ذǵ�
	
	//uart_init(UART_BAUD_SELECT(UART1_BAUD_RATE, F_CPU));
	uart1_init(UART_BAUD_SELECT(UART1_BAUD_RATE, F_CPU)); // ���� UART1_BAUD_RATE �� �̿��Ͽ� UART1 �ʱ�ȭ

	sei();

	PORTA = 0x00; // �غ� �Ϸ�Ǹ� LED ��
	

	while (1)
	{
		readAccelGyro(); // ���̷ο��� ������ ����

		calcDT(); // ������ 1ȸ ���� ���� �ɸ� �ð� dt�� ����
		calcAccelYPR(); // roll & pitch ��� 
		calcGyroYPR(); // ������ ���ӵ� ����
		calcFilteredYPR(); // ���� ���͸� ����Ͽ� �� ���� ������ ����

		calcYPRtoDualPID();
		calcMotorSpeed();

		while (checkMspPacket()); // to clear buffer

		updateMotorSpeed();// 0~255 * 4 -> 0~1020
	}
}
