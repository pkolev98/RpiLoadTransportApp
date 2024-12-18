Simple program for streaming from camera on raspberry pi, detecting arrow and going to it with rover.

Prerequisites:
- OpenCV cloned and compiled on Rpi
- libcamera dev package
- pigpiod and pigpio
- gRPC cloned and compiled on Rpi

This project is build and written directly on the RaspberryPi 4B+ with bookworm OS distro.

Build:
- To build the RpiLoadTransportApp, use the build.sh script in the root directory.

Run the RpiLoadTransportApp service:
- pigpiod should be started - Daemon, which uses the gpios of the Rpi.
- Copy build/src/RpiLoadTransportApp to /usr/local/bin/
	sudo cp RpiLoadTransportApp /usr/local/bin/
- the service file in service/rpi-load-transport.service folder should be copied to /etc/systemd/system/
	sudo cp rpi-load-transport.service /etc/systemd/system/
- After that execute on raspberry pi:
	sudo systemctl enable rpi-load-transport.service
	sudo systemctl daemon-reload
	sudo systemctl restart rpi-load-transport.service.
Note: You can optionally reboot the device.

The service is controlled via gRPC from another client application, which gives the start and stop commands
to the service. It can be local or on another host and making the calls through the wlan network.
Another repo will be created for the client application with GUI (Qt and C++).

