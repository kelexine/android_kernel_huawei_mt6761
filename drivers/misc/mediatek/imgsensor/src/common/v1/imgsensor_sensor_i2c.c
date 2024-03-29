

#include "imgsensor_common.h"
#include <linux/printk.h>
#include <linux/delay.h>
#include "kd_camera_typedef.h"
#include <linux/slab.h>
#include "imgsensor_sensor_i2c.h"

extern int iReadRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u8 *a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u16 i2cId);
extern void kdSetI2CSpeed(u16 i2cSpeed);
extern int iBurstWriteReg_multi(u8 *pData, u32 bytes, u16 i2cId,
				u16 transfer_length, u16 timing);

kal_int32 imgsensor_sensor_i2c_read(imgsensor_t *sensor,
				    kal_uint32 addr, kal_uint16 *data,
				    enum imgsensor_i2c_data_type data_type)
{
	kal_int32 rc = 0, len = 0;
	kal_uint8 buf[IMGSENSOR_I2C_DATA_MAX] = {0};
	kal_uint8 pu_send_cmd[IMGSENSOR_I2C_ADDR_MAX] = {0};

	pr_debug("Enter.\n");

	if (!data || !sensor) {
		pr_err("Fatal, Null ptr. data:%p, sensor:%p\n", data, sensor);
		return -EFAULT;
	}

	if ((sensor->addr_type != IMGSENSOR_I2C_BYTE_ADDR
	     && sensor->addr_type != IMGSENSOR_I2C_WORD_ADDR)
	    || (data_type != IMGSENSOR_I2C_BYTE_DATA
		&& data_type != IMGSENSOR_I2C_WORD_DATA)) {
		pr_err("Invalid addr_type=%d or data_type=%d\n", sensor->addr_type, data_type);
		return -EFAULT;
	}

	if (sensor->addr_type == IMGSENSOR_I2C_BYTE_ADDR)
		pu_send_cmd[len++] = (kal_uint8)addr;
	else {
		pu_send_cmd[len++] = (kal_uint8)(addr >> 8);
		pu_send_cmd[len++] = (kal_uint8)(addr & 0xFF);
	}

	rc = iReadRegI2C(pu_send_cmd, len, buf, data_type, sensor->i2c_write_id);
	if (rc < 0) {
		pr_err("i2c read failed. addr: 0x%x.\n", addr);
		return -EFAULT;
	}

	if (data_type == IMGSENSOR_I2C_BYTE_DATA)
		*data = buf[0];
	else
		*data = buf[0] << 8 | buf[1];

	pr_debug("addr: 0x%x, data: 0x%x\n", addr, *data);
	pr_debug("Exit.\n");
	return 0;
}

kal_int32 imgsensor_sensor_i2c_write(imgsensor_t *sensor,
				     kal_uint32 addr, kal_uint16 data,
				     enum imgsensor_i2c_data_type data_type)
{
	kal_int32 rc = 0, len = 0;
	kal_uint8 pu_send_cmd[IMGSENSOR_I2C_ADDR_MAX + IMGSENSOR_I2C_DATA_MAX] = {0};

	pr_debug("Enter.\n");

	if (!sensor) {
		pr_err("Fatal, Null ptr.\n");
		return -EFAULT;
	}

	if ((sensor->addr_type != IMGSENSOR_I2C_BYTE_ADDR
	     && sensor->addr_type != IMGSENSOR_I2C_WORD_ADDR)
	    || (data_type != IMGSENSOR_I2C_BYTE_DATA
		&& data_type != IMGSENSOR_I2C_WORD_DATA)) {
		pr_err("Invalid addr_type=%d or data_type=%d\n", sensor->addr_type, data_type);
		return -EFAULT;
	}

	if (sensor->addr_type == IMGSENSOR_I2C_BYTE_ADDR)
		pu_send_cmd[len++] = (u8)addr;
	else {
		pu_send_cmd[len++] = (u8)(addr >> 8);
		pu_send_cmd[len++] = (u8)(addr & 0xFF);
	}

	if (data_type == IMGSENSOR_I2C_BYTE_DATA)
		pu_send_cmd[len++] = (u8)data;
	else {
		pu_send_cmd[len++] = (u8)(data >> 8);
		pu_send_cmd[len++] = (u8)(data & 0xFF);
	}

	rc = iWriteRegI2C(pu_send_cmd, len, sensor->i2c_write_id);
	if (rc < 0) {
		pr_err("i2c write failed. addr: 0x%x data:0x%x.\n", addr, data);
		return -EFAULT;
	}
	pr_debug("Exit.\n");

	return 0;
}

kal_int32 imgsensor_sensor_i2c_poll(imgsensor_t *sensor,
				    kal_uint32 addr, kal_uint16 data,
				    enum imgsensor_i2c_data_type data_type, kal_uint16 delay)
{
	kal_int32 rc = 0, i = 0;
	kal_uint16 temp_data = 0;

	pr_debug("Enter.\n");
	if (!sensor) {
		pr_err("Fatal, Null ptr.\n");
		return -EFAULT;
	}

	for (i = 0; i < delay; i++) {
		rc = imgsensor_sensor_i2c_read(sensor, addr, &temp_data, data_type);
		pr_debug("i=%d, addr: 0x%x, data: 0x%x, delay:%d\n", i, addr, temp_data, delay);
		if (rc < 0) {
			pr_err("failed to read addr: 0x%x\n", addr);
			return -EFAULT;
		}
		if (temp_data == data) {
			pr_debug("poll sucess.\n");
			return 0;
		}
		usleep_range(1000, 1100);
	}

	pr_debug("poll failed.\n");
	return -EFAULT;
}

#define I2C_BUFFER_LEN 765 /* trans# max is 255, each 3 bytes */
kal_int32 imgsensor_sensor_write_table(imgsensor_t *sensor,
				       struct imgsensor_i2c_reg *setting, kal_uint32 size,
				       enum imgsensor_i2c_data_type data_type)
{
	kal_int32 rc = 0;
	kal_uint8 *pu_send_cmd = NULL;
	kal_uint32 tosend = 0, i = 0;
	kal_uint16 addr = 0, data = 0, delay = 0;
	kal_uint16 len_per_cycle = 0;

	pr_debug("Enter. size:%d\n", size);

	if (!setting || !sensor) {
		pr_err("Fatal.Null ptr. setting:%p, sensor:%p.\n", setting, sensor);
		return -EFAULT;
	}

	if ((sensor->addr_type != IMGSENSOR_I2C_BYTE_ADDR
	     && sensor->addr_type != IMGSENSOR_I2C_WORD_ADDR)
	    || (data_type != IMGSENSOR_I2C_BYTE_DATA
		&& data_type != IMGSENSOR_I2C_WORD_DATA)) {
		pr_err("Invalid addr_type=%d or data_type=%d\n", sensor->addr_type, data_type);
		return -EFAULT;
	}

	pu_send_cmd = kzalloc(I2C_BUFFER_LEN, GFP_KERNEL);
	if (!pu_send_cmd) {
		pr_err("Failed to alloc.\n");
		return -ENOMEM;
	}

	for (i = 0; i < size; i++) {
		len_per_cycle = 0;
		addr = setting[i].addr;
		data = setting[i].data;
		delay = setting[i].delay;
		//pr_debug("0x%x, 0x%x, 0x%x", addr, data, delay);

		if (sensor->addr_type == IMGSENSOR_I2C_BYTE_ADDR) {
			pu_send_cmd[tosend++] = (kal_uint8)addr;
			len_per_cycle += 1;
		} else {
			pu_send_cmd[tosend++] = (kal_uint8)(addr >> 8);
			pu_send_cmd[tosend++] = (kal_uint8)(addr & 0xFF);
			len_per_cycle += 2;//1word=2bytes
		}

		if (data_type == IMGSENSOR_I2C_BYTE_DATA) {
			pu_send_cmd[tosend++] = (kal_uint8)data;
			len_per_cycle += 1;
		} else {
			pu_send_cmd[tosend++] = (kal_uint8)(data >> 8);
			pu_send_cmd[tosend++] = (kal_uint8)(data & 0xFF);
			len_per_cycle += 2;//1word=2bytes
		}

		if ((I2C_BUFFER_LEN - tosend) < len_per_cycle ||
		    delay > 0 || (i + 1) == size) {

			pr_debug("begin send: tosend:%d, delay:%d i:%d size:%d\n", tosend, delay, i, size);
			rc = iBurstWriteReg_multi(pu_send_cmd, tosend,
						  sensor->i2c_write_id,
						  len_per_cycle, sensor->i2c_speed);
			if (rc < 0) {
				pr_err("write i2c failed. i=%d\n", i);
				return -EFAULT;
			}
			if (delay > 0)
				mdelay(delay);//TBD,mdelay will hang the cpu till delay complete.
			tosend = 0;
		}
	}

	kfree(pu_send_cmd);

	pr_debug("Exit. size:%d\n", size);

	return 0;
}

kal_int32 imgsensor_sensor_write_setting(imgsensor_t *sensor,
		imgsensor_i2c_reg_setting_t *settings)
{
	kal_int32 rc = 0;

	if (!sensor || !settings) {
		pr_err("Fatal. Null ptr. sensor:%p, settings:%p\n", sensor, settings);
		return -EFAULT;
	}

	rc = imgsensor_sensor_write_table(sensor, settings->setting, settings->size, settings->data_type);
	if (rc < 0) {
		pr_err("Failed.\n");
		return rc;
	}
	if (settings->delay > 0)
		mdelay(settings->delay);

	return 0;
}

kal_int32 imgsensor_sensor_i2c_process(imgsensor_t *sensor,
				       imgsensor_i2c_reg_table_array_t *settings)
{
	kal_int32 rc = 0, i = 0;
	enum imgsensor_i2c_operation i2c_operation;
	struct imgsensor_i2c_reg_table *setting = NULL;
	kal_uint16 temp_data = 0;

	if (!sensor || !settings) {
		pr_err("Fatal. Null ptr. sensor:%p, settings:%p\n", sensor, settings);
		return -EFAULT;
	}

	setting = settings->setting;
	for (i = 0; i < settings->size; i++) {
		i2c_operation = setting[i].i2c_operation;
		switch (i2c_operation) {
		case IMGSENSOR_I2C_READ:
			rc = imgsensor_sensor_i2c_read(sensor, setting[i].addr,
						       &temp_data, setting[i].data_type);
			if (rc < 0)
				pr_err("Read setting[%d].addr:0x%x, data:0x%x failed.\n", i, setting[i].addr, temp_data);
			else  //donnot decrease the print level used to dump sensor reg.
				pr_info("Read-addr:0x%x, data:0x%x\n", setting[i].addr, temp_data);
			break;
		case IMGSENSOR_I2C_WRITE:
			rc = imgsensor_sensor_i2c_write(sensor, setting[i].addr,
							setting[i].data, setting[i].data_type);
			if (rc < 0)
				pr_err("Write setting[%d].addr:0x%x, data:0x%x failed.\n", i, setting[i].addr, setting[i].data);
			else  //donnot decrease the print level used to dump sensor reg.
				pr_info("Write-addr:0x%x, data:0x%x\n", setting[i].addr, setting[i].data);
			break;
		case IMGSENSOR_I2C_POLL:
			rc = imgsensor_sensor_i2c_poll(sensor, setting[i].addr,
						       setting[i].data, setting[i].data_type, setting[i].delay);
			if (rc < 0)
				pr_err("Poll setting[%d].addr:0x%x, data:0x%x failed.\n", i, setting[i].addr, setting[i].data);
			else  //donnot decrease the print level used to dump sensor reg.
				pr_info("Poll-addr:0x%x, data:0x%x\n", setting[i].addr, setting[i].data);
			break;
		default:
			pr_err("Invalid i2c_operation.\n");
			break;
		}
	}

	return rc;
}
