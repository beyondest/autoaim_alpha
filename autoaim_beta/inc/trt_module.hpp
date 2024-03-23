#ifndef TRT_MODULE_HPP_
#define TRT_MODULE_HPP_

#include <opencv2/core.hpp>
#include <NvInfer.h>
#include "basic.hpp"



struct alignas(4) bbox_t {
    cv::Point2f pts[4]; // [pt0, pt1, pt2, pt3]
    float confidence;
    int color_id; // 0: blue, 1: red, 2: gray
    int tag_id;   // 0: guard, 1-5: number, 6: base

    bool operator==(const bbox_t& other) const
    {
        return std::equal(std::begin(pts),std::end(pts),std::begin(other.pts)) && 
               confidence == other.confidence &&
               color_id == other.color_id &&
               tag_id == other.tag_id;
    }

    bool operator!=(const bbox_t& other) const
    {
        return !(*this == other);
    }
};

struct alignas(4) classified_result_t
{
    float confidence;
    std::string cls_name;
};

class TRTModule {
    static constexpr int TOPK_NUM = 128;
    static constexpr float KEEP_THRES = 0.1f;

public:
    explicit TRTModule(const std::string &onnx_file);

    ~TRTModule();

    TRTModule(const TRTModule &) = delete;

    TRTModule operator=(const TRTModule &) = delete;

    std::vector<bbox_t> operator()(const cv::Mat &src) const;



private:
    void build_engine_from_onnx(const std::string &onnx_file);

    void build_engine_from_cache(const std::string &cache_file);

    void cache_engine(const std::string &cache_file);

    nvinfer1::ICudaEngine *engine;
    nvinfer1::IExecutionContext *context;
    mutable void *device_buffer[2];
    float *output_buffer;
    cudaStream_t stream;
    int input_idx, output_idx;
    size_t input_sz, output_sz;
};

class TRT_Engine_Params:public Params
{

public:
    std::string input_name;
    std::string output_name;
    int max_batchsize;
    std::vector<int> input_size = {32,32};
    std::string input_dtype;


    TRT_Engine_Params(){};
    ~TRT_Engine_Params(){};

    bool load_params_from_yaml(const std::string& file_path);
    void print_show_params();

};

class TRT_Engine
{
private:

    
    nvinfer1::ICudaEngine *engine;
    nvinfer1::IExecutionContext *context;
    mutable void *device_buffer[2];
    float *output_buffer;
    cudaStream_t stream;
    int input_idx, output_idx;
    size_t input_sz, output_sz;

    TRT_Engine_Params params;

public:
    TRT_Engine(const std::string& trt_file,
               const std::string& net_config_params_file);

    ~TRT_Engine();
    TRT_Engine(const TRT_Engine &) = delete;

    TRT_Engine operator=(const TRT_Engine &) = delete;

    float* operator()(const std::vector<cv::Mat> &src_imgs) const;    

};




#endif 
