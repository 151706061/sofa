#include <sofa/helper/io/ImageBMP.h>
#include <iostream>

namespace sofa
{

namespace helper
{

namespace io
{

//using namespace sofa::defaulttype;

SOFA_DECL_CLASS(ImageBMP)

Creator<Image::Factory,ImageBMP> ImageBMPClass("bmp");

bool ImageBMP::load(const std::string &filename)
{
    unsigned short int bfType;
    long int bfOffBits;
    short int biPlanes;
    short int biBitCount;
    long int biSizeImage;
    int i;
    unsigned char temp;
    FILE *file;
    /* make sure the file is there and open it read-only (binary) */
    if ((file = fopen(filename.c_str(), "rb")) == NULL)
    {
        std::cerr << "File not found : " << filename << std::endl;
        return false;
    }

    if(!fread(&bfType, sizeof(short int), 1, file))
    {
        std::cerr << "Error reading file!" << std::endl;
        return false;
    }

    /* check if file is a bitmap */
    if (bfType != 19778)
    {
        std::cerr << "Not a Bitmap-File!\n";
        return false;
    }
    /* get the file size */
    /* skip file size and reserved fields of bitmap file header */
    fseek(file, 8, SEEK_CUR);
    /* get the position of the actual bitmap data */
    if (!fread(&bfOffBits, sizeof(long int), 1, file))
    {
        std::cerr << "Error reading file!\n";
        return false;
    }
    // printf("Data at Offset: %ld\n", bfOffBits);
    /* skip size of bitmap info header */
    fseek(file, 4, SEEK_CUR);
    /* get the width of the bitmap */
    fread(&width, sizeof(int), 1, file);
    //printf("Width of Bitmap: %d\n", texture->width);
    /* get the height of the bitmap */
    fread(&height, sizeof(int), 1, file);
    //printf("Height of Bitmap: %d\n", texture->height);
    /* get the number of planes (must be set to 1) */
    fread(&biPlanes, sizeof(short int), 1, file);
    if (biPlanes != 1)
    {
        std::cerr << "Error: number of Planes not 1!\n";
        return false;
    }
    /* get the number of bits per pixel */
    if (!fread(&biBitCount, sizeof(short int), 1, file))
    {
        std::cerr << "Error reading file!\n";
        return false;
    }
    //printf("Bits per Pixel: %d\n", biBitCount);
    if (biBitCount != 24)
    {
        std::cerr << "Bits per Pixel not 24\n";
        return false;
    }
    nbBits = biBitCount;
    /* calculate the size of the image in bytes */
    biSizeImage = width * height * 3;
    // std::cout << "Size of the image data: " << biSizeImage << std::endl;
    data = (unsigned char*) malloc(biSizeImage);
    /* seek to the actual data */
    fseek(file, bfOffBits, SEEK_SET);

    if ((width%4)==0)
    {
        if (!fread(data, biSizeImage, 1, file))
        {
            std::cerr << "Error loading file!\n";
            return false;
        }
    }
    else
    {
        for (int y=0; y<height; y++)
        {
            if (!fread(data+y*width*3, width*3, 1, file))
            {
                std::cerr << "Error loading file!\n";
                return false;
            }
            char buf[3];
            if (!fread(buf, 4-(width%4), 1, file))
            {
                std::cerr << "Error loading file!\n";
                return false;
            }
        }
    }

    /* swap red and blue (bgr -> rgb) */
    for (i = 0; i < biSizeImage; i += 3)
    {
        temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    fclose(file);
    return true;
}

static bool fwriteW(FILE* file, unsigned short data)
{
    return fwrite(&data,sizeof(data),1,file)!=0;
}

static bool fwriteDW(FILE* file, unsigned long data)
{
    return fwrite(&data,sizeof(data),1,file)!=0;
}

bool ImageBMP::save(const std::string& filename)
{
    FILE *file;
    /* make sure the file is there and open it read-only (binary) */
    if ((file = fopen(filename.c_str(), "wb")) == NULL)
    {
        std::cerr << "File write access failed : " << filename << std::endl;
        return false;
    }

    unsigned long lineSizeIn = ((nbBits+7)/8)*width;
    unsigned long lineSizeOut = ((lineSizeIn+3)/4)*4;
    unsigned long biSizeImage = lineSizeOut*height;

    // BITMAPFILEHEADER
    if (!fwriteW(file, (unsigned short)'B' | ((unsigned short)'M' << 8))) return false; // Type
    if (!fwriteDW(file, 14+40+biSizeImage)) return false; // Size
    if (!fwriteW(file, 0)) return false; // Reserved1
    if (!fwriteW(file, 0)) return false; // Reserved2
    if (!fwriteDW(file, 14+40)) return false; // OffBits

    // BITMAPINFOHEADER
    if (!fwriteDW(file, 40)) return false; // Size
    if (!fwriteDW(file, width)) return false; // Width
    if (!fwriteDW(file, height)) return false; // Height
    if (!fwriteW(file, 1)) return false; // Planes
    if (!fwriteW(file, nbBits)) return false; // BitCount
    if (!fwriteDW(file, 0)) return false; // Compression
    if (!fwriteDW(file, biSizeImage)) return false; // SizeImage
    if (!fwriteDW(file, 2825)) return false; // XPelsPerMeter
    if (!fwriteDW(file, 2825)) return false; // YPelsPerMeter
    if (!fwriteDW(file, 0)) return false; // ClrUsed
    if (!fwriteDW(file, 0)) return false; // ClrImportant

    if(nbBits==24)
    {
        unsigned char temp;
        /* swap red and blue (rgb -> bgr) */
        for (int i = 0; i < width*height*3; i += 3)
        {
            temp = data[i];
            data[i] = data[i + 2];
            data[i + 2] = temp;
        }
    }
    if (lineSizeIn == lineSizeOut)
    {
        if (!fwrite(data, biSizeImage, 1, file)) return false;
    }
    else
    {
        for (int y=0; y<height; y++)
        {
            if (!fwrite(data+y*lineSizeIn, lineSizeIn, 1, file)) return false;
            char buf[3]= {0,0,0};
            if (!fwrite(buf, lineSizeOut-lineSizeIn, 1, file)) return false;
        }
    }
    if(nbBits==24)
    {
        int i;
        unsigned char temp;
        /* swap red and blue (bgr -> rgb) */
        for (i = 0; i < width*height*3; i += 3)
        {
            temp = data[i];
            data[i] = data[i + 2];
            data[i + 2] = temp;
        }
    }

    fclose(file);
    return true;
}

} // namespace io

} // namespace helper

} // namespace sofa

