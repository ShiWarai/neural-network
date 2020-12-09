#include <cstdio>
#include <string>
#include <vector>


#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

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

    static unsigned short read_u16(FILE* fp)
    {
        unsigned char b0, b1;

        b0 = getc(fp);  
        b1 = getc(fp);

        return ((b1 << 8) | b0);
    }

    static unsigned int read_u32(FILE* fp)
    {
        unsigned char b0, b1, b2, b3;

        b0 = getc(fp);
        b1 = getc(fp);
        b2 = getc(fp);
        b3 = getc(fp);

        return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
    }

    static int read_s32(FILE* fp)
    {
        unsigned char b0, b1, b2, b3;

        b0 = getc(fp);
        b1 = getc(fp);
        b2 = getc(fp);
        b3 = getc(fp);

        return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
    }

    std::vector<std::vector <double>> getPicture( std::string pictureName, double (*normalizationFunc) (double) = NULL) {

        std::vector<std::vector<double>> picture;

        FILE* pFile = fopen(pictureName.c_str(), "rb");

        // считываем заголовок файла
        BITMAPFILEHEADER header;

        header.bfType = read_u16(pFile);
        header.bfSize = read_u32(pFile);
        header.bfReserved1 = read_u16(pFile);
        header.bfReserved2 = read_u16(pFile);
        header.bfOffBits = read_u32(pFile);

        // считываем заголовок изображения
        BITMAPINFOHEADER bmiHeader;

        bmiHeader.biSize = read_u32(pFile);
        bmiHeader.biWidth = read_s32(pFile);
        bmiHeader.biHeight = read_s32(pFile);
        bmiHeader.biPlanes = read_u16(pFile);
        bmiHeader.biBitCount = read_u16(pFile);
        bmiHeader.biCompression = read_u32(pFile);
        bmiHeader.biSizeImage = read_u32(pFile);
        bmiHeader.biXPelsPerMeter = read_s32(pFile);
        bmiHeader.biYPelsPerMeter = read_s32(pFile);
        bmiHeader.biClrUsed = read_u32(pFile);
        bmiHeader.biClrImportant = read_u32(pFile);


        // Создание пустого массива
        RGBQUAD** rgb = new RGBQUAD * [bmiHeader.biWidth];
        for (int i = 0; i < bmiHeader.biWidth; i++) {
            rgb[i] = new RGBQUAD[bmiHeader.biHeight];
        }

        // Чтение цветов (BGR)
        for (int i = 0; i < bmiHeader.biWidth; i++) {
            for (int j = 0; j < bmiHeader.biHeight; j++) {
                rgb[i][j].rgbBlue = getc(pFile);
                rgb[i][j].rgbGreen = getc(pFile);
                rgb[i][j].rgbRed = getc(pFile);
            }
        }

        // Выводим результат
        for (int i = 0; i < bmiHeader.biWidth; i++) {
            picture.push_back(std::vector<double>());
            for (int j = 0; j < bmiHeader.biHeight; j++) {
                if (normalizationFunc != NULL)
                    picture[i].push_back(normalizationFunc((rgb[i][j].rgbRed + rgb[i][j].rgbGreen + rgb[i][j].rgbBlue) / 256));
                else
                    picture[i].push_back((rgb[i][j].rgbRed + rgb[i][j].rgbGreen + rgb[i][j].rgbBlue) / 3);

            }
        }
        fclose(pFile);

        
        return picture;
    }

    double normalizationFunc(double x) {
        return 1 / (1 + exp(-x));
    }


    class BMP_BW {
    private:
        std::vector<std::vector<double>> image;
        int X_SIZE;
        int Y_SIZE;

        std::string name;
        std::string fileWay;

        void loadImage(bool getNormalized) {
            if(getNormalized)
                this->image = getPicture(this->fileWay, &normalizationFunc);
            else
                this->image = getPicture(this->fileWay);

            this->Y_SIZE = this->image.size();
            this->X_SIZE = this->image[0].size();

            return;
        }

    public:

        BMP_BW(std::string fileName, std::string fileWay, bool normalized = false) {

            this->fileWay = fileWay;
            this->name = fileName;
            this->loadImage(normalized);

        }

        unsigned getWidth() {
            return (unsigned)this->X_SIZE;
        }

        unsigned getHeight() {
            return (unsigned)this->Y_SIZE;
        }

        auto operator[] (const unsigned int index)
        {
            return this->image[index];
        }

        double getPixel(unsigned int x, unsigned int y) {
            if (X_SIZE > x && Y_SIZE > y)
                return this->image[y][x];
            else
                return NULL;
        }

        double** getPixelTable() {

            double** pic = new double* [this->Y_SIZE];
            for (int i = 0; i < Y_SIZE; i++)
                pic[i] = new double[this->X_SIZE]();

            for (int y = 0; y < this->Y_SIZE; y++) {
                for (int x = 0; x < this->X_SIZE; x++)
                    pic[y][x] = this->image[x][y];
            }

            return pic;
        }

        std::string getName() {
            return this->name;
        }

        std::vector<std::vector<double>> getImage() {
            return this->image;
        }

    };

}

#endif // MAIN_H_INCLUDED