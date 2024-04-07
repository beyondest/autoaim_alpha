echo rcclub |sudo -S chmod 666 /dev/ttyTHS0 /dev/ttyTHS1
echo rcclub |sudo -S modprobe can 
echo rcclub |sudo -S modprobe can_raw 
echo rcclub |sudo -S modprobe can_dev 
echo rcclub |sudo -S modprobe mttcan 
echo rcclub |sudo -S ip link set can0 type can bitrate 100000 
echo rcclub |sudo -S ip link set up can0
echo rcclub |sudo -S jetson_clocks











