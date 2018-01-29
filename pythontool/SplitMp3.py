import librosa
import numpy as np
import argparse
import os

AllMp3List = []

def audioList(audiopath):
    mp3list = os.listdir(audiopath)
    for mp3 in mp3list:
        Secondpath = audiopath+'/'+mp3
        if os.path.isdir(Secondpath):
            audioList(Secondpath)
        elif os.path.isfile(Secondpath):
            if Secondpath[-3:]=="mp3" or Secondpath[-3:]=="wav":
                AllMp3List.append(Secondpath)
    return AllMp3List

def frame(audioPath):
    fs = 44100
    frameSize = 4096
    hopSize = 512
    Allmp3List = audioList(audioPath)
    for mp3Path in Allmp3List:
        print mp3Path
        x,fsOriginal = librosa.load(mp3Path,sr=fs)
        y = np.zeros(frameSize/2)
        x = np.hstack([y,x,y])
        x = x*2
        nFrame = np.floor((len(x)-frameSize)/hopSize)+1
        nFrame = int(nFrame)
        xFrame = np.zeros([nFrame,frameSize])
        curPos=0
        for index in xrange(nFrame):
            xFrame[index,:] = x[curPos:curPos+frameSize]
            curPos = curPos+hopSize
        index = mp3Path.rfind("/")
        filepath = mp3Path[0:index+1]+'frame.txt'
        np.savetxt(filepath,xFrame) 
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("audioPath",help="input need to be divided mp3")
    args = parser.parse_args()
    frame(args.audioPath)