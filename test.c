#include "crt0.c"

void lcd_wait(int n);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init();
int btn_check_0();
int btn_check_3();

void show_self();
void show_stage();
void stage_init();
void stage_update();
int rand();

int lcd_ready = 0;
int random = 0;
int is_downside = 0;


char str1[16];  //上の段
char str2[16];  //下の段

void interrupt_handler()
{
	if(lcd_ready){
		//num = 1;
		show_self(is_downside);
		show_stage();
		stage_update();
	}	
}

void main()
{
	lcd_init();
	lcd_ready = 1;

	stage_init();

	while(1) {
		if(is_downside == 1 && btn_check_0()){
			is_downside = 0;
			lcd_wait(10);
		}else if(is_downside == 0 &&  btn_check_0()){
        	is_downside = 1;
			lcd_wait(10);
		}
	}
}

void show_self(int downside)
{
	if(downside){
		//下に表示
		lcd_cmd(0x01);
		lcd_cmd(0xc0);
		lcd_data('>');
	}else{
		//上に表示
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_data('>');
	}
}

void stage_init()
{
	for(int i = 0; i < 16; i++){
		if(rand() % 5 == 0){
			str1[i] = '#';
		}else{
			str1[i] = ' ';
		}
	}

	for(int i = 0; i < 16; i++){
		if(rand() % 4 == 0){
			str2[i] = '#';
		}else{
			str2[i] = ' ';
		}
	}
}

int rand()
{
	random = (2 * random + 3) % 7;
	return random;
}

void stage_update()
{
	for(int i = 0; i < 15; i++){
		str1[i] = str1[i+1];
		str2[i] = str2[i+1];
	}

	if(rand() % 4 == 0){
		str1[15] = '#';
	}else{
		str1[15] = ' ';
	}

	if(rand() % 4 == 0){
		str2[15] = '#';
	}else{
		str2[15] = ' ';
	}
}

void show_stage(unsigned char *str)
{
	for(int i = 0; i < 16; i++){
		lcd_cmd(0x80 + i);
		lcd_data(str1[i]);
	}

	for(int i = 0; i < 16; i++){
		lcd_cmd(0xc0 + i);
		lcd_data(str2[i]);
	}	
}

/*
 * Switch functions
 */
int btn_check_0()
{
	volatile int *sw_ptr = (int *)0xff04;;
	return (*sw_ptr & 0x10) ? 1 : 0;
}
int btn_check_3()
{
	volatile int *sw_ptr = (int *)0xff04;;
	return (*sw_ptr & 0x80) ? 1 : 0;
}



/*
 * LCD functions
 */

void lcd_wait(int n)
{
    int i;
    for (i = 0; i < n; i++)
        ;
}
void lcd_cmd(unsigned char cmd)
{
    /* E, RS, RW, DB[7:0] */
    volatile int *lcd_ptr = (int *)0xff0c;
    *lcd_ptr = (0x000000ff & cmd) | 0x00000000; /* E=0,RS=0,RW=0 */
    lcd_wait(1);
    *lcd_ptr = (0x000000ff & cmd) | 0x00000400; /* E=1,RS=0,RW=0 */
    lcd_wait(2);
    *lcd_ptr = (0x000000ff & cmd) | 0x00000000; /* E=0,RS=0,RW=0 */
    lcd_wait(11389);
}
void lcd_data(unsigned char data)
{
    /* E, RS, RW, DB[7:0] */
    volatile int *lcd_ptr = (int *)0xff0c;
    *lcd_ptr = (0x000000ff & data) | 0x00000200; /* E=0,RS=1,RW=0 */
    lcd_wait(1);
    *lcd_ptr = (0x000000ff & data) | 0x00000600; /* E=1,RS=1,RW=0 */
    lcd_wait(2);
    *lcd_ptr = (0x000000ff & data) | 0x00000200; /* E=0,RS=1,RW=0 */
    lcd_wait(278);
}
void lcd_init()
{
    lcd_wait(104167);
    lcd_cmd(0x38);
    lcd_cmd(0x06);
    lcd_cmd(0x0c);
    lcd_cmd(0x01);
    lcd_wait(10417);
}
void lcd_str(unsigned char *str)
{
    /* 8-bit, 2-line mode */
    /* Cursor auto increment */ /* Display ON */
    /* Clear display */
    while (*str != 0x00)
        lcd_data(*str++);
}
