#include "good_template_app.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

static void good_template_app_input_callback(InputEvent* input_event, void* ctx){
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;


    GoodTemplateEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

//---------------------------------------------------------------------------

static void good_template_app_draw_callback(Canvas* canvas, void* ctx){
    UNUSED(ctx);
    canvas_clear(canvas);

    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 127, 15, AlignRight, AlignTop, "This is a good memory\n allocation/deallocation\n template for your future\n applications");
    
}

//---------------------------------------------------------------------------

static void timer_callback(FuriMessageQueue* event_queue) {
    // Проверяем, что контекст не нулевой
    furi_assert(event_queue);

    GoodTemplateEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

//---------------------------------------------------------------------------

GoodTemplateApp* good_template_app_alloc(){
    GoodTemplateApp* app = malloc(sizeof(GoodTemplateApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(GoodTemplateEvent));
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    view_port_draw_callback_set(app->view_port, good_template_app_draw_callback, NULL);
    view_port_input_callback_set(app->view_port, good_template_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, app->event_queue);

    app->output_pin = &gpio_ext_pa6;

    furi_hal_gpio_init(app->output_pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    return app;
}

//---------------------------------------------------------------------------

void good_template_app_free(GoodTemplateApp* app){
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_timer_free(app->timer);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
}

//---------------------------------------------------------------------------

int32_t good_template_app(void *p){
    UNUSED(p);
    GoodTemplateApp* app = good_template_app_alloc();

    GoodTemplateEvent event;

    furi_timer_start(app->timer, 5000);

    //---------------------------------------------------------------------------

    while(1){
        // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // Наше событие — это нажатие кнопки
        if (event.type == EventTypeInput) {
            // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
            if (event.input.key == InputKeyBack) {
                break;
            }
        // Наше событие — это сработавший таймер
        } else if (event.type == EventTypeTick) {
            // Сделаем что-то по таймеру
        }
    }

    good_template_app_free(app);

    return 0;
}