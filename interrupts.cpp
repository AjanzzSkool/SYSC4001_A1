/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @author Ajan Balaganesh
 * @author Kyle Deng
 */

#include"interrupts.hpp"

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int now = 0;
    int context_save_ms = 10;
    // int context_save_ms = 40;
    


    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/

	if (activity == "CPU") {
            int cpu_ms = duration_intr;
            
            execution += std::to_string(now) + ", " + std::to_string(cpu_ms) + ", CPU burst\n";
            now += cpu_ms;
        }
        else if (activity == "SYSCALL" || activity == "END_IO") {
            int dev = duration_intr;



            
            auto [prelude, updated_time] = intr_boilerplate(now, dev, context_save_ms, vectors);
            execution += prelude;
            now = updated_time;

            int isr_body_ms = delays.at(dev);
	    //int isr_body_ms = 100;
            //int isr_body_ms = 400;
            if (activity == "SYSCALL") {
		int remaining = isr_body_ms;
                execution += std::to_string(now) + ", " + std::to_string(40)
                           + ", Sys Call(Run the ISR)\n";
		now += 40;
		remaining -= 40;
		if (remaining < 41){
                	execution += std::to_string(now) + ", " + std::to_string(remaining - 1)
                           	+ ", transfer data from device to memory\n";
			now += (remaining - 1);
			remaining = 1;
                	execution += std::to_string(now) + ", " + std::to_string(1)
                           + ", check for errors\n";
			now += 1;
			remaining = 0;
                }else {
                	execution += std::to_string(now) + ", " + std::to_string(40)
                           + ", transfer data from device to memory\n";
			now += 40;
			remaining -= 40;
                	execution += std::to_string(now) + ", " + std::to_string(remaining)
                           + ", check for errors\n";
			now += remaining;
			remaining = 0;
		}
                
	
            } else {
                execution += std::to_string(now) + ", " + std::to_string(40)
                           + ", ENDIO(Run the ISR)\n";
		now += 40;
	        execution += std::to_string(now) + ", " + std::to_string(isr_body_ms - 40)
                           + ", Check device status\n";	
		now += (isr_body_ms - 40);
            }
            //now += isr_body_ms;

            
            execution += std::to_string(now) + ", " + std::to_string(1) + ", IRET\n";
            now += 1;
        }


        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
