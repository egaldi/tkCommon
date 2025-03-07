#include "tkCommon/gui/drawables/Prisms.h"

tk::gui::Prisms::Prisms(tk::gui::Color_t color, std::string name){
    this->name      = name;
    this->update    = false;
    this->color     = color;
    this->color.a() = 90.f/255.f;
}

tk::gui::Prisms::Prisms(tk::common::Prisms& prisms,tk::gui::Color_t color, std::string name) : Prisms(color, name){
    this->ref     = &prisms;
    this->update  = true;
}

tk::gui::Prisms::~Prisms(){

}

void 
tk::gui::Prisms::updateData(tk::common::Prisms& prisms){
    this->ref     = &prisms;
    this->update  = true;
}

void 
tk::gui::Prisms::onInit(tk::gui::Viewer *viewer){
}

void 
tk::gui::Prisms::drawPrisms(tk::gui::Viewer *viewer){

    for(int i = 0; i < prisms.data.size(); i++){
        auto points = prisms.data[i].points;
        auto base_z = prisms.data[i].base_z;
        auto height = prisms.data[i].height;

        glPushMatrix();{
            glMultMatrixf(glm::value_ptr(glm::make_mat4x4((tf * prisms.tf).matrix().data())));
            glDepthMask(GL_FALSE);
            if(i < colors.size()){
                glColor4f(colors[i].r(),colors[i].g(),colors[i].b(),color.a());
            }
            else{
                glColor4f(color.r(),color.g(),color.b(),color.a());
            }
            glBegin(GL_POLYGON);
            for(int i = points.size()-1;i >=0; i--){
                glVertex3f(points[i].x(), points[i].y(), base_z);
            }
            glEnd();
            for(int i = 0; i < points.size(); i++){
                glBegin(GL_POLYGON);
                glVertex3f(points[i].x(), points[i].y(), base_z);
                glVertex3f(points[i].x(), points[i].y(), base_z+height);
                glVertex3f(points[(i+1)%points.size()].x(), points[(i+1)%points.size()].y(), base_z+height);
                glVertex3f(points[(i+1)%points.size()].x(), points[(i+1)%points.size()].y(), base_z);
                glEnd();
            }
            glBegin(GL_POLYGON);
            for(int i = points.size()-1;i >=0; i--){
                glVertex3f(points[i].x(), points[i].y(), base_z + height);
            }
            glEnd();
            glDepthMask(GL_TRUE);

            if(i < colors.size()){
                glColor4f(colors[i].r(),colors[i].g(),colors[i].b(),color.a());
            }
            else{
                glColor4f(color.r(),color.g(),color.b(),color.a());
            }
            glLineWidth(2);
            glBegin(GL_LINES);
            for(int i = 0 ;i <points.size(); i++){
                glVertex3f(points[i].x(), points[i].y(), base_z);
                glVertex3f(points[(i+1)%points.size()].x(), points[(i+1)%points.size()].y(), base_z);
            }
            for(int i = 0; i < points.size(); i++){
                glVertex3f(points[i].x(), points[i].y(), base_z);
                glVertex3f(points[i].x(), points[i].y(), base_z+height);
            }
            for(int i = 0 ;i <points.size(); i++){
                glVertex3f(points[i].x(), points[i].y(), base_z+height);
                glVertex3f(points[(i+1)%points.size()].x(), points[(i+1)%points.size()].y(), base_z+height);
            }
            glEnd();
            glLineWidth(1);
        }glPopMatrix();
    }
}

void 
tk::gui::Prisms::draw(tk::gui::Viewer *viewer){
    if(ref->isChanged(counter) || update){
        update      = false;

        ref->lockRead();
        prisms.data.resize(ref->data.size());
        prisms.tf = ref->tf;
        for(int i = 0; i < ref->data.size(); i++){
            prisms.data[i].height = ref->data[i].height;
            prisms.data[i].points = ref->data[i].points;
            prisms.data[i].base_z = ref->data[i].base_z;
        }
        ref->unlockRead();
    }

    drawPrisms(viewer);
}

void 
tk::gui::Prisms::imGuiInfos(){
    ImGui::Text("3D prisms drawable");
}

void 
tk::gui::Prisms::imGuiSettings(){
    ImGui::ColorEdit4("Color", color.color);
}

void 
tk::gui::Prisms::onClose(){
}