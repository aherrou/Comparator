#include "float.cpp"
#include "fixed.cpp"
#include <libgen.h>

#include <sndfile.h>
#include "faust/dsp/dsp-tools.h"
#include <unistd.h>
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

  void compare(int size, bool logging, std::string name="")
        {   
            if (comp != nullptr) {
                deletebuffer(comp, FL->getNumOutputs());
            }

	    std::ofstream logging_file;

	    if (logging)
	      {
		std::string filename = "samples-"+name+".log";
		logging_file.open(filename.c_str());
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
		    
		    if (logging and chan == 0) // we only log one channel
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

// std::list<GUI*> GUI::fGuiList;
// ztimedmap GUI::gTimedZoneMap;

int main(int argc, char* argv[])
{
  // sample rate and buffer size
  int samplerate = 44100;
  int buffersize = 512;
	
  // parsing of options
  int opt;

  bool execute = false;
  bool logging = false;
  bool help = false;

  while((opt = getopt(argc, argv, ":lwh")) != -1)
    {
      switch(opt)
	{
	case 'l':
	  logging = true;
	  break;
	case 'w':
	  execute = true;
	  break;
	case 'h':
	  help = true;
	  break;
	}
    }

  if (help)
    {
      std::cout << "Usage: "
		<< argv[0] << " [options]" << std::endl
		<< "Options :"<< std::endl
		<< "\t -w : writes the floating-point and fixed-point outputs to sound files" << std::endl
		<< "\t -l : logs the floating-point and fixed-point samples to a text file" << std::endl
		<< "\t -h : displays this help message" << std::endl ;
	return 0;
    }
  
  // init 
  fldsp FL;
  fxdsp FX;
  FL.init(samplerate);
  FX.init(samplerate);

  std::string executable_name(argv[0]);
  std::string filename = executable_name.substr(executable_name.rfind("/")+1);  
    
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
      snprintf(name, 256, "%s", ("float-"+filename+".wav").c_str());

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

	FL.compute(blocsize, nullptr, ilvFL.inputs());
	ilvFL.interleave();
	writer(out_sf_fl, ilvFL.output(), blocsize);
	cur_frame += blocsize;
      } while (cur_frame < samplenb);

      sf_close(out_sf_fl);
      
      /////////////////////////////////
      // execute fixed-point version //
      /////////////////////////////////
      snprintf(name, 256, "%s", ("fixed-"+filename+".wav").c_str());

      out_info = {
	samplenb,
	samplerate,
	FX.getNumOutputs(),
	SF_FORMAT_WAV|SF_FORMAT_PCM_24|SF_ENDIAN_LITTLE,
	0,
	0
      };
      std::cout << "Format = " << (int)(SF_FORMAT_WAV|SF_FORMAT_PCM_24|SF_ENDIAN_LITTLE) << std::endl;
      SNDFILE* out_sf_fx = sf_open(name, SFM_WRITE, &out_info);
      if (!out_sf_fx) {
	std::cerr << "ERROR : cannot write output file " << name << std::endl;
	sf_perror(out_sf_fx);
	exit(1);
      }

      Interleaver ilvFX(buffersize, FX.getNumOutputs(), FX.getNumOutputs());

      // compute samples
      cur_frame = 0;
      do {
	int blocsize = std::min(buffersize, samplenb - cur_frame);

	FX.compute(blocsize, nullptr, ilvFX.inputs());
	ilvFX.interleave();
	writer(out_sf_fx, ilvFX.output(), blocsize);
	cur_frame += blocsize;
      } while (cur_frame < samplenb);

      sf_close(out_sf_fx);

      
    }

    FL.init(48000);
    FX.init(48000);

    comparateur comp((dsp*) &FL, (dsp*) &FX);
    comp.compare(200, logging, filename);
    comp.display();
}
