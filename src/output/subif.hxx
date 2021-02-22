#pragma once

#include "types.hxx"
#include "output/gfx.hxx"

#define SUBIF_INVALID_HEADER 		0xBADFEAD
#define SUBIF_SUCCESS				0x000600D
#define SUBIF_MAGIC_CHECKSUM		0x0005487
#define SUBIF_ERR(x)				(x != SUBIF_SUCCESS)

namespace gfx {
    namespace subif {
        typedef struct s_subif_header {
            dword    		magic;
            uint32   		x_res;
            uint32   		y_res;
        } __attribute__((packed)) subif_header;

        typedef struct s_subif_file {
            subif_header 	        *header;
            dword             		*data;
        } subif_file;

        int render_image(subif_file* image, shapes::positional_point pos);
    }
}