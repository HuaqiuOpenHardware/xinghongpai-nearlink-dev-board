#include "bsp_max30102.h"
#include "soc_osal.h"
//#include "app_init.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"
#define CHIP_SW63	1

#if defined(CHIP_SW63)
char bsp_max30102_write_data
	(
	unsigned char	addr,
	unsigned char 	data
	)
{
	unsigned char cmd[] = {addr, data};
	char ret = bsp_iic_master_write_data(MAX30102_IIC_BUS, MAX30102_SLAVE_ADDRESS, cmd, sizeof(cmd));
	return ret;
}


char bsp_max30102_read_data
	(
	unsigned char	addr,
	unsigned char 	*data,
	unsigned char 	data_len
	)
{
	unsigned char cmd[] = {addr};
	char ret = bsp_iic_master_write_data(MAX30102_IIC_BUS, MAX30102_SLAVE_ADDRESS, cmd, sizeof(cmd));

	ret = bsp_iic_master_read_data(MAX30102_IIC_BUS, MAX30102_SLAVE_ADDRESS, data, data_len);
	return ret;
}




char bsp_max30102_reset
	(
	void
	)
{
	printf("bsp_max30102_reset start>>>>> !\r\n");
	if(bsp_max30102_write_data(REG_MODE_CONFIG, 0x40) != 0)
    {
		printf("bsp_max30102_reset error1>>>>> !\r\n");
		//return -1;
	}
	
	if(bsp_max30102_write_data(REG_MODE_CONFIG, 0x40) != 0)
    {
		printf("bsp_max30102_reset error2>>>>> !\r\n");
		//return -1;
	}
	return 0;
}

unsigned char Max30102_I2c_Read(unsigned char data)
{
	unsigned char temp;
  
	bsp_max30102_read_data(data, &temp, 1); 

	return temp;
}

char bsp_max30102_config
	(
	void
	)
{
	char ret = 0;

	printf("bsp_max30102_config >>>>> !\r\n");
	printf("REV_ID:0x%x PART_ID:0x%x\n", Max30102_I2c_Read(REG_REV_ID), Max30102_I2c_Read(REG_PART_ID));
	uapi_tcxo_delay_ms(100);

	ret = bsp_max30102_write_data(0x06,0x0b);
	ret = bsp_max30102_write_data(0x01,0xf0);
	ret = bsp_max30102_write_data(0x00,0x00);
	ret = bsp_max30102_write_data(0x09,0x33);
	ret = bsp_max30102_write_data(0x07,0x47);
	ret = bsp_max30102_write_data(0x02,0x00);
	ret = bsp_max30102_write_data(0x03,0x00);
	ret = bsp_max30102_write_data(0x04,0x0f);
	

	// ret = bsp_max30102_write_data(0x02,0xc0);
	// ret = bsp_max30102_write_data(0x03,0x00);
	// ret = bsp_max30102_write_data(0x04,0x00);
	// ret = bsp_max30102_write_data(0x05,0x00);
	// ret = bsp_max30102_write_data(0x06,0x00);
	// ret = bsp_max30102_write_data(0x08,0x0f);
	// ret = bsp_max30102_write_data(0x09,0x03);
	// ret = bsp_max30102_write_data(0x0A,0x27);
	// ret = bsp_max30102_write_data(0x0C,0x24);
	// ret = bsp_max30102_write_data(0x0D,0x24);
	// ret = bsp_max30102_write_data(0x10,0x7f);
	
	return ret;
}

char bsp_max30102_init(void)
{
	uapi_tcxo_delay_ms(50);
	if (bsp_max30102_reset() !=  0)
	{
		printf("bsp_max30102_reset: error !\r\n");
		//return -1;
	}
	if (bsp_max30102_config() != 0)
	{
		printf("bsp_max30102_config: error !\r\n");
		//return -1;
	}
	return 0;
}

unsigned char bsp_max30102_temp_read(void)
{
	unsigned char temp;
  
	bsp_max30102_read_data(0x11, &temp, 1); 

	return temp;
}

void bsp_max30102_fifo_read(float *output_data)
{
	unsigned char receive_data[6];
	unsigned char temp;
	unsigned int data[2];
  
	// read and clear status register
	bsp_max30102_read_data(REG_INTR_STATUS_1, &temp, 1); 
	bsp_max30102_read_data(REG_INTR_STATUS_2, &temp, 1); 
  
	receive_data[0] = REG_FIFO_DATA;
	bsp_max30102_read_data(REG_FIFO_DATA, receive_data, 6);
	
	data[0] = ((receive_data[0]<<16 | receive_data[1]<<8 | receive_data[2]) & 0x03ffff);
    data[1] = ((receive_data[3]<<16 | receive_data[4]<<8 | receive_data[5]) & 0x03ffff);
	*output_data = data[0];
	*(output_data+1) = data[1];
}

unsigned short bsp_max30102_get_heart_rate
	(
	float 			*input_data,
	unsigned short	cache_nums
	)
{
	float input_data_sum_aver = 0;
	unsigned short i, temp = 0;
			
	for(i = 0; i < cache_nums; i++)
	{
		input_data_sum_aver += *(input_data+i);
	}
	input_data_sum_aver = input_data_sum_aver / cache_nums;
	for(i = 0; i < cache_nums; i++)
	{
		if((*(input_data+i) > input_data_sum_aver) && (*(input_data+i+1) < input_data_sum_aver))
		{
			temp = i;
			break;
		}
	}
	i++;
	for(; i<cache_nums; i++)
	{
		if((*(input_data+i) > input_data_sum_aver) && (*(input_data+i+1) < input_data_sum_aver))
		{
			temp = i - temp;
			break;
		}
	}
	if((temp>14) && (temp<100))
	{
		// SpO2采样频率200Hz，FIFO采样平均过滤值为4，所以最终采样频率算为50Hz
		// 采样一次的周期是0.02s，temp是一次心跳过程中的采样点个数，一次心跳就是temp/50秒,一分钟心跳数就是60/(temp/50)
		return 3000 / temp;
	}
	else
	{
		return 0;
	}
}

float bsp_max30102_get_spo2
	(
	float 			*ir_input_data,
	float 			*red_input_data,
	unsigned short	cache_nums
	)
{
	float ir_max = *ir_input_data, ir_min = *ir_input_data;
	float red_max = *red_input_data, red_min = *red_input_data;
	float R;
	unsigned short i;
	for(i = 1; i < cache_nums; i++)
	{
		if(ir_max < *(ir_input_data+i))
		{
			ir_max = *(ir_input_data+i);
		}
		if(ir_min > *(ir_input_data+i))
		{
			ir_min = *(ir_input_data+i);
		}
		if(red_max < *(red_input_data+i))
		{
			red_max = *(red_input_data+i);
		}
		if(red_min > *(red_input_data+i))
		{
			red_min = *(red_input_data+i);
		}
	}
	R = ((ir_max-ir_min) * red_min) / ((red_max-red_min) * ir_min);
	// R=((ir_max+ir_min)*(red_max-red_min))/((red_max+red_min)*(ir_max-ir_min));
	return ((-45.060)*R*R + 30.354*R + 94.845);
}
#endif

