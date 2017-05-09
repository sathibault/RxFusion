## Configuration

Functions for device/system configuration.

### ConnectWifi

<img align="left" src="../img/cpp.png">
ConnectWifi(*ssid*, *password*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Connect to a WiFi access point with the given `ssid` and `password`.

### NetworkTime

<img align="left" src="../img/cpp.png">
NetworkTime(*transport*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Set the date/time using an Internet time server with the given `transport` (e.g. a `WiFiClient`)

### Sleepy

<img align="left" src="../img/cpp.png">
Sleepy(*flags=0*)
<br>
<img align="left" src="../img/javascript.png">
N/A

The `Sleepy` function enables automatic sleep management on
both AVR and Atmel SAMD (ARM) devices.  When sleep
management is enabled, the scheduler will attempt to put the
CPU in sleep/standby mode whenever there is 2 seconds or
more before the next scheduled event.  Tests on both
architectures showed slightly over 2mA current draw while
sleeping, which is probably a 1/10th or less of your awake
mode usage.

The sleep manager uses the RTC on SAMD architectures and the
WDT on AVR.  `Sleepy` can be called from the beginning or
end of the `app` function to enable sleep management.  If
you set the RTC time during setup, you must do that before
calling `Sleepy`.

The resolution of both the RTC and WDT is 1 second.  Using
even second values in your program schedule will minimize
the amount of time the CPU will be idle between the time it
wakes and the next event happens.

**USB Interaction** - The USB interface tends to interfere
with applications using sleep mode, and it is disabled by
default when you enable sleep management.  I recommend using
the SPI interface for output when using sleep mode.  If you
must use the USB you can call `Sleepy` with the
`SLEEP_MGR_USB` argument to enable the management of the
USB.  In this case, the USB will be detached before sleeping
and reattached upon wake up.


