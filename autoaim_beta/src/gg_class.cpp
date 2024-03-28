#include "gg_class.hpp"
#include "basic.hpp"
#include <spdlog/spdlog.h>

void CHECK_GG_STATUS(GX_STATUS status)
{
    if (status != GX_STATUS_SUCCESS)
    {
        spdlog::error("GGCameraControl Error: {}", status);
        throw std::runtime_error("GGCameraControl Error");
    }
}

GG_Camera::GG_Camera(const Mode mode, double exposure_time)
{
    uint32_t nDeviceNum = 0;
    this->camera_mode = camera_mode;
    CHECK_GG_STATUS(GXInitLib());
    CHECK_GG_STATUS(GXUpdateDeviceList(&nDeviceNum, 1000));
    if (nDeviceNum <= 0)
    {
        spdlog::error("No GG Camera found!");
        throw std::runtime_error("No GG Camera found!");
    }
    CHECK_GG_STATUS(GXOpenDeviceByIndex(1, &hDevice));
    CHECK_GG_STATUS(GXSetFloat(hDevice, GX_FLOAT_EXPOSURE_TIME,exposure_time));
}

GG_Camera::~GG_Camera()
{
    this->close();
}

IF_SUCCESS GG_Camera::open()
{
    CHECK_GG_STATUS(GXStreamOn(hDevice));
    return IF_SUCCESS::SUCCESS;
}

IF_SUCCESS GG_Camera::close()
{
    CHECK_GG_STATUS(GXStreamOff(hDevice));
    CHECK_GG_STATUS(GXCloseDevice(hDevice));
    CHECK_GG_STATUS(GXCloseLib());
    return IF_SUCCESS::SUCCESS;
}

IF_SUCCESS GG_Camera::get_img(cv::Mat &img)
{
    CHECK_GG_STATUS(GXDQBuf(hDevice, &pFrameBuffer, this->wTimes_ms));
    if (pFrameBuffer->nStatus == GX_FRAME_STATUS_SUCCESS)
    {
        // 将图像数据拷贝到 OpenCV Mat 中
        DX_BAYER_CONVERT_TYPE cvtype = RAW2RGB_NEIGHBOUR; 
        DX_PIXEL_COLOR_FILTER nBayerType = BAYERRG;       
        bool bFlip = true;
        DX_RGB_CHANNEL_ORDER emChannelOrder = DX_ORDER_BGR;
        VxInt32 DxStatus = DxRaw8toRGB24Ex(pFrameBuffer->pImgBuf, img.data, pFrameBuffer->nWidth,
                                           pFrameBuffer->nHeight, cvtype, nBayerType, bFlip, emChannelOrder);
        CHECK_GG_STATUS(GXQBuf(hDevice, pFrameBuffer));
        if (DxStatus != DX_OK)
        {
            spdlog::error("DxRaw8toRGB24Ex Error: {}", DxStatus);
            return IF_SUCCESS::FAIL;
        }
        else
        {
            return IF_SUCCESS::SUCCESS;
        }
    }
    else
    {
        spdlog::error("Frame buffer status error: {}", pFrameBuffer->nStatus);
        return IF_SUCCESS::FAIL;
    }
}
