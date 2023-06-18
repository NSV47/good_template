#pragma once

#include <furi.h>
#include <gui/gui.h>

#include <notification/notification_messages.h>

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} GoodTemplateEvent;

struct GoodTemplateApp{
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    FuriTimer* timer;

    NotificationApp* notifications;

    const GpioPin* output_pin;

    bool output_value;
};

typedef struct GoodTemplateApp GoodTemplateApp;
