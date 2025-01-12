#include "mqtt.h"
#include "log.h"
#include "util.h"
#include "alloc.h"
#include <unistd.h>
#include <stdlib.h>

#define CLIENTID    "ControlPanel"
#define QOS         0
#define TIMEOUT     10000L

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_deliveryToken token;

void mqtt_publish(const char *topic, char *data, size_t len)
{
	MQTTClient_message msg = MQTTClient_message_initializer;
	msg.qos = QOS;
	msg.retained = 0;
	msg.payload = data;
	msg.payloadlen = len;
	int res = MQTTClient_publishMessage(client, topic, &msg, &token);
	if(res != MQTTCLIENT_SUCCESS)
	{
		log_warn("MQTT publish failed: %s", MQTTClient_strerror(res));
		return;
	}

	log_info("Published `%.*s` on Topic `%s`", (int)len, data, topic);
}

static void connlost(void *context, char *cause)
{
	log_error("MQTT lost connection");
	(void)context, (void)cause;
}

static void delivered(void *context, MQTTClient_deliveryToken dt)
{
	(void)context, (void)dt;
}

int mqtt_connect(const char *addr, const char **topics, size_t count,
	int (*msgarrvd)(void *, char *, int, MQTTClient_message *))
{
	int res = MQTTClient_create(&client, addr, CLIENTID,
		MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if(res != MQTTCLIENT_SUCCESS)
	{
		log_error("MQTTClient creation failed: %s", MQTTClient_strerror(res));
		return 1;
	}

	res = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if(res != MQTTCLIENT_SUCCESS)
	{
		log_warn("Failed to set callbacks: %s", MQTTClient_strerror(res));
		return 1;
	}

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	res = MQTTClient_connect(client, &conn_opts);
	if(res != MQTTCLIENT_SUCCESS)
	{
		log_warn("MQTTClient connection failed: %s", MQTTClient_strerror(res));
		return 1;
	}

	int *qoss = _calloc(count, sizeof(int));
	res = MQTTClient_subscribeMany(client, count, (char * const *)topics, qoss);
	if(res != MQTTCLIENT_SUCCESS)
	{
		log_warn("MQTTClient failed to subscribe to topics: %s",
			MQTTClient_strerror(res));
		return 1;
	}

	log_debug("MQTTClient Connected");
	return 0;
}
