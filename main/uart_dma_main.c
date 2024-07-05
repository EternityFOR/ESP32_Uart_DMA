/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* UART DMA Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/uhci.h"
#include "esp_log.h"

#define EX_UART_NUM CONFIG_UART_PORT_NUM    //UART number attached to DMA
#define EX_UHCI_NUM CONFIG_UART_UHCI_PORT_NUM    //UHCI number
#define UART_BAUD_RATE CONFIG_UART_BAUD_RATE
#define UART_TX_IO CONFIG_UART_TX_IO
#define UART_RX_IO CONFIG_UART_RX_IO

static QueueHandle_t uhci_queue;  //UHCI event queue

static const char *TAG = "uhci-example";


static void build_data(uint8_t *buf, size_t size)
{
    static int test_cnt = 0;
    for (int i = 0; i < size; i++) {
        buf[i] = (test_cnt + i) & 0xff;
    }
}

static void prase_data(uint8_t *buf, size_t size)
{
    static int test_cnt = 0;
    for (int i = 0; i < size; i++) {
        if( buf[i] != ((test_cnt) & 0xff)) {
            ESP_LOGE(TAG, "Bytes check wrong. you are sending %x, but the real is %x\n", buf[i], test_cnt);
            return;
        } else {
            ESP_LOGI(TAG, "bytes correct, bingo~!");
        }
        test_cnt++;
    }
}

static void uhci_read(void *arg)
{
    uint8_t *pdata = malloc(1024 * 4);
    uint8_t *prd = pdata;
    size_t rx_size = 0;
    uhci_event_t uhci_evt;
    while(1) {
        if (xQueueReceive(uhci_queue, (void * )&uhci_evt, (portTickType)1000)) {
            if (uhci_evt.type & (UHCI_EVENT_EOF | UHCI_EVENT_DATA)) {
                if (uhci_evt.len) {
                    uhci_dma_read_bytes(EX_UHCI_NUM, prd, uhci_evt.len, (TickType_t)0);
                    prd += uhci_evt.len;
                    rx_size += uhci_evt.len;
                }
                if (uhci_evt.type & UHCI_EVENT_EOF) {
                    prase_data(pdata, rx_size);
                    prd = pdata;
                    rx_size = 0;
                }
            }
        }
    }
    vTaskDelete(NULL);
}

static void uhci_write(void *arg)
{
    uint8_t *pdata = malloc(1024 * 4);
    int i = 256;
    build_data(pdata, i);
    uhci_dma_write_bytes(EX_UHCI_NUM, pdata, i, 1);
    uart_wait_tx_idle_polling(UART_NUM_1);
    vTaskDelay(10/portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "uhci write buffer done!");
    vTaskDelete(NULL);
}

void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    //UART paramater config
    uart_param_config(EX_UART_NUM, &uart_config);
    uart_set_pin(EX_UART_NUM, UART_TX_IO, UART_RX_IO, -1, -1);
    //Idle timeout value to generate end of frame signal for RX DMA.
    uart_set_rx_idle_thr(EX_UART_NUM, 20);
    //Disable UART interrupt
    uart_disable_intr_mask(EX_UART_NUM, ~0);

    uhci_dma_config_t uhci_dma_cfg = UHCI_CONF_DEFAULT();
    //UHCI driver install
    uhci_driver_install(EX_UHCI_NUM, &uhci_dma_cfg, 1024 * 2, 20, &uhci_queue);

    ESP_LOGI(TAG, "UART-DMA(UHCI) example start, tx io:[%d], rx io:[%d], baud rate:[%d]", UART_TX_IO, UART_RX_IO, UART_BAUD_RATE);
    //set UHCI time eof mode
    uhci_set_rx_eof_mode(EX_UHCI_NUM, UHCI_RX_IDLE_EOF);
    //attach uart to DMA and start DMA.
    uhci_attach_uart_port_and_start(EX_UHCI_NUM, EX_UART_NUM);
    xTaskCreate(uhci_read, "uhci_read", 1024 * 4, NULL, 7, NULL);
    xTaskCreate(uhci_write, "uhci_write", 1024 * 4, NULL, 7, NULL);
}
