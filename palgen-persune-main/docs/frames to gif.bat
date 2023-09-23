ffmpeg -framerate 2 -i "QAM phase %%03d.png" -filter_complex "split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" "QAM phase.gif" -y
ffmpeg -framerate 2 -i "waveform phase %%03d.png" -filter_complex "split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" "waveform phase.gif" -y
ffmpeg -framerate 2 -i "palette preview emphasis %%03d.png" -filter_complex "split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" "palette preview emphasis.gif" -y
