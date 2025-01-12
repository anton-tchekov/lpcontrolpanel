#ifndef __MQTT_H__
#define __MQTT_H__

#include "types.h"
#include <MQTTClient.h>

int mqtt_connect(const char *addr, const char **topics, size_t count,
	int (*msgarrvd)(void *, char *, int, MQTTClient_message *));
void mqtt_publish(const char *topic, char *data, size_t len);

#endif
