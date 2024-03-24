#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "basic.hpp"
using namespace cv;
using namespace std;


class PNP_Params : public Params
{

public:
    PNP_Params(int& img_show_wid,int& img_show_hei):img_show_wid(img_show_wid),img_show_hei(img_show_hei){
        camera_center = cv::Point2f(img_show_wid/2,img_show_hei/2);
    };
    ~PNP_Params(){};
    bool load_params_from_yaml(const std::string& file_path);
    void print_show_params();

    int img_show_wid;
    int img_show_hei;
    cv::Point2f camera_center;

    std::vector<cv::Point3f> small_obj_points;
    std::vector<cv::Point3f> big_obj_points;
    std::vector<std::string> small_armor_name_list;
    cv::Mat mtx = cv::Mat::zeros(3, 3, CV_32F);
    cv::Mat dist = cv::Mat::zeros(5, 1, CV_32F);


};
class PNP_Solver
{
private:
    PNP_Params params;
    Mode mode;
    float shrink_scale_x = 0.5;
    float shrink_scale_y = 0.5;

public:
    PNP_Solver(Mode mode_,
               const std::string& pnp_config_params_file,
               int& img_show_wid,
               int& img_show_hei):
               params(img_show_wid,img_show_hei)
               {    
                    mode = mode_;
                    params.load_params_from_yaml(pnp_config_params_file);
                    shrink_scale_x = (float)params.img_show_wid/1280.f;
                    shrink_scale_y = (float)params.img_show_hei/1024.f;
                    if (mode == Mode::Dbg) params.print_show_params();
                    
               };

    ~PNP_Solver(){};
    bool operator()(const std::string& armor_name_string,
                   const std::vector<cv::Point2f>& img_points,
                   std::vector<float>& tvec,
                   std::vector<float>& rvec) const
                   {
                        bool if_small_armor = false;
                        std::vector<cv::Point2f> real_img_points;
                        

                        for (auto& armor_name : params.small_armor_name_list)if (armor_name == armor_name_string) {if_small_armor = true;break;}
                        for (auto& point: img_points)
                        {
                            real_img_points.push_back(cv::Point2f(point.x/shrink_scale_x,point.y/shrink_scale_y));
                        }
                        if (if_small_armor)cv::solvePnP(params.small_obj_points,real_img_points,params.mtx,params.dist,rvec,tvec);
                        else cv::solvePnP(params.big_obj_points,real_img_points,params.mtx,params.dist,rvec,tvec);
                        tvec[0] = tvec[0]/1000.f;
                        float tvec1 = tvec[1];
                        tvec[1] = tvec[2]/1000.f;
                        tvec[2] = -tvec1/1000.f;
                        rvec[0] = rvec[0];
                        float rvec1 = rvec[1];  
                        rvec[1] = rvec[2];
                        rvec[2] = -rvec1;

                        return true;
                   }
};
bool PNP_Params::load_params_from_yaml(const std::string& file_path)
{

    YAML::Node config = YAML::LoadFile(file_path);
    auto read_small_obj_points = config["small_obj_points"];
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[0][0].as<float>(), read_small_obj_points[0][1].as<float>(), read_small_obj_points[0][2].as<float>()));
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[1][0].as<float>(), read_small_obj_points[1][1].as<float>(), read_small_obj_points[1][2].as<float>()));
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[2][0].as<float>(), read_small_obj_points[2][1].as<float>(), read_small_obj_points[2][2].as<float>()));
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[3][0].as<float>(), read_small_obj_points[3][1].as<float>(), read_small_obj_points[3][2].as<float>()));
    auto read_big_obj_points = config["big_obj_points"];
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[0][0].as<float>(), read_big_obj_points[0][1].as<float>(), read_big_obj_points[0][2].as<float>()));
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[1][0].as<float>(), read_big_obj_points[1][1].as<float>(), read_big_obj_points[1][2].as<float>()));
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[2][0].as<float>(), read_big_obj_points[2][1].as<float>(), read_big_obj_points[2][2].as<float>()));
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[3][0].as<float>(), read_big_obj_points[3][1].as<float>(), read_big_obj_points[3][2].as<float>()));

    mtx.at<float>(0,0) = config["mtx"][0][0].as<float>();
    mtx.at<float>(0,1) = config["mtx"][0][1].as<float>();
    mtx.at<float>(0,2) = config["mtx"][0][2].as<float>();
    mtx.at<float>(1,0) = config["mtx"][1][0].as<float>();
    mtx.at<float>(1,1) = config["mtx"][1][1].as<float>();
    mtx.at<float>(1,2) = config["mtx"][1][2].as<float>();
    mtx.at<float>(2,0) = config["mtx"][2][0].as<float>();
    mtx.at<float>(2,1) = config["mtx"][2][1].as<float>();
    mtx.at<float>(2,2) = config["mtx"][2][2].as<float>();

    dist.at<float>(0,0) = config["dist"][0][0].as<float>();
    dist.at<float>(0,1) = config["dist"][0][1].as<float>();
    dist.at<float>(0,2) = config["dist"][0][2].as<float>();
    dist.at<float>(0,3) = config["dist"][0][3].as<float>();
    dist.at<float>(0,4) = config["dist"][0][4].as<float>();
    auto read_small_armor_name_list = config["small_armor_name_list"];
    for (size_t i = 0; i < read_small_armor_name_list.size(); i++)
    {
        small_armor_name_list.push_back(read_small_armor_name_list[i].as<std::string>());
    }
    
    return true;



}
void PNP_Params::print_show_params()
{
    /*

    spdlog::info("mtx: {}, {}, {}, dist: {}, {}, {}, {}, {}",
                 mtx.at<float>(0,0), mtx.at<float>(0,1), mtx.at<float>(0,2),
                 mtx.at<float>(1,0), mtx.at<float>(1,1), mtx.at<float>(1,2),
                 mtx.at<float>(2,0), mtx.at<float>(2,1), mtx.at<float>(2,2));
    spdlog::info("dist: {}, {}",
                 dist.at<float>(0,0), dist.at<float>(0,1), dist.at<float>(0,2),
                 dist.at<float>(0,3), dist.at<float>(0,4));
    spdlog::info("small_armor_name_list: {}", small_armor_name_list);
    */

    for (auto& point : small_obj_points)
    {
        spdlog::info("small_obj_points: x={}, y={}, z={}", point.x, point.y, point.z);
    }
    for (auto& small_armor_name : small_armor_name_list)
    {
        spdlog::info("small_armor_name: {}", small_armor_name);
    }
    for (auto& point : big_obj_points)
    {
        spdlog::info("big_obj_points: x={}, y={}, z={}", point.x, point.y, point.z);
    }
    
    spdlog::info("camera_center: x={}, y={}", camera_center.x, camera_center.y);
    std::cout << "[INFO] : mtx: "<<mtx<<std::endl;
    std::cout << "[INFO] : dist: "<<dist<<std::endl;
    return;
}


int main()
{
    //Mat img = imread("/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/res/armorred.png");
    std::string pnp_config_params_file = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_alpha/config/other_config/pnp_params.yaml";
    int img_show_wid = 640;
    int img_show_hei = 384;
    PNP_Solver pnp_solver(  Mode::Dbg,
                            pnp_config_params_file,
                            img_show_wid,
                            img_show_hei);
    std::vector<Point2f> img_points = {cv::Point2f(100,100),cv::Point2f(150,100),cv::Point2f(150,200),cv::Point2f(100,200)};
    std::vector<float> rvec,tvec;
    pnp_solver("3x",img_points,tvec,rvec);
    std::cout<<"rvec:"<<rvec[0]<<" "<<rvec[1]<<" "<<rvec[2]<<std::endl;
    std::cout<<"tvec:"<<tvec[0]<<" "<<tvec[1]<<" "<<tvec[2]<<std::endl;
    return 0;




    
}

