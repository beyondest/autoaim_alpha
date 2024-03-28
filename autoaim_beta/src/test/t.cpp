#include <iostream>
#include <cmath>
static inline float softmax(float* buffer, int& begin_idx, const int& class_num, int& actual_idx)
{
    float sum_exp = 0;
    for (int i = begin_idx; i < class_num; i++) sum_exp += std::exp(buffer[i]);
    return std::exp(buffer[actual_idx]) / sum_exp;
}
int main()
{
    float buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int begin_idx = 0;
    int class_num = 10;
    int actual_idx = 5;
    softmax(buffer, begin_idx, class_num, actual_idx);
    std::cout << "softmax result: " << softmax(buffer, begin_idx, class_num, actual_idx) << std::endl;
    return 0;//-------------------------------
// GXDQBuf 接口一次获取一帧图像，本样例代码演示的就是如何用此接口获取一帧图像
//-------------------------------
#include "GxIAPI.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "DxImageProc.h"

int main(int argc, char *argv[])
{
    GX_STATUS status = GX_STATUS_SUCCESS;
    GX_DEV_HANDLE hDevice = NULL;
    uint32_t nDeviceNum = 0;
    cv::Mat img(1920, 1200, CV_8UC3);
    // 初始化库
    status = GXInitLib();
    if (status != GX_STATUS_SUCCESS)
    {
        std::cout << "GXInitLib Error!" << std::endl;
        return 0;
    }
    // 枚举设备列表
    status = GXUpdateDeviceList(&nDeviceNum, 1000);
    if ((status != GX_STATUS_SUCCESS) || (nDeviceNum <= 0))
    {
        std::cout << "No device found!" << std::endl;
        return 0;
    }
    // 打开设备
    status = GXOpenDeviceByIndex(1, &hDevice);
    if (status == GX_STATUS_SUCCESS)
    {
        // 定义 GXDQBuf 的传入参数
        PGX_FRAME_BUFFER pFrameBuffer;

        // 开采
        status = GXStreamOn(hDevice);
        if (status == GX_STATUS_SUCCESS)
        {
            // 调用 GXDQBuf 取一帧图像
            while (1)
            {
                status = GXDQBuf(hDevice, &pFrameBuffer, 1000);
                if (status == GX_STATUS_SUCCESS)
                {
                    if (pFrameBuffer->nStatus == GX_FRAME_STATUS_SUCCESS)
                    {
                        std::cout << "Get one frame!" << std::endl;
                        std::cout << "Width: " << pFrameBuffer->nWidth << " Height: " << pFrameBuffer->nHeight << std::endl;
                        std::cout << "Size: " << pFrameBuffer->nImgSize << std::endl;
                        std::cout << "Pixel format: " << pFrameBuffer->nPixelFormat << std::endl;
                        std::cout << "Timestamp: " << pFrameBuffer->nTimestamp << std::endl;
                        std::cout << "BufID: " << pFrameBuffer->nBufID << std::endl;
                        std::cout << "Status: " << pFrameBuffer->nStatus << std::endl;

                        uint8_t *pRGB24Buf = new uint8_t[1920 * 1200 * 3]; // 输出图像 RGB 数据
                        {
                            memset(pRGB24Buf, 0, pFrameBuffer->nWidth * pFrameBuffer->nHeight * 3 * sizeof(uint8_t)); // 缓冲区初始化
                        }
                        DX_BAYER_CONVERT_TYPE cvtype = RAW2RGB_NEIGHBOUR; // 选择插值算法
                        DX_PIXEL_COLOR_FILTER nBayerType = BAYERRG;       // 选择图像 Bayer 格式
                    
                        bool bFlip = true;
                        VxInt32 DxStatus = DxRaw8toRGB24(pFrameBuffer->pImgBuf, pRGB24Buf, pFrameBuffer->nWidth,
                                                         pFrameBuffer->nHeight, cvtype, nBayerType, bFlip);
                        if (DxStatus != DX_OK)
                        {
                            if (pRGB24Buf != NULL)
                            {
                                delete[] pRGB24Buf;
                                pRGB24Buf = NULL;
                            }
                            return;
                        }
                        // 对 RGB24 数据进行处理
                        if (pRGB24Buf != NULL)
                        {
                            delete[] pRGB24Buf;
                            pRGB24Buf = NULL;
                        }
                        // 调用 GXQBuf 将图像 buf 放回库中继续采图

                        status = GXQBuf(hDevice, pFrameBuffer);

                        if (DxStatus == DX_OK)
                        {
                            cv::imshow("test", img);
                            cv::waitKey(1);
                        }
                    }
                }
            }
        }
        // 停采
        status = GXStreamOff(hDevice);
    }
    status = GXCloseDevice(hDevice);
    status = GXCloseLib();
    return 0;
}

}