/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "i2c.h"
#include "osal_debug.h"
//#include "app_init.h"
#include "gpio.h"

#include "bsp_iic.h"
#include "bsp_max30102.h"
#include "watchdog.h"
#include <stdio.h>
#include "bsp_max30102_blood.h"
#include "algorithm.h"
#include "tcxo.h"

#define I2C_TASK_STACK_SIZE 0x5000
#define I2C_TASK_PRIO 17
#define I2C_MASTER_ADDR 0x0
#define I2C_SET_BANDRATE 400000//  400000 50000

#define GPIO_KEY GPIO_14

uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length;    //data length
uint32_t aun_red_buffer[500];    //Red LED sensor data
int32_t n_sp02; //SPO2 value
char ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
char ch_hr_valid;    //indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;

uint32_t un_min, un_max, un_prev_data;  
int i;
int32_t n_brightness;
float f_temp;
uint8_t temp_num=0;
uint8_t temo_buf[6];
uint8_t str[100];
uint8_t dis_hr=0,dis_spo2=0;

uint8_t f_dis_hr=0, f_dis_spo2=0;

#define MAX_BRIGHTNESS 255

void OledTask(void)
{
    printf("_______>>>>>>>>>>%s %d \r\n", __FILE__, __LINE__);
    printf("\r\n\r\n\r\ndemo 202_heart_rate_oximetry_sensor 250615\r\n\r\n\r\n");

    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    bsp_iic_master_init(16, 15, baudrate, hscode);

    uapi_pin_set_mode(GPIO_KEY, PIN_MODE_0);
    uapi_gpio_set_dir(GPIO_KEY, GPIO_DIRECTION_INPUT);

    if (bsp_max30102_init() == 0)
    {
         printf("bsp_max30102_init success!\r\n");
    }

    uint8_t intkey_val = 0;

    un_min=0x3FFFF;
	un_max=0;
	
	n_ir_buffer_length=500; //buffer length of 100 stores 5 seconds of samples running at 100sps
	//read the first 500 samples, and determine the signal range
    for(i=0;i<n_ir_buffer_length;i++)
    {
        while(1)   //wait until the interrupt pin asserts
        {
            uapi_watchdog_kick(); // 喂狗，防止程序出现异常系统挂死
            intkey_val = uapi_gpio_get_val(GPIO_KEY);//IoTGpioGetInputVal(5, &intkey_val);
            if(intkey_val == 0)
            {
                break;
            }
            osal_udelay(100);
            printf("intkey_val: %d\r\n", intkey_val);
        }
        
		// bsp_max30102_read_data(REG_FIFO_DATA, temo_buf, 6);
		// aun_red_buffer[i] =  (long)((long)((long)temo_buf[0]&0x03)<<16) | (long)temo_buf[1]<<8 | (long)temo_buf[2];    // Combine values to get the actual number
		// aun_ir_buffer[i] = (long)((long)((long)temo_buf[3] & 0x03)<<16) |(long)temo_buf[4]<<8 | (long)temo_buf[5];   // Combine values to get the actual number
            
        bsp_max30102_read_data(0x05, temo_buf, 4);
        aun_red_buffer[i] = ((temo_buf[0] << 8) | temo_buf[1]);
        aun_ir_buffer[i]  = ((temo_buf[2] << 8) | temo_buf[3]);

        if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];    //update signal min
        if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];    //update signal max
    }
	un_prev_data=aun_red_buffer[i];
	//calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
	
    printf("______>>>>>>> %s %d  un_min = %d un_max = %d\r\n", __FILE__, __LINE__, un_min, un_max);

    while (1)
    {
        uapi_watchdog_kick(); // 喂狗，防止程序出现异常系统挂死

        i=0;
        un_min=0x3FFFF;
        un_max=0;
		
		//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
        for(i=100;i<500;i++)
        {
            aun_red_buffer[i-100]=aun_red_buffer[i];
            aun_ir_buffer[i-100]=aun_ir_buffer[i];
            
            //update the signal min and max
            if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];
            if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];
        }

		//take 100 sets of samples before calculating the heart rate.
        for(i=400;i<500;i++)
        {
            un_prev_data=aun_red_buffer[i-1];
            while(1)   //wait until the interrupt pin asserts
            {
                intkey_val = uapi_gpio_get_val(GPIO_KEY);//IoTGpioGetInputVal(5, &intkey_val);
                if(intkey_val == 0)
                {
                    break;
                }
                osal_udelay(10);
            }
            // bsp_max30102_read_data(REG_FIFO_DATA,temo_buf, 6);
            // aun_red_buffer[i] =  (long)((long)((long)temo_buf[0]&0x03)<<16) | (long)temo_buf[1]<<8 | (long)temo_buf[2];    // Combine values to get the actual number
			// aun_ir_buffer[i] = (long)((long)((long)temo_buf[3] & 0x03)<<16) |(long)temo_buf[4]<<8 | (long)temo_buf[5];   // Combine values to get the actual number

            bsp_max30102_read_data(0x05, temo_buf, 4);
            aun_red_buffer[i] = ((temo_buf[0] << 8) | temo_buf[1]);
            aun_ir_buffer[i]  = ((temo_buf[2] << 8) | temo_buf[3]);

            if(aun_red_buffer[i]>un_prev_data)
            {
                f_temp=aun_red_buffer[i]-un_prev_data;
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness-=(int)f_temp;
                if(n_brightness<0)
                    n_brightness=0;
            }
            else
            {
                f_temp=un_prev_data-aun_red_buffer[i];
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness+=(int)f_temp;
                if(n_brightness>MAX_BRIGHTNESS)
                    n_brightness=MAX_BRIGHTNESS;
            }
			//send samples and calculation result to terminal program through UART
			if(ch_hr_valid == 1 && n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
			{
				dis_hr = n_heart_rate;
				dis_spo2 = n_sp02;
			}
			else
			{
				dis_hr = 0;
				dis_spo2 = 0;
			}
            if (n_heart_rate > 45 && n_heart_rate <180)
            {
                /* code */
                printf(">>HR=%i, ", n_heart_rate); 
			    printf("HRvalid=%i, ", ch_hr_valid);
                printf("SpO2=%i, ", n_sp02);
			    printf("SPO2Valid=%i\r\n", ch_spo2_valid);
            }
            
		}
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
		
        //printf("______>>>>>>> %s %d  dis_hr = %d dis_spo2 = %d\r\n", __FILE__, __LINE__, dis_hr, dis_spo2);

		if(dis_hr == 0 && dis_spo2 == 0)  //**dis_hr == 0 && dis_spo2 == 0
		{
			sprintf((char *)str,"HR:--- SpO2:--- ");//**HR:--- SpO2:--- 
		}
		else{
			sprintf((char *)str,"HR:%3d SpO2:%3d ",dis_hr,dis_spo2);//**HR:%3d SpO2:%3d 

            f_dis_hr = dis_hr;
            f_dis_spo2 = dis_spo2;

            printf("%s<<<\r\n", str); 
		}
		
        //printf("%s\r\n", str); 

        osal_udelay(10000);
        //uapi_tcxo_delay_ms(10);
    }
}

void max30102_entry(void)
{
    uint32_t ret;
    osal_task *taskid;

    printf("_______>>>>>>>>>>%s %d \r\n", __FILE__, __LINE__);

    // 创建任务调度
    osal_kthread_lock();
    printf("_______>>>>>>>>>>%s %d \r\n", __FILE__, __LINE__);
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)OledTask, NULL, "OledTask", I2C_TASK_STACK_SIZE);
    printf("_______>>>>>>>>>>%s %d \r\n", __FILE__, __LINE__);
    ret = osal_kthread_set_priority(taskid, I2C_TASK_PRIO);

    printf("_______>>>>>>>>>>%s %d \r\n", __FILE__, __LINE__);

    if (ret != OSAL_SUCCESS) {
        printf("_______>>>>>>>>>>%s %d \r\n", __FILE__, __LINE__);
        printf("create task1 failed .\n");
    }
    printf("_______>>>>>>>>>>%s %d \r\n", __FILE__, __LINE__);
    osal_kthread_unlock();
}
