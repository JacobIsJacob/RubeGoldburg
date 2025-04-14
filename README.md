# RubeGoldburg

## Audio

https://forums.adafruit.com/viewtopic.php?p=1045498&sid=23159f370e2f22a3435e66a3f058726b#p1045498

Put this in **/boot/firmware/config.txt**:
```
dtparam=audio=on
gpio=12,13,a5
audio_pwm_mode=2
dtoverlay=audremap,pins_12_13
```

How to play some test sounds to make sure it's working:
```
aplay -l
aplay /usr/share/sounds/alsa/Front_Right.wav
speaker-test
```
