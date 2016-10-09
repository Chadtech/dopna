            // float direction = atan2( 
            //   ensemblesPositions[ ensembleIndex ][0],
            //   ensemblesPositions[ ensembleIndex ][0]);
            // direction /= 3.14159;

            // float distance = (ensemblesPositions[ ensembleIndex ][0] * ensemblesPositions[ ensembleIndex ][0]);
            // distance      += (ensemblesPositions[ ensembleIndex ][1] * ensemblesPositions[ ensembleIndex ][1]);
            // distance       = sqrt(distance);

            // int delay = (int)((distance / 340) * 44100);

            int pieceIndex = 0;
            long timeAtThisNote = 0;
            while (pieceIndex < pieceDurationInBeats){

              if (score[ensembleIndex][pieceIndex][0] == 1){

                float direction = atan2( 
                  ensemblesPositions[ ensembleIndex ][0],
                  ensemblesPositions[ ensembleIndex ][0]);
                direction /= 3.14159;

                float distance = (ensemblesPositions[ ensembleIndex ][0] * ensemblesPositions[ ensembleIndex ][0]);
                distance      += (ensemblesPositions[ ensembleIndex ][1] * ensemblesPositions[ ensembleIndex ][1]);
                distance       = sqrt(distance);

                int delay = (int)((distance / 340) * 44100);

                std::cout << "0 \n";

                int sustain = score[ ensembleIndex ][ pieceIndex ][ indexOfSustain ];
                float frequency = score[ ensembleIndex ][ pieceIndex ][ indexOfFrequency ];
                float amplitude = score[ ensembleIndex ][ pieceIndex ][ indexOfAmplitude ];

                int lengthOfNote = sustain + leftConvolveAudioLength;
                short * audio = new short [ sustain ];
                
                int confirmation = sine(sustain, frequency, audio);
                
                confirmation = ramp(sustain, audio);

                short * convolvedAudio = new short [ lengthOfNote ];

                confirmation = convolve(
                  0.015, 

                  audio,
                  sustain, 

                  leftConvolve,
                  leftConvolveAudioLength,

                  convolvedAudio
                );

                std::cout << "1 " << audio <<"\n";
                delete[] audio;
                std::cout << "1.1 \n";

                confirmation = volume(amplitude, lengthOfNote, convolvedAudio);

                lengthOfNote += delay;

                short * delayedAudio = new short [ lengthOfNote ];

                int sampleIndex = 0;
                while (sampleIndex < delay){
                  delayedAudio[ sampleIndex ] = 0;
                  sampleIndex++;
                }
                sampleIndex = 0;
                while (sampleIndex < lengthOfNote){ 
                  delayedAudio[ sampleIndex + delay ] = convolvedAudio[ sampleIndex] ;
                  sampleIndex++;
                }

                std::cout << "2 " <<  convolvedAudio << "\n";
                delete[] convolvedAudio;

                short * lopassdAudio = new short [ lengthOfNote ];

                sampleIndex = 0;
                while (sampleIndex < lengthOfNote){
                  lopassdAudio[ sampleIndex ] = delayedAudio[ sampleIndex ];
                  sampleIndex++;
                }

                confirmation = lopass(lopassdAudio,lengthOfNote);

                // short * primaryChannel   = new short [ lengthOfNote ];
                // short * secondaryChannel = new short [ lengthOfNote ];

                // confirmation = mix(
                //   primaryChannel,
                //   lengthOfNote,

                //   delayedAudio,
                //   abs(direction),

                //   lopassdAudio,
                //   1.0 - (abs(direction)));

                // confirmation = mix(
                //   secondaryChannel,
                //   lengthOfNote,

                //   delayedAudio,
                //   1.0 - (abs(direction)),

                //   lopassdAudio,
                //   abs(direction));

                //std::cout << "3 \n";
                // delete[] lopassdAudio;
                // delete[] delayedAudio;

                if (direction > 0){
                  sampleIndex = 0;
                  while (sampleIndex < lengthOfNote){
                    pieceL[ sampleIndex + timeAtThisNote ] += delayedAudio[ sampleIndex ];
                    pieceR[ sampleIndex + timeAtThisNote ] += lopassdAudio[ sampleIndex ];
                    sampleIndex++;
                  }
                }
                else{
                  sampleIndex = 0;
                  while (sampleIndex < lengthOfNote){
                    pieceR[ sampleIndex + timeAtThisNote ] += delayedAudio[ sampleIndex ];
                    pieceL[ sampleIndex + timeAtThisNote ] += lopassdAudio[ sampleIndex ];
                    sampleIndex++;
                  }
                }

                std::cout << "4 " << lopassdAudio << " " << delayedAudio <<"\n";
                delete[] lopassdAudio;
                delete[] delayedAudio;
                // delete[] primaryChannel;
                // delete[] secondaryChannel;
                std::cout << "5 \n";