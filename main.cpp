#include <iostream>
#include <fstream>
#include <vector>
#include <speex/speex_preprocess.h>


constexpr int SAMPLE_RATE = 16000;
constexpr int FRAME_SIZE_MS = 32;
constexpr int FRAME_SIZE = (SAMPLE_RATE * FRAME_SIZE_MS) / 1000;

void process_audio_frame(short* in, short* out, SpeexPreprocessState* state) {
    speex_preprocess_run(state, in);
    std::copy(in, in + FRAME_SIZE, out);
}

int main(int argc, char *argv[]) {
    const std::string inputFileName = argv[1];
    const std::string outputFileName = argv[2];

    std::ifstream input(inputFileName, std::ios::binary);
    std::ofstream output(outputFileName, std::ios::binary);
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Failed to open input or output file." << std::endl;
        return 1;
    }

    std::vector<short> input_buffer(FRAME_SIZE);
    std::vector<short> output_buffer(FRAME_SIZE);

    SpeexPreprocessState* state = speex_preprocess_state_init(FRAME_SIZE, SAMPLE_RATE);

    // Configure the preprocessor for speech enhancement
    int denoise = 1;
    int noiseSuppress = -30; // in dB
    int agc = 1;
    int agcLevel = 8000; // target level in millibels
    int vad = 1;
    int dereverb = 1;
    float dereverb_decay = 0.4f;
    float dereverb_level = 0.3f;

    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DENOISE, &denoise);
    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noiseSuppress);
    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_AGC, &agc);
    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_AGC_LEVEL, &agcLevel);
    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_VAD, &vad);
    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DEREVERB, &dereverb);
    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &dereverb_decay);
    speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &dereverb_level);

    while (input.read(reinterpret_cast<char*>(input_buffer.data()), FRAME_SIZE * sizeof(short))) {
        process_audio_frame(input_buffer.data(), output_buffer.data(), state);
        output.write(reinterpret_cast<char*>(output_buffer.data()), FRAME_SIZE * sizeof(short));
    }

    speex_preprocess_state_destroy(state);

    input.close();
    output.close();

    return 0;
}
