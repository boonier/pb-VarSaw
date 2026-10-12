
#include "VarSaw.h"
#include <iostream>
#include <cstring>
#include <cmath>

using namespace plogue::biduleSDK;
using namespace acme;

namespace {
    static inline double polyBlep(double t, double dt) {
        if (dt <= 0.0) return 0.0;
        if (t < dt) {
            t /= dt;
            return t + t - t * t - 1.0;
        }
        if (t > 1.0 - dt) {
            t = (t - 1.0) / dt;
            return t * t + t + t + 1.0;
        }
        return 0.0;
    }

    static inline double clamp01(double x) {
        if (x < 0.0) return 0.0;
        if (x > 1.0) return 1.0;
        return x;
    }

    static inline double clampRange(double x, double lo, double hi) {
        if (x < lo) return lo;
        if (x > hi) return hi;
        return x;
    }
}

VarSaw::VarSaw(BiduleHost* host):
BidulePlugin(host){
	//fill in plugin capabilities as a mask
//  _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
    _caps = 0;
	_numAudioIns=3;
	_numAudioOuts=1;
	_numMIDIIns=0;
	_numMIDIOuts=0;
	_numFreqIns=0;
	_numFreqOuts=0;
	_numMagIns=0;
	_numMagOuts=0;
	_numParams=0;

    std::cout << "creating VarSaw"  << endl;

    initflag = true;
    _phase = 0.0;
    _ampZ = 0.0;
    _freqZ = 0.0;
    _widthZ = 0.5;
}

VarSaw::~VarSaw(){
    std::cout << "deleting VarSaw" << endl;
}

bool VarSaw::init() {
    initflag = true;
    return true;
}
void 
VarSaw::getAudioInNames(std::vector<std::string>& vec){
    vec.push_back("frequency");
	vec.push_back("amplitude");
    vec.push_back("width");
}

void 
VarSaw::getAudioOutNames(std::vector<std::string>& vec){
	vec.push_back("out");
}

void 
VarSaw::getMIDIInNames(std::vector<std::string>& vec){
	vec.push_back("passthrough midi in");
}

void 
VarSaw::getMIDIOutNames(std::vector<std::string>& vec){
	vec.push_back("passthrough midi out");
}

void 
VarSaw::getFreqInNames(std::vector<std::string>& vec){
	vec.push_back("passthrough frequency in");
}

void 
VarSaw::getFreqOutNames(std::vector<std::string>& vec){
	vec.push_back("passthrough frequency out");
}

void 
VarSaw::getMagInNames(std::vector<std::string>& vec){
	vec.push_back("passthrough magnitude in");
}

void 
VarSaw::getMagOutNames(std::vector<std::string>& vec){
	vec.push_back("passthrough magnitude out");
}

void 
VarSaw::getParametersInfos(ParameterInfo* pinfos) {

//	pinfos[0].id = 0;
//	strcpy(pinfos[0].name, "dummy parameter");
//	pinfos[0].type = DOUBLEPARAM;
//	pinfos[0].ctrlType = GUICTRL_SLIDER;
//	pinfos[0].linkable = 1;
//	pinfos[0].saveable = 1;
//	pinfos[0].paramInfo.pd.defaultValue = 4.0;
//	strcpy(pinfos[0].paramInfo.pd.units, "dollars");
//	pinfos[0].paramInfo.pd.precision = 3;
//	pinfos[0].paramInfo.pd.minValue = 1.0;
//	pinfos[0].paramInfo.pd.maxValue = 10.0;
//	pinfos[0].paramInfo.pd.scaling = SCALING_LINEAR;
//
//	pinfos[1].id = 1;
//	strcpy(pinfos[1].name, "audiofile");
//	pinfos[1].type = MEDIAPOOLPARAM;
//	pinfos[1].ctrlType = GUICTRL_CHOICE;
//	pinfos[1].linkable = 1;
//	pinfos[1].saveable = 1;
//
//	pinfos[2].id = 2;
//	strcpy(pinfos[2].name, "add to mediapool");
//	pinfos[2].type = STRINGPARAM;
//	pinfos[2].ctrlType = GUICTRL_MEDIAPOOLSHORTCUT;
//	pinfos[2].linkable = 0;
//	pinfos[2].saveable = 0;
}

void 
VarSaw::getParameterChoices(long id, std::vector<std::string>& vec) {
}

void 
VarSaw::parameterUpdate(long id){
//	if(id == 0)
//		getParameterValue(0, _dParamValue);
//	if(id == 1) {
//		_afConnected = false;
//		_dumbAFCounter = 0;
//		std::string filename;
//		getParameterValue(1, filename);
//		getAudioFile(1, filename, &_af);
//		if(_af.numChannels != 0)
//			_afConnected = true;
//	}
}

void
VarSaw::process(Sample** sampleIn, Sample** sampleOut, MIDIEvents* midiIn, MIDIEvents* midiOut,
                     Frequency*** freqIn, Frequency*** freqOut,
                     Magnitude*** magIn, Magnitude*** magOut, SyncInfo* syncIn, SyncInfo* syncOut)
{
    long sampleFrames = _dspInfo.bufferSize;

    Sample* freqInHz = sampleIn[0];
    Sample* ampIn = sampleIn[1];
    Sample* widthIn = sampleIn[2];
    Sample* out = sampleOut[0];

    const double sampleRate = _dspInfo.sampleRate;
    const double invSampleRate = (sampleRate > 0.0) ? (1.0 / sampleRate) : 0.0;
    // ~5ms time constant for smoothing
    const double smoothingHz = 200.0;
    const double a = (sampleRate > 0.0) ? (smoothingHz * invSampleRate) : 1.0;
    const double onePole = (a < 1.0) ? a : 1.0;

    if (initflag) {
        initflag = false;
        _phase = 0.0;
        _ampZ = 0.0;
        _freqZ = 0.0;
        _widthZ = 0.5;
    }

    while (--sampleFrames >= 0) {
        const double ampT = (double)(*ampIn++);
        const double freqT = (double)(*freqInHz++);
        const double widthT = (double)(*widthIn++);

        _ampZ += (ampT - _ampZ) * onePole;
        _freqZ += (freqT - _freqZ) * onePole;
        _widthZ += (widthT - _widthZ) * onePole;

        const double amp = _ampZ;
        double freq = _freqZ;
        double width = _widthZ;

        if (freq < 0.0) freq = -freq;
        if (freq > 0.5 * sampleRate) freq = 0.5 * sampleRate;
        width = clamp01(width);
        const double dt = freq * invSampleRate;

        double t = _phase;

        // Shape morph ("VarSaw"):
        //   width=0.0 -> falling saw
        //   width=0.5 -> triangle
        //   width=1.0 -> rising saw
        //
        double y;
        if (width <= 0.0) {
            // Falling saw, band-limited
            y = 1.0 - 2.0 * t;
            y += 2.0 * polyBlep(t, dt);
        } else if (width >= 1.0) {
            // Rising saw, band-limited
            y = 2.0 * t - 1.0;
            y -= 2.0 * polyBlep(t, dt);
        } else {
            const double w = width;
            const double a = t / w;
            const double b = (1.0 - t) / (1.0 - w);
            const double m = (a < b) ? a : b;
            y = 2.0 * m - 1.0;
        }

        *out++ = (Sample)(y * amp);

        t += dt;
        if (t >= 1.0) t -= 1.0;
        _phase = t;
    }
}
