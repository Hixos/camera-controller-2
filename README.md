# Camera Controller 2
## Remote DSLR control with Raspberry PI and Android

### Gphoto2 problems
#### "could not claim the usb device" error

This is due to the `gvfs-gphoto2-volume-monitor` process.   
Run `ps aux | grep gphoto` and kill the process using `kill -9 <pid>`.