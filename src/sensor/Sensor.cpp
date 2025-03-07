#include "tkCommon/sensor/Sensor.h"

namespace tk { namespace sensors {

void serial_thread(tk::communication::SerialPort *serial, bool *mRun){

    *mRun = true;
    while(*mRun){
        std::string msg;
        serial->readLine(msg);
        //std::cout<<msg<<"\n";
    }

}

Clock::Clock()
{
    synched = false;
}

Clock::~Clock()
{

}
        
void 
Clock::init(const YAML::Node conf)
{   
    port     = tk::common::YAMLgetConf<std::string>(conf, "port", "/dev/ttyUSB0");
    baud     = tk::common::YAMLgetConf<int>(conf, "baud", 115200);
    lines    = tk::common::YAMLgetConf<std::vector<int>>(conf, "lines", std::vector<int>(0));
    timezone = tk::common::YAMLgetConf<int>(conf, "timezone", 0);
}

void 
Clock::start(timeStamp_t start)
{
    // SynchBox
    if (start == 0) {
        // open serial port
        if (!serial.init(port, baud)) {
            tkWRN("Cannot open communication with SynchBox.\n");
            synched = false;
            return;
        } else {
            synched = true;
        }

        //Send data
        std::string msg = "\\gse\n";
        serial.write(msg);
        std::string ts_0;
        while(ts_0.find("$GPRMC") == std::string::npos){
            std::cout<<ts_0<<std::endl;
            if(!serial.readLine(ts_0,'\n',1000))
                tkERR("Error reciving timestamp\n");
        }
        int pos = 0;
        std::vector<std::string> seglist;
        while((pos = ts_0.find(',')) != std::string::npos){
            seglist.push_back(ts_0.substr(0, pos));
            ts_0.erase(0, pos+1);
        }

        th = new std::thread(serial_thread, &serial, &mRun);

        int hh = atoi(seglist[1].substr(0,2).c_str());
        int mm = atoi(seglist[1].substr(2,2).c_str());
        int ss = atoi(seglist[1].substr(4,2).c_str());
        int ms = atoi(seglist[1].substr(6,3).c_str());

        int day   = atoi(seglist[9].substr(0,2).c_str());
        int month = atoi(seglist[9].substr(2,2).c_str());
        int year  = atoi(seglist[9].substr(4,2).c_str())+2000;

        struct tm time = { 0 };
        time.tm_year = (int) year - 1900;
        time.tm_mon  = (int) month -1;
        time.tm_mday = (int) day;
        time.tm_hour = (int) hh;
        time.tm_min  = (int) mm;
        time.tm_sec  = (int) ss;

        time_t t = mktime(&time) + 619315200 + timezone*60*60;  /// Added 1024 weeks

        t0 = t * 1e6 + ms * 1e3;

        tkERR("First trigger stamp: "<<t0<<"\n")

        //struct tm *tim = localtime(&t);
        //timeStamp_t stamp = (t * 1e6);
        //std::cout<<stamp<<"\n"<<pc_t<<"\n";
        //t0 = std::stoi(ts_0.c_str());
    } else {
        t0      = start;
        synched = true;
    }
}

void 
Clock::stop()
{
    tkWRN("Closing Clock\n")
    if (serial.isOpen()) {
        std::string msg = "\\gsd\n";
        serial.write(msg);
        mRun = false;
        th->join();
        delete th;
        serial.close();
    }
}

timeStamp_t 
Clock::getTimeStamp(int frameCounter, int triggerLine)
{
    if (frameCounter == -1 || triggerLine == -1)
        return ::getTimeStamp();
    
    tkASSERT(synched == true);
    tkASSERT(triggerLine < lines.size(), "Out of bounds.\n");
    timeStamp_t offset = frameCounter * 1.0f/lines[triggerLine] * 1e6;

    timeStamp_t t_time = t0+offset;

    //std::cout<<t_time<<"    "<<offset<<"\n";

    return t_time; 
}


bool 
Sensor::init(const YAML::Node conf, const std::string &name, LogManager *log, const bool &aGui) {
    // get class name
    this->info.name             = name;
    this->log                   = log;
    this->readLoopStarted       = false;
    this->usingPool             = false;

    // check if paths passed are correct
    if (!conf) {
        tkERR("No sensor configuration in yaml\n");
        return false;
    }

    // read tf from configuration file
    if (conf["tf"].IsDefined()) {
        this->tf = tk::common::YAMLreadTf(conf["tf"]);
    } else {
        this->tf.resize(1);
        this->tf[0] = Eigen::Isometry3f::Identity();
    }

    // get configuration params
    this->info.triggerLine  = tk::common::YAMLgetConf<int>(conf, "trigger_line", -1);
    this->poolSize          = tk::common::YAMLgetConf<int>(conf, "pool_size", 5);

    // set sensor status
    if(this->log == nullptr)
        this->senStatus = SensorStatus::ONLINE;
    else 
        this->senStatus = SensorStatus::OFFLINE;

    // init derived class
    if (!initChild(conf, name, log)) {
        tkERR("Error.\n");
        return false;
    }

    // add a drawable foreach pool
    if (aGui && tk::gui::Viewer::getInstance()->isRunning()) {
        for (const auto &entry: pool) {
            switch (entry.first.first)
            {
            case tk::data::DataType::IMU:
                {
                    entry.second->drw   = new tk::gui::Imu(info.name + "_imu_" + std::to_string(entry.first.second));
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::GPS:
                {
                    entry.second->drw   = new tk::gui::Gps(info.name + "_gps_" + std::to_string(entry.first.second), 
                        tk::projection::ProjectionType::UTM, 1000, tk::gui::randomColor(1.0));
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::RADAR:
                {
                    entry.second->drw   = new tk::gui::Radar(info.name + "_radar_" + std::to_string(entry.first.second));
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::CLOUD:
                {
                    entry.second->drw   = new tk::gui::Cloud4f(info.name + "_cloud_" + std::to_string(entry.first.second));
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::IMAGE:
            case tk::data::DataType::IMAGEU8:
            case tk::data::DataType::IMAGEU16:
            case tk::data::DataType::IMAGEF:
                {
                    entry.second->drw   = new tk::gui::Image(info.name + "_img_" + std::to_string(entry.first.second), info.name);
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::STEREO:
                {
                    entry.second->drw   = new tk::gui::Stereo(info.name);
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::SONAR:
                {
                    entry.second->drw   = new tk::gui::Sonar(info.name);
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::ODOM:
                {
                    entry.second->drw   = new tk::gui::Odom(info.name+"_odom_"+ std::to_string(entry.first.second));
                    entry.second->drw->setPool(&entry.second->pool);
                    tk::gui::Viewer::getInstance()->add(entry.second->drw);
                }
                break;
            case tk::data::DataType::KISTLER:
            {
                entry.second->drw   = new tk::gui::Kistler(info.name+"_kistler_"+ std::to_string(entry.first.second));
                entry.second->drw->setPool(&entry.second->pool);
                tk::gui::Viewer::getInstance()->add(entry.second->drw);
            }
            break;
            default:
                tkWRN("Data of type: "<<tk::data::ToStr(entry.first.first)<<" not supported in tk::Viewer.\n");
                break;
            }
        }
    }
    return true;
}


void 
Sensor::start() 
{
    usingPool   = true;
    // start loop threads
    readingThreads.resize(pool.size());
    int i = 0;
    for (const auto &entry: pool) {
        auto key = entry.first;
        readingThreads[i] = std::thread(&Sensor::loop, this, key);
        i++;
    }
    readLoopStarted = true;
}

void 
Sensor::loop(sensorKey key) 
{
    int     idx;
    std::map<sensorKey, SensorPool_t*>::iterator it = pool.find(key); 
    if (it != pool.end()) {
        while (senStatus != SensorStatus::STOPPING && senStatus != SensorStatus::ERROR) {
            tk::data::SensorData* data = it->second->pool.add(idx);

            if (idx == -2) {
                tkWRN("No free element in the "<<tk::data::ToStr(it->first.first)<<" pool. Trying again in 2 seconds...\n");
                sleep(2);
                continue;
            }
            
            bool ok = read(data);

            // compute FPS
            if(!it->second->lastStamps.empty()) {
                it->second->lastStamps[it->second->lastStampsIdx] = data->header.stamp;
                it->second->lastStampsIdx = (it->second->lastStampsIdx+1) % it->second->lastStamps.size();
                float diff_sum = 0;
                for(int i=0; i<it->second->lastStamps.size()-1; i++) {
                    int id0 = (it->second->lastStampsIdx + i) % it->second->lastStamps.size();
                    int id1 = (it->second->lastStampsIdx + i+1) % it->second->lastStamps.size();
                    diff_sum += float(it->second->lastStamps[id1] - it->second->lastStamps[id0])/1000000;
                }
                data->header.fps = 1.0 / (diff_sum/(it->second->lastStamps.size()-1));
            }

            if (it->second->empty == true && ok == true) {
                it->second->empty = false;
            }

            it->second->pool.releaseAdd(idx, ok);

            if (ok) {
                info.dataArrived.at(key)    = it->second->pool.inserted;
            } else {
                if (status() == SensorStatus::STOPPING)
                    continue;
                tkWRN("Error while reading "<<tk::data::ToStr(it->first.first)<<". Trying again in 2 seconds...\n");
                sleep(2);
                continue;
            }
        }
    }
}

tk::common::Tfpose 
Sensor::getTf(int id) const
{
    if(id >= tf.size())
        return tk::common::Tfpose::Identity();
    else
        return tf[id];
}

std::vector<tk::common::Tfpose> 
Sensor::getTfs() const
{
    return tf;
}

bool 
tk::sensors::Sensor::close() 
{
    // stop recording
    if (senStatus == SensorStatus::RECORDING)
        stopRecord();

    // stop log
	if (senStatus == SensorStatus::OFFLINE)
		log->stop();
    
    // stop reading thread
    senStatus = SensorStatus::STOPPING;
    if(readLoopStarted) {
        for (int i = 0; i < readingThreads.size(); i++) {
            readingThreads[i].join();
        }
    }

    // close child
    if (!closeChild()) {
        tkERR("Error.\n");
        return false;
    }

    // clear pools
    for (const auto &entry: pool) {
        entry.second->pool.close();
    }
    pool.clear();

    return true;
}

bool
Sensor::read(tk::data::SensorData* data)
{
    bool    retval;
    // read data
    if (senStatus != SensorStatus::OFFLINE) {
        retval = readOnline(data);
    } else {         
        retval = readLog(data);
        if(retval)
            log->wait(data->header.stamp, info.synched);
        else
            info.synched = true;
    }

    if (retval && !usingPool) {
        ++data->header.messageID;
    }
    
    return retval;
}

tk::sensors::SensorStatus 
Sensor::status() const
{
    return this->senStatus;
}
}}