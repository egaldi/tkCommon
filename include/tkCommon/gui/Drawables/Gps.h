#pragma once
#include "tkCommon/gui/Drawables/Drawable.h"
#include "tkCommon/gui/shader/linesMonocolor.h"
#include "tkCommon/data/GPSData.h"
#include "tkCommon/gui/shader/circle.h"

namespace tk{ namespace gui{

	class Gps : public Drawable {

        private:
            tk::data::GPSData* gps;

            int nPos;
            int lastPos;
            tk::math::VecStatic<tk::gui::shader::circle*,40> circles;

            tk::common::GeodeticConverter geoConv;

            float lineSize = 2.0f;
            bool  update = false;

        public:
            tk::gui::Color_t        color;

            Gps(tk::data::GPSData* gps, int nPos = 10, tk::gui::Color_t color = tk::gui::color::RED){
                this->gps = gps;
                this->color = color;  
                this->nPos = nPos;
                this->lastPos = -1;
            }

            ~Gps(){

            }

            void updateRef(tk::data::GPSData* gps){
                this->gps = gps;   
                update = true;
            }

            void onInit(tk::gui::Viewer *viewer){
                for(int i = 0; i < circles.size(); i++){
                    circles[i] = new tk::gui::shader::circle();
                }
            }

            void draw(tk::gui::Viewer *viewer){
                if(gps->isChanged() || update){
                    update = false;

                    gps->lock();
                    if(!geoConv.isInitialised()) {
                        geoConv.initialiseReference(gps->lat,gps->lon,gps->heigth);
                    }
                    double x, y, z;
                    geoConv.geodetic2Enu(gps->lat,gps->lon,gps->heigth,&x, &y, &z);
                    gps->unlockRead();

                    
                    float RAGGIO = 2.0f; //TODO: BOSI
                    lastPos = (lastPos+1) % nPos;
                    circles[lastPos]->makeCircle(x,y,z,RAGGIO);                
                }

                for(int i = 0; i < nPos; i++){
                    circles[i]->draw(color,lineSize);
                }   	
            }

            void imGuiSettings(){
                ImGui::ColorEdit4("Color", color.color);
                ImGui::SliderFloat("Size",&lineSize,1.0f,20.0f,"%.1f");
                ImGui::SliderInt("Last poses",&nPos,1,40);
            }

            void imGuiInfos(){
                std::stringstream print;
                print<<(*gps);
                ImGui::Text("%s",print.str().c_str());
                print.clear();
            }

            void onClose(){
                tk::gui::shader::linesMonocolor* shaderLines = (tk::gui::shader::linesMonocolor*) shader;
                shaderLines->close();
                delete shader;

                for(int i = 0; i < circles.size(); i++){
                    circles[i]->close();
                    delete circles[i];
                }
            }

            std::string toString(){
                return gps->header.name;
            }
	};
}}