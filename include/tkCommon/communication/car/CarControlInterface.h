#include "tkCommon/data/ActuationData.h"
#include "tkCommon/communication/car/CarControl.h"
#include "tkCommon/gui/drawables/Drawable.h"
#include "tkCommon/gui/imgui/imgui_internal.h"
#include "tkCommon/rt/Task.h"

namespace tk { namespace communication {

class CarControlInterface : public tk::gui::DrawableUnManaged {
    private:
        tk::communication::CarControl *carCtrl;
        tk::rt::Thread tUpdateLoop;   

        tk::common::PID pid;

        bool active = false;
        bool setZero = false;
        float steerReqDeg = 0;

        float brakeReq = 0;
        float throttleReq = 0;
        float speedReqKMH = 0;
        float speedReq = 0;

        bool speedControl = false;
        bool odomActive = true;

        bool manual = false;
        int recivedInputRequestN = 0;

        bool enableBrake = true, enableThrottle = true, enableSteer = true;

        bool running = true;

    public:
        CarControlInterface() {
        }
        ~CarControlInterface() {}

        bool init(tk::communication::CarControl *carCtrl) {
            this->carCtrl = carCtrl;
            pid.init(0.2, 0, 0, -1.0, 1.0);
            running = true;

            steerReqDeg = 0;
            brakeReq = 0;
            throttleReq = 0;
            speedReqKMH = speedReq = -1;

            tUpdateLoop.init(CarControlInterface::runThread, this);
            return true;
        }

        bool close() {
            running = false;
            tUpdateLoop.join();
            return true;
        }

        void setInput(tk::data::ActuationData &act) {
            if(!manual) {
                recivedInputRequestN++;
                steerReqDeg = act.steerAngle*180.0/M_PI;
                speedReqKMH = act.speed*3.6;
                speedReq = act.speed;
            }
        }

        void setInput(float steerDeg, float speedKMH) {
            if(!manual) {
                recivedInputRequestN++;
                steerReqDeg = steerDeg;
                speedReqKMH = speedKMH;
                speedReq = speedReqKMH/3.6;
            }
        }

        void setActive(bool active) {
            this->active = active;
            tkMSG("Set CAR state: "<<this->active);
            carCtrl->enable(this->active);
        }

        void draw(tk::gui::Viewer *viewer){
            ImGui::Begin("Car control", NULL, ImGuiWindowFlags_NoScrollbar);
            if(ImGui::Button("ENGINE START")) {
                tkMSG("Send engine button");
                carCtrl->sendEngineStart();
            }
            if(ImGui::Button("QUERY ECUs")) {
                tkMSG("Send query");
                carCtrl->requestMotorId();
            }
            //if(ImGui::Button("STEER SET ZERO")) {
            //    tkMSG("Set Steer ZERO");
            //    carCtrl->setSteerZero();
            //}
            if(ImGui::Button("STEER RESET")) {
                tkMSG("Reset steer");
                carCtrl->resetSteerMotor();
            }
            if(ImGui::Checkbox("ACTIVATE", &active)) {
                tkMSG("Set CAR state: "<<active);
                carCtrl->enable(active);
            }
            if(ImGui::Checkbox("ODOM ENABLE", &odomActive)) {
                std::cout<<"Set ODOM state: "<<odomActive<<"\n";
                carCtrl->sendOdomEnable(odomActive);
            }
            ImGui::Checkbox("ENABLE steer", &enableSteer);
            ImGui::Checkbox("ENABLE throttle", &enableThrottle);
            ImGui::Checkbox("ENABLE brake", &enableBrake);


            // status
            ImGui::Text("Speed: %lf kmh", carCtrl->odom.linear_velocity.x()*3.6);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            float steer_act = carCtrl->getActSteer();
            ImGui::SliderFloat("SteerAct", &steer_act, -30, +30);
            ImGui::PopItemFlag();
            ImGui::Text("Steer pos read: %d", carCtrl->steerPos);
            // steer params
            int sOff = carCtrl->getSteerOffset();
            int sAcc = carCtrl->getSteerAcc();
            int sVel = carCtrl->getSteerVel();
            ImGui::InputInt("Param: Steer Offset", &sOff);
            ImGui::InputInt("Param: Steer Acc", &sAcc);
            ImGui::InputInt("Param: Steer Vel", &sVel); 
            carCtrl->setSteerParams(sOff, sAcc, sVel);
            ImGui::NewLine();

            ImVec4 ca = {0.1f,0.9f,0.1f,1.0f};
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ca);
            ImGui::ProgressBar(carCtrl->getActThrottle(), ImVec2(0.0f, 0.0f));
            ImGui::PopStyleColor();
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("ThrottleAct");
            ca = {0.9f,0.1f,0.1f,1.0f};
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ca);
            ImGui::ProgressBar(carCtrl->getActBrake(), ImVec2(0.0f, 0.0f));
            ImGui::PopStyleColor();
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("BrakeAct");

            // input type
            ImGui::NewLine();
            

            // manual
            if(ImGui::Checkbox("Manual", &manual)) {
                // reset inputs
                steerReqDeg = 0;
                brakeReq = 0;
                throttleReq = 0;
                speedReqKMH = 0;
                speedReq = 0;
            }
            if(manual) {
                ImGui::SliderFloat("Steer", &steerReqDeg, -30, +30);
                ImGui::InputFloat("Steer_", &steerReqDeg, -30, +30);

                ImGui::NewLine();
                ImGui::Checkbox("SpeedControl", &speedControl);
                if(!speedControl) {
                    ImGui::SliderFloat("Brake", &brakeReq, 0, 1.0);
                    ImGui::InputFloat("Brake_", &brakeReq, 0, 1.0);
                    ImGui::SliderFloat("Throttle", &throttleReq, 0, 1.0);
                    ImGui::InputFloat ("Throttle_", &throttleReq, 0, 1.0);
                } else {
                    ImGui::SliderFloat("Speed kmh", &speedReqKMH, -1, 40);
                    speedReq = speedReqKMH/3.6;
                }

            }
            else {
                speedControl = true;
                ImGui::Text("Getting act from code");
                ImGui::Text("Requested Speed: %f", speedReqKMH);
                ImGui::Text("Requested Steer: %f", steerReqDeg);
                ImGui::Text("requests: %d", recivedInputRequestN);
            }
            ImGui::End();

       }


        static void* runThread(void*data) {
            CarControlInterface *self = (CarControlInterface *)data;
            self->run();
            pthread_exit(NULL);
        }

        void run() {
            tkWRN("Start car control interface");
            tk::rt::Task t;
            t.init(30000);
            while (running) {

                if(active) {
                    if(speedControl) {
                        static timeStamp_t lastTS = getTimeStamp();
                        timeStamp_t thisTS = getTimeStamp();
                        double dt = double(thisTS - lastTS)/1000000.0;
                        lastTS = thisTS;
                        double act = pid.calculate(dt, speedReq - carCtrl->odom.linear_velocity.x());
                        float preBrake = 0.40;
                        if(speedReq < 0 && carCtrl->odom.linear_velocity.x() < 1)
                            preBrake = 0.6;
                        if(speedReq < 0 && carCtrl->odom.linear_velocity.x() <= 0.01)
                            preBrake = 0.7;
                        if(act < 0) {
                            brakeReq = preBrake - (act);
                            throttleReq = 0;
                        } else {
                            throttleReq = act;
                            brakeReq = preBrake;
                        }
                    }


                    carCtrl->setTargetSteer(enableSteer * steerReqDeg);
                    carCtrl->setTargetBrake(enableBrake * brakeReq);
                    carCtrl->setTargetThrottle(enableThrottle * throttleReq);
                }
                t.wait();
            }
        }
        
};

}}
