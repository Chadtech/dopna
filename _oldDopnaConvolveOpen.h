  // // Get Left and Right Convolve file name data
  // char leftConvolveData [13];
  // char rightConvolveData [13];
  // datumIndex = 0;
  // while (datumIndex < 12){
  //   leftConvolveData[datumIndex] = dopnaFile.get();
  //   datumIndex++;
  // }
  // leftConvolveData[12] = '\0';
  // datumIndex = 0;
  // while (datumIndex < 12){
  //   rightConvolveData[datumIndex] = dopnaFile.get();
  //   datumIndex++;
  // }
  // rightConvolveData[12] = '\0';
  // bool seekRealChar = true;
  // int leftConvolveNameLength;
  // datumIndex = 0;
  // while (seekRealChar){
  //   if (leftConvolveData[datumIndex] != '0'){
  //     seekRealChar = false;
  //     leftConvolveNameLength = 12 - datumIndex;
  //   }
  //   datumIndex++;
  // }
  // seekRealChar = true;
  // int rightConvolveNameLength;
  // datumIndex = 0;
  // while (seekRealChar){
  //   if (rightConvolveData[datumIndex] != '0'){
  //     seekRealChar = false;
  //     rightConvolveNameLength = 12 - datumIndex;
  //   }
  //   datumIndex++;
  // }


  // // Get left and right convolve file names (sub strings of the file name data)
  // char leftConvolveName [leftConvolveNameLength + 1];
  // char rightConvolveName [rightConvolveNameLength + 1];
  // datumIndex = 0;
  // int nameStartingAt = 12 - leftConvolveNameLength;
  // while (datumIndex < leftConvolveNameLength){
  //   leftConvolveName[datumIndex] = leftConvolveData[nameStartingAt + datumIndex];
  //   datumIndex++;
  // }
  // leftConvolveName[leftConvolveNameLength] = '\0';
  // datumIndex = 0;
  // nameStartingAt = 12 - rightConvolveNameLength;
  // while (datumIndex < rightConvolveNameLength){
  //   rightConvolveName[datumIndex] = rightConvolveData[nameStartingAt + datumIndex];
  //   datumIndex++;
  // }
  // rightConvolveName[rightConvolveNameLength] = '\0';

  // // Get the convolve files
  // std::ifstream leftConvolveFile;
  // leftConvolveFile.open(leftConvolveName, std::ifstream::in);
  // leftConvolveFile.seekg(0, leftConvolveFile.end);
  // int lengthL = leftConvolveFile.tellg();
  // leftConvolveFile.seekg(0, leftConvolveFile.beg);

  // int dataL [lengthL];

  // datumIndex = 0;
  // while (datumIndex < lengthL){
  //   dataL[datumIndex] = leftConvolveFile.get();
  //   datumIndex++;
  // }

  // datumIndex = 44;
  // int leftConvolveAudioLength = (lengthL - 44) / 2;
  // float leftConvolve [ leftConvolveAudioLength ];
  // int audioDatumIndex = 0;
  // int thisSampleDatum [2];

  // while (datumIndex < lengthL){
  //   if ((datumIndex % 2) == 0){
  //     thisSampleDatum[0] = dataL[datumIndex];
  //   }
  //   else{
  //     thisSampleDatum[1] = dataL[datumIndex];
  //     short sample = thisSampleDatum[1] * 256;
  //     sample += thisSampleDatum[0];     
  //     //std::cout << "9 " << sample << " " << ((float) sample)  / 32767 << "\n";
  //     leftConvolve[ audioDatumIndex ] = (((float) sample) / 32767);
  //     audioDatumIndex++;
  //   }
  //   datumIndex++;
  // }
  // leftConvolveFile.close();

  // std::ifstream rightConvolveFile;
  // rightConvolveFile.open(rightConvolveName, std::ifstream::in);

  // rightConvolveFile.seekg(0, rightConvolveFile.end);
  // int lengthR = rightConvolveFile.tellg();
  // rightConvolveFile.seekg(0, rightConvolveFile.beg);

  // int dataR [lengthR];

  // datumIndex = 0;
  // while (datumIndex < lengthR){
  //   dataR[datumIndex] = rightConvolveFile.get();
  //   datumIndex++;
  // }

  // datumIndex = 44;
  // int rightConvolveAudioLength = (lengthR - 44) / 2;
  // float rightConvolve [ rightConvolveAudioLength ];
  // audioDatumIndex = 0;

  // while (datumIndex < lengthR){
  //   if ((datumIndex % 2) == 0){
  //     thisSampleDatum[0] = dataR[datumIndex];
  //   }
  //   else{
  //     thisSampleDatum[1] = dataR[datumIndex];
  //     short sample = thisSampleDatum[1] * 256;
  //     sample += thisSampleDatum[0];
  //     rightConvolve[audioDatumIndex] = (((float) sample) / 32767);
  //     audioDatumIndex++;
  //   }
  //   datumIndex++;
  // }
  // rightConvolveFile.close();