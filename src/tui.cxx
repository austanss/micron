#include "kernel/tui.hxx"

// Pointers to locations of text in the terminal text buffer
uint16_t* label;
uint16_t* selection_stars[6];

struct tui_element {
    char* text;
    bool functioned;
    void (*selection_handler)();
};

struct tui {
    char* label;
    tui_element elements[6]; 
};

// Values for interfacing/selection
int selected_element = 1;
const uint16_t selector_entry = 2858;

void interface_controller() 
{
    *selection_stars[selected_element - 1] = NULL;

    if (io::keyboard::char_buffer[0] == 0xF1)
        selected_element--;
    else if (io::keyboard::char_buffer[0] == 0xF2)
        selected_element++;

    if (selected_element > 6)
        selected_element = 1;
    else if (selected_element < 1)
        selected_element = 6;    

    *selection_stars[selected_element - 1] = selector_entry;

    terminal::instance().render_buffer();
}

void sys::tui::start()
{
    terminal& term = terminal::instance();
    term.write(terminal_line);

    term.setCursor(6, 9);
    term.write("\t  Testing Menu");
    term.setCursor(6, 12);
    term.write("CPU");
    term.setCursor(6, 13);
    term.write("Memory");
    term.setCursor(6, 14);
    term.write("Graphics");
    term.setCursor(6, 15);
    term.write("I/O");
    term.setCursor(6, 16);
    term.write("Input");
    term.setCursor(6, 17);
    term.write("UEFI");

    selection_stars[0] = (uint16_t *)(((uint64_t)terminal::text_buffer) + ((12 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[1] = (uint16_t *)(((uint64_t)terminal::text_buffer) + ((13 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[2] = (uint16_t *)(((uint64_t)terminal::text_buffer) + ((14 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[3] = (uint16_t *)(((uint64_t)terminal::text_buffer) + ((15 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[4] = (uint16_t *)(((uint64_t)terminal::text_buffer) + ((16 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[5] = (uint16_t *)(((uint64_t)terminal::text_buffer) + ((17 * term.VGA_WIDTH) + 4) * 2);

    *selection_stars[0] = selector_entry;
    term.render_buffer();

    io::keyboard::keyboard_event_subscribe(&interface_controller);
}