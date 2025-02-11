#pragma once

#include "tkCommon/common.h"
#include "tkCommon/math/MatIO.h"

#ifdef TKROS_ENABLED
#if TKROS_VERSION == 1
#include <std_msgs/Header.h>
#endif
#if TKROS_VERSION == 2
#include <std_msgs/msg/header.hpp>
#endif
#endif

namespace tk { namespace data {

    
    enum class DataType : uint32_t{
        NOT_SPEC        = 0,
        CLOUD           = 1,
        VEHICLE         = 2,
        GPS             = 3,
        IMAGE           = 4,
        RADAR           = 5,
        LINES           = 6,
        PERCEPTION      = 7,
        IMU             = 8,
        GPSIMU          = 9,
        STEREO          = 10,
        CAN             = 11,
        VECTOR          = 12,
        ACTUATION       = 13,
        DEPTH           = 14,
        ODOM            = 15,
        IMAGEU8         = 16,
        IMAGEU16        = 17,
        IMAGEF          = 18,
        SONAR           = 19,
        OCCUPANCY_GRID  = 20,
        KISTLER         = 21,
        POSE            = 22,
        TWIST           = 23,
        PATH            = 24
    };

    static const char* ToStr (const DataType& type)
    {
        switch (type) {
            case DataType::CLOUD:           return "cloud";
            case DataType::VEHICLE:         return "vehicle";
            case DataType::GPS:             return "gps";
            case DataType::IMAGE:           return "image";
            case DataType::IMAGEU8:         return "imageU8";
            case DataType::IMAGEU16:        return "imageU16";
            case DataType::IMAGEF:          return "imageF";
            case DataType::RADAR:           return "radar";
            case DataType::LINES:           return "lines";
            case DataType::PERCEPTION:      return "perception";
            case DataType::IMU:             return "imu";
            case DataType::GPSIMU:          return "gps&imu";
            case DataType::STEREO:          return "stereo";
            case DataType::CAN:             return "can";
            case DataType::VECTOR:          return "vector";
            case DataType::ACTUATION:       return "actuation";
            case DataType::DEPTH:           return "depth";
            case DataType::ODOM:            return "odom";
            case DataType::SONAR:           return "sonar";
            case DataType::OCCUPANCY_GRID:  return "occupancy_grid";
            case DataType::KISTLER:         return "kistler";
            case DataType::POSE:            return "pose";
            case DataType::TWIST:           return "twist";
            case DataType::PATH:            return "path";
            default:                        return "???";
                
        }
    }

    /**
     * @brief Header data class.
     * Standard metadata for higher-level data class.
     */
    class HeaderData : public tk::math::MatDump {
    public:
        std::string         name   = "";                             /**< Name of the sensor. */
        tk::common::Tfpose  tf     = tk::common::Tfpose::Identity(); /**< TF in respect to back axel, @see tk::common::Tfpose. */
        DataType            type;
        
        timeStamp_t         stamp     = 0; /**< Time stamp, expressed in microseconds. */
        int                 sensorID  = 0; /**< ID of the sensor. */
        int                 messageID = 0; /**< Incremental message counter. */

        float               fps = 0;

        void init() {
            this->stamp         = 0;
            this->tf            = tk::common::Tfpose::Identity();
            this->sensorID      = 0;
            this->messageID     = 0;
            this->type          = DataType::NOT_SPEC;
            this->fps           = 0;
            this->name          = "???";
        }

        /**
         * @brief Overloading of operator = for class copy.
         *
         * @param s
         * @return
         */
        HeaderData &operator=(const HeaderData &s) {
            this->stamp         = s.stamp;
            this->sensorID      = s.sensorID;
            this->messageID     = s.messageID;
            this->type          = s.type;
            this->fps           = s.fps;

            // safe string update
            if(this->name != s.name) {
                /*tkWRN("Changing header name from " + name + " to " + s.name +
                       " if this message appers it is a problem\n");*/
                // this WARNING could be caused by the use of a tmpData in the sensor
                // if you are using tmpData please be sure to not rewrite the header name
                this->name = s.name;
            }
            // safe TF update
            tf = s.tf;

            return *this;
        }

        bool toVar(std::string name, tk::math::MatIO::var_t &var) {
            std::vector<tk::math::MatIO::var_t> structVars(3);
            structVars[0].set("stamp", stamp);
            structVars[1].set("tf", tf.matrix());
            structVars[2].set("name", this->name);
            return var.setStruct(name, structVars);
        }
        bool fromVar(tk::math::MatIO::var_t &var) {
            if(var.empty())
                return false;
            var["stamp"].get(stamp);
            var["tf"].get(tf.matrix());
            var["name"].get(this->name);
            return true;
        }

#ifdef TKROS_ENABLED
#if TKROS_VERSION == 1
        void toRos(std_msgs::Header &msg) const {
            msg.seq          = this->messageID;
            msg.stamp.sec    = this->stamp / 1e6;
            msg.stamp.nsec   = (this->stamp - msg.stamp.sec*1e6) * 1e3;
#endif
#if TKROS_VERSION == 2
        void toRos(std_msgs::msg::Header &msg) const {
            msg.stamp.sec    = this->stamp / 1e6;
            msg.stamp.nanosec  = (this->stamp - msg.stamp.sec*1e6) * 1e3;
#endif
            msg.frame_id     = this->name;
        }

#if TKROS_VERSION == 1
        void fromRos(const std_msgs::Header &msg) {
            this->messageID  = msg.seq;
            this->stamp      = msg.stamp.toNSec() / 1e3;
#endif
#if TKROS_VERSION == 2
        void fromRos(const std_msgs::msg::Header &msg) {
            this->stamp      = msg.stamp.nanosec / 1e3;
#endif
            this->name       = msg.frame_id;
        }
#endif
    };
}}
