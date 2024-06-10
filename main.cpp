#include <iostream>
#include <fstream>
#include <vector>
#include <speex/speex_preprocess.h>

int SAMPLE_RATE = 16000;
int FRAME_SIZE_MS = 32;
int FRAME_SIZE = (SAMPLE_RATE * FRAME_SIZE_MS) / 1000;

void process_audio_frame(short* in, short* out, SpeexPreprocessState* state) {
    speex_preprocess_run(state, in);
    std::copy(in, in + FRAME_SIZE, out);
}

int main() {
    std::ifstream input("TestFile.raw", std::ios::binary);
    std::ofstream output("output.pcm", std::ios::binary);
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Failed to open input or output file." << std::endl;
        return 1;
    }

    std::vector<short> input_buffer(FRAME_SIZE);
    std::vector<short> output_buffer(FRAME_SIZE);

    SpeexPreprocessState* state = speex_preprocess_state_init(FRAME_SIZE, SAMPLE_RATE);

    while (input.read(reinterpret_cast<char*>(input_buffer.data()), FRAME_SIZE * sizeof(short))) {
        process_audio_frame(input_buffer.data(), output_buffer.data(), state);
        output.write(reinterpret_cast<char*>(output_buffer.data()), FRAME_SIZE * sizeof(short));
    }

    speex_preprocess_state_destroy(state);

    input.close();
    output.close();

    return 0;
}
