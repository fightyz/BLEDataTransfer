/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

#include "ble_lbs.h"
#include <string.h>
#include <stdlib.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_lbs       LED Button Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_lbs_t * p_lbs, ble_evt_t * p_ble_evt)
{
    p_lbs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_lbs       LED Button Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_lbs_t * p_lbs, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_lbs->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_lbs       LED Button Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_lbs_t * p_lbs, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    if ((p_evt_write->handle == p_lbs->led_char_handles.value_handle) &&
        (p_evt_write->len == 1) &&
        (p_lbs->led_write_handler != NULL))
    {
        p_lbs->led_write_handler(p_lbs, p_evt_write->data[0]);
    }
		
		if ((p_evt_write->handle == p_lbs->apdu_char_handles.value_handle) &&
				(p_lbs->led_write_handler != NULL))
		{
				p_lbs->apdu_write_handler(p_lbs, p_evt_write->data);
		}
}
extern uint8_t *s_apdu;
static void on_indicate(ble_lbs_t * p_lbs, ble_evt_t * p_ble_evt)
{
//		ble_lbs_apdu_button_change(p_lbs, s_apdu);
		p_lbs->led_write_handler(p_lbs, 0x01);
}

void ble_lbs_on_ble_evt(ble_lbs_t * p_lbs, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_lbs, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_lbs, p_ble_evt);
            break;
            
        case BLE_GATTS_EVT_WRITE:
            on_write(p_lbs, p_ble_evt);
            break;
				
				case BLE_GATTS_EVT_HVC:
						on_indicate(p_lbs, p_ble_evt);
						break;
            
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for adding the LED characteristic.
 *
 */
static uint32_t led_char_add(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = LBS_UUID_LED_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_lbs->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_lbs->led_char_handles);
}

static uint32_t apdu_char_add(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
		ble_gatts_char_md_t char_md;
		ble_gatts_attr_md_t cccd_md;
		ble_gatts_attr_t 		attr_char_value;
		ble_uuid_t					ble_uuid;
		ble_gatts_attr_md_t attr_md;
	
		memset(&cccd_md, 0, sizeof(char_md));
		
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
		cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	
		memset(&char_md, 0, sizeof(char_md));
	
		char_md.char_props.read		= 1;
		char_md.char_props.write	= 1;
//		char_md.char_props.notify	= 1;
		char_md.char_props.indicate = 1;
		char_md.p_char_user_desc	= NULL;
		char_md.p_char_pf					= NULL;
		char_md.p_user_desc_md		= NULL;
		char_md.p_cccd_md					= &cccd_md;
		char_md.p_sccd_md					= NULL;
		
		ble_uuid.type = p_lbs->uuid_type;
		ble_uuid.uuid = LBS_UUID_APDU_CHAR;
		
		memset(&attr_md, 0, sizeof(attr_md));
		
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
		attr_md.vloc				= BLE_GATTS_VLOC_STACK;
		attr_md.rd_auth			= 0;
		attr_md.wr_auth			= 0;
		attr_md.vlen				= 0;
		
		memset(&attr_char_value, 0, sizeof(attr_char_value));
		
		attr_char_value.p_uuid			= &ble_uuid;
		attr_char_value.p_attr_md		= &attr_md;
		attr_char_value.init_len		 = 300;
    attr_char_value.init_offs    = 0;
		attr_char_value.max_len		 = 300;
    attr_char_value.p_value      = NULL;
		
		return sd_ble_gatts_characteristic_add(p_lbs->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_lbs->apdu_char_handles);
}

/**@brief Function for adding the Button characteristic.
 *
 */
static uint32_t button_char_add(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = LBS_UUID_BUTTON_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
//  attr_char_value.init_len     = sizeof(uint8_t);
		attr_char_value.init_len		 = 300;
    attr_char_value.init_offs    = 0;
//  attr_char_value.max_len      = sizeof(uint8_t);
		attr_char_value.max_len		 = 300;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_lbs->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_lbs->button_char_handles);
}

uint32_t ble_lbs_init(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_lbs->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_lbs->led_write_handler = p_lbs_init->led_write_handler;
		p_lbs->apdu_write_handler = p_lbs_init->apdu_write_handler;
    
    // Add service
    ble_uuid128_t base_uuid = LBS_UUID_BASE;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_lbs->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = LBS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_lbs->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code = button_char_add(p_lbs, p_lbs_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code = led_char_add(p_lbs, p_lbs_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		
		err_code = apdu_char_add(p_lbs, p_lbs_init);
		if (err_code != NRF_SUCCESS)
		{
				return err_code;
		}
    
    return NRF_SUCCESS;
}

uint32_t ble_lbs_on_button_change(ble_lbs_t * p_lbs, uint8_t button_state)
{
		int size = 100;
    ble_gatts_hvx_params_t params;
//  uint16_t len = sizeof(button_state);
		uint8_t * data;
		data = (uint8_t*)malloc(size * sizeof(uint8_t));
		memset(data, 0, sizeof(size * sizeof(uint8_t)));
		uint16_t len = size * sizeof(uint8_t);
		
		for(int i = 0; i < size; i++)
		{
			data[i] = i;
		}
    
    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_lbs->button_char_handles.value_handle;
    params.p_data = data;
    params.p_len = &len;
    
    return sd_ble_gatts_hvx(p_lbs->conn_handle, &params);
}

uint8_t split_apdu(uint8_t * apdu)
{
		
}

uint32_t ble_lbs_apdu_button_change(ble_lbs_t * p_lbs, uint8_t * apdu)
{
	/*
		int size = 300;
		ble_gatts_hvx_params_t params;
		uint8_t * data;
		data = (uint8_t*)malloc(size * sizeof(uint8_t));
		memset(data, 0, sizeof(size * sizeof(uint8_t)));
		uint16_t len = size * sizeof(uint8_t);
	
		for(int i = 0; i < size; i++)
		{
			data[i] = i;
		}
		
		memset(&params, 0, sizeof(params));
		params.type = BLE_GATT_HVX_NOTIFICATION;
		params.handle = p_lbs->apdu_char_handles.value_handle;
		params.p_data = data;
		params.p_len = &len;
		
		return sd_ble_gatts_hvx(p_lbs->conn_handle, &params);
	*/
	/*
		int len = *apdu;
		uint16_t size = len + 1;
		ble_gatts_hvx_params_t params;
		uint8_t * data;
		data = (uint8_t *)malloc(len * sizeof(uint8_t));
		memset(data, 0, sizeof(len * sizeof(uint8_t)));
		for(int i = 0; i < len+1; i++)
		{
				data[i] = apdu[i];
		}
		
		memset(&params, 0, sizeof(params));
		params.type = BLE_GATT_HVX_NOTIFICATION;
		params.handle = p_lbs->apdu_char_handles.value_handle;
		params.p_data = data;
		params.p_len = &size;
		
		return sd_ble_gatts_hvx(p_lbs->conn_handle, &params);
	*/

		uint16_t len = 20;
		ble_gatts_hvx_params_t params;
		uint8_t * data;
		data = (uint8_t *)malloc(len * sizeof(uint8_t));
		memset(data, 0, sizeof(len * sizeof(uint8_t)));
		for(int i = 0; i < len; i++)
		{
				//data[i] = apdu[i];
			data[i] = i;
		}
		
		memset(&params, 0, sizeof(params));
//		params.type = BLE_GATT_HVX_NOTIFICATION;
		params.type = BLE_GATT_HVX_INDICATION;
		params.handle = p_lbs->apdu_char_handles.value_handle;
		params.p_data = data;
		params.p_len = &len;
		//p_lbs->led_write_handler(p_lbs, 0x01);
		
		return sd_ble_gatts_hvx(p_lbs->conn_handle, &params);
}
