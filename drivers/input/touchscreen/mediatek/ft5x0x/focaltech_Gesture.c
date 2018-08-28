/*
 *
 * FocalTech TouchScreen driver.
 * 
 * Copyright (c) 2010-2015, Focaltech Ltd. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

 /*******************************************************************************
*
* File Name: Focaltech_Gestrue.c
*
* Author: Xu YongFeng
*
* Created: 2015-01-29
*   
* Modify by mshl on 2015-03-20
*
* Abstract:
*
* Reference:
*
*******************************************************************************/

/*******************************************************************************
* 1.Included header files
*******************************************************************************/
#include "focaltech_core.h"
#if FTS_GESTRUE_EN
#include "ft_gesture_lib.h"
/*******************************************************************************
* Private constant and macro definitions using #define
*******************************************************************************/
#define GESTURE_LEFT								0x20
#define GESTURE_RIGHT								0x21
#define GESTURE_UP		    							0x22
#define GESTURE_DOWN								0x23
#define GESTURE_DOUBLECLICK						0x24
#define GESTURE_O		    							0x30
#define GESTURE_W		    							0x31
#define GESTURE_M		   	 						0x32
#define GESTURE_E		    							0x33
#define GESTURE_C		                                0x34
#define GESTURE_L		    							0x44
#define GESTURE_S		    							0x46
#define GESTURE_V		    							0x54
#define GESTURE_Z		    							0x41
#define FTS_GESTRUE_POINTS 						255
#define FTS_GESTRUE_POINTS_ONETIME  				62
#define FTS_GESTRUE_POINTS_HEADER 				8
#define FTS_GESTURE_OUTPUT_ADRESS 				0xD3
#define FTS_GESTURE_OUTPUT_UNIT_LENGTH 			4

/*******************************************************************************
* Private enumerations, structures and unions using typedef
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/
short pointnum 					= 0;
unsigned short coordinate_x[150] 	= {0};
unsigned short coordinate_y[150] 	= {0};

/*******************************************************************************
* Global variable or extern global variabls/functions
*******************************************************************************/
extern struct i2c_client *i2c_client;
extern unsigned int ft_touch_irq;
/*******************************************************************************
* Static function prototypes
*******************************************************************************/

int _is_open_gesture_mode = 0;

static ssize_t focaltech_gesture_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	ssize_t num_read_chars = 0;

	num_read_chars = snprintf(buf, 128, "%d\n", _is_open_gesture_mode);

	return num_read_chars;
}

static ssize_t focaltech_gesture_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{
	if(count == 0)
		return;

    disable_irq(ft_touch_irq);

	if(buf[0] == '1' && _is_open_gesture_mode == 0){
		_is_open_gesture_mode = 1;
	}
	if(buf[0] == '0' && _is_open_gesture_mode == 1){
		_is_open_gesture_mode = 0;
	}


    enable_irq(ft_touch_irq);
    return count;
}

static DEVICE_ATTR(gesture, 0664, focaltech_gesture_show, focaltech_gesture_store);

static ssize_t focaltech_coordinate_x_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	ssize_t num_read_chars = 0, count =0;
	int i;
	for (i=0; i< pointnum; i++)
	{
		count = sprintf(buf, "%d ", coordinate_x[i]);
		buf += count;
		num_read_chars += count;
	}

	return num_read_chars;
}

static DEVICE_ATTR(coordinate_x, 0664, focaltech_coordinate_x_show, NULL);

static ssize_t focaltech_coordinate_y_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	ssize_t num_read_chars = 0, count =0;
	int i;
	for (i=0; i< pointnum; i++)
	{
		count = sprintf(buf, "%d ", coordinate_y[i]);
		buf += count;
		num_read_chars += count;
	}

	return num_read_chars;
}

static DEVICE_ATTR(coordinate_y, 0664, focaltech_coordinate_y_show, NULL);


static struct attribute *focaltech_attributes[] = {
	&dev_attr_gesture.attr,
	&dev_attr_coordinate_x.attr,
	&dev_attr_coordinate_y.attr,
    NULL
};

static struct attribute_group focaltech_attribute_group = {
    .attrs = focaltech_attributes
};
/*******************************************************************************
*   Name: fts_Gesture_init
*  Brief:
*  Input:
* Output: None
* Return: None
*******************************************************************************/
int fts_Gesture_init(struct input_dev *input_dev)
{
    int ret;
    
    //init_para(480,854,60,0,0);
	input_set_capability(input_dev, EV_KEY, KEY_POWER);
	input_set_capability(input_dev, EV_KEY, KEY_U); 
	input_set_capability(input_dev, EV_KEY, KEY_UP); 
	input_set_capability(input_dev, EV_KEY, KEY_DOWN);
	input_set_capability(input_dev, EV_KEY, KEY_LEFT); 
	input_set_capability(input_dev, EV_KEY, KEY_RIGHT); 
	input_set_capability(input_dev, EV_KEY, KEY_O);
	input_set_capability(input_dev, EV_KEY, KEY_E); 
	input_set_capability(input_dev, EV_KEY, KEY_M); 
	input_set_capability(input_dev, EV_KEY, KEY_L);
	input_set_capability(input_dev, EV_KEY, KEY_W);
	input_set_capability(input_dev, EV_KEY, KEY_S); 
	input_set_capability(input_dev, EV_KEY, KEY_V);
	input_set_capability(input_dev, EV_KEY, KEY_Z);
	input_set_capability(input_dev, EV_KEY, KEY_C);
		
	__set_bit(KEY_RIGHT, input_dev->keybit);
	__set_bit(KEY_LEFT, input_dev->keybit);
	__set_bit(KEY_UP, input_dev->keybit);
	__set_bit(KEY_DOWN, input_dev->keybit);
	__set_bit(KEY_U, input_dev->keybit);
	__set_bit(KEY_O, input_dev->keybit);
	__set_bit(KEY_E, input_dev->keybit);
	__set_bit(KEY_M, input_dev->keybit);
	__set_bit(KEY_W, input_dev->keybit);
	__set_bit(KEY_L, input_dev->keybit);
	__set_bit(KEY_S, input_dev->keybit);
	__set_bit(KEY_V, input_dev->keybit);
	__set_bit(KEY_Z, input_dev->keybit);
	__set_bit(KEY_C, input_dev->keybit);

    
    if ((ret = sysfs_create_group(&i2c_client->dev.kobj, &focaltech_attribute_group))) {
        printk("create attr fail\n");
        return -1;
    }
    
	return 0;
}

/*******************************************************************************
*   Name: fts_check_gesture
*  Brief:
*  Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_check_gesture(struct input_dev *input_dev,int gesture_id)
{
	printk("fts gesture_id==0x%x\n ",gesture_id);

	switch(gesture_id)
	{
	        case GESTURE_LEFT:
	                input_report_key(input_dev, KEY_LEFT, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_LEFT, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_RIGHT:
	                input_report_key(input_dev, KEY_RIGHT, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_RIGHT, 0);
	                input_sync(input_dev);
			    break;
	        case GESTURE_UP:
	                input_report_key(input_dev, KEY_UP, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_UP, 0);
	                input_sync(input_dev);                                  
	                break;
	        case GESTURE_DOWN:
	                input_report_key(input_dev, KEY_DOWN, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_DOWN, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_DOUBLECLICK:
	                input_report_key(input_dev, KEY_U, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_U, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_O:
	                input_report_key(input_dev, KEY_O, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_O, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_W:
	                input_report_key(input_dev, KEY_W, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_W, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_M:
	                input_report_key(input_dev, KEY_M, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_M, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_E:
	                input_report_key(input_dev, KEY_E, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_E, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_L:
	                input_report_key(input_dev, KEY_L, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_L, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_S:
	                input_report_key(input_dev, KEY_S, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_S, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_V:
	                input_report_key(input_dev, KEY_V, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_V, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_Z:
	                input_report_key(input_dev, KEY_Z, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_Z, 0);
	                input_sync(input_dev);
	                break;
	        case GESTURE_C:
	                input_report_key(input_dev, KEY_C, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_C, 0);
	                input_sync(input_dev);
	                break;                    
	        default:
	                break;
	}

}

 /************************************************************************
*   Name: fts_read_Gestruedata
* Brief: read data from TP register
* Input: no
* Output: no
* Return: fail <0
***********************************************************************/
int fts_read_Gestruedata(void)
{
    unsigned char buf[FTS_GESTRUE_POINTS * 3] = { 0 };
    int ret = -1;
    int i = 0;
    int gestrue_id = 0;

    buf[0] = 0xd3;
    pointnum = 0;

    ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, FTS_GESTRUE_POINTS_HEADER);
        //printk( "tpd read FTS_GESTRUE_POINTS_HEADER.\n");

    if (ret < 0)
    {
        printk( "%s read touchdata failed.\n", __func__);
        return ret;
    }


	// other IC's gestrue in driver
	if (0x24 == buf[0])
	{
	    gestrue_id = 0x24;
	    fts_check_gesture(fts_input_dev,gestrue_id);
		printk( "%d check_gesture gestrue_id.\n", gestrue_id);
	    return -1;
	}

	pointnum = (short)(buf[1]) & 0xff;
	buf[0] = 0xd3;
	if((pointnum * 4 + 8)<255)
	{
		ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, (pointnum * 4 + 8));
	}
	else
	{
	     ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, 255);
	     ret = fts_i2c_read(fts_i2c_client, buf, 0, buf+255, (pointnum * 4 + 8) -255);
	}
	if (ret < 0)
	{
	    printk( "%s read touchdata failed.\n", __func__);
	    return ret;
	}

	//gestrue_id = fetch_object_sample(buf, pointnum);//need gesture lib.a
	gestrue_id = 0x24;
	fts_check_gesture(fts_input_dev,gestrue_id);
	printk( "%d read gestrue_id.\n", gestrue_id);

	for(i = 0;i < pointnum;i++)
	{
	    coordinate_x[i] =  (((s16) buf[0 + (4 * i+8)]) & 0x0F) <<
	        8 | (((s16) buf[1 + (4 * i+8)])& 0xFF);
	    coordinate_y[i] = (((s16) buf[2 + (4 * i+8)]) & 0x0F) <<
	        8 | (((s16) buf[3 + (4 * i+8)]) & 0xFF);
	}
	return -1;
}
#endif
