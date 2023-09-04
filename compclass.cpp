#include "float.cpp"
#include "fixed.cpp"
#include <libgen.h>
#include "faust/gui/GTKUI.h"
#include "faust/audio/coreaudio-dsp.h"
#include <iostream>
#include <fstream>


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class comparateur {

    private:
        
        int buffersize = 24;
        FAUSTFLOAT** inputs;
        FAUSTFLOAT** FX_outputs;
        FAUSTFLOAT** FL_outputs;
        FAUSTFLOAT** comp;
        dsp* FL;
        dsp* FX;
        int sizechosen;
        float snr;

    public : 
        
        FAUSTFLOAT** createbuffer (int chan, int buffer_size)
        {
            
            FAUSTFLOAT** buffer = new FAUSTFLOAT*[chan];
                for (int i = 0; i < chan; i++) {
                    buffer[i] = new FAUSTFLOAT[buffer_size];
                    memset(buffer[i], 0, sizeof(FAUSTFLOAT) * buffer_size);
                }
                return buffer;
        }

        void deletebuffer (FAUSTFLOAT** buffer, int chan)
        {
                for (int i = 0; i <chan; i++) {
                    delete [] buffer[i];
                }
                delete [] buffer;
        }



        comparateur(dsp* fl, dsp* fx)
        {
            FL = fl;
            FX = fx; 
            inputs = createbuffer(FL->getNumInputs(), buffersize);
            FL_outputs = createbuffer(FL->getNumOutputs(), buffersize);
            FX_outputs = createbuffer(FX->getNumOutputs(), buffersize);
            comp = nullptr;
        }

  void compare(int size, bool logging)
        {   
            
            if (comp != nullptr) {
                deletebuffer(comp, FL->getNumOutputs());
            }

	    std::ofstream logging_file;

	    if (logging)
	      {
		logging_file.open("samples.logging");
	      }
	    
            sizechosen = size;
            int slice = size;
            
            comp = createbuffer(FL->getNumOutputs(),size);

	    snr = 0; //signal to noise ratio
	    float power_signal = 0;
	    float power_noise = 0;
            
            int t = 0; 
            do {
            int blocsize = std::min(buffersize, slice);
            FX->compute(blocsize, inputs, FX_outputs);
            FL->compute(blocsize, inputs, FL_outputs);
            
            for (int chan=0; chan<FL->getNumOutputs(); ++chan)
            {
                FAUSTFLOAT* sub_comp = comp[chan];
                FAUSTFLOAT* sub_FL_outputs = FL_outputs[chan];
                FAUSTFLOAT* sub_FX_outputs = FX_outputs[chan];
                for (int frame=0; frame<blocsize; ++frame) {
                    std::cout << "FL " << sub_FL_outputs[frame] << " | FX " << sub_FX_outputs[frame] << std::endl;
                    sub_comp[frame+(t*buffersize)] = sub_FL_outputs[frame] - sub_FX_outputs[frame];

		    if (logging and chan == 0) // we only logging one channel
		      logging_file << sub_FL_outputs[frame] << ";" << sub_FX_outputs[frame] << ";" << std::endl;

		    power_signal += sub_FL_outputs[frame]*sub_FL_outputs[frame];
		    power_noise += sub_comp[frame+(t*buffersize)]*sub_comp[frame+(t*buffersize)];
                }
            }
            ++t;
            slice = slice - buffersize;
            }while (slice > 0);

	    snr = log(power_signal/power_noise);
        }
                    
        void display()
        {   
            FAUSTFLOAT totalcomp = 0;
            FAUSTFLOAT compmax=0;

            for (int frame=0; frame < sizechosen; ++frame) 
            { 
                std::cout << "frame: "<< frame;
                for (int chan=0; chan< FL->getNumOutputs(); ++chan) 
		  {
                    FAUSTFLOAT* sub_comp = comp[chan];

                    std::cout << " | Channel " << chan+1 << " :" << sub_comp[frame] << "\t";
                    
                    totalcomp = totalcomp + std::abs(sub_comp[frame]);
                    compmax = std::max(compmax,std::abs(sub_comp[frame]));
		  }

                std::cout << std::endl;
            }
            std :: cout << "Total Amount : " << totalcomp << std::endl;
	    std :: cout << "Relative error : " << totalcomp/(FL->getNumOutputs()*sizechosen) << std::endl;
            std :: cout << "Max : " << compmax << std::endl;
	    std :: cout << "SNR : " << snr << std::endl;
        }

        virtual ~comparateur()
        {
            deletebuffer(inputs, FL->getNumInputs());
            deletebuffer(FL_outputs, FL->getNumOutputs());
            deletebuffer(FX_outputs, FX->getNumOutputs());
        }

};

std::list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;

int main(int argc, char* argv[])
{
  // parsing of options
  int opt;

  bool execute = false;
  bool logging = false;

  while((opt = getopt(argc, argv, ":le")) != -1)
    {
      switch(opt)
	{
	case 'l':
	  logging = true;
	  break;
	case 'e':
	  execute = true;
	  break;
	}
    }
  
  // init 
    fldsp FL;
    fxdsp FX;
    FL.init(48000);
    FX.init(48000);
    
    if (execute){
      // execute floating-point version
      char name[256];
      snprintf(name, 256, "%s", basename(argv[0]));
      
      GTKUI* interface = new GTKUI(name, &argc, &argv);
      //FUI finterface;
      
      FL.buildUserInterface(interface);
      //FX.buildUserInterface(&finterface);
      
      jackaudio audio;
      
      if (!audio.init(name, &FL)) {
        std::cerr << "\033[32mUnable to init audio\033[0m" << std::endl;
        exit(1);
      }
      
      audio.start();
      
      interface->run();
      interface->stop();
      
      audio.stop();
      delete interface;
      
      // execute fixed-point version
      snprintf(name, 256, "%s", basename(argv[0]));
      
      GTKUI* interface2 = new GTKUI(name, &argc, &argv);
      
      FX.buildUserInterface(interface2);
      //FX.buildUserInterface(&finterface);
      
      jackaudio audio2;
      
      
      if (!audio2.init(name, &FX)) {
        std::cerr << "\033[32mUnable to init audio\033[0m" << std::endl;
        exit(1);
      }
      
      audio2.start();
      
      interface2->run();
      interface2->stop();
      
      audio2.stop();
      delete interface2;
    }

    FL.init(48000);
    FX.init(48000);

    comparateur comp((dsp*) &FL, (dsp*) &FX);
    comp.compare(200, logging);
    comp.display();
}
