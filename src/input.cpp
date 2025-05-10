#define is_down(b) input -> buttons[b].is_down
#define pressed(b) (is_down(b) && input->buttons[b].changed)
#define released(b) (!is_down(b) && input->buttons[b].changed)
struct ButtonState {
    bool is_down, changed;
};

enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_COUNT, // Should be last item
};

struct Input {
    ButtonState buttons[BUTTON_COUNT];
};