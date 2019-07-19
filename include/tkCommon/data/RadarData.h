#pragma once
#include <tkCommon/common.h>

namespace tk { namespace data {

    #define RADAR_MAX_POINTS 1000
    #define RADAR_MAX_FEATURES 10
    #define N_RADAR 6

    enum RadarFeatureType_t { VELOCITY      = 0,
                              FALSE_DET     = 1,
                              RANGE_VAR     = 2,
                              VELOCITY_VAR  = 3,
                              ANGLE_VAR     = 4
                            };

    struct RadarData_t {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        Eigen::Matrix<float, 4, RADAR_MAX_POINTS, Eigen::ColMajor> near_points[N_RADAR];
        Eigen::Matrix<float, 4, RADAR_MAX_POINTS, Eigen::ColMajor> far_points[N_RADAR];
        timeStamp_t             near_stamp[N_RADAR];
        timeStamp_t             far_stamp[N_RADAR];
        int                     near_n_points[N_RADAR];
        int                     far_n_points[N_RADAR];
        float                   features[N_RADAR][RADAR_MAX_FEATURES];
        
        RadarData_t& operator=(const RadarData_t& s){

            for(int i = 0; i < N_RADAR; i++){

                near_stamp[i]       = s.near_stamp[i];
                far_stamp[i]        = s.far_stamp[i];
                near_n_points[i]    = s.near_n_points[i];
                far_n_points[i]     = s.far_n_points[i];

                std::memcpy(near_points[i].data(),  s.near_points[i].data(),    near_n_points[i] * 4 * sizeof(float));
                std::memcpy(far_points[i].data(),   s.far_points[i].data(),     far_n_points[i] * 4 * sizeof(float));
                std::memcpy(features[i], s.features[i], RADAR_MAX_FEATURES * sizeof(float));
            }
        }
    };

}}
