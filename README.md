# SignalProcessing

This project served as a better path towards understanding signal processing concepts through practical experimentation and visualization.

It has two main parts:

## Time Domain Processing

The first panel focuses on analyzing and manipulating the signal directly in the time domain. It provides:

Two filter types:

>Moving Average Filter – used for smoothing the signal by averaging values over a sliding window.

>Alpha Criterion Filter – a recursive smoothing filter controlled by an alpha parameter.

Step-by-step signal navigation, allowing the user to move second by second through the signal and observe how processing affects it over time.

Envelope visualization, which highlights the overall amplitude contour of the signal and helps in identifying peaks and variations.

This part of the application helps in understanding how filtering techniques affect signals directly as they evolve in time.

## Frequency Domain Processing

The second panel provides tools for analyzing the signal in the frequency domain. Instead of observing how the signal evolves over time, this section focuses on identifying the frequencies that compose the signal.

The panel includes:

>Fast Fourier Transform (FFT) computation for converting the signal from the time domain into the frequency domain.

>Spectrum visualization, allowing users to observe dominant frequencies and overall spectral distribution.

>Frequency filtering, enabling the attenuation or removal of selected frequency components.

>Comparative visualization, showing how the spectrum changes before and after applying filters.

>Through these tools, users can better understand how signals can be decomposed into their frequency components and how filtering in the frequency domain affects the reconstructed signal.

##Purpose of the Project

The goal of this project is educational. It provides an interactive environment where users can:

>experiment with different filtering techniques,
>observe signal behavior in both time and frequency domains,
>understand the relationship between filtering operations and signal characteristics.

By combining visualization and step-by-step interaction, the project makes fundamental signal processing concepts easier to explore and understand.
