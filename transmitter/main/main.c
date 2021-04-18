#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"

#include "direct_wifi.h"
#include "screen_handler.h"
#include "sticks.h"

#include <math.h>

#define PLOT_FAST_PERIOD 100000
#define PLOT_FULL_PERIOD 3000000

#define MAX_NB_POINTS 1000
#define SCREEN_MARGIN 10

#define WIFI_TIMEOUT 10000000

static const char *MAIN_TAG = "RC-Test main";

static FontxFile myfont[2];
static char mytext[40];

struct GPS_data {
    int GPS_count;
    bool isLost;
    unsigned long time;
    float lat, lon;
    float head;
};

unsigned int prev_GPS_count = -1;

static float o_x=0, o_y=0, c_x=240, c_y=240;
static float min_x=1000, min_y=1000, max_x=-1000, max_y=-1000;
static unsigned long plot_period_count = 0;

unsigned long last_receive = 0;

typedef struct point {
    float x, y, theta;
    bool isGpsLost;
    bool isWiFiLost;
};

static struct point point_list[MAX_NB_POINTS];
static int n_points_last = 0;
static int n_points = 0;


static void add_point(struct GPS_data pos) {
    /* Check if the point is valid */
    if(pos.isLost) {
        if(n_points > 0) point_list[n_points-1].isGpsLost = true;
        return;
    }

    /* Rescale the window if necessary */
    if(pos.lon < min_x) {
        n_points_last = 0;
        min_x = pos.lon;
    }
    if(pos.lat < min_y) {
        n_points_last = 0;
        min_y = pos.lat;
    }
    if(pos.lon > max_x) {
        n_points_last = 0;
        max_x = pos.lon;
    }
    if(pos.lat > max_y) {
        n_points_last = 0;
        max_y = pos.lat;
    }

    if(n_points_last == 0) {
        o_x = min_x;
        o_y = min_y;

        if(max_x != min_x) c_x = (CONFIG_WIDTH - 2*SCREEN_MARGIN) / (float) (max_x - min_x);
        if(max_y != min_y) c_y = (CONFIG_HEIGHT- 2*SCREEN_MARGIN) / (float) (max_y - min_y);

        if(c_y > c_x) c_y = c_x;
        else c_x = c_y;
    }

    /* add the point to the list */
    point_list[n_points].x = pos.lon;
    point_list[n_points].y = pos.lat;
    point_list[n_points].theta = pos.head;
    point_list[n_points].isWiFiLost = false;
    n_points++;
}

static void wifi_recv_cb(uint8_t src_mac[6], uint8_t *data, int len) {
    if(len != sizeof(struct GPS_data)) {
        printf("Wrong len !\n");
        return;
    }

    last_receive = esp_timer_get_time();

    struct GPS_data *my_pos = data;

    printf("Count = %d, IsLost = %s, Time = %ld, Lat = %f, long = %f, heading = %f\n", my_pos->GPS_count, my_pos->isLost ? "true" : "false", my_pos->time, my_pos->lat, my_pos->lon, my_pos->head);


    if(my_pos->GPS_count != prev_GPS_count) {
        prev_GPS_count = my_pos->GPS_count;
        add_point(*my_pos);
    }
}

static void periodic_timer_callback(void* arg) {
    plot_period_count += PLOT_FAST_PERIOD;
    if(plot_period_count >= PLOT_FULL_PERIOD) {
        n_points_last = 0; //Replot all
        plot_period_count = 0;
    }

    if(esp_timer_get_time() - last_receive > WIFI_TIMEOUT) {
        if(n_points > 0) n_points_last = n_points-1;
        if(n_points > 0) point_list[n_points-1].isWiFiLost = true;
    }


    if(n_points_last == 0) mylcd_FillScreen(BLACK);

    uint16_t color;
    int x;
    int y;

    if(n_points_last > 0) n_points_last--;

    for(int i=n_points_last;i<n_points;i++) {
        color = point_list[i].isWiFiLost ? RED : (point_list[i].isGpsLost ? YELLOW : CYAN);
        x =                 (SCREEN_MARGIN + (point_list[i].x-o_x)*c_x);
        y = CONFIG_HEIGHT - (SCREEN_MARGIN + (point_list[i].y-o_y)*c_y);

        if(true || color != CYAN || i == n_points - 1) {
            mylcd_DrawFillArrow(x, y, x+8*cos(point_list[i].theta), y+8*sin(point_list[i].theta), 3, color);
        } else {
            mylcd_DrawFillCircle(x, y, 1, color);
        }
    }
    n_points_last = n_points;
}

void app_main(void)
{
    /*
    init_screen_handler();
    InitFontx(myfont,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic

    wifi_init();
    wifi_attach_recv_cb(wifi_recv_cb);

    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            .name = "spi_send"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, PLOT_FAST_PERIOD));
    */

    sticks_init(true); //auto trim
    wifi_init();

    int data[2];

    while(true) {
        data[0] = sticks_readA() * 200 / STICKS_VALUEMAX - 99;
        data[1] = sticks_readB() * 200 / STICKS_VALUEMAX - 99;
        wifi_send_data(data, 2*sizeof(int));
        vTaskDelay(1 + 1/portTICK_PERIOD_MS);
    }
}