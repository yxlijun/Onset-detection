# -*- coding:utf-8 -*-

from __future__ import division
import numpy as np
import scipy.fftpack as fft
from scipy.io import wavfile
import librosa
import os.path
import glob     
import fnmatch

class Spectrum(object):
    def __init__(self,filename):
        self.fs = 44100
        self.frameSize = 4096
        self.hopSize = 512
        self.filename = filename
        self.FFTSIZE = 1024
        self.SPECTRUMLENGTH = 513
        self.FRAMESIZE = 1024


    
    def frame(self):
        x,fsOriginal = librosa.load(self.filename,sr=self.fs)
        y = np.zeros(int(self.frameSize/2))
        x = np.hstack([y,x,y])
        x = x*2
        nFrame = np.floor((len(x)-self.frameSize)/self.hopSize)+1
        nFrame = int(nFrame)
        self.xFrame = np.zeros([nFrame,self.frameSize])
        curPos=0
        for index in xrange(nFrame):
            self.xFrame[index,:] = x[curPos:curPos+self.frameSize]
            curPos = curPos+self.hopSize
    
    def calspectrum(self):
        self.frame()
        frameSum = self.xFrame.shape[0]
        reframe = np.zeros([frameSum,self.FRAMESIZE])
        self.spectrum = np.zeros([frameSum,self.SPECTRUMLENGTH])
        for i in range(frameSum):
            reframe[i,:] = librosa.resample(self.xFrame[i],4096,self.FRAMESIZE)
        for index in range(frameSum):
            self.spectrum[index,:] = np.abs(librosa.stft(reframe[index],n_fft = self.FFTSIZE,hop_length =1024,center=False).reshape(self.SPECTRUMLENGTH))

    def log(self,mul=20,add=1):
        if add<=0:
            raise ValueError(" a postive value must be added before taking the logarithm")
        self.spectrum = np.log10(mul*self.spectrum+add)
    
    def filter(self):
        filterbank= Filter().filterbank
        self.spectrum = np.dot(self.spectrum,filterbank)
        self.bins = np.shape(filterbank)[1]



class Filter(object):
    def __init__(self,fs=44100):
        self.bands = 12
        self.fmin = 27.5
        self.fmax = 16000
        self.a = 440
        self.SPECTRUMLENGTH = 513              
        if self.fmax>fs/2:
            self.fmax = fs/2
        frequencies = self.frequencies()
        factor = (fs/2.0)/self.SPECTRUMLENGTH
        frequencies = np.round(np.asarray(frequencies) / factor).astype(int)
        frequencies = np.unique(frequencies)
        frequencies = [f for f in frequencies if f<self.SPECTRUMLENGTH]
        bands = len(frequencies)-2
        self.filterbank = np.zeros([self.SPECTRUMLENGTH,bands],dtype=np.float)
        for band in range(bands):
            start,mid,stop = frequencies[band:band+3]
            self.filterbank[start:stop,band] = self.triang(start,mid,stop)

    def frequencies(self):
        factor = 2.0**(1.0/self.bands)
        freq = self.a
        frequencies = [freq]
        while freq<=self.fmax:
            freq*=factor
            frequencies.append(freq)
        freq = self.a
        while freq>=self.fmin:
            freq/=factor
            frequencies.append(freq)
        frequencies.sort()
        return frequencies

    def triang(self,start,mid,stop):
        height = 1
        traing_filter = np.empty(stop-start)
        traing_filter[:mid-start] = np.linspace(0,height,(mid-start),endpoint=False)
        traing_filter[mid-start:] = np.linspace(height,0,(stop-mid),endpoint = False)
        return traing_filter


class SpctrumODF(object):
    def __init__(self,spectrogram):
        self.s = spectrogram
    
    def diff(self,spec):
        diff = np.zeros_like(spec)
        diff[1:] = spec[1:]-spec[0:-1]
        diff = np.maximum(diff,0)
        return diff 
    
    def sf(self):
        return np.sum(self.diff(self.s.spectrum),axis=1)

class Onsets(object):
    def __init__(self,activations):
        self.activations = activations
        self.fps = 44100/512
        self.detections = []
    
    def detect(self,threshold=2.5,combine=7,pre_max=6,pre_avg=20):
        import scipy.ndimage as sim  
        max_length = pre_max+1
        max_origin = int(np.floor(pre_max / 2))
        mov_max = sim.filters.maximum_filter1d(self.activations,max_length,mode='constant', origin=max_origin)
        avg_length = pre_avg + 1
        avg_origin = int(np.floor(pre_avg / 2))
        mov_avg = sim.filters.uniform_filter1d(self.activations, avg_length, mode='constant', origin=avg_origin)
        detections = self.activations * (self.activations == mov_max)
        detections = detections * (detections >= mov_avg + threshold)
        last_onset = 0
        for i in np.nonzero(detections)[0]:
            onset = float(i)/float(self.fps)
            if i>last_onset+combine:
                self.detections.append(onset)
                last_onset = i

    def write(self,filename):
        with open(filename,"w") as f:
            for pos in self.detections:
                f.write(str(pos)+"\n")


def parser():
    import argparse  
    p = argparse.ArgumentParser()
    p.add_argument('--wav',help="wav to be onset detection")
    p.add_argument('--filter',action='store_true',default=None,help="spectrum filter")
    p.add_argument('--log',action='store_true',default=None,help='spectrum log')
    p.add_argument('--mul', action='store', default=20, type=float, help='multiplier (before taking the log) [default=1]')
    p.add_argument('--add', action='store', default=1, type=float, help='value added (before taking the log) [default=1]')
    args = p.parse_args()
    return args   


def main():
    args = parser()
    audiofile = args.wav               
    if not fnmatch.fnmatch(audiofile,'*.wav') and not fnmatch.fnmatch(audiofile,'*.mp3'):
        raise ValueError("files must be mp3 or wav")
    filename = os.path.splitext(audiofile)[0]
    s = Spectrum(audiofile)
    s.calspectrum()
    if args.filter:
        s.filter()
    if args.log:
        s.log()
    sdof = SpctrumODF(s)
    act = sdof.sf()
    onsetdec = Onsets(act)
    onsetdec.detect()
    onsetdec.write("%s.onsets.txt" % (filename))

if __name__=='__main__':
    main()







   
