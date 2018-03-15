
#include <string.h>
#include <stdio.h>

#include "../include/jconfig.h"
#include "../include/jmorecfg.h"
#include "../include/jpeglib.h"

#include "function.h"

//#include "jconfig.h"
//#include "jmorecfg.h"
//#include "jpeglib.h"


int rgb2jpeg(unsigned char* rgb_buffer, int width, int height, int quality, unsigned char** jpeg_buffer, unsigned long* jpeg_size)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int row_stride = 0;
	JSAMPROW row_pointer[1];

	if (jpeg_buffer == NULL)
	{
		printf("you need a pointer for jpeg buffer.\n");
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_compress(&cinfo);

	jpeg_mem_dest(&cinfo, jpeg_buffer, jpeg_size);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 1; // 3 为彩图
	cinfo.in_color_space = JCS_GRAYSCALE; // JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, 1);// todo 1 == true  
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = width * cinfo.input_components;

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &rgb_buffer[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	return 0;
}


int jpeg2rgb(unsigned char* jpeg_buffer, int jpeg_size, unsigned char* rgb_buffer, int*size, int* width, int* height)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPARRAY buffer;
	int row_stride = 0;
	unsigned char* tmp_buffer = NULL;
	int rgb_size;

	if (jpeg_buffer == NULL)
	{
		printf("no jpeg buffer here.\n");
		return -1;
	}
	if(rgb_buffer == NULL)
	{
		printf("you need to alloc rgb buffer.\n");
		return -1;
	}

	cinfo.err  = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpeg_buffer, jpeg_size);

	jpeg_read_header(&cinfo, TRUE);

	cinfo.out_color_space = JCS_GRAYSCALE; // JCS_RGB; // 设置输出格式  

	jpeg_start_decompress(&cinfo);

	row_stride = cinfo.output_width * cinfo.output_components;

	*width = cinfo.output_width;
	*height = cinfo.output_height;

	rgb_size = row_stride * cinfo.output_height; // 总大小  
	if (*size < rgb_size)
	{
//		printf("rgb buffer to small, we need %d but has only: %d\n", rgb_size, *size);
	}

	*size = rgb_size;

	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	//printf("debug--:\nrgb_size: %d, size: %d w: %d h: %d row_stride: %d \n", rgb_size,
	//	cinfo.image_width*cinfo.image_height * 3,
	//	cinfo.image_width,
	//	cinfo.image_height,
	//	row_stride);

	tmp_buffer  = rgb_buffer;
	while (cinfo.output_scanline < cinfo.output_height)// 解压每一行  
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);
		// 复制到内存  
		memcpy(tmp_buffer, buffer[0], row_stride);
		tmp_buffer += row_stride;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 0;
}
