#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

#define readfile "type_your_file_here"  //defining reading file
#define writefile "downed.bmp" //defining downed file
#define writefilesoft "soft.bmp" //defining smoothned file

typedef struct
{
    uint8_t rgbtBlue;
    uint8_t rgbtGreen;
    uint8_t rgbtRed;
}  __attribute__((__packed__)) RGBTRIPLE;

FILE* rPtr;
FILE* rPtrsoft; //file function pointer definitions
FILE* wPtr;
FILE* wPtrsoft;

void wImage(); //defining 3 main functions, those will be do image processings.
void rImage();
void wImageSoft();

BITMAPFILEHEADER bf,bfsoft;
BITMAPINFOHEADER bi,bisoft; //defining new structure variables to bitmap.h

int height, width, padding;
int nHeight, nWidth, nPadding; //globals

int main(int argc, char* argv[]) {

	rImage();
	wImage();
	wImageSoft();

    fclose(rPtr);
    fclose(wPtr);
    fclose(wPtrsoft);   //file attached functions must be closed, otherwise windows will try to close automatically and that may be cause some errors

    return 0;
}

void rImage()
{
	rPtr = fopen(readfile, "rb");
    rPtrsoft= fopen(readfile, "rb");    //defining pointers for reading files on binary mode

	fread(&bf, 14, 1, rPtr);
    fread(&bi, 40, 1, rPtr);

    fread(&bfsoft, 14, 1, rPtrsoft);
    fread(&bisoft, 40, 1, rPtrsoft);

	height = abs(bi.biHeight); // arranging dimensions
    width = bi.biWidth;
    padding = (4 - (width * 3) % 4) % 4; //settings for padding

}

void wImage() //scaled image writer
{
	wPtr = fopen(writefile, "wb");

	nHeight = height / 2;
    nWidth = width / 2;
	nPadding = (4 - (nWidth * 3) % 4) % 4;    //new .bmp file's dimensions

	bi.biHeight = nHeight; //reading from headers
    bi.biWidth = nWidth;
    bi.biSizeImage = ((3 * nWidth) + nPadding) * abs(nHeight);
    bf.bfSize = bi.biSizeImage + 14 + 40;

    RGBTRIPLE(*image)[width] = calloc(height, width * 3);     //allocating empty memory for both of the images
	RGBTRIPLE(*nImage)[nWidth] = calloc(nHeight, nWidth * 3);

	fwrite(&bf, 14, 1, wPtr);
    fwrite(&bi, 40, 1, wPtr);

    for (int i = 0; i < height; i++) //takes image's info
    {
        fread(image[i], 3, width, rPtr);
        fseek(rPtr, padding, SEEK_CUR);
    }

    //scaling
    for (int i = 0; i < height; i += 2)
    {
        for (int j = 0; j < width; j += 2) {
           float avr_red = (image[i][j].rgbtRed + image[i][j+1].rgbtRed + image[i+1][j].rgbtRed + image[i+1][j+1].rgbtRed) / 4.0;
           float avr_green = (image[i][j].rgbtGreen + image[i][j+1].rgbtGreen + image[i+1][j].rgbtGreen + image[i+1][j+1].rgbtGreen) / 4.0; //takes averages of every 4 pixels
           float avr_blue = (image[i][j].rgbtBlue + image[i][j+1].rgbtBlue + image[i+1][j].rgbtBlue + image[i+1][j+1].rgbtBlue) / 4.0;
           nImage[i / 2][j / 2].rgbtBlue = avr_blue;
           nImage[i / 2][j / 2].rgbtGreen = avr_green; //arranging rgb of scaled image with average colors.
           nImage[i / 2][j / 2].rgbtRed = avr_red;
        }
    }

    for (int i = 0; i < nHeight; i++){ //writes new image
        fwrite(nImage[i], 3, nWidth, wPtr);
    }
}

void wImageSoft() //write smoothened image
{
	wPtrsoft = fopen(writefilesoft, "wb");

    RGBTRIPLE(*image2)[width+ 2] = calloc(height + 2, (width + 2) * 3);
	RGBTRIPLE(*softImage)[width] = calloc(height, width * 3);

	fwrite(&bfsoft, 14, 1, wPtrsoft);
    fwrite(&bisoft, 40, 1, wPtrsoft);

    for (int i = 1; i <= height; i++)
    {
        fread(*(image2 + i) + 1, 3, width, rPtrsoft);
        fseek(rPtrsoft, padding, SEEK_CUR);
    }

    for (int i = 1; i <= height; i++)  //softer functions
    {
        for (int j = 1; j <= width; j++) {
			softImage[i - 1][j - 1].rgbtRed = image2[i - 1][j - 1].rgbtRed * 0.0625 + image2[i - 1][j].rgbtRed * 0.125 + image2[i - 1][j + 1].rgbtRed * 0.0625 + image2[i][j - 1].rgbtRed * 0.125 + image2[i][j].rgbtRed * 0.25 + image2[i][j + 1].rgbtRed * 0.125 + image2[i + 1][j - 1].rgbtRed * 0.0625 + image2[i + 1][j].rgbtRed * 0.125 + image2[i + 1][j + 1].rgbtRed * 0.0625;
			softImage[i - 1][j - 1].rgbtGreen = image2[i - 1][j - 1].rgbtGreen * 0.0625 + image2[i - 1][j].rgbtGreen * 0.125 + image2[i - 1][j + 1].rgbtGreen * 0.0625 + image2[i][j - 1].rgbtGreen * 0.125 + image2[i][j].rgbtGreen * 0.25 + image2[i][j + 1].rgbtGreen * 0.125 + image2[i + 1][j - 1].rgbtGreen * 0.0625 + image2[i + 1][j].rgbtGreen * 0.125 + image2[i + 1][j + 1].rgbtGreen * 0.0625;
			softImage[i - 1][j - 1].rgbtBlue = image2[i - 1][j - 1].rgbtBlue * 0.0625 + image2[i - 1][j].rgbtBlue * 0.125 + image2[i - 1][j + 1].rgbtBlue * 0.0625 + image2[i][j - 1].rgbtBlue * 0.125 + image2[i][j].rgbtBlue * 0.25 + image2[i][j + 1].rgbtBlue * 0.125 + image2[i + 1][j - 1].rgbtBlue * 0.0625 + image2[i + 1][j].rgbtBlue * 0.125 + image2[i + 1][j + 1].rgbtBlue * 0.0625;
		}
	}

	for (int i = 0; i < height; i++) // writes softed version of itu.bmp
	{
        fwrite(softImage[i], 3, width, wPtrsoft);

        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, wPtrsoft);
        }
    }

}
