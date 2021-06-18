/*****************************************************************************/
/**
 *  @file   OpenCV.h
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#ifndef KVS__OPENCV__OPENCV_H_INCLUDE
#define KVS__OPENCV__OPENCV_H_INCLUDE

#if defined( KVS_OPENCV4 )
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc_c.h> // for IplImage
#include <opencv2/videoio/videoio_c.h> // for CV_CAP_ANY
#include <opencv2/highgui/highgui_c.h> // for CV_WINDOW_AUTOSIZE etc.
#else
#include <cv.h>
#include <highgui.h>
#endif
#include <kvs/String>


namespace kvs
{

namespace opencv
{

/*===========================================================================*/
/**
 *  @brief  Returns OpenCV description
 *  @return description
 */
/*===========================================================================*/
inline const std::string Description()
{
    const std::string description( "OpenCV - Intel Open Source Computer Vision Library" );
    return description;
}

/*===========================================================================*/
/**
 *  @brief  Returns OpenCV version.
 *  @return OpenCV version
 */
/*===========================================================================*/
inline const std::string Version()
{
    const std::string version = kvs::String::From( CV_VERSION );
    return version;
}

} // end of namespace opencv

} // end of namespace kvs

#endif // KVS__OPENCV__OPENCV_H_INCLUDE
