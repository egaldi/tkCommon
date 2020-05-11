#pragma once

#include "tkCommon/data/SensorData.h"
#include <tkCommon/common.h>

namespace tk { namespace data {

    /**
     *  Vehicle data definition
     */
    class VehicleData : public SensorData {
        
        public:

        double CAR_WHEELBASE;                   /** car wheelbase        [m]*/
        double CAR_DIM_X, CAR_DIM_Y, CAR_DIM_Z; /** car dimensions       [m]*/
        double CAR_BACK2AXLE;                   /** from back to axle    [m]*/
        double CAR_MASS;                        /** car weight           [kg]*/
        double CAR_FRONTAXLE_W;                 /** width of front axle  [m]*/
        double CAR_BACKAXLE_W;                  /** width of back axle   [m]*/
        double CAR_WHEEL_R;                     /** wheel radius         [m]*/

        double     speed    = 0;                /** speed                [ m/s   ]*/
        double     speedKMH = 0;                /** speed                [ Km/h  ]*/

        double     yawRate = 0;                 /** yaw rate             [ rad/s ]*/
        double     accelX  = 0;                 /** longitudinal acc     [ m/s2  ]*/
        double     accelY  = 0;                 /** lateral acc          [ m/s2  ]*/

        double     steerAngle     = 0;          /** steering wheel angle [ rad   ]*/
        double     steerAngleRate = 0;          /** steering wheel angle [ rad/s ]*/
        double     wheelAngle     = 0;          /** wheel angle          [ rad   ]*/

        uint8_t    brakePedalSts        = 0;    /** brake status         [ on/off ]*/
        double     brakeMasterPressure  = 0;    /** brake pressure       [ bar ]*/
        double     gasPedal             = 0;    /** gas padal percentage [ 0-1 ]*/
        double     engineTorque         = 0;    /** engine torque        [ % ]*/
        double     engineFrictionTorque = 0;    /** friction torque      [ % ]*/

        uint8_t    actualGear = 0;              /** current Gear         [ int ]*/
        uint16_t   RPM = 0;                     /** RPM                  [ int ]*/

        /**wheel speeds         [ m/s ]*/
        double     wheelFLspeed =0, wheelFRspeed =0, wheelRLspeed =0, wheelRRspeed =0;
        /**wheel dir            [int] ** 0:no_dir   1:forward   2:backward   4:invalid*/
        uint8_t    wheelFLDir =0, wheelFRDir =0, wheelRLDir =0, wheelRRDir =0;


        double     sideSlip     = 0;        /** beta angle               [ rad   ]*/
        uint8_t    tractionGrip = 0;        /** traction control grip    [ ? ]*/

        // faults
        uint8_t ESPFault = 0;

        // inputs
        uint8_t activateDrive = 0, activateSteer = 0;

        /** Odometry vales in space*/
        long double x = 0;
        long double y = 0;

        /** Direction of car in space*/
        long double carDirection = 0;

        /** Boolean variable for know if new data is arrived*/
        bool Bspeed = 0, Bsteer = 0;
        int  posBuffer = 0;

        #define ODOM_BUFFER_SIZE 10
        struct odom_t {
            long double x = 0;
            long double y = 0;
            long double z = 0;
            long double yaw = 0;
            uint64_t t = 0;
        } odometryBuffer[ODOM_BUFFER_SIZE];

        bool carOdometry(odom_t &odom, uint64_t time = 0){

            int bufPos = posBuffer;
            for(int i=1; i<ODOM_BUFFER_SIZE; i++) {
                int c = (bufPos - i) % ODOM_BUFFER_SIZE;
                if(c <0)
                    c += ODOM_BUFFER_SIZE;

                if(odometryBuffer[c].t < time || time == 0){
                    odom = odometryBuffer[c];
                    //if(odom.t == 0)
                    //    return false;
                    return true;
                }
            }
            return false;
        }

        bool carOdometry(tk::common::Tfpose &tf, uint64_t time = 0){
            odom_t odom;
            bool status = carOdometry(odom, time);
            tf = tk::common::odom2tf(odom.x, odom.y, odom.yaw);
            return status;
        }

        void init() override{
            tk::data::SensorData::init();
            header.sensor = sensorName::VEHICLE;
        }

        void release() override {}

        bool checkDimension(SensorData *s) override {
            return true;
        }

        VehicleData& operator=(const VehicleData &s) {
            SensorData::operator=(s);

            this->CAR_WHEELBASE         = s.CAR_WHEELBASE;             
            this->CAR_DIM_X             = s.CAR_DIM_X;    
            this->CAR_DIM_Y             = s.CAR_DIM_Y;
            this->CAR_DIM_Z             = s.CAR_DIM_Z;
            this->CAR_BACK2AXLE         = s.CAR_BACK2AXLE;          
            this->CAR_MASS              = s.CAR_MASS;      
            this->CAR_FRONTAXLE_W       = s.CAR_FRONTAXLE_W;         
            this->CAR_BACKAXLE_W        = s.CAR_BACKAXLE_W;        
            this->CAR_WHEEL_R           = s.CAR_WHEEL_R;
            this->speed                 = s.speed;
            this->speedKMH              = s.speedKMH;
            this->yawRate               = s.yawRate;
            this->accelX                = s.accelX;
            this->accelY                = s.accelY;
            this->steerAngle            = s.steerAngle;
            this->steerAngleRate        = s.steerAngleRate;
            this->wheelAngle            = s.wheelAngle;
            this->brakePedalSts         = s.brakePedalSts;
            this->brakeMasterPressure   = s.brakeMasterPressure;
            this->gasPedal              = s.gasPedal;
            this->engineTorque          = s.engineTorque;
            this->engineFrictionTorque  = s.engineFrictionTorque;
            this->actualGear            = s.actualGear;
            this->RPM                   = s.RPM;
            this->wheelFLspeed          = s.wheelFLspeed;
            this->wheelFRspeed          = s.wheelFRspeed;
            this->wheelRLspeed          = s.wheelRLspeed;
            this->wheelRRspeed          = s.wheelRRspeed;
            this->wheelFLDir            = s.wheelFLDir;
            this->wheelFRDir            = s.wheelFRDir;
            this->wheelRLDir            = s.wheelRLDir;
            this->wheelRRDir            = s.wheelRRDir;
            this->sideSlip              = s.sideSlip;
            this->tractionGrip          = s.tractionGrip;
            this->ESPFault              = s.ESPFault;
            this->activateDrive         = s.activateDrive;
            this->activateSteer         = s.activateSteer;
            this->x                     = s.x;
            this->y                     = s.y;
            this->carDirection          = s.carDirection;
            this->Bspeed                = s.Bspeed;
            this->Bsteer                = s.Bsteer;
            this->posBuffer             = s.posBuffer;

            for(int i=0; i < ODOM_BUFFER_SIZE; i++){
                this->odometryBuffer[i].x   = s.odometryBuffer[i].x;
                this->odometryBuffer[i].y   = s.odometryBuffer[i].y;
                this->odometryBuffer[i].z   = s.odometryBuffer[i].z;
                this->odometryBuffer[i].yaw = s.odometryBuffer[i].yaw;
                this->odometryBuffer[i].t   = s.odometryBuffer[i].t;
            }

            return *this;
         }


        friend std::ostream& operator<<(std::ostream& os, const VehicleData& m) {
            os << m.header.stamp<< " speed: "<<m.speed<<" wheelangle: "<<m.wheelAngle;
            return os;
        }

        static const int VEHICLE_FIELDS = 25;
        const char  *fields[VEHICLE_FIELDS] = {"stamp",
           "CAR_WHEELBASE", "CAR_DIM_X", "CAR_DIM_Y", "CAR_DIM_Z", "CAR_BACK2AXLE", "CAR_MASS", "CAR_FRONTAXLE_W", "CAR_BACKAXLE_W",
           "CAR_WHEEL_R", "speed", "yawRate", "accelX", "accelY", "wheelAngle", "brakeMasterPressure", "gasPedal", "engineTorque",
           "actualGear_d", "wheelFLspeed", "wheelFRspeed", "wheelRLspeed", "wheelRRspeed", "sideSlip", "tractionGrip_d"};

        /**
         *
         * @param name
         * @return
         */
        matvar_t *toMatVar(std::string name = "gps") {

            #define TK_VEHDATA_MATVAR_DOUBLE(x) var = Mat_VarCreate(#x, MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dim, &x, 0); \
                                                Mat_VarSetStructFieldByName(matstruct, #x, 0, var); //0 for first row

            size_t dim[2] = { 1, 1 }; // create 1x1 struct
            matvar_t* matstruct = Mat_VarCreateStruct(name.c_str(), 2, dim, fields, VEHICLE_FIELDS); //main struct: Data

            matvar_t *var = Mat_VarCreate("stamp", MAT_C_UINT64, MAT_T_UINT64, 2, dim, &header.stamp, 0);
            Mat_VarSetStructFieldByName(matstruct, "stamp", 0, var); //0 for first row
            TK_VEHDATA_MATVAR_DOUBLE(CAR_WHEELBASE);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_DIM_X);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_DIM_Y);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_DIM_Z);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_BACK2AXLE);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_MASS);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_FRONTAXLE_W);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_BACKAXLE_W);
            TK_VEHDATA_MATVAR_DOUBLE(CAR_WHEEL_R);
            TK_VEHDATA_MATVAR_DOUBLE(speed);
            TK_VEHDATA_MATVAR_DOUBLE(yawRate);
            TK_VEHDATA_MATVAR_DOUBLE(accelX);
            TK_VEHDATA_MATVAR_DOUBLE(accelY);
            TK_VEHDATA_MATVAR_DOUBLE(wheelAngle);
            TK_VEHDATA_MATVAR_DOUBLE(brakeMasterPressure);
            TK_VEHDATA_MATVAR_DOUBLE(gasPedal);
            TK_VEHDATA_MATVAR_DOUBLE(engineTorque);
            double actualGear_d = actualGear;
            TK_VEHDATA_MATVAR_DOUBLE(actualGear_d);
            TK_VEHDATA_MATVAR_DOUBLE(wheelFLspeed);
            TK_VEHDATA_MATVAR_DOUBLE(wheelFRspeed);
            TK_VEHDATA_MATVAR_DOUBLE(wheelRLspeed);
            TK_VEHDATA_MATVAR_DOUBLE(wheelRRspeed);
            TK_VEHDATA_MATVAR_DOUBLE(sideSlip);
            double tractionGrip_d = tractionGrip;
            TK_VEHDATA_MATVAR_DOUBLE(tractionGrip_d);
            return matstruct;
        }

        /**
         *
         * @param var
         * @return
         */
        bool fromMatVar(matvar_t *var) {

            matvar_t *pvar = Mat_VarGetStructFieldByName(var, "stamp", 0);
            tkASSERT(pvar->class_type == MAT_C_UINT64);
            memcpy(&header.stamp, pvar->data, sizeof(uint64_t));

            #define TK_VEHDATA_MATVAR_READ_DOUBLE(x) pvar = Mat_VarGetStructFieldByName(var, #x, 0); \
                                                     tkASSERT(pvar != 0); \
                                                     tkASSERT(pvar->class_type == MAT_C_DOUBLE); \
                                                     memcpy(&x, pvar->data, sizeof(double));
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_WHEELBASE);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_DIM_X);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_DIM_Y);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_DIM_Z);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_BACK2AXLE);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_MASS);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_FRONTAXLE_W);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_BACKAXLE_W);
            TK_VEHDATA_MATVAR_READ_DOUBLE(CAR_WHEEL_R);
            TK_VEHDATA_MATVAR_READ_DOUBLE(speed);
            TK_VEHDATA_MATVAR_READ_DOUBLE(yawRate);
            TK_VEHDATA_MATVAR_READ_DOUBLE(accelX);
            TK_VEHDATA_MATVAR_READ_DOUBLE(accelY);
            TK_VEHDATA_MATVAR_READ_DOUBLE(wheelAngle);
            TK_VEHDATA_MATVAR_READ_DOUBLE(brakeMasterPressure);
            TK_VEHDATA_MATVAR_READ_DOUBLE(gasPedal);
            TK_VEHDATA_MATVAR_READ_DOUBLE(engineTorque);
            double actualGear_d;
            TK_VEHDATA_MATVAR_READ_DOUBLE(actualGear_d);
            actualGear = actualGear_d;
            TK_VEHDATA_MATVAR_READ_DOUBLE(wheelFLspeed);
            TK_VEHDATA_MATVAR_READ_DOUBLE(wheelFRspeed);
            TK_VEHDATA_MATVAR_READ_DOUBLE(wheelRLspeed);
            TK_VEHDATA_MATVAR_READ_DOUBLE(wheelRRspeed);
            TK_VEHDATA_MATVAR_READ_DOUBLE(sideSlip);
            double tractionGrip_d;
            TK_VEHDATA_MATVAR_READ_DOUBLE(tractionGrip_d);
            tractionGrip = tractionGrip_d;
        }

        void draw(tk::gui::Viewer *viewer){
			tk::gui::Viewer::tkDrawTf(header.name, header.tf);
			tk::gui::Viewer::tkSetColor(tk::gui::color::AMBER);
			tk::common::Vector3<float> dim{(float) CAR_DIM_X, (float) CAR_DIM_Y,
										   (float) CAR_DIM_Z};
			tk::common::Vector3<float> wheels[4];
			wheels[0] = {0, (float) -CAR_BACKAXLE_W / 2, 0};
			wheels[1] = wheels[0];
			wheels[1].y *= -1;
			wheels[2] = wheels[0], wheels[3] = wheels[1];
			wheels[2].x += CAR_WHEELBASE;
			wheels[3].x += CAR_WHEELBASE;

			tk::gui::Viewer::tkDrawLine(wheels[0], wheels[1]);
			tk::gui::Viewer::tkDrawLine(wheels[2], wheels[3]);
			tk::gui::Viewer::tkDrawLine(tk::common::Vector3<float>{wheels[0].x, 0, 0},
										tk::common::Vector3<float>{wheels[2].x, 0, 0});

			for (int i = 0; i < 4; i++) {
				glPushMatrix();
				glTranslatef(wheels[i].x, wheels[i].y, wheels[i].z);
				glRotatef(90, 1, 0, 0);
				tk::gui::Viewer::tkDrawCircle(tk::common::Vector3<float>{0, 0, 0}, CAR_WHEEL_R);
				glPopMatrix();
			}
			tk::common::Vector3<float> pose{dim.x / 2 - (float) CAR_BACK2AXLE, 0, dim.z / 2};

        }

        void draw2D(tk::gui::Viewer *viewer) {

			int width = viewer->width;
			int height = viewer->height;
			float xLim = viewer->xLim;
			float yLim = viewer->yLim;

			tk::common::Vector2<float> pos;
			float size;
			pos = tk::common::Vector2<float>{0,-yLim+0.25f};
			size = 0.2;
			double speed = this->speedKMH;
			int gear = actualGear;
			double rpm = RPM;

			glPushMatrix();{
				tk::gui::Viewer::tkViewport2D(width, height);
				tk::gui::Viewer::tkDrawSpeedometer(pos, speed, size);
			}glPopMatrix();
        }
    };
}
}