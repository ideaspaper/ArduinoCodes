void setup() {
  
}

void loop() {
  delay(5000);
  system("ifconfig -a > /dev/ttyGS0");
}
