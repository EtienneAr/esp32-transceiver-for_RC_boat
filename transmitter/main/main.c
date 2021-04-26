#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"

#include "direct_wifi.h"
#include "screen_handler.h"
#include "inputs.h"
#include "wifi_datagram.h"
#include "myleds.h"

#include <math.h>

#define WIFI_TIMEOUT 10000000

static const char *MAIN_TAG = "RC-Test main";

typedef enum CurveType {
    LINEAR = 0,
    X_2 = 1,
    X_3 = 2,
    X_4 = 3,
    X_8 = 4,
    CURVE_MAX_TYPE = 5,
} CurveType_t;

static CurveType_t curveControl;
static bool isLogButtonPressed = false;


typedef enum AccType {
    UNLIMITED = 0,
    LIMITED = 1,
    ACC_MAX_TYPE = 2,
} AccType_t;

static AccType_t accControl;
static bool isAccButtonPressed = false;

float applyCurve(CurveType_t curve, float in) {
    float out = in / 1000.;
    int exponent = 0;
    switch(curve) {
        case LINEAR:
            exponent = 1;
            break;
        case X_2:
            exponent = 2;
            break;
        case X_3:
            exponent = 3;
            break;
        case X_4:
            exponent = 4;
            break;
        case X_8:
            exponent = 8;
            break;
        default:
            exponent = 1;
            break;
    }
    out = 1000. * pow(out, exponent);

    if(in < 0 && out > 0)
        out *= -1;
    
    return (int) out;
}

int applyAccLim(AccType_t acc, int in) {
    switch(acc) {
        case UNLIMITED:
            return 1000;
        case LIMITED:
            return in;
        default:
            return in;
    }
}


static void wifi_recv_cb(uint8_t src_mac[6], uint8_t *data, int len) {
    if(len != sizeof(int))
            return;

    int signal_quality = *((int*) data);

    if(signal_quality <=1000 && signal_quality >= 0)
        myleds_display_rg(signal_quality/10);
}

void app_main(void)
{
    myleds_init();
    myleds_allRed();

    inputs_init(true); //auto trim
    wifi_init();

    wifi_attach_recv_cb(wifi_recv_cb);

    myleds_allOff();

    wifi_datagram_t data;
    data.cnt = 0;

    while(true) {
        /* Data to send */
        data.cnt++;
        data.speed = (int) applyCurve(curveControl, inputs_readJoyA() * 2000 / INPUTS_VALUEMAX - 1000);
        data.dir   = (int)                          inputs_readJoyB() * 2000 / INPUTS_VALUEMAX - 1000 ;
        data.limit_speed =                          inputs_readPotA() * 1000 / INPUTS_VALUEMAX ;
        data.limit_acc   =  applyAccLim(accControl, inputs_readPotB() * 1000 / INPUTS_VALUEMAX);

        /* Speed curve control */
        if(inputs_readButtonA()) {
            if(!isLogButtonPressed) {
                isLogButtonPressed = true;
                curveControl = (curveControl+1) % CURVE_MAX_TYPE;
            }
        } else {
            isLogButtonPressed = false;
        }

        /* Acc limit control */
        if(inputs_readButtonB()) {
            if(!isAccButtonPressed) {
                isAccButtonPressed = true;
                accControl = (accControl+1) % ACC_MAX_TYPE;
            }
        } else {
            isAccButtonPressed = false;
        }


        /* Send and display */
        myleds_display_A(curveControl);
        myleds_display_B(accControl*4);

        wifi_datagram_print(&data);

        wifi_send_data(&data, sizeof(wifi_datagram_t));

        /* Wait */
        vTaskDelay(1 + 1/portTICK_PERIOD_MS);
    }
}