#include <windows.h>
#include "pch.h"
#include "jni.h"
#include "top_ncserver_chatimg_Tools_dll_ClipboardImage.h"


int width = 0;
int height = 0;

JNIEXPORT jint JNICALL Java_top_ncserver_chatimg_Tools_dll_ClipboardImage_getImageWidth(JNIEnv* env, jobject cls)
{
    return width;
}


JNIEXPORT jint JNICALL Java_top_ncserver_chatimg_Tools_dll_ClipboardImage_getImageHeight(JNIEnv * env, jobject cls)
{
    return height;
}

JNIEXPORT jbyteArray JNICALL Java_top_ncserver_chatimg_Tools_dll_ClipboardImage_getImageData(JNIEnv* env, jobject cls) {
    // 打开剪贴板
    if (!OpenClipboard(NULL)) {
      
        return NULL;
    }

    // 获取剪贴板中的 CF_BITMAP 数据
    HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
    if (!hBitmap) {
        CloseClipboard();
        return NULL;
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

    int size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
    char* imageAllData = new char[size];
    memcpy(imageAllData, &fileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(imageAllData + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(BITMAPINFOHEADER));
    memcpy(imageAllData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), imageData, imageSize);



    jbyteArray result = env->NewByteArray(size);
    env->SetByteArrayRegion(result, 0, size, (jbyte*)imageAllData);
    delete[] imageData;
    delete[] imageAllData;
    // 关闭剪贴板
    CloseClipboard();

    return result;
}
