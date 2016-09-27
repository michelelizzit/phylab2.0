int readAcc() {
  return analogRead(ACC_X_PIN) - TARAX;
}

int readAcc(long num) {
  long accData2 = 0;
  for (int cnt = 0; cnt < num; cnt++) {
    //delay(3);
    long accData = readAcc();
    accData2 += accData;
  }
  accData2 = accData2 / num;
  return accData2;
}
