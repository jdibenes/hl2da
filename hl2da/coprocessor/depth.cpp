
#include <opencv2/opencv.hpp>
#include "../hl2da/research_mode.h"

struct float2
{
    float x, y;
};

struct float3
{
    float x, y, z;
};

cv::Mat g_rays[] = { cv::Mat(RM_ZHT_HEIGHT, RM_ZHT_WIDTH, CV_32FC3), cv::Mat(RM_ZLT_HEIGHT, RM_ZLT_WIDTH, CV_32FC3) };

static bool RM_DepthGetIndex(int id, int& index)
{
    switch (id)
    {
    case DEPTH_AHAT:       index = 0; break;
    case DEPTH_LONG_THROW: index = 1; break;
    default: return false;
    }

    return true;
}

void RM_DepthInitializeRays(int id, float* uv2xy)
{
    int index;

    if (!RM_DepthGetIndex(id, index)) { return; }

    cv::Mat& rays = g_rays[index];

    float* uv2x = uv2xy;
    float* uv2y = uv2xy + (rays.rows * rays.cols);

    for (int row = 0; row < rays.rows; ++row)
    {
    for (int col = 0; col < rays.cols; ++col)
    {
    int i = (row * rays.cols) + col;
    ((float3*)rays.data)[i] = { uv2x[i], uv2y[i], 1.0f };
    }
    }

    cv::normalize(rays, rays);
}

void RM_DepthNormalize(int id, uint16_t* depth_in, float* depth_out)
{
    int index;

    if (!RM_DepthGetIndex(id, index)) { return; }

    cv::Mat& rays = g_rays[index];

    cv::Mat in  = cv::Mat(rays.rows, rays.cols, CV_16UC1, depth_in);
    cv::Mat out = cv::Mat(rays.rows, rays.cols, CV_32FC1, depth_out);

    in.convertTo(out, CV_32FC1, 1.0 / 1000.0);
}

void RM_DepthTo3D(int id, float* depth, float* points)
{
    int index;

    if (!RM_DepthGetIndex(id, index)) { return; }

    cv::Mat& rays = g_rays[index];

    cv::Mat in  = cv::Mat(rays.rows, rays.cols, CV_32FC1, depth);
    cv::Mat in3 = cv::Mat(rays.rows, rays.cols, CV_32FC3);

    cv::cvtColor(in, in3, cv::COLOR_GRAY2BGR);

    cv::Mat out = cv::Mat(rays.rows, rays.cols, CV_32FC3, points);
    
    cv::multiply(rays, in3, out);
}

void RM_DepthFill1(int id, float* depth, float* image_points, float* local_depths, float* out, int width, int height)
{
    int index;

    if (!RM_DepthGetIndex(id, index)) { return; }

    cv::Mat& rays = g_rays[index];

    float2* uv = (float2*)image_points;
    int br = rays.cols + 1;

    for (int row = 0; row < rays.rows - 1; ++row)
    {
        int b1 = row * rays.cols;
    for (int col = 0; col < rays.cols - 1; ++col)
    {
        int i1 = b1 + col;
        int i2 = i1 + br;
        if (depth[i1] <= 0) { continue; }
        if (depth[i2] <= 0) { continue; }
        float2 uv1 = uv[i1];
        float2 uv2 = uv[i2];
        float z1 = local_depths[i1];
        //float z2 = local_depths[i2];
        for (int v = (int)uv1.y; v < (int)uv2.y; ++v)
        {
        if ((v < 0) || (v >= height)) { continue; }
        for (int u = (int)uv1.x; u < (int)uv2.x; ++u)
        {
        if ((u < 0) || (u >= width)) { continue; }
        out[(v * width) + u] = z1;
        }
        }
    }
    }
}
