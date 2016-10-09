//#include <nan.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "./generate/sine.h"
#include "./generate/saw.h"
#include "./generate/sample.h"

#include "./effect/ramp.h"
#include "./effect/convolve.h"
#include "./effect/volume.h"
#include "./effect/lopass.h"
#include "./effect/fadeOut.h"

#include "./mix.h"
#include "./wavWrite.h"

int main( 
    int argNumber,
    char ** args
  ){

  const char * dopnaFileName = args[1];
  const char * saveFileNameL = args[2];
  const char * saveFileNameR = args[3];

  std::ifstream dopnaFile;
  dopnaFile.open(dopnaFileName, std::ifstream::in);

  dopnaFile.seekg(0, dopnaFile.end);
  int dopnaFileLength = dopnaFile.tellg();
  dopnaFile.seekg(0, dopnaFile.beg);

  std::cout << "5 " << dopnaFileLength << "\n";

  char header [8];
  int placeholderIndex = 0;
  int datumIndex = 0;
  while (datumIndex < 8){
    int thisHeaderItem = dopnaFile.get();
    header[datumIndex] = thisHeaderItem;
    datumIndex++;
  }

  int scaleLength = dopnaFile.get();

  // Get the scale, first as strings, then converted
  // to floats.
  float scale [scaleLength];
  placeholderIndex = 0;
  while (placeholderIndex < scaleLength){
    datumIndex = 0;
    char intervalAsChar [9];
    while (datumIndex < 8){
      char thisChar = (char) dopnaFile.get();
      intervalAsChar[datumIndex] = thisChar;
      datumIndex++;
    }
    intervalAsChar[datumIndex] = '\0';
    scale[placeholderIndex] = atof(intervalAsChar);
    placeholderIndex++;
  }


  // Get the ensemble, and save the voice types
  // and the voice positions in their respective
  // arrays
  int ensembleSize0 = dopnaFile.get();
  int ensembleSize1 = dopnaFile.get();
  int ensembleSize = (ensembleSize0 * 256) + ensembleSize1;

  short ensemblesPositions [ ensembleSize ][2];
  char ensembleTypes [ ensembleSize ][5];
  char ensembleConvolves [ ensembleSize ][13];
  placeholderIndex = 0;
  while (placeholderIndex < ensembleSize){    
    int xPos0;
    int xPos1;
    short xPos;
    
    int yPos0;
    int yPos1;
    short yPos;

    datumIndex = 0;
    while (datumIndex < 4){
      ensembleTypes[placeholderIndex][datumIndex] = (char) dopnaFile.get();
      datumIndex++;
    }
    ensembleTypes[placeholderIndex][4] = '\0';

    xPos0 = dopnaFile.get();
    xPos1 = dopnaFile.get();
    xPos = (xPos0 * 256) + xPos1;
    if (xPos < 0){
      xPos += 32768;
      xPos *= -1;
    }

    yPos0 = dopnaFile.get();
    yPos1 = dopnaFile.get();
    yPos = (yPos0 * 256) + (yPos1);
    if (yPos < 0){
      yPos += 32768;
      yPos *= -1;
    }
    ensemblesPositions[placeholderIndex][0] = xPos;
    ensemblesPositions[placeholderIndex][1] = yPos;

    datumIndex = 0;
    while (datumIndex < 12){
      ensembleConvolves[ placeholderIndex ][ datumIndex ] = (char) dopnaFile.get();
      datumIndex++;
    }
    ensembleConvolves[ placeholderIndex ][ 12 ] = '\0';

    placeholderIndex++;
  }

  std::cout << "7 " << ensembleConvolves[0] << "\n";

  // Get Dimensions
  int numberOfDimensions = dopnaFile.get();
  char dimensions [numberOfDimensions][13];

  std::cout << "7.1 \n";

  placeholderIndex = 0;
  while (placeholderIndex < numberOfDimensions){
    datumIndex = 0;
    while (datumIndex < 12){
      dimensions[placeholderIndex][datumIndex] = (char) dopnaFile.get();
      datumIndex++;
    }
    dimensions[placeholderIndex][12] = '\0';
    placeholderIndex++;
  }

  std::cout << "7.2 \n";

  // Get the timing
  int pieceDuration0 = dopnaFile.get();
  int pieceDuration1 = dopnaFile.get();

  std::cout << "7.2.1 " << pieceDuration0 << " " << pieceDuration1 << "\n";

  int pieceDurationInBeats = pieceDuration0 * 256;
  pieceDurationInBeats += pieceDuration1;

  int beatTimes [ pieceDurationInBeats ];
  int thisBeatTempo [2];

  std::cout << "7.3 \n";

  datumIndex = 0;
  while (datumIndex < pieceDurationInBeats){
    thisBeatTempo[0] = dopnaFile.get();
    thisBeatTempo[1] = dopnaFile.get();
    int thisBeatDuration = thisBeatTempo[0] * 256;
    thisBeatDuration += thisBeatTempo[1];
    beatTimes[datumIndex] = thisBeatDuration;
    datumIndex++;
  }

  std::cout << "8 \n";

  // Get the score
  float score [ensembleSize][pieceDurationInBeats][numberOfDimensions + 1];
  
  int ensembleIndex = 0;
  while (ensembleIndex < ensembleSize){
    int beatIndex = 0;
    while (beatIndex < pieceDurationInBeats){
      int existsOrNot = dopnaFile.get();
      score[ensembleIndex][beatIndex][0] = (float) existsOrNot;

      int dimensionIndex = 0;
      while (dimensionIndex < numberOfDimensions){
        char thisDimension [8];
        int dimensionCharIndex = 0;
        while (dimensionCharIndex < 8){
          thisDimension[dimensionCharIndex] = dopnaFile.get();
          dimensionCharIndex++;
        }

        thisDimension[dimensionCharIndex] = '\0';
        score[ensembleIndex][beatIndex][dimensionIndex + 1] = atof(thisDimension);
        dimensionIndex++;
      }
      beatIndex++;
    }
    ensembleIndex++;
  }

  std::cout << "9 \n";

  long pieceDurationInSamples = 0;
  int timeIndex = 0;
  while (timeIndex <  pieceDurationInBeats){
    pieceDurationInSamples += beatTimes[timeIndex];
    timeIndex++;
  }


  short * pieceL;
  short * pieceR;
  pieceL = new short [ pieceDurationInSamples ];
  pieceR = new short [ pieceDurationInSamples ];

  long pieceIndex = 0;
  while (pieceIndex < pieceDurationInSamples){
    pieceL[ pieceIndex ] = 0;
    pieceR[ pieceIndex ] = 0;
    pieceIndex++;
  }

  std::cout << "9.1 \n";

  int indexOfSustain;
  int indexOfFrequency;
  int indexOfAmplitude;

  int dimensionIndex = 0;
  while (dimensionIndex < numberOfDimensions){

    if (dimensions[dimensionIndex][0] == '0'){
      if (dimensions[dimensionIndex][1] == '0'){
        if (dimensions[dimensionIndex][2] == '0'){
          if (dimensions[dimensionIndex][3] == '0'){
            if (dimensions[dimensionIndex][4] == '0'){
              if (dimensions[dimensionIndex][5] == 's'){
                if (dimensions[dimensionIndex][6] == 'u'){
                  if (dimensions[dimensionIndex][7] == 's'){
                    if (dimensions[dimensionIndex][8] == 't'){
                      if (dimensions[dimensionIndex][9] == 'a'){
                        if (dimensions[dimensionIndex][10] == 'i'){
                          if (dimensions[dimensionIndex][11] == 'n'){
                            indexOfSustain = dimensionIndex + 1;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    if (dimensions[dimensionIndex][0] == '0'){
      if (dimensions[dimensionIndex][1] == '0'){
        if (dimensions[dimensionIndex][2] == '0'){
          if (dimensions[dimensionIndex][3] == '0'){
            if (dimensions[dimensionIndex][4] == '0'){
              if (dimensions[dimensionIndex][5] == '0'){
                if (dimensions[dimensionIndex][6] == '0'){
                  if (dimensions[dimensionIndex][7] == '0'){
                    if (dimensions[dimensionIndex][8] == 't'){
                      if (dimensions[dimensionIndex][9] == 'o'){
                        if (dimensions[dimensionIndex][10] == 'n'){
                          if (dimensions[dimensionIndex][11] == 'e'){
                            indexOfFrequency = dimensionIndex + 1;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    if (dimensions[dimensionIndex][0] == '0'){
      if (dimensions[dimensionIndex][1] == '0'){
        if (dimensions[dimensionIndex][2] == '0'){
          if (dimensions[dimensionIndex][3] == 'a'){
            if (dimensions[dimensionIndex][4] == 'm'){
              if (dimensions[dimensionIndex][5] == 'p'){
                if (dimensions[dimensionIndex][6] == 'l'){
                  if (dimensions[dimensionIndex][7] == 'i'){
                    if (dimensions[dimensionIndex][8] == 't'){
                      if (dimensions[dimensionIndex][9] == 'u'){
                        if (dimensions[dimensionIndex][10] == 'd'){
                          if (dimensions[dimensionIndex][11] == 'e'){
                            indexOfAmplitude = dimensionIndex + 1;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    dimensionIndex++;
  }

  std::cout << "9.2 \n";

  // Sort through the notes
  ensembleIndex = 0;
  while (ensembleIndex < ensembleSize){
    if (ensembleTypes[ensembleIndex][0] == 's'){
      if (ensembleTypes[ensembleIndex][1] == 'i'){
        if (ensembleTypes[ensembleIndex][2] == 'n'){
          if (ensembleTypes[ensembleIndex][3] == 'e'){


            std::cout << "A \n";

            float direction = atan2( 
              ensemblesPositions[ ensembleIndex ][0],
              ensemblesPositions[ ensembleIndex ][0]);
            direction /= 3.14159;

            float distance = (ensemblesPositions[ ensembleIndex ][0] * ensemblesPositions[ ensembleIndex ][0]);
            distance      += (ensemblesPositions[ ensembleIndex ][1] * ensemblesPositions[ ensembleIndex ][1]);
            distance       = sqrt(distance);

            int delay = (int)((distance / 340) * 44100);

            bool seekRealChar = true;
            int convolveNameLength;
            int convolveNameIndex = 0;
            while (seekRealChar){
              if (ensembleConvolves[ ensembleIndex ][ convolveNameIndex ] != '0'){
                seekRealChar = false;
                convolveNameLength = 12 - convolveNameIndex;
              }
              convolveNameIndex++;
            }

            const char * convolveName1 [ (convolveNameLength + 1) ];
            int nameStartingAt = 12 - convolveNameLength;

            convolveNameIndex = 0;
            while (convolveNameIndex < convolveNameLength){
              convolveName1[ convolveNameIndex ] = ensembleConvolves[ ensembleIndex ][nameStartingAt + convolveNameIndex];
              convolveNameIndex++;
            }

            //std::cout << "B " << convolveName << " " <<  ensembleConvolves[ensembleIndex] << "\n";

            std::string convolveName = convolveName1;

            std::cout << "B.0.1 " << convolveName << " " << convolveName1 << " stPuchL.wav" << "\n";

            std::ifstream convolveFile;

            std::cout << "B.0.3 " << convolveName << " " << convolveName.c_str() << "\n";

            convolveFile.open(convolveName.c_str());
            convolveFile.seekg(0, convolveFile.end);
            int convolveFileLength = convolveFile.tellg();
            convolveFile.seekg(0, convolveFile.beg);

            std::cout << "B.1 " << convolveFileLength << "\n";

            // int convolveData [ convolveFileLength ];

            // int convolveDatumIndex = 0;
            // while ( convolveDatumIndex < convolveFileLength){
            //   convolveData[ convolveDatumIndex ] = convolveFile.get();
            //   convolveDatumIndex++;
            // }

            // std::cout << "B.2 \n";

            // convolveDatumIndex = 44;
            // int convolveLength = (convolveFileLength - 44) / 2;

            // std::cout << "B.2.1 " << convolveLength << "\n";

            // float * convolveAudio = new float [ convolveLength ];
            // int convolveAudioIndex = 0;
            // int thisSampleDatum [2];
            // short sample;

            // std::cout << "B.3 \n";

            // while (convolveDatumIndex < convolveFileLength){
            //   if ((datumIndex % 2) == 0){
            //     thisSampleDatum[0] = convolveData[ convolveDatumIndex ];
            //   }
            //   else{
            //     thisSampleDatum[1] = convolveData[ convolveDatumIndex ];
            //     sample  = thisSampleDatum[1] * 256;
            //     sample += thisSampleDatum[0];
            //     convolveAudio[ convolveAudioIndex ] = (((float) sample) / 32767);
            //     convolveAudioIndex++;
            //   }
            //   convolveDatumIndex++;
            // }
            // convolveFile.close();

            std::cout << "C \n";

            long timeAtThisNote = 0;
            int pieceIndex = 0;
            while (pieceIndex < pieceDurationInBeats){
              // std::cout << "C.1 \n";
              if (score[ ensembleIndex ][ pieceIndex ][0] == 1){

                // std::cout << "D \n";

                int sustain = score[ ensembleIndex ][ pieceIndex ][ indexOfSustain ];

                //std::cout << "SUSTAIN IS " << sustain << "\n";

                float frequency = score[ ensembleIndex ][ pieceIndex ][ indexOfFrequency ];
                float amplitude = score[ ensembleIndex ][ pieceIndex ][ indexOfAmplitude ];

                std::cout << " D.1 " << sustain << " " << frequency << " " << amplitude << "\n";

                short * sineWave;
                sineWave = new short [ sustain ];

                int confirmation = sine( sustain, frequency, sineWave );


                std::cout << "WOW \n";


                confirmation     = ramp( sustain, sineWave );
                confirmation     = volume( amplitude, sustain, sineWave);

                // int duration = sustain + delay;
                // short * audioOut0 = new short [ duration ];

                // int sampleIndex = 0;
                // while ( sampleIndex < delay){
                //   audioOut0[ sampleIndex ] = 0;
                //   sampleIndex++;
                // }
                // sampleIndex = 0;
                // while ( sampleIndex < sustain ){
                //   //std::cout << sineWave[sampleIndex] << "\n";
                //   audioOut0[ sampleIndex + delay ] = sineWave[ sampleIndex ];
                //   sampleIndex++;
                // }

                // std::cout << "E \n";
                
                // duration += convolveLength;
                
                // short * audioOut1 = new short [ duration ];

                // sampleIndex = 0;
                // while (sampleIndex < duration){
                //   audioOut1[ sampleIndex ] = 0;
                //   sampleIndex++;
                // }

                // confirmation = convolve( 
                //   0.015,

                //   audioOut0,
                //   duration,

                //   convolveAudio,
                //   convolveLength,

                //   audioOut1
                // );


                // std::cout << "E \n";

                if (direction > 0){
                  int sampleIndex = 0;
                  while (sampleIndex < sustain){
                    pieceL[ sampleIndex + timeAtThisNote ] += sineWave[ sampleIndex ];
                    pieceR[ sampleIndex + timeAtThisNote ] += sineWave[ sampleIndex ];
                    sampleIndex++;
                  }
                }
                else{
                  int sampleIndex = 0;
                  while (sampleIndex < sustain){
                    pieceR[ sampleIndex + timeAtThisNote ] += sineWave[ sampleIndex ];
                    pieceL[ sampleIndex + timeAtThisNote ] += sineWave[ sampleIndex ];
                    sampleIndex++;
                  }
                }

                delete[] sineWave;

                // delete[] audioOut0;
                // delete[] audioOut1;

                // std::cout << "F \n";

              }
              pieceIndex++;
              timeAtThisNote += beatTimes[ pieceIndex ]; 
            }
            // delete[] convolveAudio;
          }
        }
      }
    }

    // if (ensembleTypes[ensembleIndex][0] == 's'){
    //   if (ensembleTypes[ensembleIndex][1] == 'a'){
    //     if (ensembleTypes[ensembleIndex][2] == 'm'){
    //       if (ensembleTypes[ensembleIndex][3] == 'p'){
    //         //std::cout << "IS SAMP!!!" << "\n";

    //         int pieceIndex = 0;
    //         while (pieceIndex < pieceDurationInBeats){
    //           // if (score[ensembleIndex][pieceIndex][0] == 1){
    //           //   short * audio = new short[(int) score[ensembleIndex][pieceIndex][1]];
    //           //   confirmation = sine(score[ensembleIndex][pieceIndex], audio);
    //           // }
    //           pieceIndex++;
    //         }
    //       }
    //     }
    //   }
    // }
    ensembleIndex++;
  }

  std::cout << "G \n";

  writeWAVData( saveFileNameL, pieceL, pieceDurationInSamples * 2, 44100, 1 );
  writeWAVData( saveFileNameR, pieceR, pieceDurationInSamples * 2, 44100, 1 );

  std::cout << "H \n";

  delete[] pieceL;
  delete[] pieceR;

  return 0;
}