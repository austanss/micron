#pragma once

#include <stdint.h>
#include "kernel/gfx.hxx"

#define SUBIF_INVALID_HEADER 		0xBADFEAD
#define SUBIF_SUCCESS				0x000600D
#define SUBIF_MAGIC_CHECKSUM		0x0005487
#define SUBIF_ERR(x)				(x != SUBIF_SUCCESS)

namespace gfx {
    namespace subif {
        typedef struct s_subif_header {
            uint32_t 		magic;
            uint32_t 		x_res;
            uint32_t 		y_res;
        } subif_header;

        typedef struct s_subif_file {
            subif_header 	header;
            uint32_t 		*data;
        } subif_file;

        int render_image(subif_file* image, shapes::positional_point pos);
    };
};