#include "input/tui.hxx"

// Pointers to locations of text in the terminal text buffer
word* label;
word* selection_stars[6];

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
uint selected_element = 1;
const word selector_entry = 2858; // the value used for the selection star

void interface_controller(struct io::keyboard::keyboard_packet kbpacket) 
{
    if (!kbpacket.release_or_press)
        return;

    *selection_stars[selected_element - 1] = 0;

    if (kbpacket.key_code == 0xB8)
        selected_element--;
    else if (kbpacket.key_code == 0xC0)
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
    term.write("\n");
    term.write(terminal_line);

    term.setCursor(6, 12);
    term.write("\t  Testing Menu");
    term.setCursor(6, 15);
    term.write("CPU");
    term.setCursor(6, 16);
    term.write("Memory");
    term.setCursor(6, 17);
    term.write("Graphics");
    term.setCursor(6, 18);
    term.write("I/O");
    term.setCursor(6, 19);
    term.write("Input");
    term.setCursor(6, 20);
    term.write("UEFI");

    selection_stars[0] = (word *)(((address)terminal::text_buffer) + ((15 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[1] = (word *)(((address)terminal::text_buffer) + ((16 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[2] = (word *)(((address)terminal::text_buffer) + ((17 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[3] = (word *)(((address)terminal::text_buffer) + ((18 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[4] = (word *)(((address)terminal::text_buffer) + ((19 * term.VGA_WIDTH) + 4) * 2);
    selection_stars[5] = (word *)(((address)terminal::text_buffer) + ((20 * term.VGA_WIDTH) + 4) * 2);

    *selection_stars[0] = selector_entry;
    term.render_buffer();

    io::keyboard::keyboard_event_subscribe(interface_controller);
}