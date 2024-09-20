
#include <opencv2/opencv.hpp>

static cv::Mat WrapTransform(float* transform)
{
    return cv::Mat(4, 4, CV_32FC1, transform);
}

static cv::Mat WrapPoints1(float* points, int count)
{
    return cv::Mat(count, 1, CV_32FC1, points);
}

static cv::Mat WrapPoints2(float* points, int count)
{
    return cv::Mat(count, 1, CV_32FC2, points);
}

static cv::Mat WrapPoints3(float* points, int count)
{
    return cv::Mat(count, 1, CV_32FC3, points);
}

void TransformIdentity(float* transform_out)
{
    cv::Mat T = WrapTransform(transform_out);
    cv::setIdentity(T);
}

void TranformInvert(float *transform_in, float* transform_out)
{
    cv::Mat in  = WrapTransform(transform_in);
    cv::Mat out = WrapTransform(transform_out);

    cv::invert(in, out);
}

void TransformMultiply(float* transform_in_A, float* transform_in_B, float* transform_out)
{
    cv::Mat in_L = WrapTransform(transform_in_A);
    cv::Mat in_R = WrapTransform(transform_in_B);
    cv::Mat out  = WrapTransform(transform_out);

    cv::gemm(in_L, in_R, 1.0, cv::noArray(), 0.0, out);
}

void TransformPoints3(float* transform, float* points_in, int count, float* points_out)
{
    cv::Mat T = WrapTransform(transform)(cv::Range(0, 4), cv::Range(0, 3));
    cv::Mat p = WrapPoints3(points_in,  count);
    cv::Mat q = WrapPoints3(points_out, count);

    cv::transform(p, q, T.t());
}

void GetPoints2Channel(float* points_in, int count, int channel, float* points_out)
{
    int x[] = { channel, 0 };

    cv::Mat p = WrapPoints2(points_in, count);
    cv::Mat q = WrapPoints1(points_out, count);

    cv::mixChannels(&p, 1, &q, 1, x, 1);
}

void GetPoints3Channel(float* points_in, int count, int channel, float* points_out)
{
    int x[] = { channel, 0 };

    cv::Mat p = WrapPoints3(points_in,  count);
    cv::Mat q = WrapPoints1(points_out, count);

    cv::mixChannels(&p, 1, &q, 1, x, 1);
}

void ProjectPoints3(float* intrinsics, float* transform, float* points_in, int count, float* points_out)
{
    cv::Mat K = cv::Mat(3, 3, CV_32FC1, intrinsics);
    cv::Mat T = WrapTransform(transform);
    cv::Mat p = WrapPoints3(points_in,  count);
    cv::Mat q = WrapPoints2(points_out, count);

    cv::Mat R    = T(cv::Range(0, 3), cv::Range(0, 3));
    cv::Mat tvec = T(cv::Range(3, 4), cv::Range(0, 3));    
    cv::Mat rvec;

    cv::Rodrigues(R.t(), rvec);

    cv::projectPoints(p, rvec, tvec, K, cv::noArray(), q);
}
