float player_x = 0.f;
float player_y = 0.f;

static void simulate_game(Input *input, float delta) {
    clear_screen(0x000000);
    //printf("FPS: %lf\n", (double)1.f/delta);

    if (is_down(BUTTON_UP)) player_y += 10.f * delta;
    if (is_down(BUTTON_DOWN)) player_y -= 10.f * delta;
    if (is_down(BUTTON_LEFT)) player_x -= 10.f * delta;
    if (is_down(BUTTON_RIGHT)) player_x += 10.f * delta;

    draw_rect(player_x, player_y, 0.25, 0.5, 0x00ff22);
    draw_rect(30, 30, 5, 5, 0x00ff22);
    draw_rect(-20, 20, 8, 3, 0x00ff22);

}