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
#include <stdlib.h>
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

static char buf_bedroom_color[32];
static char buf_lounge_color[32];
static char buf_dining_color[32];
static char buf_kitchen_color[32];
static char buf_all_bright[32];

static Lamp *lamp_cur = NULL;

static void sel_bedroom(void);
static void sel_lounge(void);
static void sel_dining(void);
static void sel_kitchen(void);

static Lamp c_bedroom = { 550, 1350, COLOR_WHITE, "DMX Schlafzimmer", buf_bedroom_color, sel_bedroom };
static Lamp c_lounge = { 1900, 400, COLOR_WHITE, "DMX Lounge", buf_lounge_color, sel_lounge };
static Lamp c_dining = { 300, 300, COLOR_WHITE, "DMX Esszimmer", buf_dining_color, sel_dining };
static Lamp c_kitchen = { 900, 420, COLOR_WHITE, "DMX Kueche", buf_kitchen_color, sel_kitchen };
static Lamp d_all = { 800, 900, COLOR_WHITE, "Dali", buf_all_bright, NULL };

static void sel_bedroom(void)
{
	lamp_cur = &c_bedroom;
}

static void sel_lounge(void)
{
	lamp_cur = &c_lounge;
}

static void sel_dining(void)
{
	lamp_cur = &c_dining;
}

static void sel_kitchen(void)
{
	lamp_cur = &c_kitchen;
}

static char buf_red[16] = "128", buf_green[16] = "128", buf_blue[16] = "128";
static char buf_sel[64] = "Selected RGB Lamp: None";
static char buf_bright[16] = "50";

static void publish_color(void);
static void publish_brightness(void);

static void s_change_color(void);
static void t_change_color(void);

static Label lbl_sel = { 1750, 900, 0, buf_sel };
static Label lbl_red = { 1750, 925, 0, "Red:" };
static Slider sl_red = { 1750, 950, 255, 128, s_change_color };
static Textbox tb_red = { 1750 + 265, 950, 50, 0, 0, 0, buf_red, t_change_color };

static Label lbl_green = { 1750, 1000, 0, "Green:" };
static Slider sl_green = { 1750, 1025, 255, 128, s_change_color };
static Textbox tb_green = { 1750 + 265, 1025, 50, 0, 0, 0, buf_green, t_change_color };

static Label lbl_blue = { 1750, 1075, 0, "Blue:" };
static Slider sl_blue = { 1750, 1100, 255, 128, s_change_color };
static Textbox tb_blue = { 1750 + 265, 1100, 50, 0, 0, 0, buf_blue, t_change_color };

static Button btn_rgb = { 1750, 1150, 200, 38, "Set Color", publish_color };

static Label lbl_bright = { 1750, 1300, 0, "Brightness:" };
static Slider sl_bright = { 1750, 1325, 100, 50, s_change_color };
static Textbox tb_bright = { 1750 + 110, 1325, 50, 0, 0, 0, buf_bright, t_change_color };

static Button btn_bright = { 1750, 1375, 200, 38, "Set Brightness", publish_brightness };

static void publish_color(void)
{
	if(!lamp_cur)
	{
		return;
	}

	snprintf(lamp_cur->Bottom, 32, "[ %d, %d, %d ]",
		sl_red.Value, sl_green.Value, sl_blue.Value);
	
	// TODO
}

static void publish_brightness(void)
{
	// TODO

	snprintf(d_all.Bottom, 16, "[ %d ]",
		sl_bright.Value);
}

static void s_change_color(void)
{
	textbox_set(&tb_red, snprintf(buf_red, sizeof(buf_red), "%d", sl_red.Value));
	textbox_set(&tb_green, snprintf(buf_green, sizeof(buf_green), "%d", sl_green.Value));
	textbox_set(&tb_blue, snprintf(buf_blue, sizeof(buf_blue), "%d", sl_blue.Value));
	textbox_set(&tb_bright, snprintf(buf_bright, sizeof(buf_bright), "%d", sl_bright.Value));
}

static i32 pchannel(const char *b)
{
	i32 n = atoi(b);
	if(n < 0) { return 0; }
	if(n > 255) { return 255; }
	return n;
}

static i32 pchannel100(const char *b)
{
	i32 n = atoi(b);
	if(n < 0) { return 0; }
	if(n > 100) { return 100; }
	return n;
}

static void t_change_color(void)
{
	sl_red.Value = pchannel(buf_red);
	sl_green.Value = pchannel(buf_green);
	sl_blue.Value = pchannel(buf_blue);
	sl_bright.Value = pchannel100(buf_bright);
}

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

	textbox_set(&tb_addr, strlen(buf_addr));
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

	gui_element_add(ELEMENT_TYPE_LAMP, &c_bedroom);
	gui_element_add(ELEMENT_TYPE_LAMP, &c_lounge);
	gui_element_add(ELEMENT_TYPE_LAMP, &c_dining);
	gui_element_add(ELEMENT_TYPE_LAMP, &c_kitchen);
	gui_element_add(ELEMENT_TYPE_LAMP, &d_all);

	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_sel);
	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_red);
	gui_element_add(ELEMENT_TYPE_SLIDER, &sl_red);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_red);
	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_green);
	gui_element_add(ELEMENT_TYPE_SLIDER, &sl_green);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_green);
	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_blue);
	gui_element_add(ELEMENT_TYPE_SLIDER, &sl_blue);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_blue);

	gui_element_add(ELEMENT_TYPE_BUTTON, &btn_rgb);

	gui_element_add(ELEMENT_TYPE_LABEL, &lbl_bright);
	gui_element_add(ELEMENT_TYPE_SLIDER, &sl_bright);
	gui_element_add(ELEMENT_TYPE_TEXTBOX, &tb_bright);

	gui_element_add(ELEMENT_TYPE_BUTTON, &btn_bright);

	s_change_color();
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
