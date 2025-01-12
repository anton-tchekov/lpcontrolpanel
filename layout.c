#include "layout.h"
#include "gfx.h"
#include "gui.h"
#include "colors.h"
#include "button.h"
#include "textbox.h"
#include "label.h"
#include "slider.h"
#include "lamp.h"
#include "mqtt.h"
#include <stdio.h>
#include <string.h>
#include "millis.h"
#include "util.h"
#include "log.h"

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/* Framerate Label */
static u64 last_ms;
static u32 frame_cnt;
static char str_framerate[64];
static Label label_framerate = { 0, -30, ALIGN_LEFT, str_framerate };

/* Connection */
static char buf_addr[64] = "127.0.0.1:1883";

static const char *topics[] =
{
	"lp/gesture"
};

static void connect(void)
{
	log_info("Connecting to MQTT on address: %s", buf_addr);
	mqtt_connect(buf_addr, topics, ARRLEN(topics), msgarrvd);
}

static Textbox tb_addr = { 0, -100, 300, 0, 0, 0, buf_addr, connect };
static Button button_connect = { 310, -100, 200, 38, "LP MQTT Connect", connect };

/* MQTT Toolbox */
static char buf_topic[64];
static char buf_value[64];

static void publish(void);

static Label lbl_topic = { -300, 50, ALIGN_LEFT, "Topic:" };
static Textbox tb_topic = { -300, 75, 290, 0, 0, 0, buf_topic, NULL };
static Label lbl_value = { -300, 125, ALIGN_LEFT, "Value:" };
static Textbox tb_value = { -300, 150, 290, 0, 0, 0, buf_value, publish };
static Button btn_publish = { -300, 200, 290, 38, "Publish", publish };

static void publish(void)
{
	mqtt_publish(buf_topic, buf_value, tb_value.Length);
}

/* Gesture Toolbox */
static char buf_send_gesture[64];
static char buf_recv_gesture[64] = "Last gesture: ???";

static void publish_gesture(void);

static Label lbl_lastgesture = { -300, 300, ALIGN_LEFT, buf_recv_gesture };
static Label lbl_sendgesture = { -300, 325, ALIGN_LEFT, "Send on `lp/gesture`:" };
static Textbox tb_gesture = { -300, 350, 290, 0, 0, 0, buf_send_gesture, publish_gesture };
static Button btn_gesture = { -300, 400, 290, 38, "Publish Gesture", publish_gesture };

static void publish_gesture(void)
{
	mqtt_publish("lp/gesture", buf_send_gesture, tb_gesture.Length);
}

/* Lamps */
static Lamp c_bedroom = {};
static Lamp c_lounge = {};
static Lamp c_dining = {};
static Lamp c_kitchen = {};
static Lamp d_all = {};

/* Curtains */

/* Heaters */


int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	if(topicLen != 0)
	{
		MQTTClient_freeMessage(&message);
		MQTTClient_free(topicName);
		return 1;
	}

	log_debug("Received message `%.*s` on topic `%s`",
		message->payloadlen, (char *)message->payload, topicName);

	if(!strcmp(topicName, "lp/gesture"))
	{
		snprintf(buf_recv_gesture, sizeof(buf_recv_gesture),
			"Last gesture: %.*s", message->payloadlen, (char *)message->payload);
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
	(void)context;
}

void layout_init(void)
{
	gui_element_add(ELEMENT_TYPE_LABEL, &label_framerate);

	tb_addr.Length = tb_addr.Selection = tb_addr.Position = strlen(buf_addr);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_addr);
	gui_element_add(ELEMENT_TYPE_BUTTON, &button_connect);

	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_topic);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_topic);
	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_value);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_value);
	gui_element_add(ELEMENT_TYPE_BUTTON, &btn_publish);

	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_lastgesture);
	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_sendgesture);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_gesture);
	gui_element_add(ELEMENT_TYPE_BUTTON, &btn_gesture);
}

void layout_render(void)
{
	++frame_cnt;

	u64 now = millis();
	if(now - last_ms > 1000)
	{
		last_ms = now;
		double frametime = 1000.0 / frame_cnt;
		snprintf(str_framerate, sizeof(str_framerate), "FPS: %d (%.2f ms)",
			frame_cnt, frametime);
		frame_cnt = 0;
	}

	gfx_bg();
	gui_render();
	log_render(0, -200);
}
