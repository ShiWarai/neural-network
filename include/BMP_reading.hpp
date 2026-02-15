#ifndef BMP_READING_HPP_INCLUDED
#define BMP_READING_HPP_INCLUDED

#include <cstdio>
#include <string>
#include <vector>

namespace BMP {

    typedef struct
    {
        unsigned int    bfType;
        unsigned long   bfSize;
        unsigned int    bfReserved1;
        unsigned int    bfReserved2;
        unsigned long   bfOffBits;
    } BITMAPFILEHEADER;

    typedef struct
    {
        unsigned int    biSize;
        int             biWidth;
        int             biHeight;
        unsigned short  biPlanes;
        unsigned short  biBitCount;
        unsigned int    biCompression;
        unsigned int    biSizeImage;
        int             biXPelsPerMeter;
        int             biYPelsPerMeter;
        unsigned int    biClrUsed;
        unsigned int    biClrImportant;
    } BITMAPINFOHEADER;

    typedef struct
    {
        int   rgbBlue;
        int   rgbGreen;
        int   rgbRed;
        int   rgbReserved;
    } RGBQUAD;

    std::vector<std::vector<double>> getPicture(std::string pictureName, double (*normalizationFunc)(double) = NULL);

    double normalizationFunc(double x);

    class BMP_BW {
    private:
        std::vector<std::vector<double>> image;
        int X_SIZE;
        int Y_SIZE;

        std::string name;
        std::string fileWay;

        void loadImage(bool getNormalized);

    public:
        BMP_BW(std::string fileName, std::string fileWay, bool normalized = false);

        unsigned getWidth();
        unsigned getHeight();
        std::vector<double> operator[](const unsigned int index);
        double getPixel(unsigned int x, unsigned int y);
        double** getPixelTable();
        std::string getName();
        std::vector<std::vector<double>> getImage();
    };

}

#endif // BMP_READING_HPP_INCLUDED
