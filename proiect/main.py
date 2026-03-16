import os
import numpy as np
from scipy.io import wavfile
from scipy.signal import hilbert

info_file = "waveInfo.txt"
data_file = "waveData.txt"
envelope_file = "waveEnvelope.txt"

# citire WAV
samplerate, data = wavfile.read('43.wav')

# dacă semnalul este stereo, folosim doar un canal
if data.ndim > 1:
    data = data[:, 0]

# salvare informații
with open(info_file, "w") as wavFileInfo:
    wavFileInfo.write(f"{samplerate}\n")
    wavFileInfo.write(f"{data.size}\n")

# calcul anvelopă (Hilbert)
analytic_signal = hilbert(data)
amplitude_envelope = np.abs(analytic_signal)

# salvare semnal original
np.savetxt(data_file, data, fmt="%d")

# salvare anvelopă
np.savetxt(envelope_file, amplitude_envelope, fmt="%.6f")
