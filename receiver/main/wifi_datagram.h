#ifndef WIFI_DATAGRAM_H
#define WIFI_DATAGRAM_H

static const char *DATAGRAM_TAG = "wifi_datagram_t";

typedef struct __attribute__((__packed__)) 
{
	int speed;       // [-1000;1000]
	int dir;   		 // [-1000;1000]
	int limit_speed; // [0;1000]
} wifi_datagram_t;

void wifi_datagram_print(wifi_datagram_t *data) {
	ESP_LOGI(DATAGRAM_TAG, "Speed : %d \t|\t Dir : %d \t|\t Limit speed : %d", data->speed, data->dir, data->limit_speed);
}

#endif