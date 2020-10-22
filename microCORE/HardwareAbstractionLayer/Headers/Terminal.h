#pragma once

#include "stddef.h"
#include "stdint.h"

class Terminal
{
        static const size_t VGA_WIDTH = 80;
        static const size_t VGA_HEIGHT = 25;

        uint16_t* buffer;

public:
	size_t row;
	size_t column;
    static constexpr char* Status = "$WHITE![$LIGHT_BLUE!-$WHITE!] $LIGHT_GREY!\0";
    static constexpr char* Good = "$WHITE![$LIGHT_GREEN!+$WHITE!] $LIGHT_GREY\0";
    static constexpr char* Fail = "$WHITE![$RED!X$WHITE!] $LIGHT_GREY\0";
    static constexpr char* EOL = "\n";
    static Terminal &instance();
    void put_entry_at(char c, uint8_t color, size_t x, size_t y);
    void put_char(char c, uint8_t color);
    void write(const char* data, size_t size);
    void write(const char* data);
    void write(int num);
    void println(const char* data = "");
    void shift();
	void clear();
	void setCursor(size_t columnc, size_t rowc);
	bool staticLogo = false;

private:
        Terminal();
        Terminal(Terminal const&);
        void operator=(Terminal const&);
};

template<typename T>
Terminal& operator<<(Terminal& term, T data)
{
        term.write(data);
        return term;
}

template<typename T>
Terminal* operator<<(Terminal* term, T data)
{
        term->write(data);
        return term;
}
