#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <libopenmpt/libopenmpt_ext.hpp>

#include "AudioFile.h"
#include "args.hxx"

int g_nSubsongs, g_nInstruments, g_sampleRate;
std::vector<std::string> g_instrumentNames;

std::vector<std::string> renderedPaths;

void renderInstrument(openmpt::module_ext *mod, int subsong, int instrument) {
    mod->set_position_order_row(0, 0);
    openmpt::ext::interactive *modInteractive =
        static_cast<openmpt::ext::interactive *>(mod->get_interface(openmpt::ext::interactive_id));

    for (int i = 0; i < g_nInstruments && i != instrument; i++) {
        modInteractive->set_instrument_mute_status(i, true);
    }

    AudioFile<float> outputFile;
    outputFile.setNumChannels(2);
    outputFile.setNumSamplesPerChannel(g_sampleRate);
    outputFile.setBitDepth(32);

    const std::string path = std::format("out/{}/{}_{}.wav", subsong, instrument, g_instrumentNames[instrument]);
    std::cout << "rendering to " << path.c_str() << '\n';

    while (true) {
        float l, r = 0;
        if (mod->read(g_sampleRate, 1, &l, &r) == 0) {
            break;
        };
        outputFile.samples[0].push_back(l);
        outputFile.samples[1].push_back(r);
    }

    if (outputFile.samples[0].empty() && outputFile.samples[1].empty()) {
        std::cout << path << "is empty\n";
        return;
    }

    outputFile.save(path);
    renderedPaths.push_back(path);
    std::cout << "rendered to " << path << '\n';
}

int main(int argc, char **argv) {
    args::ArgumentParser parser("Dump stems from tracker modules.");
    parser.SetArgumentSeparations(false, false, true, true);

    args::HelpFlag help(parser, "help", "Show help", {'h', "help"});
    args::Positional<std::string> path(parser, "path", "Path to module file", "", args::Options::Required);
    args::ValueFlag<int> sampleRate(parser, "sample_rate", "Sample rate to use", {'s', "sample-rate"}, 48000);

    try {
        parser.ParseCLI(argc, argv);
    } catch (const args::Completion &e) {
        std::cout << e.what();
        return 0;
    } catch (const args::Help &) {
        std::cout << parser;
        return 0;
    } catch (const args::RequiredError &e) {
        std::cerr << e.what() << '\n';
        std::cerr << parser;
        return 1;
    } catch (const args::ParseError &e) {
        std::cerr << e.what() << '\n';
        std::cerr << parser;
        return 1;
    }

    g_sampleRate = sampleRate.Get();

    std::ifstream file(path.Get());
    openmpt::module_ext *mod = new openmpt::module_ext(file);
    g_nSubsongs = mod->get_num_subsongs();

    g_nInstruments = mod->get_num_instruments();
    g_instrumentNames = mod->get_instrument_names();
    if (g_nInstruments == 0) {
        g_nInstruments = mod->get_num_samples();
        g_instrumentNames = mod->get_sample_names();
    }

    std::cout << "subsongs: " << g_nSubsongs << '\n';
    std::cout << "instruments: " << g_nInstruments << '\n';

    std::filesystem::create_directory("out");

    for (int subsong = 0; subsong < g_nSubsongs; subsong++) {
        std::cout << "rendering subsong " << subsong << '\n';
        mod->select_subsong(subsong);
        mod->set_position_order_row(0, 0);

        std::filesystem::create_directory(std::format("out/{}", subsong));

        for (int instrument = 0; instrument < g_nInstruments; instrument++) {
            renderInstrument(mod, subsong, instrument);
        }
    }

    file.close();

    std::cout << std::endl << "rendered paths:\n";
    for (std::string &path : renderedPaths) {
        std::cout << "- " << path << '\n';
    }

    return 0;
}
