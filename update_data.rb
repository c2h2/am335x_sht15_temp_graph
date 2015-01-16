DB="temp_humi.rrd"
temp=File.read("/sys/devices/platform/sht15/temp1_input").strip.to_i / 1000.0
humi=File.read("/sys/devices/platform/sht15/humidity1_input").strip.to_i / 1000.0

puts "rrdtool update #{DB} N:#{temp.round(1)}:#{humi.round(1)}"
