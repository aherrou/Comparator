#include "float.cpp"
#include "fixed.cpp"
#include <libgen.h>

#include <sndfile.h>
#include "faust/dsp/dsp-tools.h"
#include "faust/gui/GTKUI.h"
#include "faust/audio/coreaudio-dsp.h"
#include "faust/audio/jack-dsp.h"
#include <iostream>
#include <fstream>


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

// typedef sf_count_t (* sample_read)(SNDFILE* sndfile, void* buffer, sf_count_t frames);
typedef sf_count_t (* sample_write)(SNDFILE* sndfile, void* buffer, sf_count_t frames);

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
  // sample rate and buffer size
  int samplerate = 44100;
  int buffersize = 512;
	
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
  FL.init(samplerate);
  FX.init(samplerate);
    
  if (execute){
      // number of samples to compute for each DSP
      int audiolength = 3;
      int samplenb = audiolength*samplerate;
      
      sample_write writer;
      if (sizeof(FAUSTFLOAT) == 4) 
        writer = reinterpret_cast<sample_write>(sf_writef_float);
      else 
	writer = reinterpret_cast<sample_write>(sf_writef_double);

      ////////////////////////////////////
      // execute floating-point version //
      ////////////////////////////////////
      char name[256];
      snprintf(name, 256, "%s", "float.wav");

      /* FAUSTFLOAT** inputs = new FAUSTFLOAT*[FL.getNumInputs()];
      for (int i=0; i< FL.getNumInputs(); i++){
	inputs[i] = new FAUSTFLOAT[buffersize];
	memset(inputs[i], 0, sizeof(FAUSTFLOAT) * buffersize);
	}*/ 

      SF_INFO out_info = {
	samplenb,
	samplerate,
	FL.getNumOutputs(),
	SF_FORMAT_WAV|SF_FORMAT_PCM_24|SF_ENDIAN_LITTLE,
	0,
	0
      };
      std::cout << "Format = " << (int)(SF_FORMAT_WAV|SF_FORMAT_PCM_24|SF_ENDIAN_LITTLE) << std::endl;
      // memset(&out_info, 0, sizeof(out_info));
      // out_info.channels = FL.getNumOutputs();
      SNDFILE* out_sf_fl = sf_open(name, SFM_WRITE, &out_info);
      if (!out_sf_fl) {
	std::cerr << "ERROR : cannot write output file " << name << std::endl;
	sf_perror(out_sf_fl);
	exit(1);
      }

      Interleaver ilvFL(buffersize, FL.getNumOutputs(), FL.getNumOutputs());

      // compute samples
      int cur_frame = 0;
      do {
	int blocsize = std::min(buffersize, samplenb - cur_frame);
	std::cout << "Computing a block of " << blocsize << " samples at frame " << cur_frame << std::endl;

	FL.compute(blocsize, nullptr, ilvFL.inputs());
	ilvFL.interleave();
	writer(out_sf_fl, ilvFL.output(), blocsize);
	cur_frame += blocsize;
      } while (cur_frame < samplenb);

      sf_close(out_sf_fl);
      
      /////////////////////////////////
      // execute fixed-point version //
      /////////////////////////////////
      snprintf(name, 256, "%s", "fixed.wav");
      
      jackaudio audio2;
      
      if (!audio2.init(name, &FX)) {
        std::cerr << "\033[32mUnable to init audio\033[0m" << std::endl;
        exit(1);
      }
      
      audio2.start();
      audio2.stop();
    }

    FL.init(48000);
    FX.init(48000);

    comparateur comp((dsp*) &FL, (dsp*) &FX);
    comp.compare(200, logging);
    comp.display();
}
