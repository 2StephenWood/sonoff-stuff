# fauxmo_garagedoor

FauxmoESP from https://bitbucket.org/xoseperez/fauxmoesp
Just implement the example to toggle the relay on modded sonoff.

# programming
Using esptool.py 1.0.1 with the following command:
python /usr/local/bin/esptool.py --port /dev/cu.usbserial-A504DTZJ --baud 408600 write_flash --flash_size=8m -fm dout 0 firmware.bin
