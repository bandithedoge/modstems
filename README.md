Dumps "stems" from module files using [libopenmpt](https://lib.openmpt.org/libopenmpt/), made for the purposes of [misadventures in ludomusicology](https://www.youtube.com/@misadventuresinludomusicology).

The way this program works is stupidly simple – it loops over every instrument in the module and mutes every other instrument. Beats opening up OpenMPT and rendering stems from every subtrack.

The only dependencies are [meson](https://mesonbuild.com/), [libopenmpt](https://lib.openmpt.org/libopenmpt/) and a C++20-capable compiler.

```
modstems {OPTIONS} path

  Dump stems from tracker modules.

OPTIONS:

    -h, --help                        Show help
    path                              Path to module file
    -s [sample_rate], --sample-rate
    [sample_rate]                     Sample rate to use
```

# TODO

- Threading
- Smarter way to detect silent samples
- Loop points and BPM information in .wav metadata

# Projects used

- [libopenmpt](https://lib.openmpt.org/libopenmpt/) (BSD-3-Clause)
- [args](https://taywee.github.io/args/) (MIT)
- [AudioFile](https://github.com/adamstark/AudioFile) (MIT)
