# cliPOD

Play music from your terminal.

## Description

A simple cross-playform cli/tui music player side project.
>>This project is a massive WIP. Use it to look at my messy code as I clean up and change ideas etc.

## Getting Started

### Dependencies

* CMake
* vcpkg
* C++20

### Building

* Should build with a valid CMake setup and vcpkg setup.
* Tested builds with Windows and Linux.
* I tried to keep it simple and pull dependancies with CMake.

### Executing program

#### Adding tracks
```
clipod-db -s <music-dir>
```
#### Playing tracks
```
tuipod
```

| Keys | Operation |
|-|-|
| Arrow keys | Naviagtion |
| Enter | Selection |
| Backspace | Go-back a page |
| Escape | Exit |
| Mouse | Buttons etc |




## Help

At the moment the code is hardcoded to look for the music dir within the run location. I symlink mine for testing.

Skipping tracks works. No transition to next track yet.

Tracks are expected to be tagged against [MusicBrainz](https://musicbrainz.org/) tags

## ToDo
- [ ] Clean up the database code
- [ ] Continuous smooth playback
- [ ] Settings
    - [X] Music dir location
    - [ ] DB location
- [ ] Integrate DB operations within app
    - [X] Scan for new tracks
    - [ ] Update tracks
    - [ ] Remove tracks
- [ ] Playlists maybe
- [ ] Keyboard only support

## Authors

[Kayzleigh Prejean](https://www.linkedin.com/in/kayzleigh-prejean/)


## License

This project is licensed under the __GNU GPLv3 License__ - see the LICENSE.md file for details

## Acknowledgments
### Inspo
* [musikcube](https://github.com/clangen/musikcube)
### 3rd Party Libs
* [ftxui](https://github.com/ArthurSonzogni/FTXUI)
* [openal-soft](https://github.com/kcat/openal-soft)
* [ffmpeg](https://github.com/FFmpeg/FFmpeg)
* [modern-uuid](https://github.com/gershnik/modern-uuid)
* [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp)
* [sigslot](https://github.com/palacaze/sigslot)
* [spdlog](https://github.com/gabime/spdlog)
* [taglib](https://github.com/taglib/taglib)
* [ctrl-c](https://github.com/evgenykislov/ctrl-c.git)
* [cxxopts](https://github.com/jarro2783/cxxopts)
