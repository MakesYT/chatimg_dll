#include <windows.h>
#include <gdiplus.h>
#include <iostream>
using namespace std;
int main()
{
    
    // 打开剪贴板
    if (!OpenClipboard(NULL)) {
        std::cerr << "Failed to open clipboard\n";
        return -1;
    }

    // 获取剪贴板中的 CF_BITMAP 数据
    HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
    if (!hBitmap) {
        std::cerr << "No CF_BITMAP data in clipboard\n";
        CloseClipboard();
        return -1;
    }

    // 获取位图信息
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    // 计算图像数据大小
    int imageSize = bitmap.bmWidthBytes * bitmap.bmHeight;

    // 创建位图文件头和信息头
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fileHeader.bfType = 0x4D42; // "BM"
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = bitmap.bmWidth;
    infoHeader.biHeight = bitmap.bmHeight;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = bitmap.bmBitsPixel;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = imageSize;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    // 分配内存以保存图像数据
    char* imageData = new char[imageSize];

    // 获取图像数据
    GetDIBits(GetDC(NULL), hBitmap, 0, bitmap.bmHeight, imageData,
        (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);

    // 打开文件以写入数据
    FILE* file;
    errno_t err = fopen_s(&file, "image.bmp", "wb");
    if (err != 0) {
        std::cerr << "Failed to open file\n";
        delete[] imageData;
        CloseClipboard();
        return -1;
    }

    // 写入位图文件头和信息头
    int size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
    char* imageAllData = new char[size];
    memcpy(imageAllData, &fileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(imageAllData + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(BITMAPINFOHEADER));
    memcpy(imageAllData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), imageData, imageSize);

    // 写入图像数据
    fwrite(imageAllData, size, 1, file);

    // 关闭文件
    fclose(file);

    // 释放内存
    delete[] imageData;

    // 关闭剪贴板
    CloseClipboard();

    std::cout << "Image saved to image.bmp\n";

    return 0;

}
