#include <iostream>
#include <tkCommon/common.h>
#include <tkCommon/log.h>
#include <csignal>
#include <thread>
#include <tkCommon/communication/EthInterface.h>


bool                            gRun = true;
tk::communication::Ethinterface iface;
tk::communication::UDPSocket    sender;

void replayLoop(std::string file, int port, std::string filter,std::string ip){

    tkASSERT(iface.initPcapReplay(file,filter));
    sender.initSender(port,ip);
    uint8_t buffer[2000];
    int count = 0;
    timeStamp_t first = 0, now;
    std::chrono::system_clock::time_point startpt;
    while(gRun){      

        int n = iface.read(buffer,now);
        if(first == 0) {
            first = now;
            startpt = std::chrono::system_clock::now();
        }

        if(n == -1){
            //gRun = false;
            continue;
        }

        sender.send(buffer,n);

        std::chrono::microseconds inc_chrono {now - first};
        std::this_thread::sleep_until(startpt + inc_chrono);

        count++;
        if(count%500 == 0){
            tkMSG(std::string{"send "}+std::to_string(count)+" packets\n");
        }
    }

    iface.close();
    sender.close();
}

int main(int argc, char* argv[]){

    tk::common::CmdParser   cmd(argv, "Samples for handle ethernet packets");
    std::string file        = cmd.addArg("file", "", "pcap replay file");
    std::string filter      = cmd.addOpt("-filter", "", "pcap filter");
    std::string ip          = cmd.addOpt("-ip", "127.0.0.1", "pcap filter");
    int port                = cmd.addIntOpt("-port", 2368, "pcap replay file");
    cmd.parse();

    replayLoop(file,port,filter,ip);
    return 0;
}
