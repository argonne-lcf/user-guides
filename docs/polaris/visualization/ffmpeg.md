# FFmpeg on Polaris

## NOTE: The FFmpeg module is currently missing on Polaris after a recent upgrade. A Spack build of FFmpeg will be available soon.

To use FFmpeg on Polaris, first load the corresponding module:

```
module load ffmpeg
```

This is a typical command line to create a movie from a series of snapshots in PNG format:

```
ffmpeg -r 15 -i frames.%03d.png -r 25 -pix_fmt yuv420p movie.mp4
```

where:

- `-r 15` is the input frame rate. Experiment with values smaller than the output frame rate for longer movies.
- `-r 25` is the output frame rate (use this value for standard 25 frames per second).
- `-i frames.%03d.png` reads the input frames in sequence.
- `-pix_fmt yuv420p` is needed for movies to play in browsers.
- `movie.mp4` is the resulting movie.