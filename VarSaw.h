#ifndef VARSAW_H
#define VARSAW_H

#include <string>
#include "../common/BiduleSDK.h"

namespace acme {
    using namespace plogue::biduleSDK;
    class VarSaw: public BidulePlugin {

	    public:
		    VarSaw(BiduleHost* host);
			virtual ~VarSaw();
            virtual bool init();
			virtual void getAudioInNames(std::vector<std::string>& vec);
			virtual void getAudioOutNames(std::vector<std::string>& vec);
			virtual void getMIDIInNames(std::vector<std::string>& vec);
			virtual void getMIDIOutNames(std::vector<std::string>& vec);
			virtual void getFreqInNames(std::vector<std::string>& vec);
			virtual void getFreqOutNames(std::vector<std::string>& vec);
			virtual void getMagInNames(std::vector<std::string>& vec);
			virtual void getMagOutNames(std::vector<std::string>& vec);

            virtual void parameterUpdate(long id);
			virtual void getParametersInfos(ParameterInfo* pinfos);
			virtual void getParameterChoices(long id, std::vector<std::string>& vec);

			virtual void process(Sample** sampleIn, Sample** sampleOut, MIDIEvents* midiIn, MIDIEvents* midiOut, Frequency*** freqIn, Frequency*** freqOut, Magnitude*** magIn, Magnitude*** magOut, SyncInfo* syncIn, SyncInfo* syncOut);
        
            
		protected:
			bool initflag;

			double _phase;
			double _ampZ;
			double _freqZ;
			double _widthZ;
        
            
		
    };
};

#endif
