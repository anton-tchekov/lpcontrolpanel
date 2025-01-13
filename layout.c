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

typedef struct
{
	char *Buf;
	const char *Topic;
	char Set[16];
	char Status[16];
} CurtainTag;

CurtainTag *tag_vlounge, *tag_vflur, *tag_vbed;

void ct_format(CurtainTag *ct)
{
	snprintf(ct->Buf, 256, "%s : %s / %s", ct->Topic, ct->Set, ct->Status);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/* Framerate Label */
static u64 last_ms;
static u32 frame_cnt;
static char str_framerate[64];
static Label label_framerate = { 0, -30, ALIGN_LEFT, str_framerate };

/* Connection */
static char buf_addr[64] = "127.0.0.1:1883"; // "192.168.167.50:1901"

static const char *topics[] =
{
	"lp/gesture",
	"lp/dali",
	"lp/schlafzimmer/dmx/set",
	"lp/schlafzimmer/dmx/color/set",
	"lp/esszimmer/dmx/set",
	"lp/esszimmer/dmx/color/set",
	"lp/kueche/dmx/set",
	"lp/kueche/dmx/color/set",
	"lp/lounge/dmx/set",
	"lp/lounge/dmx/color/set",
	"lp/lounge/vorhang/set",
	"lp/lounge/vorhang/status",
	"lp/schlafzimmer/vorhang/set",
	"lp/schlafzimmer/vorhang/status",
	"lp/flur/vorhang/set",
	"lp/flur/vorhang/status",
};

static void connect(void)
{
	log_info("Connecting to MQTT on address: %s", buf_addr);
	mqtt_connect(buf_addr, topics, ARRLEN(topics), msgarrvd);
}

static void b_connect(void *tag)
{
	connect();
	(void)tag;
}

static Textbox tb_addr = { 0, -100, 300, 0, 0, 0, buf_addr, connect };
static Button button_connect = { 310, -100, 200, 38, "LP MQTT Connect", b_connect, NULL };

/* MQTT Toolbox */
static char buf_topic[64];
static char buf_value[64];

static void publish(void);
static void b_publish(void *);

static Label lbl_topic = { -300, 50, ALIGN_LEFT, "Topic:" };
static Textbox tb_topic = { -300, 75, 290, 0, 0, 0, buf_topic, NULL };
static Label lbl_value = { -300, 125, ALIGN_LEFT, "Value:" };
static Textbox tb_value = { -300, 150, 290, 0, 0, 0, buf_value, publish };
static Button btn_publish = { -300, 200, 290, 38, "Publish", b_publish, NULL };

static void b_publish(void *tag)
{
	publish();
	(void)tag;
}

static void publish(void)
{
	mqtt_publish(buf_topic, buf_value, tb_value.Length);
}

/* Gesture Toolbox */
static char buf_send_gesture[64];
static char buf_recv_gesture[64] = "Last gesture: ???";

static void b_publish_gesture(void *);
static void publish_gesture(void);

static Label lbl_lastgesture = { -300, 300, ALIGN_LEFT, buf_recv_gesture };
static Label lbl_sendgesture = { -300, 325, ALIGN_LEFT, "Send on `lp/gesture`:" };
static Textbox tb_gesture = { -300, 350, 290, 0, 0, 0, buf_send_gesture, publish_gesture };
static Button btn_gesture = { -300, 400, 290, 38, "Publish Gesture", b_publish_gesture, NULL };

static void publish_gesture(void)
{
	mqtt_publish("lp/gesture", buf_send_gesture, tb_gesture.Length);
}

static void b_publish_gesture(void *tag)
{
	publish_gesture();
	(void)tag;
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

static char buf_sel[64] = "Selected Lamp: None";

static void lamp_sel(const char *name)
{
	snprintf(buf_sel, sizeof(buf_sel), "Selected Lamp: %s", name);
}

static void sel_bedroom(void)
{
	lamp_cur = &c_bedroom;
	lamp_sel("Schlafzimmer");
}

static void sel_lounge(void)
{
	lamp_cur = &c_lounge;
	lamp_sel("Lounge");
}

static void sel_dining(void)
{
	lamp_cur = &c_dining;
	lamp_sel("Esszimmer");
}

static void sel_kitchen(void)
{
	lamp_cur = &c_kitchen;
	lamp_sel("Kueche");
}

static char buf_red[16] = "128", buf_green[16] = "128", buf_blue[16] = "128";
static char buf_bright[16] = "50";

static void publish_color(void *);
static void publish_brightness(void *);

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

static Button btn_rgb = { 1750, 1150, 200, 38, "Set Color", publish_color, NULL };

static Label lbl_bright = { 1750, 1300, 0, "Brightness:" };
static Slider sl_bright = { 1750, 1325, 100, 50, s_change_color };
static Textbox tb_bright = { 1750 + 110, 1325, 50, 0, 0, 0, buf_bright, t_change_color };

static Button btn_bright = { 1750, 1375, 200, 38, "Set Brightness", publish_brightness, NULL };

static void publish_color(void *tag)
{
	if(!lamp_cur)
	{
		return;
	}

	snprintf(lamp_cur->Bottom, 32, "[ %d, %d, %d ]",
		sl_red.Value, sl_green.Value, sl_blue.Value);
	
	// TODO

	(void)tag;
}

static void publish_brightness(void *tag)
{
	// TODO

	snprintf(d_all.Bottom, 16, "[ %d ]",
		sl_bright.Value);
	
	(void)tag;
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

void handle_msg(char *topic, char *msg, int len)
{
	if(!strcmp(topic, "lp/gesture"))
	{
		snprintf(buf_recv_gesture, sizeof(buf_recv_gesture),
			"Last gesture: %.*s", len, msg);
		return;
	}

	/* ---------- LAMPEN ----------- */
	if(!strcmp(topic, "lp/dali"))
	{
		return;
	}

	if(!strcmp(topic, "lp/schlafzimmer/dmx/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/schlafzimmer/dmx/color/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/esszimmer/dmx/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/esszimmer/dmx/color/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/kueche/dmx/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/kueche/dmx/color/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/lounge/dmx/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/lounge/dmx/color/set"))
	{
		return;
	}

	/* ---------- VORHANG ----------- */
	if(!strcmp(topic, "lp/lounge/vorhang/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/lounge/vorhang/status"))
	{
		return;
	}

	if(!strcmp(topic, "lp/schlafzimmer/vorhang/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/schlafzimmer/vorhang/status"))
	{
		return;
	}

	if(!strcmp(topic, "lp/flur/vorhang/set"))
	{
		return;
	}

	if(!strcmp(topic, "lp/flur/vorhang/status"))
	{
		return;
	}
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

	handle_msg(topicName, (char *)message->payload, message->payloadlen);

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
	(void)context;
}


void curtainpubl(const char *topic, char *val)
{
	char buf[128];
	snprintf(buf, sizeof(buf), "%sset", topic);
	mqtt_publish(buf, val, strlen(val));
}

void click_open(void *tag)
{
	CurtainTag *ct = tag;
	curtainpubl(ct->Topic, "OPEN");
}

void click_half(void *tag)
{
	CurtainTag *ct = tag;
	curtainpubl(ct->Topic, "HALF");
}

void click_close(void *tag)
{
	CurtainTag *ct = tag;
	curtainpubl(ct->Topic, "CLOSE");
}

CurtainTag *gencuco(i32 x, i32 y, const char *topic)
{
	CurtainTag *tag = malloc(sizeof(CurtainTag));
	tag->Buf = malloc(256);
	strcpy(tag->Set, "UNKNOWN");
	strcpy(tag->Status, "UNKNOWN");
	tag->Topic = topic;
	ct_format(tag);

	Label *label = malloc(sizeof(Label));
	label->X = x;
	label->Y = y;
	label->Align = 0;
	label->Text = tag->Buf;
	gui_element_add(ELEMENT_TYPE_LABEL, label);

	Button *btn_open = malloc(sizeof(Button));
	btn_open->X = x;
	btn_open->Y = y + 25;
	btn_open->W = 80;
	btn_open->H = 38;
	btn_open->Text = "Open";
	btn_open->Click = click_open;
	btn_open->Tag = tag;
	gui_element_add(ELEMENT_TYPE_BUTTON, btn_open);

	Button *btn_half = malloc(sizeof(Button));
	btn_half->X = x + 90;
	btn_half->Y = y + 25;
	btn_half->W = 80;
	btn_half->H = 38;
	btn_half->Text = "Half";
	btn_half->Click = click_half;
	btn_half->Tag = tag;
	gui_element_add(ELEMENT_TYPE_BUTTON, btn_half);

	Button *btn_close = malloc(sizeof(Button));
	btn_close->X = x + 180;
	btn_close->Y = y + 25;
	btn_close->W = 80;
	btn_close->H = 38;
	btn_close->Text = "Close";
	btn_close->Click = click_close;
	btn_close->Tag = tag;
	gui_element_add(ELEMENT_TYPE_BUTTON, btn_close);

	return tag;
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

	tag_vlounge = gencuco(1800, 600, "lp/lounge/vorhang/");
	tag_vflur = gencuco(400, 1000, "lp/flur/vorhang/");
	tag_vbed = gencuco(400, 1900, "lp/schlafzimmer/vorhang/");

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
